/*
 * SpykeeServerRobot.cpp
 *
 *  Created on: 16 déc. 2013
 *      Author: remi
 */

#include "headers/SpykeeServerRobot.h"
#include <iostream> //Fixme Juste pour les test

std::vector<int> SpykeeServerRobot::idUsed_;
std::vector<int> SpykeeServerRobot::portUsed_;

SpykeeServerRobot::SpykeeServerRobot(int id, std::string name, std::string ip, int port, std::string username, std::string password,
		int defaultSpeed, std::vector<std::string> ipAllowed) : id(-1), ip("0.0.0.0"), controllerPort(0), sockRobot_(0), server({0}), batteryLevel_(-1), speed_(100), isRunning_(false)/*,
		_threadListenRobot(0)*/{
	std::cout << "CREATION DE L'OBJET !" << std::endl;
	this->setRobotId_(id);
	this->name = name;
	this->setRobotIp_(ip);
	this->setControllerPort_(port);
	this->username = username;
	this->password = password;
	this->speed_ = defaultSpeed;
	this->ipAllowed_ = ipAllowed;

}

/*
 * Methods which test input values
 */

// Todo Revoir les prérequis du l'exception. Car là on a besoin de variables non définies
void SpykeeServerRobot::setRobotId_(const int &id) {
	if (std::find(SpykeeServerRobot::idUsed_.begin(), SpykeeServerRobot::idUsed_.end(), id)!=SpykeeServerRobot::idUsed_.end()){
		throw SpykeeServerException(SpykeeServerException::idAlreadyUsed, "The robot id is already used"
			, SpykeeServerException::fatal, this->name, ip, this->controllerPort);
	}
	this->id = id;
	SpykeeServerRobot::idUsed_.push_back(id);
}

void SpykeeServerRobot::setRobotIp_(const std::string &ip) {
	struct sockaddr_in sa;
	if (inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 1){ // If isn't a valid IP address
		throw SpykeeServerException(SpykeeServerException::badIpAddress, "The ip address entered of the robot isn't "
			"a valid IP address", SpykeeServerException::fatal, this->name, ip, this->controllerPort);
	}
	this->ip = ip;
}

void SpykeeServerRobot::setControllerPort_(const int &port) {
	if (std::find(SpykeeServerRobot::portUsed_.begin(), SpykeeServerRobot::portUsed_.end(), port)!=SpykeeServerRobot::portUsed_.end()){
		throw SpykeeServerException(SpykeeServerException::badPortNumber, "The port number entered is already used by an other controller "
						, SpykeeServerException::fatal, this->name, ip, this->controllerPort);
	}
	else if ( !(port > 1023 && port <= 49151)){ // Invalid port number
		throw SpykeeServerException(SpykeeServerException::badPortNumber, "The port number entered of the controller isn't "
			"valid", SpykeeServerException::fatal, this->name, ip, this->controllerPort);
	}
	else{
		struct sockaddr_in controller = { 0 };
		int sock;

		controller.sin_family = AF_INET;
		controller.sin_port = htons(port);
		controller.sin_addr.s_addr = inet_addr("127.0.0.1");

		sock = (int) socket(AF_INET, SOCK_STREAM, 0);
		int result = connect(sock, (struct sockaddr *) &controller, sizeof(controller));
		if (result == 0){ // If the port is in use
			close(sock);
			throw SpykeeServerException(SpykeeServerException::badPortNumber, "The port number entered is in used."
				, SpykeeServerException::fatal, this->name, ip, this->controllerPort);
		}
		close(sock);
	}
	this->controllerPort = port;
	SpykeeServerRobot::portUsed_.push_back(port);
}

/*
 * Main method
 */

void SpykeeServerRobot::test2_(){
	std::cout << "Pointeur du this test2_ : " << this << std::endl;

	std::thread thread_ListenRobot(&SpykeeServerRobot::test_, this);
	//thread_ListenRobot.detach();

	thread_ListenRobot.join();
}

void SpykeeServerRobot::test3_(){
	this->launch();
}

void SpykeeServerRobot::launch(){
	std::cout << "Le thread qui gère le robot " << this->name << std::endl;
	std::cout << "Pointeur this (launch) : " << this << std::endl;
	this->connectToTheRobot_();
	this->isRunning_ = true; // To launch all threat
	std::cout << "Is running MAIN : " << this->isRunning_ << std::endl;

	// Launch a thread which always listen robot
	//std::thread thread_ListenRobot(&SpykeeServerRobot::_listenRobot, this);

	//this->_setVideo(true);
	//std::cout << std::endl << "Activate" << std::endl;
	//this->_activate(); Cela fait planter le serveur

	std::cout<< std::endl << "Charge stop" << std::endl;
	this->chargeStop_();
	// changement de la valeur du this
	std::cout << "Pointeur this2 (launch) : " << this << std::endl;
	this->forward_();
	//std::cout << std::endl << "Left" << std::endl;
	//this->left_();

	std::cout << "Pointeur this (launch) : " << this << std::endl;

	//this->test2_();
	std::thread thread_ListenRobot(&SpykeeServerRobot::test_, this);
	//thread_ListenRobot.detach();

	thread_ListenRobot.join();

	//close(this->_sockRobot); Segmentation fault
	std::cout << "Arrêt du serveur" << std::endl;
}

