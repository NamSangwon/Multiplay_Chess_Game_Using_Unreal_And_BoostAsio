#pragma once
#include <iostream>
#include <vector>
#include <map>

#include "ChessGamePacket.hpp"

class NSW_PacketHandler
{
public:
	inline static NSW_PacketHandler& GetInstance() {
		static NSW_PacketHandler instance;
		return instance;
	}

public:
	void PackRequestPacket(class Buffer& SendBuffer, const uint32_t pkt_id);
	void PackChatPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const std::string chat_msg);
	void PackRoomListInfoPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t seq_num, const std::vector<PACKET_DATA::SessionRoom>& RoomList);
	void PackRoomInfoPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t room_index, const std::string room_name, const uint8_t participants);
	void PackPlayerListInfoPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t seq_num, const std::vector<PACKET_DATA::SessionPlayer>& PlayerList);
	void PackPlayerInfoPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const std::string player_id, const uint8_t player_state);
	void PackGameInfoPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const std::string white_team_player, const std::string black_team_player);
	void PackChessEventPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t event_type, const std::string event_player);
	void PackChessMovePacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const uint8_t src_slot_index, const uint8_t dest_slot_index);
	void PackChessCastlingPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const uint8_t king_slot_index, const uint8_t rook_slot_index);
	void PackChessPromotionPacket(class Buffer& SendBuffer, const uint32_t pkt_id, const uint8_t round, const uint8_t pawn_slot_index, const uint8_t promotion_type);

public:
	void UnpackHeader(class Buffer& RecvPacket, uint16_t& content_length, uint32_t& pkt_id, uint8_t& seq_num);
	uint16_t UnpackChatPacket(class Buffer& RecvPacket, std::string& chat_msg);
	uint16_t UnpackRoomListInfoPacket(class Buffer& RecvPacket, std::vector<PACKET_DATA::SessionRoom>& RoomList);
	uint16_t UnpackRoomInfoPacket(class Buffer& RecvPacket, uint8_t& room_index, std::string& room_name, uint8_t& participants);
	uint16_t UnpackPlayerListInfoPacket(class Buffer& RecvPacket, std::vector<PACKET_DATA::SessionPlayer>& PlayerList);
	uint16_t UnpackPlayerInfoPacket(class Buffer& RecvPacket, std::string& player_id, uint8_t& player_state);
	uint16_t UnpackGameInfoPacket(class Buffer& RecvPacket, uint8_t& round, std::string& white_team_player, std::string& black_team_player);
	uint16_t UnpackChessEventPacket(class Buffer& RecvPacket, uint8_t& event_type, std::string& event_player);
	uint16_t UnpackChessMovePacket(class Buffer& RecvPacket, uint8_t& round, uint8_t& src_slot_index, uint8_t& dest_slot_index);
	uint16_t UnpackChessCastlingPacket(class Buffer& RecvPacket, uint8_t& round, uint8_t& king_slot_index, uint8_t& rook_slot_index);
	uint16_t UnpackChessPromotionPacket(class Buffer& RecvPacket, uint8_t& round, uint8_t& pawn_slot_index, uint8_t& promotion_type);

private:
	NSW_PacketHandler() = default;
	~NSW_PacketHandler() = default;

	NSW_PacketHandler(const NSW_PacketHandler&) = delete; // 복제 생성 막음
	NSW_PacketHandler(NSW_PacketHandler&) = delete; // 이동 생성 막음

	NSW_PacketHandler& operator=(const NSW_PacketHandler&) = delete;  // 복제 대입 생성 막음
	NSW_PacketHandler& operator=(NSW_PacketHandler&&) = delete;       // 이동 대입 생성 막음
};