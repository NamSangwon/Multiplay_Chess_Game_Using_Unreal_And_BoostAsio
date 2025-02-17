// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <ChessProject/Network/ChessGamePacket.hpp>

#include "ChessGameInstance.generated.h"

enum class GAME_STATE : uint8_t
{
	INIT,
	IN_LOBBY,
	IN_ROOM,
	IN_GAME,
	END
};

enum CHESSPIECE_TYPE : uint8_t
{
	PAWN,
	ROOK,
	BISHOP,
	KNIGHT,
	QUEEN,
	KING
};

enum class TURN_STATE : uint8_t
{
	START,
	END
};

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API UChessGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual void Shutdown() override;

private:
	bool Tick(float DeltaSeconds);

public:
	void ProgramStart(const FText PlayerID);
	void ShowChat(const FString ChatMessage);
	void PreUpdateLobby();
	void UpdateLobby(const std::vector<PACKET_DATA::SessionRoom>& RoomList);
	void UpdateRoom(const std::vector<PACKET_DATA::SessionPlayer>& PlayerList);
	void EnterRoom(const FText RoomName);
	void EnterLobby();
	void UpdatePlayerState(const FText PlayerID, const uint8_t PlayerState);
	void GameStart(const uint8_t GameRound, const FText WhiteTeamPlayer, const FText BlackTeamPlayer);
	void UpdateRound(int round);
	void ProcessChessEvent(const uint8_t event_type, const FString event_player);
	void ProcessChessPieceMove(const uint8_t round, const uint8_t src_slot_index, const uint8_t dest_slot_index);
	void ProcessCastling(const uint8_t round, const uint8_t king_slot_index, const uint8_t rook_slot_index);
	void ProcessPromotion(const uint8_t round, const uint8_t pawn_slot_index, const uint8_t promotion_type);
	void ProgramEnd();

private:
	void GameEnd(const FText game_state, const FText game_result);

public:
	void HideAllWidget();
	void ShowProgramStartWidget();
	void ShowLobbyWidget();
	void ShowPromotionWidget();
	void ShowRoomWidget();
	void ShowInGameWidget();
	void ShowGameResultWidget(const FText game_state, const FText game_result);

public:
	void SetPlayerID(FText PlayerID) { m_PlayerID = PlayerID; }
	FText GetPlayerID() const { return m_PlayerID; }
	void ToggleTurn(bool bToggle);

private:
	FTSTicker::FDelegateHandle TickDelegateHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	float StartTimerSeconds;
	float m_fCurrTimerSeconds;

	TURN_STATE m_eTurnState = TURN_STATE::END;

	GAME_STATE m_SessionState;
	FText m_PlayerID;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> ProgramStartWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> LobbyWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> RoomWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> PromotionWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> InGameWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> GameResultWidgetClass;

	class UProgramStartWidget* m_ProgramStartWidget;
	class ULobbyWidget* m_LobbyWidget;
	class URoomWidget* m_RoomWidget;
	class UInGameWidget* m_InGameWidget;
	class UUserWidget* GameResultWidget;
};