/*
 * Privates methods
 */

void SpykeeServerRobot::connectToTheRobot_(){
	std::cout << "Pointeur this (connectToTheRobot) : " << this << std::endl;
	this->sockRobot_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockRobot_ == -1){
		// FIXME gérer les erreurs
		std::cout << "ERREUR : Impossible de crée la socket" << std::endl;
	}
	std::cout << "Socket crée" << std::endl;

	struct sockaddr_in robot={0};
	robot.sin_addr.s_addr = inet_addr(this->ip.c_str());
	robot.sin_family = AF_INET;
	robot.sin_port = htons(this->robotPort);

	// Connect to the robot
	if (connect(this->sockRobot_, (struct sockaddr *)&robot, sizeof(robot)) < 0){
		// FIXME gérer les erreurs
		std::cout << "ERREUR : impossible de se connecter au robot" << std::endl;
	}
	std::cout << "Connexion avec le robot établie" << std::endl;

	int messageLength = 1 + this->username.length() + 1 + this->password.length();
	char *message = new char[messageLength];
	sprintf(message, "%c%s%c%s", (unsigned) this->username.length(), this->username.c_str(), (unsigned) this->password.length(), this->password.c_str());

	this->sendPacketToRobot_(SpykeeServerRobot::AUTH_REQUEST, message);

	delete [] message;
	this->getResponse_(); // Get firmwere version
	this->getResponse_(); // Get battery level

}

// TODO verifier que la taille des char* est ok pour gérer le '\0'

void SpykeeServerRobot::sendPacketToRobot_(SpykeeServerRobot::PacketType type, char* data){
	int length=strlen(data);
	int packetLength = 2 + 1 + 2 + length;
	char *packet = new char[packetLength];
	sprintf(packet, "%s%c%c%c", "PK", type, length/255, length%255);
	strcpy((char *)(packet+5), data);

	std::cout << "Paquet envoyé (type" << type << ") : \"" << packet << "\"" << std::endl;
	if (write(this->sockRobot_, (char*) packet, packetLength) < 0){
		// FIXME gérer les erreurs
		std::cout << "ERREUR : impossible d'envoyer le message" << std::endl;
	}
	delete [] packet;
}

void SpykeeServerRobot::getResponse_(){
	char *header = new char[SpykeeServerRobot::headerSize];
	std::cout << "Pointeur this : " << this << std::endl;
	std::cout << "Stream sock : " << this->sockRobot_ << std::endl;
	std::cout << "Header size : " << SpykeeServerRobot::headerSize << std::endl;
	std::cout << "AVANT" << std::endl; // TODO vérifier le this->_sockRobot
	if ( recv(this->sockRobot_, header, SpykeeServerRobot::headerSize, 0) < 0){
		delete [] header;
		// FIXME gérer les erreurs
		std::cout << "ERREUR : impossible de lire le header du paquet" << std::endl;
		return;
	}
	std::cout << "APRES" << std::endl;

	if (strlen(header) == 0){
		delete [] header;
		// FIXME gérer les erreurs
		std::cout << "ERREUR : connexion réinitialisée par le robot" << std::endl;
		return;
	}

	if (header[0] != 'P' || header[1] != 'K'){ // It's not the correct header
		delete [] header;
		// FIXME gérer les erreurs
		std::cout << "ERREUR : mauvais header reçus" << std::endl;
		return;
	}

	int type = header[2];
	int packetLength = header[3]*255 + header[4];
	delete [] header;
	char *message = new char[packetLength];
	if ( recv(this->sockRobot_, message, packetLength, 0) < 0 ){
		delete [] message;
		// FIXME gérer les erreurs
		std::cout << "ERREUR : impossible de lire le contenu du paquet" << std::endl;
		return;
	}

	std::cout << "MESSAGE RECU : " << message << std::endl;

	switch (type){
		case SpykeeServerRobot::AUDIO:

		break;
		case SpykeeServerRobot::VIDEO:

		break;
		case SpykeeServerRobot::POWER:
			this->batteryLevel_ = message[0];
		break;
		case SpykeeServerRobot::AUTH_REPLY:

		break;
		case SpykeeServerRobot::STOP:

		break;
		case SpykeeServerRobot::WIRELESS_NETWORKS:

		break;
		case SpykeeServerRobot::CONFIG:

		break;
		case SpykeeServerRobot::LOG:

		break;

		default:
			// FIXME gérer les erreurs
			std::cout << "ERREUR : paquet de type inconnu reçus" << std::endl;
		break;
	}

	delete [] message;
}

/*
 * Actions methodes
 */
void SpykeeServerRobot::move_(int left, int right){
	if (left > 255) left = 255;
	if (left < 0) left = 0;
	if (right > 255) right = 255;
	if (right < 0) right = 0;
	char message[2]={0};
	sprintf(message, "%c%c", left, right);
	this->sendPacketToRobot_(SpykeeServerRobot::MOVE, message);
}

