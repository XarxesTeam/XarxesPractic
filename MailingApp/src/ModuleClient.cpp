#include <time.h>
#include "ModuleClient.h"
#include "Log.h"
#include "imgui/imgui.h"
#include "serialization/PacketTypes.h"
#include "Console.h"
#include "Application.h"
#include "ModuleServer.h"

#define HEADER_SIZE sizeof(uint32_t)
#define RECV_CHUNK_SIZE 4096

ModuleClient::ModuleClient()
{
	chat_console = new Console(this);
}

ModuleClient::~ModuleClient()
{
	delete chat_console;
}

bool ModuleClient::update()
{
	updateGUI();

	switch (state)
	{
	case ModuleClient::ClientState::Connecting:
		connectToServer();
		break;
	case ModuleClient::ClientState::Connected:
		handleIncomingData();
		updateMessenger();
		handleOutgoingData();
		break;
	case ModuleClient::ClientState::Disconnecting:
		disconnectFromServer();
		break;
	default:
		break;
	}

	return true;
}

bool ModuleClient::cleanUp()
{
	disconnectFromServer();
	return true;
}

void ModuleClient::_SendGlobalMessage(const char * body)
{
	int size = strlen(body);
	strcpy_s(messageBuf, size + 1, body);
	char rec[10] = "all";
	size = strlen(rec);
	strcpy_s(receiverBuf, size + 1, rec);
	char sub[30] = "global_message";
	size = strlen(sub);
	strcpy_s(subjectBuf, size + 1, sub);

	messengerState = MessengerState::SendingGlobalMessage;
}

void ModuleClient::updateMessenger()
{
	switch (messengerState)
	{
	case ModuleClient::MessengerState::SendingLogin:
		sendPacketLogin(senderBuf);
		break;
	case ModuleClient::MessengerState::RequestingMessages:
		sendPacketQueryMessages();
		break;
	case ModuleClient::MessengerState::RequestingMessagesClear:
		sendPacketClearMessages();
		break;
	case ModuleClient::MessengerState::RequestingClearOneMessage:
		sendPacketClearMessages(idMessageToDelete);
		break;
	case ModuleClient::MessengerState::ReceivingMessages:
		// Idle, do nothing
		break;
	case ModuleClient::MessengerState::ReceivingChatMessages:
		// Idle, do nothing
		break;
	case ModuleClient::MessengerState::ShowingMessages:
		// Idle, do nothing
		break;
	case ModuleClient::MessengerState::ComposingMessage:
		// Idle, do nothing
		break;
	case ModuleClient::MessengerState::SendingMessage:
		sendPacketSendMessage(receiverBuf, subjectBuf, messageBuf);
		break;
	case ModuleClient::MessengerState::SendingGlobalMessage:
		sendPacketSendMessage(receiverBuf, subjectBuf, messageBuf);
		break;
	default:
		break;
	}
}

void ModuleClient::onPacketReceived(const InputMemoryStream & stream)
{
	PacketType packetType;
	stream.Read(packetType);

	LOG("onPacketReceived() - packetType: %d", (int)packetType);

	switch (packetType)
	{
	case PacketType::QueryAllChatMessagesResponse:
		onPacketReceivedQueryAllChatMessagesResponse(stream);
		break;
	case PacketType::QueryAllMessagesResponse:
		onPacketReceivedQueryAllMessagesResponse(stream);
		break;
	case PacketType::ClearChatResponse:
		onPacketReceivedClearChatMessagesResponse(stream);
		break;
	default:
		LOG("Unknown packet type received");
		break;
	}
}

void ModuleClient::onPacketReceivedQueryAllMessagesResponse(const InputMemoryStream & stream)
{
	messages.clear();

	uint32_t messageCount;
	// TODO: Deserialize the number of messages
	stream.Read(messageCount);

	// TODO: Deserialize messages one by one and push_back them into the messages vector
	// NOTE: The messages vector is an attribute of this class
	for (int i = 0; i < messageCount; i++)
	{
		Message new_message;
		stream.Read(new_message.receiverUsername);
		stream.Read(new_message.senderUsername);
		stream.Read(new_message.subject);
		stream.Read(new_message.body);
		stream.Read(new_message.time);
		stream.Read(new_message.date);
		stream.Read(new_message.id);

		messages.push_back(new_message);
	}

	

	messengerState = MessengerState::ShowingMessages;
}

