// Fill out your copyright notice in the Description page of Project Settings.

#include "AIChessPlayer.h"

#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessBoardSlotComponent.h>

#include <ChessProject/ChessPiece/BaseChessPiece.h>
#include <ChessProject/ChessPiece/PawnChessPiece.h>
#include <ChessProject/ChessPiece/RookChessPiece.h>
#include <ChessProject/ChessPiece/BishopChessPiece.h>
#include <ChessProject/ChessPiece/KnightChessPiece.h>
#include <ChessProject/ChessPiece/QueenChessPiece.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>

#include <Kismet/GameplayStatics.h>
#include <ChessProject/ChessGameMode.h>

AAIChessPlayer::AAIChessPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAIChessPlayer::InitializePlayerState()
{
	Super::InitializePlayerState();

}

void AAIChessPlayer::SetPlayerState(bool bFirst)
{
	Super::SetPlayerState(bFirst);

}