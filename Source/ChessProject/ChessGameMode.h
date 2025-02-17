// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChessGameMode.generated.h"

#define BOARD_LINE_NUM 8

/**
 * 
 */

UCLASS()
class CHESSPROJECT_API AChessGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AChessGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	bool IsGameOver();
	void GameStart(bool bFirst);
	void GamePause(bool bPaused);
	void GameEnd();

public:
	void NextRound(int round);
	int GetRound() const { return m_nRound; }

private:
	void InitProgram();
	void InitChessGame(bool bFirst);
	void InitChessBoard();

public:
	void SpawnPromotedChessPiece(const int spawn_slot_index, const FText OwnerType, const int promotion_type);

private:
	class ABaseChessPiece* InitSpawnChessPiece(int index, FVector SpawnLocation);
	class ABaseChessPiece* SpawnChessPiece(int ChessPieceType, FVector SpawnLocation);

private:
	void SetChessPieceAppearance(class ABaseChessPiece* ChessPiece, FText OwnerType);
	void SetChessPieceProperty(class ABaseChessPiece* ChessPiece, class UChessBoardSlotComponent* BoardSlot, FText OwnerType);

private:
	class AChessPlayer* MyPlayer;
	class AChessBoard* ChessBoard;
	class AChessPlayerController* MyController;
	class UChessGameInstance* ChessGameInstance;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterial* WhiteTeamMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterial* BlackTeamMaterial;

	FRotator WhiteTeamPieceRotation = FRotator(0.0f, 180.0f, 0.0f);
	FRotator BlackTeamPieceRotation = FRotator::ZeroRotator;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Actor Class")
	TSubclassOf<class AChessBoard> ChessBoardClass;
	UPROPERTY(EditDefaultsOnly, Category = "Actor Class")
	TSubclassOf<class APawnChessPiece> PawnClass;
	UPROPERTY(EditDefaultsOnly, Category = "Actor Class")
	TSubclassOf<class AKingChessPiece> KingClass;
	UPROPERTY(EditDefaultsOnly, Category = "Actor Class")
	TSubclassOf<class AQueenChessPiece> QueenClass;
	UPROPERTY(EditDefaultsOnly, Category = "Actor Class")
	TSubclassOf<class ARookChessPiece> RookClass;
	UPROPERTY(EditDefaultsOnly, Category = "Actor Class")
	TSubclassOf<class AKnightChessPiece> KnightClass;
	UPROPERTY(EditDefaultsOnly, Category = "Actor Class")
	TSubclassOf<class ABishopChessPiece> BishopClass;

private:
	int m_nRound = 0;
};
