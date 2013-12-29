/*
 * SpykeeServerRobot.cpp
 *
 *  Created on: 16 déc. 2013
 *      Author: remi
 */

#include "headers/SpykeeServerRobot.h"
#include <iostream> //Fixme Juste pour les test

std::vector<int> SpykeeServerRobot::_idUsed;

SpykeeServerRobot::SpykeeServerRobot(int id, std::string name, std::string ip, int port, std::string username, std::string password) {
	this->_setRobotId(id);
	this->name = name;
	this->_setRobotIp(ip);
	this->_setRobotPort(port);
	this->username = username;
	this->password = password;

}
// Todo Revoir les prérequis du l'exception. Car là on a besoin de variables non définies
void SpykeeServerRobot::_setRobotId(const int &id) {
	if (std::find(SpykeeServerRobot::_idUsed.begin(), SpykeeServerRobot::_idUsed.end(), id)!=SpykeeServerRobot::_idUsed.end()){
		throw SpykeeServerException(SpykeeServerException::idAlreadyUsed, "The robot id is already used"
				, SpykeeServerException::fatal, this->name, ip, this->port);
	}
	this->id = id;
	SpykeeServerRobot::_idUsed.push_back(id);
}

void SpykeeServerRobot::_setRobotIp(const std::string &ip) {
	struct sockaddr_in sa;
	if (inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 1){ // If isn't a valid IP address
		throw SpykeeServerException(SpykeeServerException::badIpAddress, "The ip address entered of the robot isn't"
				"a valid IP address", SpykeeServerException::fatal, this->name, ip, this->port);
	}
	this->ip = ip;
}

void SpykeeServerRobot::_setRobotPort(const int &port) {
	if ( !(port > 1023 && port <= 49151)){ // Invalid port number
		throw SpykeeServerException(SpykeeServerException::badPortNumber, "The port number entered of the robot isn't"
				"valid", SpykeeServerException::fatal, this->name, ip, this->port);
	}
	this->port = port;
}

void SpykeeServerRobot::launch(){
	std::cout << "Le thread qui gère le robot " << this->name << std::endl;
}


SpykeeServerRobot::~SpykeeServerRobot() {
	// TODO Auto-generated destructor stub
}

