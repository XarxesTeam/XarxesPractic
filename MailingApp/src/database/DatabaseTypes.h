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

	bool IsMessageOlder(Message other) // returns true if 'other' is a previous message than this
	{
		std::string strDate;
		strDate = date.at(6);	// Date year
		strDate += date.at(7);	// Date year
		strDate += date.at(8);	// Date year
		strDate += date.at(9);	// Date year
		strDate += date.at(3);	// Date month
		strDate += date.at(4);	// Date month
		strDate += date.at(0);	// Date day
		strDate += date.at(1);	// Date day

		// Other
		std::string otherStrDate;
		otherStrDate = other.date.at(6);	// Date year
		otherStrDate += other.date.at(7);	// Date year
		otherStrDate += other.date.at(8);	// Date year
		otherStrDate += other.date.at(9);	// Date year
		otherStrDate += other.date.at(3);	// Date month
		otherStrDate += other.date.at(4);	// Date month
		otherStrDate += other.date.at(0);	// Date day
		otherStrDate += other.date.at(1);	// Date day


		if (strDate != otherStrDate)
			return std::stoi(strDate) > std::stoi(otherStrDate);
		else
		{
			std::string strTime;
			strTime = time.at(0);	// Time hour
			strTime += time.at(1);	// Time hour
			strTime += time.at(3);	// Time min
			strTime += time.at(4);	// Time min
			strTime += time.at(6);	// Time sec
			strTime += time.at(7);	// Time sec

			// Other
			std::string otherStrTime;
			otherStrTime = other.time.at(0);	// Time hour
			otherStrTime += other.time.at(1);	// Time hour
			otherStrTime += other.time.at(3);	// Time min
			otherStrTime += other.time.at(4);	// Time min
			otherStrTime += other.time.at(6);	// Time sec
			otherStrTime += other.time.at(7);	// Time sec

			return std::stoi(strTime) > std::stoi(otherStrTime);
		}
	}
};
