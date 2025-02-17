#pragma once
#include <iostream>
#include <vector>

const int ROOM_NAME_SIZE = 50;
const int SESSION_ID_SIZE = 50;
const int CHAT_SIZE = 1000;

enum PACKET_ID : uint32_t
{
	PT_PROGRAM_START,
	PT_CHAT,
	PT_REQ_ROOM_LIST,
	PT_RES_ROOM_LIST,
	PT_REQ_PLAYER_LIST,
	PT_RES_PLAYER_LIST,
	PT_CREATE_ROOM,
	PT_ENTER_ROOM,
	PT_LEAVE_ROOM,
	PT_UPDATE_PLAYER_STATE,
	PT_GAME_START,
	PT_CHESS_EVENT,
	PT_CHESS_MOVE,
	PT_CHESS_CASTLING,
	PT_CHESS_PROMOTION
};

struct PACKET_HEADER
{
	uint16_t content_length;
	uint32_t packet_id;
	uint8_t sequence_num;
};

namespace PACKET_DATA
{
	struct SessionRoom
	{
		uint8_t index;
		char name[ROOM_NAME_SIZE];
		uint8_t participant_num;
	};

	struct SessionPlayer
	{
		enum STATE_IN_ROOM : uint8_t
		{
			NONE,
			NOT_READY,
			READY
		};

		char id[SESSION_ID_SIZE];
		uint8_t state;
	};

	struct CHAT
	{
		char msg[CHAT_SIZE];
	};
	struct ROOM_INFO
	{
		PACKET_DATA::SessionRoom room;
	};
	struct ROOM_LIST_INFO
	{
		uint8_t room_cnt;
		std::vector<SessionRoom> RoomList;
	};
	struct PLAYER_INFO
	{
		PACKET_DATA::SessionPlayer player;
	};
	struct PLAYER_LIST_INFO
	{
		uint8_t player_cnt;
		std::vector<PACKET_DATA::SessionPlayer> PlayerList;
	};
	struct GAME_INFO
	{
		uint8_t round;
		PACKET_DATA::SessionPlayer WhiteTeamPlayer;
		PACKET_DATA::SessionPlayer BlackTeamPlayer;
	};
	struct CHESS_EVENT
	{
		enum EVENT_TYPE : uint8_t { NONE, CHECK, CHECKMATE, STALEMATE };

		uint8_t event_type;
		PACKET_DATA::SessionPlayer event_player;
	};
	struct CHESS_MOVE
	{
		uint8_t round;
		uint8_t src_slot_index;
		uint8_t dest_slot_index;
	};
	struct CHESS_CASTLING
	{
		uint8_t round;
		uint8_t king_slot_index;
		uint8_t rook_slot_index;
	};
	struct CHESS_PROMOTION
	{
		uint8_t round;
		uint8_t pawn_slot_index;
		uint8_t promotion_type;
	};
}