// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "KingChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API AKingChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	AKingChessPiece() {}

public:
	void Castling(class ARookChessPiece* CastlingTarget);

public:
	bool IsCheckedBy(ABaseChessPiece* Attacker) const;
	bool IsCheckState();
	bool IsCheckmate();
	bool IsStalemate();

private:
	TArray<int> CalculateAllPath() override;
	TArray<int> CalculatePathToAvoidCheck() override;
	TArray<int> CalculatePathToSlot(int SlotIndex) override;

private:
	bool IsPossibleCastling(EChessPieceColumn RookColumn);
};
