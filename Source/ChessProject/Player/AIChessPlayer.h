// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPlayer.h"
#include "AIChessPlayer.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API AAIChessPlayer : public AChessPlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIChessPlayer();

public:
	void InitializePlayerState() override;
	void SetPlayerState(bool bFirst) override;


};