void ModuleClient::onPacketReceivedQueryAllChatMessagesResponse(const InputMemoryStream & stream)
{
	uint32_t messageCount;
	// TODO: Deserialize the number of messages
	stream.Read(messageCount);

	// TODO: Deserialize messages one by one and push_back them into the messages vector
	// NOTE: The messages vector is an attribute of this class
	chat_console->ClearLog();

	for (int i = 0; i < messageCount; i++)
	{
		Message new_message;
		stream.Read(new_message.receiverUsername);
		stream.Read(new_message.senderUsername);
		stream.Read(new_message.subject);
		stream.Read(new_message.body);
		stream.Read(new_message.time);
		stream.Read(new_message.date);
		stream.Read(new_message.id);

		std::string completeMsg = new_message.time.c_str();
		completeMsg += new_message.senderUsername.c_str();
		completeMsg += ": ";
		completeMsg += new_message.body.c_str();
		chat_console->AddLog(completeMsg.c_str());
	}

	messengerState = MessengerState::ShowingMessages;
}

void ModuleClient::onPacketReceivedClearChatMessagesResponse(const InputMemoryStream & stream)
{
	chat_console->ClearLog();
}

void ModuleClient::sendPacketLogin(const char * username)
{
	OutputMemoryStream stream;

	stream.Write(PacketType::LoginRequest);
	stream.Write(std::string(username));

	sendPacket(stream);

	messengerState = MessengerState::RequestingMessages;
}

void ModuleClient::sendPacketQueryMessages()
{
	OutputMemoryStream stream;

	// TODO: Serialize message (only the packet type)
	stream.Write(PacketType::QueryAllMessagesRequest);
	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	messengerState = MessengerState::ReceivingMessages;
}

void ModuleClient::sendPacketQueryChatMessages()
{
	OutputMemoryStream stream;

	// TODO: Serialize message (only the packet type)
	stream.Write(PacketType::QueryAllChatMessagesRequest);
	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	messengerState = MessengerState::ReceivingChatMessages;
}

void ModuleClient::sendPacketClearMessages(int id)
{
	OutputMemoryStream stream;
	if (id == -1)
	{
		// TODO: Serialize message (only the packet type)
		stream.Write(PacketType::ClearAllMessagesRequest);
		// TODO: Use sendPacket() to send the packet
	}
	else
	{
		stream.Write(PacketType::ClearOneMessage);
		
		stream.Write(idMessageToDelete);
	}

	stream.Write(std::string(userMessageToDelete));

	sendPacket(stream);

	messengerState = MessengerState::RequestingMessages;
}

void ModuleClient::sendPacketSendMessage(const char * receiver, const char * subject, const char *message)
{
	OutputMemoryStream stream;

	stream.Write(PacketType::SendMessageRequest);
	// TODO: Serialize message (packet type and all fields in the message)
	// NOTE: remember that senderBuf contains the current client (i.e. the sender of the message)
	stream.Write(std::string(receiver));
	stream.Write(std::string(senderBuf));
	stream.Write(std::string(subject));
	stream.Write(std::string(message));

	time_t theTime = time(NULL);
	struct tm *localTime = localtime(&theTime);

	//int day = localTime->tm_mday;
	//int month = localTime->tm_mon + 1; // Month is 0 – 11, add 1 to get a jan-dec 1-12 concept
	//int year = localTime->tm_year + 1900; // Year is # years since 1900
	//int hour = localTime->tm_hour;
	//int min = localTime->tm_min;
	//int sec = localTime->tm_sec;
	//
	//if (min < 10)
	//	timeDate = (std::to_string(hour) + ":0" + std::to_string(min) + ":" + std::to_string(sec) + "-" + std::to_string(day) + "-" + std::to_string(month) + "-" + std::to_string(year));
	//else
	//	timeDate = (std::to_string(hour) + ":" + std::to_string(min) + ":" + std::to_string(sec) + "-" + std::to_string(day) + "-" + std::to_string(month) + "-" + std::to_string(year));

	std::string timeLocal;
	int hour = localTime->tm_hour;
	int min = localTime->tm_min;
	int sec = localTime->tm_sec;
	// Hours
	if (hour < 10)
		timeLocal = ("0" + std::to_string(hour));
	else
		timeLocal = std::to_string(hour);
	// Minutes
	if (min < 10)
		timeLocal += (":0" + std::to_string(min));
	else
		timeLocal += (":" + std::to_string(min));
	// Seconds
	if (sec < 10)
		timeLocal += (":0" + std::to_string(sec) + " ");
	else
		timeLocal += (":" + std::to_string(sec) + " ");

	stream.Write(std::string(timeLocal));
	stream.Write(std::string(timeLocal));

	int rand_val = rand();
	
	stream.Write(rand_val);

	sendPacket(stream);

	messengerState = MessengerState::RequestingMessages;
}

