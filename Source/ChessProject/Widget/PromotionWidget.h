// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PromotionWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API UPromotionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void OnPromoteToRook();
	UFUNCTION()
	void OnPromoteToBishop();
	UFUNCTION()
	void OnPromoteToKnight();
	UFUNCTION()
	void OnPromoteToQueen();

	void SelectPromotionType(const int promotion_type);

private:
	UPROPERTY(meta=(BindWidget))
	class UChessPieceSlotWidget* RookSlot;
	UPROPERTY(meta = (BindWidget))
	class UButton* RookButton;

	UPROPERTY(meta=(BindWidget))
	class UChessPieceSlotWidget* BishopSlot;
	UPROPERTY(meta = (BindWidget))
	class UButton* BishopButton;

	UPROPERTY(meta=(BindWidget))
	class UChessPieceSlotWidget* KnightSlot;
	UPROPERTY(meta = (BindWidget))
	class UButton* KnightButton;

	UPROPERTY(meta=(BindWidget))
	class UChessPieceSlotWidget* QueenSlot;
	UPROPERTY(meta=(BindWidget))
	class UButton* QueenButton;
};
