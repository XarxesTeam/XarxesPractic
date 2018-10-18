#include "MySqlDatabaseGateway.h"
#include "DBConnection.h"
#include "../imgui/imgui.h"
#include <cstdarg>

#define _CRT_SECURE_NO_WARNINGS

// You can use this function to create the SQL statements easily, works like the printf function
std::string stringFormat(const char *fmt, ...)
{
	// Find out the final string length
	va_list ap;
	va_start(ap, fmt);
	int size = vsnprintf(nullptr, 0, fmt, ap);
	va_end(ap);

	// Format the actual resulting string
	std::string resultString;
	resultString.resize(size + 1, '\0');
	va_start(ap, fmt);
	vsnprintf(&resultString[0], resultString.size(), fmt, ap);
	va_end(ap);

	return resultString;
}


MySqlDatabaseGateway::MySqlDatabaseGateway()
{
}


MySqlDatabaseGateway::~MySqlDatabaseGateway()
{
}

void MySqlDatabaseGateway::insertMessage(const Message & message)
{
	DBConnection db(bufMySqlHost, bufMySqlPort, bufMySqlDatabase, bufMySqlUsername, bufMySqlPassword);

	if (db.isConnected())
	{
		DBResultSet res;

		char buffer[64];
		_itoa_s(message.id, buffer, 10);


		std::string sqlStatement;
		// TODO: Create the SQL statement to insert the passed message into the DB (INSERT)
		sqlStatement = "INSERT INTO MessengerServerDatabase (sender, receiver, subject, body, time, date, id) VALUES ('" + message.senderUsername + "', '"
			+ message.receiverUsername + "', '" + message.subject + "', '" + message.body + "', '" + message.time + "', '" + message.date + "', '" + buffer + ");";

		// insert some messages
		db.sql(sqlStatement.c_str());
	}
}

void MySqlDatabaseGateway::clearMessages(const std::string & username)
{
	DBConnection db(bufMySqlHost, bufMySqlPort, bufMySqlDatabase, bufMySqlUsername, bufMySqlPassword);

	if (db.isConnected())
	{
		DBResultSet res;

		//sqlStatement to clear all messages
		std::string sqlStatement;
		sqlStatement = "DELETE FROM MessengerServerDatabase WHERE receiver = '" + username +"'";

		// insert some messages
		db.sql(sqlStatement.c_str());
	}
}

void MySqlDatabaseGateway::clearMessage(int id, const std::string & username)
{
	DBConnection db(bufMySqlHost, bufMySqlPort, bufMySqlDatabase, bufMySqlUsername, bufMySqlPassword);

	if (db.isConnected())
	{
		DBResultSet res;

		char buffer[64];
		_itoa_s(id, buffer, 10);

		//sqlStatement to clear an especific message
		std::string sqlStatement;
		sqlStatement = "DELETE FROM MessengerServerDatabase WHERE receiver = '" + username + "' AND id = '" + buffer + "'";
		
		// insert some messages
		db.sql(sqlStatement.c_str());
	}
}

std::vector<Message> MySqlDatabaseGateway::getAllMessagesReceivedByUser(const std::string & username)
{
	std::vector<Message> messages;

	DBConnection db(bufMySqlHost, bufMySqlPort, bufMySqlDatabase, bufMySqlUsername, bufMySqlPassword);

	if (db.isConnected())
	{
		std::string sqlStatement;
		// TODO: Create the SQL statement to query all messages from the given user (SELECT)
		sqlStatement = "SELECT sender, receiver, subject, body, time, date, id FROM MessengerServerDatabase WHERE receiver = '" + username + "'";

		// consult all messages
		DBResultSet res = db.sql(sqlStatement.c_str());

		// fill the array of messages
		for (auto & messageRow : res.rows)
		{
			Message message;
			message.senderUsername = messageRow.columns[0];
			message.receiverUsername = messageRow.columns[1];
			message.subject = messageRow.columns[2];
			message.body = messageRow.columns[3];
			message.time = messageRow.columns[4];
			message.date = messageRow.columns[5];
			message.id = atoi((const char*)messageRow.columns[6].c_str());
			messages.push_back(message);
		}
	}
	
	return messages;
}

std::vector<Message> MySqlDatabaseGateway::getAllMessagesReceivedByChat()
{
	return getAllMessagesReceivedByUser("all");
}

void MySqlDatabaseGateway::updateGUI()
{
	ImGui::Separator();

	ImGui::Text("MySQL Server info");
	ImGui::InputText("Host", bufMySqlHost, sizeof(bufMySqlHost));
	ImGui::InputText("Port", bufMySqlPort, sizeof(bufMySqlPort));
	ImGui::InputText("Database", bufMySqlDatabase, sizeof(bufMySqlDatabase));
	ImGui::InputText("Username", bufMySqlUsername, sizeof(bufMySqlUsername));
	ImGui::InputText("Password", bufMySqlPassword, sizeof(bufMySqlUsername), ImGuiInputTextFlags_Password);
}
