// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "QueenChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API AQueenChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	AQueenChessPiece() { }

private:
	TArray<int> CalculateAllPath() override;
	TArray<int> CalculatePathToSlot(int SlotIndex) override;
};
