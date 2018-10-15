#include "SimulatedDatabaseGateway.h"



SimulatedDatabaseGateway::SimulatedDatabaseGateway()
{
}


SimulatedDatabaseGateway::~SimulatedDatabaseGateway()
{
}

void SimulatedDatabaseGateway::insertMessage(const Message & message)
{
	allMessages.push_back(message);
}

void SimulatedDatabaseGateway::clearMessages(const std::string & username)
{
	allMessages.clear();
}

void SimulatedDatabaseGateway::clearMessage(int index, const std::string & username)
{
	std::vector<Message>::const_iterator it = allMessages.begin();
	while (it->id != index && it != allMessages.end())
		it++;

	allMessages.erase(it);
}

std::vector<Message> SimulatedDatabaseGateway::getAllMessagesReceivedByUser(const std::string & username)
{
	std::vector<Message> messages;
	for (const auto & message : allMessages)
	{
		if (message.receiverUsername == username)
		{
			messages.push_back(message);
		}
	}
	return messages;
}
