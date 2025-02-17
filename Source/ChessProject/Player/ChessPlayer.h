// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessPlayer.generated.h"

UCLASS()
class CHESSPROJECT_API AChessPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChessPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void InitializePlayerState();
	virtual void SetPlayerState(bool bFirst);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void DoRandomMove();
	void DoMoveChessPiece(const int src_slot_index, const int dest_slot_index);
	void DoCastling(const int king_src_slot_index, const int rook_src_slot_index);
	void DoPromotion(const int slot_index, const int promotion_type);

public:
	void SelectChessPiece(class ABaseChessPiece* Selected) { SelectedChessPiece = Selected; }
	class ABaseChessPiece* GetSelectedChessPiece() { return SelectedChessPiece; }
	bool IsSelected(class ABaseChessPiece* Selected) { return (SelectedChessPiece == Selected); }
	bool AlreadySelect() const { return (SelectedChessPiece != nullptr); }

public:
	FText GetPlayerType() const;
	bool IsMine(class ABaseChessPiece* Others) const;
	bool IsMyTurn() const;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, Category="Camera")
	class UCameraComponent* Camera;

protected:
	class ABaseChessPiece* SelectedChessPiece = nullptr;
};
