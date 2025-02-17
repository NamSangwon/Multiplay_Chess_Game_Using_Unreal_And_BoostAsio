// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ChessBoardSlotComponent.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API UChessBoardSlotComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UChessBoardSlotComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void SetSlotIndex(int index) { SlotIndex = index; }
	int GetSlotIndex() const { return SlotIndex; }
	bool IsChessPieceExist() const { return bChessPieceExisted; }

	void SetPathAvailable(bool bEnable) { bAvailablePath = bEnable; }

public:
	class ABaseChessPiece* GetChessPieceOnSlot();
	void SetExistMark(bool bEnabled, const FName ChessPieceTag = TEXT("NONE"));

private:
	UFUNCTION()
	void OnClickedSlot(UPrimitiveComponent* Target, FKey ButtonPressed);
	UFUNCTION()
	void OnTouchedSlot(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void ChessPieceMoveToSlot();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterial* ExistMarkMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterial* NotExistMarkMaterial;

	int SlotIndex = INDEX_NONE;
	bool bChessPieceExisted = false;
	bool bAvailablePath = false;
};
