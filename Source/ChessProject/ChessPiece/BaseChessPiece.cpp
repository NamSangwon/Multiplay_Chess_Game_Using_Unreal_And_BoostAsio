// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseChessPiece.h"

#include "Components/BoxComponent.h"
#include <ChessProject/ChessBoardSlotComponent.h>

#include <ChessProject/ChessGameMode.h>
#include <ChessProject/ChessGameInstance.h>
#include <ChessProject/Player/ChessPlayer.h>
#include <ChessProject/ChessBoard.h>
#include <Kismet/GameplayStatics.h>

#include <ChessProject/ChessPiece/PawnChessPiece.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>

// Sets default values
ABaseChessPiece::ABaseChessPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	RootComponent = BoxComp;

	PieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chess Piece Mesh"));
	PieceMesh->SetupAttachment(BoxComp);
}

// Called when the game starts or when spawned
void ABaseChessPiece::BeginPlay()
{
	Super::BeginPlay();

	OnInputTouchEnd.AddUniqueDynamic(this, &ABaseChessPiece::OnChessPieceTouchEnd);
	OnClicked.AddUniqueDynamic(this, &ABaseChessPiece::OnChessPieceClicked);
	PieceMesh->OnBeginCursorOver.AddUniqueDynamic(this, &ABaseChessPiece::OnMouseHoverBegin);
	PieceMesh->OnEndCursorOver.AddUniqueDynamic(this, &ABaseChessPiece::OnMouseHoverEnd);
}

// Called every frame
void ABaseChessPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseChessPiece::OnChessPieceClicked(AActor* Target, FKey ButtonPressed)
{
	if (Target == this)
	{
		TogglePathVisibility();
	}
}

void ABaseChessPiece::OnChessPieceTouchEnd(ETouchIndex::Type FingerIndex, AActor* TouchedActor)
{
	if (FingerIndex == ETouchIndex::Type::Touch1 && TouchedActor == this)
	{
		TogglePathVisibility();
	}
}

void ABaseChessPiece::OnMouseHoverBegin(UPrimitiveComponent* TouchedComponent)
{
	if (TouchedComponent == PieceMesh)
	{
		AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (MyPlayer && MyPlayer->IsMine(this))
		{
			if (!MyPlayer->AlreadySelect() && CurrentPlacedSlot)
			{
				CurrentPlacedSlot->SetVisibility(true);
			}
		}
	}
}

void ABaseChessPiece::OnMouseHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	if (TouchedComponent == PieceMesh)
	{
		AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (MyPlayer && MyPlayer->IsMine(this))
		{
			if (!MyPlayer->AlreadySelect() && CurrentPlacedSlot)
			{
				CurrentPlacedSlot->SetVisibility(false);
			}
		}
	}
}

void ABaseChessPiece::TogglePathVisibility()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (ChessBoard && MyPlayer && MyPlayer->IsMine(this) && CurrentPlacedSlot)
	{
		bool bToggle = true;

		if (MyPlayer->AlreadySelect())
		{
			if (MyPlayer->IsSelected(this))
			{
				MyPlayer->SelectChessPiece(nullptr);
				bToggle = false;
			}
			else
			{
				return;
			}
		}
		else
		{
			MyPlayer->SelectChessPiece(this);
		}

		ShowPath(bToggle);
	}
}

void ABaseChessPiece::Move(UChessBoardSlotComponent* Destination)
{
	AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	if (ChessGameMode && MyPlayer && CurrentPlacedSlot && Destination)
	{
		ShowPath(false);

		// Kill
		ABaseChessPiece* Target = Destination->GetChessPieceOnSlot();
		if (Target)
		{
			Target->Death();
		}

		// Move
		DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		AttachToComponent(Destination, FAttachmentTransformRules::KeepRelativeTransform);
		CurrentPlacedSlot->SetExistMark(false);

		// Update Info
		SetCurrentPlacedSlot(Destination);
		AddMoveCount();
		LastMovedRound = ChessGameMode->GetRound();

		MyPlayer->SelectChessPiece(nullptr);
	}
}

void ABaseChessPiece::Death()
{
	this->Destroy();

	if (CurrentPlacedSlot)
	{
		CurrentPlacedSlot->SetExistMark(false);
	}
}

