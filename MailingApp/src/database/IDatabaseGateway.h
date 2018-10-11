#pragma once

#include "DatabaseTypes.h"
#include <vector>

class IDatabaseGateway
{
public:

	// Constructor and destructor

	IDatabaseGateway() { }
	
	virtual ~IDatabaseGateway() { }


	// Virtual methods

	virtual void insertMessage(const Message &message) = 0;

	virtual void clearMessages(const std::string & username) = 0;
	
	virtual void clearMessage(int index, const std::string & username) {};
	
	virtual std::vector<Message> getAllMessagesReceivedByUser(const std::string &username) = 0;

	virtual std::vector<Message> getAllMessagesReceivedByChat() { return std::vector<Message>(); };

	virtual void updateGUI() { }
};

