// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPlayer.h"

#include <Kismet/GameplayStatics.h>

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"

#include <ChessProject/ChessGameMode.h>
#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/Network/MyNetworkSubsystem.h>

#include <ChessProject/ChessPiece/BaseChessPiece.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>
#include <ChessProject/ChessPiece/RookChessPiece.h>
#include <ChessProject/ChessPiece/PawnChessPiece.h>

// Sets default values
AChessPlayer::AChessPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	RootComponent = SphereComp;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(SphereComp);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AChessPlayer::InitializePlayerState()
{
	Tags.Empty();
	SelectedChessPiece = nullptr;
}

void AChessPlayer::SetPlayerState(bool bFirst)
{
	FText PlayerTag;
	FRotator StartRotation;

	Tags.Empty();
	
	if (bFirst)
	{
		PlayerTag = FText::FromString(TEXT("WHITE"));
		StartRotation = FRotator(-45.f, -90.f, 0.f);
	}
	else
	{
		PlayerTag = FText::FromString(TEXT("BLACK"));
		StartRotation = FRotator(-45.f, 90.f, 0.f);
	}

	Tags.Add(FName(PlayerTag.ToString()));
	SpringArm->SetWorldRotation(StartRotation);
}

// Called when the game starts or when spawned
void AChessPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChessPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChessPlayer::DoRandomMove()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoard::StaticClass()));
	if (ChessBoard)
	{
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);

		TArray<ABaseChessPiece*> MyChessPieces;
		for (auto& ChessPiece : AllChessPieces)
		{
			if (IsMine(ChessPiece))
				MyChessPieces.Add(ChessPiece);
		}

		TArray<int32> AllPath;
		int random_src_idx = -1;
		int random_dest_idx = -1;
		while(true)
		{
			// Select Random Chess Piece
			int random_chess_piece_idx = FMath::RandRange(0, MyChessPieces.Num() - 1);

			// Select Random Destination
			AllPath = MyChessPieces[random_chess_piece_idx]->CalculateAllPath();
			int random_path_idx = FMath::RandRange(0, AllPath.Num() - 1);

			if (!AllPath.IsEmpty())
			{
				random_src_idx = MyChessPieces[random_chess_piece_idx]->GetCurrentPlacedSlotIndex();
				random_dest_idx = AllPath[random_path_idx];
				break;
			}
		} 

		// Random Move
		if ((0 <= random_src_idx && random_src_idx < 64) && (0 <= random_dest_idx && random_dest_idx < 64))
		{
			UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
			if (MyNetworkSystem)
			{
				MyNetworkSystem->RequestChessMove(random_src_idx, random_dest_idx);
			}
		}
	}
}

void AChessPlayer::DoMoveChessPiece(const int src_slot_index, const int dest_slot_index)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoard::StaticClass()));
	if (ChessBoard)
	{
		UChessBoardSlotComponent* SrcSlot = ChessBoard->GetSlotComponent(src_slot_index);
		UChessBoardSlotComponent* DestSlot = ChessBoard->GetSlotComponent(dest_slot_index);
		if (SrcSlot && SrcSlot->IsChessPieceExist() && DestSlot)
		{
			ABaseChessPiece* MovingChessPiece = SrcSlot->GetChessPieceOnSlot();
			if (MovingChessPiece)
			{
				MovingChessPiece->Move(DestSlot);
			}
		}
	}
}

void AChessPlayer::DoCastling(const int king_src_slot_index, const int rook_src_slot_index)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoard::StaticClass()));
	if (ChessBoard)
	{
		UChessBoardSlotComponent* KingSlot = ChessBoard->GetSlotComponent(king_src_slot_index);
		UChessBoardSlotComponent* RookSlot = ChessBoard->GetSlotComponent(rook_src_slot_index);
		if (KingSlot && KingSlot->IsChessPieceExist() && RookSlot && RookSlot->IsChessPieceExist())
		{
			AKingChessPiece* CastlingKing = Cast<AKingChessPiece>(KingSlot->GetChessPieceOnSlot());
			if (CastlingKing)
			{
				ARookChessPiece* CastlingTarget = Cast<ARookChessPiece>(RookSlot->GetChessPieceOnSlot());
				if (CastlingTarget && !CastlingKing->IsEnemy(CastlingTarget))
				{
					CastlingKing->Castling(CastlingTarget);
				}
			}
		}
	}
}

void AChessPlayer::DoPromotion(const int slot_index, const int promotion_type)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoard::StaticClass()));
	if (ChessBoard)
	{
		UChessBoardSlotComponent* SrcSlot = ChessBoard->GetSlotComponent(slot_index);
		if (SrcSlot && SrcSlot->IsChessPieceExist())
		{
			APawnChessPiece* PromotingPawn = Cast<APawnChessPiece>(SrcSlot->GetChessPieceOnSlot());
			if (PromotingPawn)
			{
				PromotingPawn->Promote(promotion_type);
			}
		}
		
	}
}

FText AChessPlayer::GetPlayerType() const
{
	if (this->Tags.IsEmpty()) return FText::GetEmpty();

	FText PlayerType;
	if (this->Tags.Find(TEXT("WHITE")) != INDEX_NONE)
	{
		PlayerType = FText::FromString(TEXT("WHITE"));
	}
	else if (this->Tags.Find(TEXT("BLACK")) != INDEX_NONE)
	{
		PlayerType = FText::FromString(TEXT("BLACK"));
	}

	return PlayerType;
}

bool AChessPlayer::IsMine(ABaseChessPiece* Others) const
{
	if (!Tags.IsEmpty() && Others)
	{
		FName OthersTag = Others->GetOwnerType();
		if (Tags.Find(OthersTag) != INDEX_NONE)
		{
			return true;
		}
	}

	return false;
}

bool AChessPlayer::IsMyTurn() const
{
	AChessGameMode* ChessGameMode = Cast<AChessGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (ChessGameMode)
	{
		int round = ChessGameMode->GetRound();
		if (round > 0)
		{
			if (GetPlayerType().EqualTo(FText::FromString(TEXT("WHITE"))))
			{
				return (round % 2 == 1);
			}
			else if (GetPlayerType().EqualTo(FText::FromString(TEXT("BLACK"))))
			{
				return (round % 2 == 0);
			}
		}
	}

	return false;
}
