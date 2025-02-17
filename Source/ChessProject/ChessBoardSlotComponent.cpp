// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessBoardSlotComponent.h"

#include <ChessProject/Network/MyNetworkSubsystem.h>
#include <ChessProject/ChessGameMode.h>
#include <ChessProject/ChessBoard.h>
#include <ChessProject/Player/ChessPlayer.h>

#include <ChessProject/ChessPiece/BaseChessPiece.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>
#include <ChessProject/ChessPiece/RookChessPiece.h>

#include <Kismet/GameplayStatics.h>

UChessBoardSlotComponent::UChessBoardSlotComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Shape(TEXT("/Engine/BasicShapes/Plane"));
	if (Shape.Succeeded())
	{
		SetStaticMesh(Shape.Object);
	}

	static ConstructorHelpers::FObjectFinder <UMaterial> ExistMark(TEXT("/Engine/MapTemplates/Materials/BasicAsset01"));
	if (ExistMark.Succeeded())
	{
		ExistMarkMaterial = ExistMark.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> NotExistMark(TEXT("/Engine/MapTemplates/Materials/BasicAsset03"));
	if (NotExistMark.Succeeded())
	{
		NotExistMarkMaterial = NotExistMark.Object;
	}

	SetExistMark(false);
}

// Called when the game starts or when spawned
void UChessBoardSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	OnInputTouchEnd.AddUniqueDynamic(this, &UChessBoardSlotComponent::OnTouchedSlot);
	OnClicked.AddUniqueDynamic(this, &UChessBoardSlotComponent::OnClickedSlot);
}

void UChessBoardSlotComponent::OnClickedSlot(UPrimitiveComponent* Target, FKey ButtonPressed)
{
	if (Target == this)
	{
		ChessPieceMoveToSlot();
	}
}

void UChessBoardSlotComponent::OnTouchedSlot(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	if (FingerIndex == ETouchIndex::Type::Touch1 && TouchedComponent == this)
	{
		ChessPieceMoveToSlot();
	}
}

void UChessBoardSlotComponent::ChessPieceMoveToSlot()
{
	if (bAvailablePath)
	{
		UMyNetworkSubsystem* MyNetworkSystem = GetWorld()->GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
		AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (MyNetworkSystem && MyPlayer && MyPlayer->AlreadySelect())
		{
			ABaseChessPiece* SelectedChessPiece = MyPlayer->GetSelectedChessPiece();
			if (SelectedChessPiece)
			{
				// Castling
				AKingChessPiece* SelectedKing = Cast<AKingChessPiece>(SelectedChessPiece);
				if (SelectedKing)
				{
					ARookChessPiece* CastlingTarget = Cast<ARookChessPiece>(GetChessPieceOnSlot());
					if (CastlingTarget && !SelectedKing->IsEnemy(CastlingTarget))
					{
						int king_slot_index = SelectedKing->GetCurrentPlacedSlotIndex();
						int rook_slot_index = CastlingTarget->GetCurrentPlacedSlotIndex();
						MyNetworkSystem->RequestChessCastling(king_slot_index, rook_slot_index);
						return;
					}
				}

				// Move
				int src_slot_index = SelectedChessPiece->GetCurrentPlacedSlotIndex();
				int dest_slot_index = this->GetSlotIndex();
				MyNetworkSystem->RequestChessMove(src_slot_index, dest_slot_index);
			}
		}
	}
}

void UChessBoardSlotComponent::SetExistMark(bool bEnabled, FName ChessPieceTag)
{
	ComponentTags.Empty();

	if (bEnabled)
	{
		if (ExistMarkMaterial)
		{
			SetMaterial(0, ExistMarkMaterial);
			ComponentTags.Add(ChessPieceTag);
		}
	}
	else
	{
		if (NotExistMarkMaterial)
		{
			SetMaterial(0, NotExistMarkMaterial);
		}
	}

	bChessPieceExisted = bEnabled;
}

ABaseChessPiece* UChessBoardSlotComponent::GetChessPieceOnSlot()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);

		for (int i = 0; i < AllChessPieces.Num(); i++)
		{
			if (AllChessPieces[i]->GetCurrentPlacedSlot() == this)
			{
				return AllChessPieces[i];
			}
		}
	}

	return nullptr;
}