void ABaseChessPiece::ShowPath(bool bEnabled)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		// Get My King
		AKingChessPiece* MyKing = nullptr;
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);
		for (int i = 0; i < AllChessPieces.Num(); i++)
		{
			if (AllChessPieces[i]->IsA(AKingChessPiece::StaticClass()) && !this->IsEnemy(AllChessPieces[i]))
			{
				MyKing = Cast<AKingChessPiece>(AllChessPieces[i]);
				break;
			}
		}

		TArray<int> Path;
		if (MyKing && MyKing->IsCheckState())
		{
			Path = CalculatePathToAvoidCheck();
		}
		else
		{
			Path = CalculateAllPath();
		}
		ChessBoard->ShowAvailablePath(bEnabled, Path);
		CurrentPlacedSlot->SetVisibility(bEnabled);
	}
}

bool ABaseChessPiece::IsLastMoved() const
{
	AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	if (ChessGameMode)
	{
		int CurrentRound = ChessGameMode->GetRound();
		
		// 해당 체스 피스가 마지막으로 움직인 라운드가 전 라운드인가
		return (LastMovedRound == CurrentRound - 1);
	}

	return false;
}

void ABaseChessPiece::SetCurrentPlacedSlot(UChessBoardSlotComponent* slot)
{
	if (slot)
	{
		CurrentPlacedSlot = slot;

		FName ChessPieceTag = GetOwnerType();
		if (!ChessPieceTag.IsNone())
		{
			CurrentPlacedSlot->SetExistMark(true, ChessPieceTag);
		}
	}
}

int ABaseChessPiece::GetCurrentPlacedSlotIndex() const
{
	if (CurrentPlacedSlot)
	{
		int index = CurrentPlacedSlot->GetSlotIndex();
		if (index >= 0 && index <= EChessPieceColumn::TotalSlotInBoard)
		{
			return index;
		}
	}

	return -1;
}

TArray<int> ABaseChessPiece::CalculatePathToAvoidCheck()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);

		// Find Enemy Chess Pieces & Checked King
		AKingChessPiece* CheckedKing = nullptr;
		TArray<ABaseChessPiece*> EnemyChessPieces;
		for (int i = 0; i < AllChessPieces.Num(); i++)
		{
			if (this->IsEnemy(AllChessPieces[i]))
			{
				EnemyChessPieces.Add(AllChessPieces[i]);
			}
			else
			{
				AKingChessPiece* CastedChessPiece = Cast<AKingChessPiece>(AllChessPieces[i]);
				if (CastedChessPiece)
				{
					CheckedKing = CastedChessPiece;
					continue;
				}
			}
		}
		
		if (CheckedKing)
		{
			// Find All Threatened Slot
			TSet<int> AllCheckPath;
			for (auto& EnemyChessPiece : EnemyChessPieces)
			{
				// Check Path
				TArray<int> EnemyAttackPath = EnemyChessPiece->CalculateAttackPathToSlot(CheckedKing->GetCurrentPlacedSlotIndex());

				// This is Blocking Check State --> Can not Move
				if (EnemyAttackPath.Find(GetCurrentPlacedSlotIndex()) != INDEX_NONE)
				{
					return TArray<int>();
				}

				AllCheckPath.Append(EnemyAttackPath);
			}

			// Find 
			TArray<int> AvailablePath = CalculateAllPath();
			for (int i = 0; i < AvailablePath.Num(); i++)
			{
				if (AllCheckPath.Find(AvailablePath[i]) == NULL)
				{
					AvailablePath.RemoveAt(i);
					i--;
					continue;
				}
			}

			return AvailablePath;
		}
	}

	return TArray<int>();
}

FName ABaseChessPiece::GetOwnerType() const
{
	FName ResultTag;

	for (int i = 0; i < Tags.Num(); i++)
	{
		FName ChessPieceTag = Tags[i];
		if (ChessPieceTag.IsEqual(TEXT("WHITE")) || ChessPieceTag.IsEqual(TEXT("BLACK")))
		{
			ResultTag = ChessPieceTag;
		}
	}

	return ResultTag;
}

bool ABaseChessPiece::IsEnemy(ABaseChessPiece* Others) const
{
	if (!Tags.IsEmpty() && Others)
	{
		FName OthersTag = Others->GetOwnerType();

		if (this->Tags.Find(OthersTag) == INDEX_NONE)
		{
			return true;
		}
	}

	return false;
}

bool ABaseChessPiece::IsInRoute(const int TargetSlotIndex)
{
	if (TargetSlotIndex >= 0 && TargetSlotIndex < EChessPieceColumn::TotalSlotInBoard)
	{
		TArray<int> AvailablePath = CalculateAllPath();

		if (AvailablePath.Find(TargetSlotIndex) != INDEX_NONE)
		{
			return true;
		}
	}

	return false;
}
