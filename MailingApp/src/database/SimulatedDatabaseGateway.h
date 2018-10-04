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

	void clearMessages() override;
	void clearMessage(int index) override;

	std::vector<Message> getAllMessagesReceivedByUser(const std::string &username) override;

private:

	std::vector<Message> allMessages;
};
