#pragma once

#include <string>

struct Message
{
	std::string senderUsername;
	std::string receiverUsername;
	std::string subject;
	std::string body;
	std::string time;
	std::string date;
	int id;

	int CalcTimeVal()
	{
		char sec[2];
		sec[0] = time.at(7);
		sec[1] = time.at(6);

		return atoi(sec);
	}
};
