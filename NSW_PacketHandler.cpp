#include "NSW_PacketHandler.h"

#include "Buffer.hpp"

void NSW_PacketHandler::PackRequestPacket(Buffer& SendBuffer, const uint32_t pkt_id)
{
	PACKET_HEADER pkt_header;

	pkt_header.content_length = 0;
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
}

void NSW_PacketHandler::PackChatPacket(Buffer& SendBuffer, const uint32_t pkt_id, const std::string chat_msg)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::CHAT pkt_data;
	strcpy(pkt_data.msg, chat_msg.c_str());

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::PackRoomListInfoPacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t seq_num, const std::vector<PACKET_DATA::SessionRoom>& RoomList)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::ROOM_LIST_INFO pkt_data;

	pkt_data.room_cnt = RoomList.size();
	for (auto& room : RoomList)
	{
		pkt_data.RoomList.push_back(room);
	}

	pkt_header.content_length = sizeof(PACKET_DATA::SessionRoom) * pkt_data.room_cnt;
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = seq_num;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data.room_cnt;
	for (auto& room : pkt_data.RoomList)
	{
		SendBuffer << room;
	}
}

void NSW_PacketHandler::PackRoomInfoPacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t room_index, const std::string room_name, const uint8_t participants)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::ROOM_INFO pkt_data;

	pkt_data.room.index = room_index;
	strcpy(pkt_data.room.name, room_name.c_str());
	pkt_data.room.participant_num = participants;

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::PackPlayerListInfoPacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t seq_num, const std::vector<PACKET_DATA::SessionPlayer>& PlayerList)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::PLAYER_LIST_INFO pkt_data;

	pkt_data.player_cnt = PlayerList.size();
	for (auto& player : PlayerList)
	{
		pkt_data.PlayerList.push_back(player);
	}

	pkt_header.content_length = sizeof(PACKET_DATA::SessionPlayer) * pkt_data.player_cnt;
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = seq_num;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data.player_cnt;
	for (auto& room : pkt_data.PlayerList)
	{
		SendBuffer << room;
	}
}

void NSW_PacketHandler::PackPlayerInfoPacket(Buffer& SendBuffer, const uint32_t pkt_id, const std::string player_id, const uint8_t player_state)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::PLAYER_INFO pkt_data;

	strcpy(pkt_data.player.id, player_id.c_str());
	pkt_data.player.state = player_state;

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::PackGameInfoPacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const std::string white_team_player, const std::string black_team_player)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::GAME_INFO pkt_data;

	pkt_data.round = round;
	strcpy(pkt_data.WhiteTeamPlayer.id, white_team_player.c_str());
	strcpy(pkt_data.BlackTeamPlayer.id, black_team_player.c_str());

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::PackChessEventPacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t event_type, const std::string event_player)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::CHESS_EVENT pkt_data;

	pkt_data.event_type = event_type;
	strcpy(pkt_data.event_player.id, event_player.c_str());

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::PackChessMovePacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const uint8_t src_slot_index, const uint8_t dest_slot_index)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::CHESS_MOVE pkt_data;

	pkt_data.round = round;
	pkt_data.src_slot_index = src_slot_index;
	pkt_data.dest_slot_index = dest_slot_index;

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::PackChessCastlingPacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const uint8_t king_slot_index, const uint8_t rook_slot_index)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::CHESS_CASTLING pkt_data;
	pkt_data.round = round;
	pkt_data.king_slot_index = king_slot_index;
	pkt_data.rook_slot_index = rook_slot_index;

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::PackChessPromotionPacket(Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const uint8_t pawn_slot_index, const uint8_t promotion_type)
{
	PACKET_HEADER pkt_header;
	PACKET_DATA::CHESS_PROMOTION pkt_data;
	pkt_data.round = round;
	pkt_data.pawn_slot_index = pawn_slot_index;
	pkt_data.promotion_type = promotion_type;

	pkt_header.content_length = sizeof(pkt_data);
	pkt_header.packet_id = pkt_id;
	pkt_header.sequence_num = 0;

	SendBuffer << pkt_header;
	SendBuffer << pkt_data;
}

void NSW_PacketHandler::UnpackHeader(Buffer& RecvPacket, uint16_t& content_length, uint32_t& pkt_id, uint8_t& seq_num)
{
	PACKET_HEADER pkt_header;
	RecvPacket >> pkt_header;

	content_length = pkt_header.content_length;
	pkt_id = pkt_header.packet_id;
	seq_num = pkt_header.sequence_num;
}

