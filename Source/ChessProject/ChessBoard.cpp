// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessBoard.h"

#include "Components/BoxComponent.h"
#include "ChessBoardSlotComponent.h"

#include <ChessProject/ChessPiece/BaseChessPiece.h>

// Sets default values
AChessBoard::AChessBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	RootComponent = BoxComp;

	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board Mesh"));
	BoardMesh->SetupAttachment(BoxComp);

	for (int i = 0; i < BOARD_LINE_NUM * BOARD_LINE_NUM; i++) {
		FString SlotName = FString::Printf(TEXT("Slot_%d"), i);
		UChessBoardSlotComponent* NewSlot = CreateDefaultSubobject<UChessBoardSlotComponent>(FName(*SlotName));
		if (NewSlot)
		{
			NewSlot->SetVisibility(false);
			NewSlot->SetSlotIndex(i);
			NewSlot->SetupAttachment(BoardMesh);
			NewSlot->SetRelativeScale3D(FVector(0.045f, 0.045f, 1.f));
			ChessBoardSlots.Add(NewSlot);
		}
	}
}

UChessBoardSlotComponent* AChessBoard::GetSlotComponent(int index)
{
	if (index >= 0 && index < BOARD_LINE_NUM * BOARD_LINE_NUM)
	{
		return ChessBoardSlots[index];
	}

	return nullptr;
}

void AChessBoard::GetAllChessPieces(TArray<class ABaseChessPiece*>& ChessPieces)
{
	TArray<AActor*> Actors;
	GetAttachedActors(Actors);
	for (int i = 0; i < Actors.Num(); i++)
	{
		ABaseChessPiece* ChessPiece = Cast<ABaseChessPiece>(Actors[i]);
		if (ChessPiece)
		{
			ChessPieces.Add(ChessPiece);
		}
	}
}

void AChessBoard::ShowAvailablePath(bool bEnabled, TArray<int>& Path)
{
	for (int i = 0; i < Path.Num(); i++)
	{
		UChessBoardSlotComponent* SlotComponent = ChessBoardSlots[Path[i]];
		if (SlotComponent)
		{
			SlotComponent->SetVisibility(bEnabled);
			SlotComponent->SetPathAvailable(bEnabled);
		}
	}
}

// Called when the game starts or when spawned
void AChessBoard::BeginPlay()
{
	Super::BeginPlay();
	
	// 체스 보드의 각 슬롯을 일정 간격으로 배치
	InitBoardSlot();
}

// Called every frame
void AChessBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChessBoard::InitBoardSlot()
{
	float StartPos = (SlotSize * 4) - (SlotSize / 2 + SlotPad);
	FVector StartLocation = FVector(-StartPos, StartPos, 2.01f);

	for (int index = 0; index < BOARD_LINE_NUM* BOARD_LINE_NUM; ++index) {
		if (ChessBoardSlots.IsValidIndex(index))
		{
			if (ChessBoardSlots[index])
			{
				float XPos = (index % BOARD_LINE_NUM) * SlotSize + 2 * SlotPad;
				float YPos = (index / BOARD_LINE_NUM) * SlotSize + 2 * SlotPad;
				FVector SlotLocation = StartLocation + FVector(XPos, -YPos, 0.f);
				ChessBoardSlots[index]->SetRelativeLocation(SlotLocation);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Null ChessBoardSlots[%d]"), index);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid ChessBoardSlots[%d]"), index);
		}
	}
}