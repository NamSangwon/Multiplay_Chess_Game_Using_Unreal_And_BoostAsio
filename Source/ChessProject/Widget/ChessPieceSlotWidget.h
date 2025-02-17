// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChessPieceSlotWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EPromotionType : uint8
{
	Rook UMETA(DisplayName = "Rook"),
	Knight UMETA(DisplayName = "knight"),
	Bishop UMETA(DisplayName = "Bishop"),
	Queen UMETA(DisplayName = "Queen")
};

UCLASS()
class CHESSPROJECT_API UChessPieceSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	void SetupSlotForEachType();

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ChessPieceText;
	UPROPERTY(meta=(BindWidget))
	class UImage* ChessPieceImage;

	UPROPERTY(EditAnywhere, Category="ChessPieceType")
	EPromotionType ChessPieceType;
};
