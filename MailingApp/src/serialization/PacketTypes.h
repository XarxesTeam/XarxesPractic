#pragma once

#include <cstdint>

enum class PacketType : int8_t
{
	LoginRequest,
	QueryAllMessagesRequest,
	QueryAllChatMessagesRequest,
	ClearAllMessagesRequest,
	ClearOneMessage,
	QueryAllMessagesResponse,
	QueryAllChatMessagesResponse,
	SendMessageRequest
};