// This function is done for you: Takes the stream and schedules its internal buffer to be sent
void ModuleClient::sendPacket(const OutputMemoryStream & stream)
{
	// Copy the packet into the send buffer
	size_t oldSize = sendBuffer.size();
	sendBuffer.resize(oldSize + HEADER_SIZE + stream.GetSize());
	uint32_t &packetSize = *(uint32_t*)&sendBuffer[oldSize];
	packetSize = HEADER_SIZE + stream.GetSize(); // header size + payload size
												 //std::copy(stream.GetBufferPtr(), stream.GetBufferPtr() + stream.GetSize(), &sendBuffer[oldSize] + HEADER_SIZE);
	memcpy(&sendBuffer[oldSize] + HEADER_SIZE, stream.GetBufferPtr(), stream.GetSize());
}


// GUI: Modify this to add extra features...

void ModuleClient::updateGUI()
{
	time_req = clock() - time_req;
	tot_time += time_req;

	ImGui::Begin("Client Window");

	if (state == ClientState::Disconnected)
	{
		if (ImGui::CollapsingHeader("Server data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// IP address
			static char ipBuffer[64] = "127.0.0.1";
			ImGui::InputText("IP", ipBuffer, sizeof(ipBuffer));

			// Port
			static int port = 8000;
			ImGui::InputInt("Port", &port);

			// Connect button
			ImGui::InputText("Login name", senderBuf, sizeof(senderBuf));

			if (ImGui::Button("Connect"))
			{
				if (state == ClientState::Disconnected)
				{
					state = ClientState::Connecting;
				}
			}
		}
	}
	else if (state == ClientState::Connected)
	{
		// Disconnect button
		if (ImGui::Button("Disconnect"))
		{
			if (state == ClientState::Connected)
			{
				state = ClientState::Disconnecting;
				
				chat_console->ClearLog();
			}
		}

		if (messengerState == MessengerState::ComposingMessage)
		{
			ImGui::InputText("Receiver", receiverBuf, sizeof(receiverBuf));
			ImGui::InputText("Subject", subjectBuf, sizeof(subjectBuf));
			ImGui::InputTextMultiline("Message", messageBuf, sizeof(messageBuf));
			if (ImGui::Button("Send"))
			{
				messengerState = MessengerState::SendingMessage;
			}
			if (ImGui::Button("Discard"))
			{
				messengerState = MessengerState::ShowingMessages;
			}
		}
		else if (messengerState == MessengerState::ShowingMessages || messengerState == MessengerState::SendingGlobalMessage || messengerState == MessengerState::ReceivingChatMessages)
		{
			if (ImGui::Button("Compose message"))
			{
				messengerState = MessengerState::ComposingMessage;
			}

			if (ImGui::Button("Refresh inbox"))
			{
				messengerState = MessengerState::RequestingMessages;
			}

			if (ImGui::Button("Clear inbox"))
			{
				messengerState = MessengerState::RequestingMessagesClear;
				userMessageToDelete = senderBuf;
			}

			ImGui::Text("Inbox:");

			if (messages.empty()) {
				ImGui::Text(" - Your inbox is empty.");
			}

			int i = 0;
			for (auto &message : messages)
			{
				ImGui::PushID(i++);
				if (ImGui::TreeNode(&message, "%s - %s", message.senderUsername.c_str(), message.subject.c_str()))
				{
					ImGui::Text("%s", message.time.c_str());
					ImGui::TextWrapped("%s", message.body.c_str());
					if (ImGui::Button("Delete this message"))
					{
						messengerState = MessengerState::RequestingClearOneMessage;
						idMessageToDelete = message.id;
						userMessageToDelete = senderBuf;
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}

		if (tot_time > 1000)
		{
			//Update console
			//sendPacketQueryChatMessages();
			tot_time = 0;
		}
		
		//Draw chat console
		bool open = true;
		chat_console->_Draw("Chat", &open);
	}

	ImGui::End();

	time_req = clock();
}


// Low-level networking stuff...

void ModuleClient::connectToServer()
{
	// Create socket
	connSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connSocket == INVALID_SOCKET)
	{
		printWSErrorAndExit("socket()");
	}

	// Connect
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);
	int res = connect(connSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
	if (res == SOCKET_ERROR)
	{
		printWSError("connect()");
		LOG("Could not connect to the server %s:%d", serverIP, serverPort);
		state = ClientState::Disconnecting;
	}
	else
	{
		state = ClientState::Connected;
		LOG("Server connected to %s:%d", serverIP, serverPort);

		messengerState = MessengerState::SendingLogin;

		time_req = clock();
	}

	// Set non-blocking socket
	u_long nonBlocking = 1;
	res = ioctlsocket(connSocket, FIONBIO, &nonBlocking);
	if (res == SOCKET_ERROR) {
		printWSError("ioctlsocket() non-blocking");
		LOG("Could not set the socket in non-blocking mode.", serverIP, serverPort);
		state = ClientState::Disconnecting;
	}
}

void ModuleClient::disconnectFromServer()
{
	closesocket(connSocket);
	recvBuffer.clear();
	recvPacketHead = 0;
	recvByteHead = 0;
	sendBuffer.clear();
	sendHead = 0;
	state = ClientState::Disconnected;
}

void ModuleClient::handleIncomingData()
{
	if (recvBuffer.size() - recvByteHead < RECV_CHUNK_SIZE) {
		recvBuffer.resize(recvByteHead + RECV_CHUNK_SIZE);
	}

	int res = recv(connSocket, (char*)&recvBuffer[recvByteHead], RECV_CHUNK_SIZE, 0);
	if (res == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// Do nothing
		}
		else
		{
			printWSError("recv() - socket disconnected forcily");
			state = ClientState::Disconnecting;
		}
	}
	else
	{
		if (res == 0)
		{
			state = ClientState::Disconnecting;
			LOG("Disconnection from server");
			return;
		}

		recvByteHead += res;
		while (recvByteHead - recvPacketHead > HEADER_SIZE)
		{
			const size_t recvWindow = recvByteHead - recvPacketHead;
			const uint32_t packetSize = *(uint32_t*)&recvBuffer[recvPacketHead];
			if (recvWindow >= packetSize)
			{
				InputMemoryStream stream(packetSize - HEADER_SIZE);
				//std::copy(&recvBuffer[recvPacketHead + HEADER_SIZE], &recvBuffer[recvPacketHead + packetSize], (uint8_t*)stream.GetBufferPtr());
				memcpy(stream.GetBufferPtr(), &recvBuffer[recvPacketHead + HEADER_SIZE], packetSize - HEADER_SIZE);
				onPacketReceived(stream);
				recvPacketHead += packetSize;
			}
		}

		if (recvPacketHead >= recvByteHead)
		{
			recvPacketHead = 0;
			recvByteHead = 0;
		}
	}
}

void ModuleClient::handleOutgoingData()
{
	if (sendHead < sendBuffer.size())
	{
		int res = send(connSocket, (const char *)&sendBuffer[sendHead], (int)sendBuffer.size(), 0);
		if (res == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// Do nothing
			}
			else
			{
				printWSError("send()");
				state = ClientState::Disconnecting;
			}
		}
		else
		{
			sendHead += res;
		}

		if (sendHead >= sendBuffer.size())
		{
			sendHead = 0;
			sendBuffer.clear();
		}
	}
}