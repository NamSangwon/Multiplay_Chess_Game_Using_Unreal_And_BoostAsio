// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPieceSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include <ChessProject/ChessGameMode.h>

void UChessPieceSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ChessPieceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ChessPieceText")));
	ChessPieceImage = Cast<UImage>(GetWidgetFromName(TEXT("ChessPieceImage")));
}

void UChessPieceSlotWidget::SetupSlotForEachType()
{
	FText TypeName;
	FText ImagePath;

	switch (ChessPieceType)
	{
	case EPromotionType::Rook:
		TypeName = FText::FromString(TEXT("Rook"));
		ImagePath = FText::FromStringTable(TEXT("/Game/ChessStringTable"), TEXT("IMAGE_ROOK"));
		break;
	case EPromotionType::Bishop:
		TypeName = FText::FromString(TEXT("Bishop"));
		ImagePath = FText::FromStringTable(TEXT("/Game/ChessStringTable"), TEXT("IMAGE_BISHOP"));
		break;
	case EPromotionType::Knight:
		TypeName = FText::FromString(TEXT("Knight"));
		ImagePath = FText::FromStringTable(TEXT("/Game/ChessStringTable"), TEXT("IMAGE_KNIGHT"));
		break;
	case EPromotionType::Queen:
		TypeName = FText::FromString(TEXT("Queen"));
		ImagePath = FText::FromStringTable(TEXT("/Game/ChessStringTable"), TEXT("IMAGE_QUEEN"));
		break;
	default:
		TypeName = FText::FromString(TEXT("Pawn"));
		ImagePath = FText::FromStringTable(TEXT("/Game/ChessStringTable"), TEXT("IMAGE_PAWN"));
		break;
	}

	ChessPieceText->SetText(TypeName);

	UTexture2D* ButtonImage = LoadObject<UTexture2D>(0, *ImagePath.ToString());
	if (ButtonImage)
	{
		ChessPieceImage->SetBrushFromTexture(ButtonImage);
	}
}
