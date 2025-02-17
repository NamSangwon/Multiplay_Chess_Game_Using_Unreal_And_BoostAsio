// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseChessPiece.generated.h"

UENUM(BlueprintType)
enum class EChessPieceType : uint8
{
	Pawn UMETA(DisplayName = "Pawn"),
	Rook UMETA(DisplayName = "Rook"),
	Knight UMETA(DisplayName = "knight"),
	Bishop UMETA(DisplayName = "Bishop"),
	Queen UMETA(DisplayName = "Queen"),
	King UMETA(DisplayName = "King")
};

enum EChessPieceColumn : uint8
{
	LeftRook,
	LeftKnight,
	LeftBishop,
	Queen,
	King,
	RightBishop,
	RightKnight,
	RightRook,
	SlotNumInLine,
	TotalSlotInBoard = 64
};

UCLASS()
class CHESSPROJECT_API ABaseChessPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseChessPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnChessPieceClicked(AActor* Target, FKey ButtonPressed);
	UFUNCTION()
	void OnChessPieceTouchEnd(ETouchIndex::Type FingerIndex, AActor* TouchedActor);
	UFUNCTION()
	void OnMouseHoverBegin(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void OnMouseHoverEnd(UPrimitiveComponent* TouchedComponent);

private:
	void TogglePathVisibility();

public:
	virtual void Move(class UChessBoardSlotComponent* Destination);
	void Death();

public:
	void ShowPath(bool bEnabled);
	virtual TArray<int> CalculateAllPath() PURE_VIRTUAL(ABaseChessPiece::CalculateAllPath, return TArray<int>(); );
	virtual TArray<int> CalculatePathToSlot(int SlotIndex) PURE_VIRTUAL(ABaseChessPiece::CalculateAttackPath, return TArray<int>(); );
	virtual TArray<int> CalculateAttackPathToSlot(int SlotIndex) { return CalculatePathToSlot(SlotIndex); }
	virtual TArray<int> CalculatePathToAvoidCheck();

public:
	FName GetOwnerType() const;
	bool IsEnemy(ABaseChessPiece* Others) const;
	bool IsInRoute(const int TargetSlotIndex);

public:
	void SetCurrentPlacedSlot(class UChessBoardSlotComponent* slot);
	class UChessBoardSlotComponent* GetCurrentPlacedSlot() const { return CurrentPlacedSlot; }
	int GetCurrentPlacedSlotIndex() const;

public:
	void AddMoveCount() { MoveCount++; }
	int GetMoveCount() { return MoveCount; }
	bool IsMoveOnce() const { return (MoveCount == 1); }
	bool IsNotMove() const { return (MoveCount == 0); }

public:
	void SetLastMovedRound(int Round) { LastMovedRound = Round; }
	bool IsLastMoved() const;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	UStaticMeshComponent* PieceMesh;

	UPROPERTY(EditDefaultsOnly, Category="Type")
	EChessPieceType PieceType;

protected:
	class UChessBoardSlotComponent* CurrentPlacedSlot;
	int MoveCount = 0;
	int LastMovedRound = 0;
};
