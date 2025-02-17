
// Fill out your copyright notice in the Description page of Project Settings.


#include "PromotionWidget.h"

#include "Components/Button.h"

#include <ChessProject/Widget/ChessPieceSlotWidget.h>

#include <ChessProject/ChessGameInstance.h>
#include <ChessProject/Network/MyNetworkSubsystem.h>

#include <ChessProject/Player/ChessPlayer.h>

#include <ChessProject/ChessPiece/PawnChessPiece.h>

#include <Kismet/GameplayStatics.h>

void UPromotionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RookSlot = Cast<UChessPieceSlotWidget>(GetWidgetFromName(TEXT("RookSlot")));
	if (RookSlot)
	{
		RookSlot->SetupSlotForEachType();
	}
	RookButton = Cast<UButton>(GetWidgetFromName(TEXT("RookButton")));
	if (RookButton)
	{
		RookButton->OnClicked.AddUniqueDynamic(this, &UPromotionWidget::OnPromoteToRook);
	}

	BishopSlot = Cast<UChessPieceSlotWidget>(GetWidgetFromName(TEXT("BishopSlot")));
	if (BishopSlot)
	{
		BishopSlot->SetupSlotForEachType();
	}
	BishopButton = Cast<UButton>(GetWidgetFromName(TEXT("BishopButton")));
	if (BishopButton)
	{
		BishopButton->OnClicked.AddUniqueDynamic(this, &UPromotionWidget::OnPromoteToBishop);
	}

	KnightSlot = Cast<UChessPieceSlotWidget>(GetWidgetFromName(TEXT("KnightSlot")));
	if (KnightSlot)
	{
		KnightSlot->SetupSlotForEachType();
	}
	KnightButton = Cast<UButton>(GetWidgetFromName(TEXT("KnightButton")));
	if (KnightButton)
	{
		KnightButton->OnClicked.AddUniqueDynamic(this, &UPromotionWidget::OnPromoteToKnight);
	}

	QueenSlot = Cast<UChessPieceSlotWidget>(GetWidgetFromName(TEXT("QueenSlot")));
	if (QueenSlot)
	{
		QueenSlot->SetupSlotForEachType();
	}
	QueenButton = Cast<UButton>(GetWidgetFromName(TEXT("QueenButton")));
	if (QueenButton)
	{
		QueenButton->OnClicked.AddUniqueDynamic(this, &UPromotionWidget::OnPromoteToQueen);
	}
}

void UPromotionWidget::OnPromoteToRook()
{
	SelectPromotionType(static_cast<int>(CHESSPIECE_TYPE::ROOK));
}

void UPromotionWidget::OnPromoteToBishop()
{
	SelectPromotionType(static_cast<int>(CHESSPIECE_TYPE::BISHOP));
}

void UPromotionWidget::OnPromoteToKnight()
{
	SelectPromotionType(static_cast<int>(CHESSPIECE_TYPE::KNIGHT));
}

void UPromotionWidget::OnPromoteToQueen()
{
	SelectPromotionType(static_cast<int>(CHESSPIECE_TYPE::QUEEN));
}

void UPromotionWidget::SelectPromotionType(const int promotion_type)
{
	AChessPlayer* ChessPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
	if (MyNetworkSystem && ChessPlayer)
	{
		APawnChessPiece* SelectedPawn = Cast<APawnChessPiece>(ChessPlayer->GetSelectedChessPiece());
		if (SelectedPawn)
		{
			int SelecetedPawnSlotIndex = SelectedPawn->GetCurrentPlacedSlotIndex();
			MyNetworkSystem->RequestChessPromotion(SelecetedPawnSlotIndex, promotion_type);
			this->RemoveFromParent();
		}
	}
}