void SpykeeServerRobot::left_(){
	this->move_(128 + this->speed_, this->speed_);
}

void SpykeeServerRobot::right_(){
	this->move_(this->speed_, 128 + this->speed_);
}

void SpykeeServerRobot::forward_(){
	this->move_(this->speed_, this->speed_);
}

void SpykeeServerRobot::back_(){
	this->move_(128 + this->speed_, 128 + this->speed_);
}

void SpykeeServerRobot::stop_(){
	this->move_(0, 0);
}

void SpykeeServerRobot::activate_(){
	char message[1]= {0};
	sprintf(message, "%c", SpykeeServerRobot::ACTIVATE);
	this->sendPacketToRobot_(SpykeeServerRobot::ENGINE, message);
}

void SpykeeServerRobot::chargeStop_(){
	char message[1]= {0};
	sprintf(message, "%c", SpykeeServerRobot::CHARGE_STOP);
	this->sendPacketToRobot_(SpykeeServerRobot::ENGINE, message);
	std::cout << "THIIIIIIISSS" << this << std::endl;
}

// TODO gérer les docks

int SpykeeServerRobot::getBatteryLevel_(){
	if (this->batteryLevel_ == -1) // Battery level unknown
		return this->refreshBatteryLevel_();
	else
		return this->batteryLevel_;
}

int SpykeeServerRobot::refreshBatteryLevel_(){
	this->sendPacketToRobot_(SpykeeServerRobot::POWER); // FIXME vérifier que tout s'est bien passé
	return this->batteryLevel_;
}

void SpykeeServerRobot::setVideo_(bool b){
	char message[2]={0};
	sprintf(message, "%c%c", 1, b);
	this->sendPacketToRobot_(SpykeeServerRobot::STREAM_CTRL, message);
}

void SpykeeServerRobot::setSpeed_(int speed){
	if (speed < 28) speed=28;
	if (speed > 128) speed=128;
	this->speed_ = speed;
}

void SpykeeServerRobot::test_(){
	this->listenRobot_();
}

void SpykeeServerRobot::listenRobot_(){
	std::cout << "Pointeur de this LISTEN :" << this << std::endl;
	std::cout << "Is running (listen) : " << this->isRunning_ << std::endl;
	//std::cout << "Pointeur o :" << o << std::endl;

	while (this->isRunning_){
		//std::cout << "Is running : " << _isRunning << std::endl;
		std::cout << " J'écoute le robot" << std::endl;
		this->getResponse_();
	}
	std::cout << "STOP _listenRobot" << std::endl;
}

void SpykeeServerRobot::listenClient_(){
	int masterSock=0;
	int currentClientSock=0;
	std::vector<int> clientsSock;
	std::vector<std::thread> threads;
	struct sockaddr_in server, client;

	masterSock = socket(AF_INET, SOCK_STREAM, 0);
	if (masterSock == -1){
		// FIXME gérer les erreurs
		std::cout << "ERREUR : Impossible de crée la socket" << std::endl;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(this->controllerPort);

	if ( bind(masterSock,(struct sockaddr *)&server , sizeof(server)) < 0 ){
		// FIXME gérer les erreurs
		std::cout << "ERREUR : Impossible de bind le socket" << std::endl;
	}

	listen(masterSock, 5); // Only the PHP server can be connected but with multiples connexions

	struct sockaddr_storage peer; // IP/Port of the peer is store
	socklen_t lengthSockaddr_storage = sizeof(peer);
	char peerIp[INET_ADDRSTRLEN]; // To store IP address in human representation
	int lengthSockaddr_in = sizeof(sockaddr_in);
	while ( (currentClientSock = accept(masterSock, (struct sockaddr *)&client, (socklen_t*)&lengthSockaddr_in)) && this->isRunning_ ){
		getpeername(currentClientSock, (struct sockaddr*)&peer, &lengthSockaddr_storage);
		struct sockaddr_in *peer = (struct sockaddr_in *)&peer;
		inet_ntop(AF_INET, &peer->sin_addr, peerIp, sizeof(peerIp));

		bool founed=false;
		for(auto& ip : this->ipAllowed_){
			if (strcmp(peerIp, ip.c_str())){
				founed = true;
				break;
			}
		}
		if (!founed){
			// FIXME gérer les erreurs
			std::cout << "ERREUR : La connexion provient d'une connexion non authorisée" << std::endl;
		}
		else{
			clientsSock.push_back(currentClientSock);
			threads.push_back(std::thread(&SpykeeServerRobot::processClient_, this, std::ref(clientsSock.back())));
		}
	}
	if (currentClientSock < 0){
		// FIXME gérer les erreurs
		std::cout << "ERREUR : Accept n'a pas fonctionné" << std::endl;
	}

	for(auto& t : threads){
		t.join();
	}
}

void SpykeeServerRobot::processClient_(int sock){

}

SpykeeServerRobot::~SpykeeServerRobot() {
	// TODO Auto-generated destructor stub
}

