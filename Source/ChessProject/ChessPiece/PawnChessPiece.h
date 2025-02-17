// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "PawnChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API APawnChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()

public:
	APawnChessPiece() {}

public:
	void Move(class UChessBoardSlotComponent* Destination) override;

	void EnPassant();
	void Promote(const int promotion_type);

private:
	TArray<int> CalculateAllPath() override;
	TArray<int> CalculatePathToSlot(int SlotIndex) override;
	TArray<int> CalculateAttackPathToSlot(int SlotIndex) override;

	bool CheckPossiblePromotion();
	bool CheckEnPassantCondition(const APawnChessPiece* TargetPawn);

	bool IsNormalAttack(const ABaseChessPiece* OtherChessPiece);
	bool IsEnPassant(const ABaseChessPiece* OtherChessPiece);
	
	int GetDirection() const;
};
