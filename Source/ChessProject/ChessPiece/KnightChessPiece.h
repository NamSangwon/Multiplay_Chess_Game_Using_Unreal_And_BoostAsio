// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "KnightChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API AKnightChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()

public:
	AKnightChessPiece() {}

private:
	TArray<int> CalculateAllPath() override;
	TArray<int> CalculatePathToSlot(int SlotIndex) override;
};
