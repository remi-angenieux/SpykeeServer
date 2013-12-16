/*
 * SpykeeServer.h
 *
 *  Created on: 3 déc. 2013
 *      Author: remi
 */

#ifndef SPYKEESERVER_H_
#define SPYKEESERVER_H_

#include<stdio.h>
#include<string>    //strlen
#include<iostream>
#include<string.h>
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>

class SpykeeServer {

public:
	SpykeeServer();
	virtual ~SpykeeServer();
	int main(int argc, char *argv[]);

	void launcherServer();
	static void *connection_handler(void *socket_desc);

private:
	int socketMaster;
	int socketClient;
	struct sockaddr_in server;
	struct sockaddr_in client;
	pthread_t snifferThread;
};

#endif /* SPYKEESERVER_H_ */
