// Fill out your copyright notice in the Description page of Project Settings.


#include "UserChessPlayer.h"

#include <Kismet/GameplayStatics.h>

#include "GameFramework/SpringArmComponent.h"


AUserChessPlayer::AUserChessPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called to bind functionality to input
void AUserChessPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("RotateViewX"), this, &AUserChessPlayer::RotateViewX);
	PlayerInputComponent->BindAxis(TEXT("RotateViewY"), this, &AUserChessPlayer::RotateViewY);
}

void AUserChessPlayer::InitializePlayerState()
{
	Super::InitializePlayerState();
}

void AUserChessPlayer::SetPlayerState(bool bFirst)
{
	Super::SetPlayerState(bFirst);
}

void AUserChessPlayer::RotateViewX(float Value)
{
	FRotator DeltaRotator = FRotator::ZeroRotator;
	DeltaRotator.Yaw = Value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	AddActorLocalRotation(DeltaRotator, true);
}

void AUserChessPlayer::RotateViewY(float Value)
{
	FRotator DeltaRotator = FRotator::ZeroRotator;
	DeltaRotator.Pitch = Value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	SpringArm->AddLocalRotation(DeltaRotator, true);
}