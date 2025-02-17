// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChessPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API AChessPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AChessPlayerController();

public:
	void InitializePlayerState();
	void SetPlayerActivate(bool bEnabled);
	void SetPlayerEventState(bool bEnabled);
	void SetPlayerState(bool bFirst);
	void TurnPlayerState(int round);
};
