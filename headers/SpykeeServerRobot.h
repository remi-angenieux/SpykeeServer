/*
 * SpykeeServerRobot.h
 *
 *  Created on: 16 déc. 2013
 *      Author: remi
 */

#ifndef SPYKEESERVERROBOT_H_
#define SPYKEESERVERROBOT_H_

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <string>
#include <string.h>
//#include <sstream> // To concatenate string
#include <arpa/inet.h> // To test valid IP address
#include <vector> // To store used ID
#include <algorithm> // To search in the vector
#include <sys/socket.h> // To use socket API
#include <unistd.h> // To close sockets
#include <stdlib.h> // Convert a char* to int
#include <thread> // To launch others thread
#include <mutex> // To create thread conditions
#include <atomic> // To create thread conditions
#include "SpykeeServerException.h"


class SpykeeServerRobot {
public:
	SpykeeServerRobot(int id, std::string name, std::string ip, int port, std::string username, std::string password, int defaultSpeed, std::vector<std::string> ipAllowed);
	/*SpykeeServerRobot(SpykeeServerRobot const&) = delete;
	SpykeeServerRobot& operator =(SpykeeServerRobot const&) = delete;*/
	virtual ~SpykeeServerRobot();
	void launch();
	void test3_();

protected: //TODO Verifier si faut pas passer en private
	int id;
	std::string name;
	std::string ip;
	int controllerPort;
	std::string username;
	std::string password;
	const int robotPort=9000;
	static const int headerSize = 5;
	const int packetMaxLength=1024;

private:
	void setRobotId_(const int &id);
	void setRobotIp_(const std::string &ip);
	void setControllerPort_(const int &port);
	static std::vector<int> idUsed_;
	static std::vector<int> portUsed_;
	void connectToTheRobot_();
	int sockRobot_;
	struct sockaddr_in server;
	enum PacketType {AUDIO=1, VIDEO=2, POWER=3, MOVE=5, FILE=6, PLAY=7, STOP=8, AUTH_REQUEST=10,
		AUTH_REPLY=11, CONFIG=13, WIRELESS_NETWORKS=14, STREAM_CTRL=15, ENGINE=16, LOG=17};
	enum MessageType {ACTIVATE=1, CHARGE_STOP=5, BASE_FIND=6, BASE_FIND_CANCEL=7};
	void sendPacketToRobot_(PacketType type, char* data="");
	void getResponse_();
	int batteryLevel_;
	int speed_;
	bool isRunning_;
	//std::atomic<bool> _isRunning;
	std::vector<std::string> ipAllowed_;
	void move_(int left, int right);
	void left_();
	void right_();
	void forward_();
	void back_();
	void stop_();
	void activate_();
	void chargeStop_();
	int getBatteryLevel_();
	int refreshBatteryLevel_();
	void setVideo_(bool b);
	void setSpeed_(int speed);
	void listenRobot_();
	void listenClient_();
	void processClient_(int sock);
	SpykeeServerRobot& getThis_(){
		return *this;
	}
	void test_();
	void test2_();
};

#endif /* SPYKEESERVERROBOT_H_ */
