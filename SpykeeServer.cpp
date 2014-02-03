/*
 * SpykeeServer.cpp
 *
 *  Created on: 3 déc. 2013
 *      Author: remi
 */

#include "headers/SpykeeServer.h"

/*
 * Private
 */
int socketMaster, socketClient;
struct sockaddr_in server, client;
pthread_t snifferThread;

SpykeeServer::SpykeeServer() {

}

int main(int argc, char *argv[]) {
	/*SpykeeServer *server = new SpykeeServer();
	server->launcherServer();*/

	const std::string configFile="../config/robots.xml";
	FILE *f = std::fopen(configFile.c_str(), "r");
	if (f == NULL){
		//TODO gérer les erreurs
	}
	tinyxml2::XMLDocument xmlFile;
	if ( xmlFile.LoadFile(f) != tinyxml2::XML_NO_ERROR){
		//TODO gérer les erreurs
	}

	std::vector<std::string> ipAllowed;
	tinyxml2::XMLElement *currentElement = xmlFile.FirstChildElement("controller")->FirstChildElement("config")->FirstChildElement("ipAllowed");
	do{
		ipAllowed.push_back(currentElement->GetText());
	}
	while ( (currentElement = currentElement->NextSiblingElement("ipAllowed")) );

	tinyxml2::XMLNode *currentNode = xmlFile.FirstChildElement("controller")->FirstChildElement("robots")->FirstChildElement("robot");

	std::string name, ip, username, password;
	int id, controllerPort, defaultSpeed;
	// To store all instance of SpykeeServerRobot
	std::vector<SpykeeServerRobot> robots;
	robots.reserve(2); // Minimum 2 robots is expected
	// To store all thread
	std::vector<std::thread> threads;
	threads.reserve(2); // Minimum 2 thread is expected (2 robots)
	do{
		if (atoi(currentNode->FirstChildElement("enabled")->GetText()) == 1){
			// TODO gérer les erreurs
			id = atoi(currentNode->FirstChildElement("id")->GetText());
			name = currentNode->FirstChildElement("name")->GetText();
			ip = currentNode->FirstChildElement("ipAddress")->GetText();
			controllerPort = atoi(currentNode->FirstChildElement("controllerPort")->GetText());
			username = currentNode->FirstChildElement("username")->GetText();
			password = currentNode->FirstChildElement("password")->GetText();
			defaultSpeed = atoi(currentNode->FirstChildElement("defaultSpeed")->GetText());

			try{
				robots.push_back(SpykeeServerRobot(id, name, ip, controllerPort, username, password, defaultSpeed, ipAllowed));
				threads.push_back(std::thread(&SpykeeServerRobot::test3_, &robots.back()));
			}
			catch (SpykeeServerException &e) {
				std::cout << "ERREUR : " << e.what() << std::endl;
				std::cout << "MESSAGE : " << e.getErDescription() << std::endl;
				std::cout << "PORT : " << controllerPort << std::endl;
				// TODO Gérer les erreurs
			}
		}
	}
	while ( (currentNode = currentNode->NextSibling()) );

	std::fclose(f); // Close the XML file

	// TODO Lancer le serveur "Frontal"

	for(auto& t : threads){
		t.join();
	}

	std::cout << "Arrêt du main" << std::endl;

}

/*void SpykeeServer::launcherServer(){
		//Create socket
		this->socketMaster = socket(AF_INET, SOCK_STREAM, 0);
		if (this->socketMaster == -1) {
			std::cout << "Could not create socket";
		}
		puts("Socket created");

		//Prepare the sockaddr_in structure
		this->server.sin_family = AF_INET;
		this->server.sin_addr.s_addr = INADDR_ANY;
		this->server.sin_port = htons(8888);

		//Bind
		if (bind(this->socketMaster, (struct sockaddr *) &this->server, sizeof(this->server)) < 0) {
			//print the error message
			perror("bind failed. Error");
		}
		puts("bind done");

		//Listen
		listen(this->socketMaster, 3);

		//Accept and incoming connection
		puts("Waiting for incoming connections...");
		// FIXME definition de variable plus propre
		int c;
		c = sizeof(struct sockaddr_in);

		while ((socketClient = accept(this->socketMaster, (struct sockaddr *) &this->client,
				(socklen_t*) &c))) {
			puts("Connection accepted");

			int *newSocket(0);
			newSocket = (int*) malloc(1); // Prepare a new var to store the socket
			newSocket = &(this->socketClient);

			if (pthread_create(&this->snifferThread, NULL, SpykeeServer::connection_handler, (void*) newSocket) < 0) {
				perror("could not create thread");
			}

			//Now join the thread , so that we dont terminate before the thread
			//pthread_join( sniffer_thread , NULL);
			puts("Handler assigned");
		}

		if (this->socketClient < 0) {
			perror("accept failed");
		}

}

/*
 * This will handle connection for each client
 * */
/*void *SpykeeServer::connection_handler(void *socket_desc) {
	//Get the socket descriptor
	int sock = *(int*) socket_desc;
	int read_size;
	std::string message;
	char client_message[2000];

	//Send some messages to the client
	message = "Greetings! I am your connection handler\n";
	write(sock, message.c_str(), message.length());

	message = "Now type something and i shall repeat what you type \n";
	write(sock, message.c_str(), message.length());

	//Receive a message from client
	while ((read_size = recv(sock, client_message, 2000, 0)) > 0) {
		//Send the message back to client
		write(sock, client_message, strlen(client_message));
	}

	if (read_size == 0) {
		puts("Client disconnected");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}

	//Free the socket pointer
	free(socket_desc);

	return 0;
}*/

SpykeeServer::~SpykeeServer() {
}

