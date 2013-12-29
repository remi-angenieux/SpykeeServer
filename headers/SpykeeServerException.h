/*
 * SpykeeServerException.h
 *
 *  Created on: 16 déc. 2013
 *      Author: remi
 */

#ifndef SPYKEESERVEREXCEPTION_H_
#define SPYKEESERVEREXCEPTION_H_

#include<string>
#include<exception>

class SpykeeServerException: public std::exception {
public:
	enum ErLevel{ information=0, missFunction=1, unsed=2, fatal=3 }; // TODO verifier l'utilité d'un erLevel suplémentaire
	enum ErNumber{ idAlreadyUsed=0, badIpAddress=1, badPortNumber=2 };
	SpykeeServerException(ErNumber erNumber, std::string erDescription, ErLevel erLevel,
			std::string robotName, std::string robotIp, int robotPort);
	~SpykeeServerException() throw();
	const std::string& getErDescription() const;
	ErLevel getErLevel() const;
	ErNumber getErNumber() const;
	const std::string& getRobotIp() const;
	const std::string& getRobotName() const;
	int getRobotPort() const;

private:
	ErNumber erNumber;
	std::string erDescription;
	ErLevel erLevel;
	std::string robotName;
	std::string robotIp;
	int robotPort;
};

#endif /* SPYKEESERVEREXCEPTION_H_ */
