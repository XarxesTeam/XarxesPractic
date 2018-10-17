#pragma once

#include <string>

struct Message
{
	std::string senderUsername;
	std::string receiverUsername;
	std::string subject;
	std::string body;
	std::string time;
	int id;
};
