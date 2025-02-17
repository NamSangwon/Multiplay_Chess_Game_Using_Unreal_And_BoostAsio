// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPlayer.h"
#include "UserChessPlayer.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API AUserChessPlayer : public AChessPlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUserChessPlayer();

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void InitializePlayerState() override;
	void SetPlayerState(bool bFirst) override;

private:
	void RotateViewX(float Value);
	void RotateViewY(float Value);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float TurnRate = 60.0f;
};
