// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "RookChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API ARookChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	ARookChessPiece() {}

private:
	TArray<int> CalculateAllPath() override;
	TArray<int> CalculatePathToSlot(int SlotIndex) override;
};
