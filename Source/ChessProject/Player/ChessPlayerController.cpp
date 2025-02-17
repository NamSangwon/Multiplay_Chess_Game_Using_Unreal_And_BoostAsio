// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPlayerController.h"

AChessPlayerController::AChessPlayerController()
{
	DefaultClickTraceChannel = ECollisionChannel::ECC_Visibility;
}

void AChessPlayerController::InitializePlayerState()
{
	Tags.Empty();
	SetPlayerActivate(false);
}

void AChessPlayerController::SetPlayerActivate(bool bEnabled)
{
	if (bEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}

	SetPlayerEventState(bEnabled);
}

void AChessPlayerController::SetPlayerEventState(bool bEnabled)
{
	bEnableTouchEvents = bEnabled;
	bEnableTouchOverEvents = bEnabled;

	bEnableClickEvents = bEnabled;
	bEnableMouseOverEvents = bEnabled;
}

void AChessPlayerController::SetPlayerState(bool bFirst)
{
	FText PlayerTag;

	Tags.Empty();

	if (bFirst)
	{
		PlayerTag = FText::FromString(TEXT("WHITE"));
	}
	else
	{
		PlayerTag = FText::FromString(TEXT("BLACK"));
	}

	Tags.Add(FName(PlayerTag.ToString()));
}

void AChessPlayerController::TurnPlayerState(int round)
{
	if (!this->Tags.IsEmpty())
	{
		bool bEnabled = false;
		if (this->Tags.Find(FName(TEXT("WHITE"))) != INDEX_NONE)
		{
			bEnabled = (round % 2 == 1);
		}
		else if (this->Tags.Find(FName(TEXT("BLACK"))) != INDEX_NONE)
		{
			bEnabled = (round % 2 == 0);
		}

		SetPlayerEventState(bEnabled);
	}
}