uint16_t NSW_PacketHandler::UnpackChatPacket(Buffer& RecvPacket, std::string& chat_msg)
{
	PACKET_DATA::CHAT pkt_data;
	RecvPacket >> pkt_data;

	chat_msg = pkt_data.msg;

	return sizeof(pkt_data);
}

uint16_t NSW_PacketHandler::UnpackRoomListInfoPacket(Buffer& RecvPacket, std::vector<PACKET_DATA::SessionRoom>& RoomList)
{
	PACKET_DATA::ROOM_LIST_INFO pkt_data;
	RecvPacket >> pkt_data.room_cnt;

	for (int i = 0; i < pkt_data.room_cnt; i++)
	{
		PACKET_DATA::SessionRoom room;
		RecvPacket >> room;
		RoomList.push_back(room);
	}

	return (sizeof(PACKET_DATA::SessionRoom) * RoomList.size());
}

uint16_t NSW_PacketHandler::UnpackRoomInfoPacket(Buffer& RecvPacket, uint8_t& room_index, std::string& room_name, uint8_t& participants)
{
	PACKET_DATA::ROOM_INFO pkt_data;
	RecvPacket >> pkt_data;

	room_index = pkt_data.room.index;
	room_name = pkt_data.room.name;
	participants = pkt_data.room.participant_num;

	return sizeof(pkt_data);
}

uint16_t NSW_PacketHandler::UnpackPlayerListInfoPacket(Buffer& RecvPacket, std::vector<PACKET_DATA::SessionPlayer>& PlayerList)
{
	PACKET_DATA::PLAYER_LIST_INFO pkt_data;
	RecvPacket >> pkt_data.player_cnt;

	for (int i = 0; i < pkt_data.player_cnt; i++)
	{
		PACKET_DATA::SessionPlayer player;
		RecvPacket >> player;
		PlayerList.push_back(player);
	}

	return (sizeof(PACKET_DATA::SessionPlayer) * PlayerList.size());
}

uint16_t NSW_PacketHandler::UnpackPlayerInfoPacket(Buffer& RecvPacket, std::string& player_id, uint8_t& player_state)
{
	PACKET_DATA::PLAYER_INFO pkt_data;
	RecvPacket >> pkt_data;

	player_id = pkt_data.player.id;
	player_state = pkt_data.player.state;

	return sizeof(pkt_data);
}

uint16_t NSW_PacketHandler::UnpackGameInfoPacket(Buffer& RecvPacket, uint8_t& round, std::string& white_team_player, std::string& black_team_player)
{
	PACKET_DATA::GAME_INFO pkt_data;
	RecvPacket >> pkt_data;

	round = pkt_data.round;
	white_team_player = pkt_data.WhiteTeamPlayer.id;
	black_team_player = pkt_data.BlackTeamPlayer.id;

	return sizeof(pkt_data);
}

uint16_t NSW_PacketHandler::UnpackChessEventPacket(Buffer& RecvPacket, uint8_t& event_type, std::string& event_player)
{
	PACKET_DATA::CHESS_EVENT pkt_data;
	RecvPacket >> pkt_data;

	event_type = pkt_data.event_type;
	event_player = pkt_data.event_player.id;

	return sizeof(pkt_data);
}

uint16_t NSW_PacketHandler::UnpackChessMovePacket(Buffer& RecvPacket, uint8_t& round, uint8_t& src_slot_index, uint8_t& dest_slot_index)
{
	PACKET_DATA::CHESS_MOVE pkt_data;
	RecvPacket >> pkt_data;

	round = pkt_data.round;
	src_slot_index = pkt_data.src_slot_index;
	dest_slot_index = pkt_data.dest_slot_index;

	return sizeof(pkt_data);
}

uint16_t NSW_PacketHandler::UnpackChessCastlingPacket(Buffer& RecvPacket, uint8_t& round, uint8_t& king_slot_index, uint8_t& rook_slot_index)
{
	PACKET_DATA::CHESS_CASTLING pkt_data;
	RecvPacket >> pkt_data;

	round = pkt_data.round;
	king_slot_index = pkt_data.king_slot_index;
	rook_slot_index = pkt_data.rook_slot_index;

	return sizeof(pkt_data);
}

uint16_t NSW_PacketHandler::UnpackChessPromotionPacket(Buffer& RecvPacket, uint8_t& round, uint8_t& pawn_slot_index, uint8_t& promotion_type)
{
	PACKET_DATA::CHESS_PROMOTION pkt_data;
	RecvPacket >> pkt_data;

	round = pkt_data.round;
	pawn_slot_index = pkt_data.pawn_slot_index;
	promotion_type = pkt_data.promotion_type;

	return sizeof(pkt_data);
}