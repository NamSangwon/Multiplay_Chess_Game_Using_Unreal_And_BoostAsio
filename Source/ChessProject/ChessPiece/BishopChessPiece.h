// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "BishopChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API ABishopChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()

public:
	ABishopChessPiece() {}

private:
	TArray<int> CalculateAllPath() override;
	TArray<int> CalculatePathToSlot(int SlotIndex) override;
};
