// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessBoard.generated.h"

#define BOARD_LINE_NUM 8

UCLASS()
class CHESSPROJECT_API AChessBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessBoard();

public:
	class UChessBoardSlotComponent* GetSlotComponent(int index);
	void GetAllChessPieces(TArray<class ABaseChessPiece*>& ChessPieces);

public:
	void ShowAvailablePath(bool bEnabled, TArray<int>& Path);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void InitBoardSlot();

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	UStaticMeshComponent* BoardMesh;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	TArray<class UChessBoardSlotComponent*> ChessBoardSlots;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class")
	TSubclassOf<class UChessBoardSlotComponent> SlotComponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Size")
	float SlotSize = 4.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Size")
	float SlotPad = 0.025f;
};
