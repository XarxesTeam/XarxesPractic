#pragma once

#include "IDatabaseGateway.h"
#include <vector>

class SimulatedDatabaseGateway :
	public IDatabaseGateway
{
public:

	// Constructor and destructor

	SimulatedDatabaseGateway();

	~SimulatedDatabaseGateway();


	// Virtual methods from IDatabaseGateway

	void insertMessage(const Message &message) override;

	void clearMessages(const std::string & username) override;
	void clearMessage(int index, const std::string & username) override;

	std::vector<Message> getAllMessagesReceivedByUser(const std::string &username) override;
	std::vector<Message> getAllMessagesReceivedByChat() override;

private:

	std::vector<Message> allMessages;
};
