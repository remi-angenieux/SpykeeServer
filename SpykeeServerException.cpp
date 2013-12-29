/*
 * SpykeeServerException.cpp
 *
 *  Created on: 16 déc. 2013
 *      Author: remi
 */

#include "headers/SpykeeServerException.h"

SpykeeServerException::SpykeeServerException(ErNumber erNumber, std::string erDescription, ErLevel erLevel,
		std::string robotName, std::string robotIp, int robotPort) {
	this->erNumber = erNumber;
	this->erDescription = erDescription;
	this->erLevel = erLevel;
	this->robotName = robotName;
	this->robotIp = robotIp;
	this->robotPort = robotPort;

}

/**
 * Getters
 */

const std::string& SpykeeServerException::getErDescription() const {
	return this->erDescription;
}

SpykeeServerException::ErLevel SpykeeServerException::getErLevel() const {
	return this->erLevel;
}

SpykeeServerException::ErNumber SpykeeServerException::getErNumber() const {
	return this->erNumber;
}

const std::string& SpykeeServerException::getRobotIp() const {
	return this->robotIp;
}

const std::string& SpykeeServerException::getRobotName() const {
	return this->robotName;
}

int SpykeeServerException::getRobotPort() const {
	return this->robotPort;
}

SpykeeServerException::~SpykeeServerException() throw(){

}
