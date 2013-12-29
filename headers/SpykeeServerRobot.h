/*
 * SpykeeServerRobot.h
 *
 *  Created on: 16 déc. 2013
 *      Author: remi
 */

#ifndef SPYKEESERVERROBOT_H_
#define SPYKEESERVERROBOT_H_

#include <string>
#include <arpa/inet.h> // To test valid IP address
#include <vector> // To store used ID
#include <algorithm> // To search in the vector
#include "SpykeeServerException.h"


class SpykeeServerRobot {
public:
	SpykeeServerRobot(int id, std::string name, std::string ip, int port, std::string username, std::string password);
	virtual ~SpykeeServerRobot();
	void launch();

protected: //TODO Verifier si faut pas passer en private
	int id;
	std::string name;
	std::string ip;
	int port;
	std::string username;
	std::string password;

private:
	void _setRobotId(const int &id);
	void _setRobotIp(const std::string &ip);
	void _setRobotPort(const int &port);
	static std::vector<int> _idUsed;
};

#endif /* SPYKEESERVERROBOT_H_ */
