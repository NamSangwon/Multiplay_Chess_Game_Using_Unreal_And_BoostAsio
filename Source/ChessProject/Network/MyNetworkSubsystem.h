// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyNetworkSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API UMyNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable)
	void ConnectToServer(FString Address, int port);

	UFUNCTION(BlueprintCallable)
	void DisconnectToServer();

public:
	void Run();
	void Disconnect();
	bool IsConnected() const;

public:
	void RequestProgramStart(const FString PlayerID);
	void RequestCreateRoom(const FString RoomName);
	void RequestEnterRoom(const uint8_t RoomIndex);
	void RequestUpdatePlayerState(const uint8_t player_state);
	void RequestUpdateRoom();
	void RequestUpdateLobby();
	void RequestChessMove(const uint8_t src_slot_index, const uint8_t dest_slot_index);
	void RequestChessCastling(const uint8_t king_slot_index, const uint8_t rook_slot_index);
	void RequestChessPromotion(const uint8_t pawn_slot_index, const uint8_t promotion_type);

public:
	void ResponsePacket();

private:
	void ResponseProgramStart(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseChat(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseUpdateLobby(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseUpdateRoom(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseCreateRoom(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseEnterRoom(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseEnterLobby(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseUpdatePlayerState(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseGameStart(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseChessEvent(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseMovingChessPiece(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponseCastlingChessPiece(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);
	void ResponsePromoteChessPiece(class Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num);

private:
	class FSocket* m_Socket;

	TSharedPtr<class FRecvWorker> RecvWorkerThread;
	TSharedPtr<class FSendWorker> SendWorkerThread;
};