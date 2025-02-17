// Fill out your copyright notice in the Description page of Project Settings.


#include <ChessProject/ChessGameMode.h>

#include <ChessProject/ChessGameInstance.h>
#include <ChessProject/Network/MyNetworkSubsystem.h>

#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/Player/ChessPlayerController.h>
#include <ChessProject/Player/UserChessPlayer.h>

#include <ChessProject/ChessPiece/PawnChessPiece.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>
#include <ChessProject/ChessPiece/QueenChessPiece.h>
#include <ChessProject/ChessPiece/RookChessPiece.h>
#include <ChessProject/ChessPiece/KnightChessPiece.h>
#include <ChessProject/ChessPiece/BishopChessPiece.h>

#include <Kismet/GameplayStatics.h>

AChessGameMode::AChessGameMode()
{

}

void AChessGameMode::GameStart(bool bFirst)
{
	if (MyController)
	{
		m_nRound = 0;

		InitChessGame(bFirst);

		InitChessBoard();

		MyController->SetPlayerEventState(false);
	}
}

void AChessGameMode::GamePause(bool bPaused)
{
	if (MyController)
	{
		bool bEnabled = !bPaused;
		MyController->SetPlayerActivate(bEnabled);
	}
}

void AChessGameMode::GameEnd()
{
	m_nRound = 0;

	if (MyPlayer && MyController)
	{
		MyController->InitializePlayerState();
		MyPlayer->InitializePlayerState();
	}

	if (ChessBoard)
	{
		TArray<ABaseChessPiece*> Actors;
		ChessBoard->GetAllChessPieces(Actors);
		for (auto& actor : Actors)
		{
			actor->Destroy();
		}
		ChessBoard->Destroy();
	}
}

bool AChessGameMode::IsGameOver()
{
	if (ChessGameInstance && ChessBoard && MyPlayer)
	{
		// Get Enemy King
		TArray<AKingChessPiece*> Kings;
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);
		for (int i = 0; i < AllChessPieces.Num(); i++)
		{
			if (AllChessPieces[i]->IsA(AKingChessPiece::StaticClass()))
			{
				Kings.Add(Cast<AKingChessPiece>(AllChessPieces[i]));
			}
		}

		for (auto& King : Kings)
		{
			if (King->IsCheckmate())
			{
				FText game_state = FText::FromString(TEXT("Checkmate"));
				FText game_result = (MyPlayer->IsMine(King)) ? FText::FromString(TEXT("YOU LOSE")) : FText::FromString(TEXT("YOU WIN"));
				ChessGameInstance->ShowGameResultWidget(game_state, game_result);
				return true;
			}

			if (King->IsStalemate())
			{
				FText game_state = FText::FromString(TEXT("Stalemate"));
				FText game_result = FText::FromString(TEXT("Draw"));
				ChessGameInstance->ShowGameResultWidget(game_state, game_result);
				return true;
			}
		}
	}

	return false;
}

void AChessGameMode::NextRound(int round)
{
	if (m_nRound < round)
	{
		if (ChessBoard)
		{
			TArray<int> AllSlot;
			for (int i = 0; i < 64; i++) AllSlot.Add(i);
			ChessBoard->ShowAvailablePath(false, AllSlot);
		}

		m_nRound = round;

		if (MyController)
		{
			MyController->TurnPlayerState(m_nRound);
		}
	}
}

// Called when the game starts or when spawned
void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitProgram();
}

void AChessGameMode::InitChessBoard()
{
	ChessBoard = GetWorld()->SpawnActor<AChessBoard>(ChessBoardClass);
	if (ChessBoard)
	{
		for (int i = 0; i < BOARD_LINE_NUM * 2; i++) 
		{
			FText PlayerTag;
			UChessBoardSlotComponent* WhiteTeamSideSlot = ChessBoard->GetSlotComponent(i);
			if (WhiteTeamSideSlot)
			{
				FVector WhiteTeamPieceLocation = WhiteTeamSideSlot->GetComponentLocation();

				PlayerTag = FText::FromString(TEXT("WHITE"));
				ABaseChessPiece* WhiteTeamPiece = InitSpawnChessPiece(i, WhiteTeamPieceLocation);
				if (WhiteTeamPiece)
				{
					SetChessPieceAppearance(WhiteTeamPiece, PlayerTag);
					SetChessPieceProperty(WhiteTeamPiece, WhiteTeamSideSlot, PlayerTag);
				}
			}

			int BlackTeamSideIndex = BOARD_LINE_NUM * BOARD_LINE_NUM - 1;
			UChessBoardSlotComponent* BlackTeamSideSlot = ChessBoard->GetSlotComponent(BlackTeamSideIndex - i); 
			if (BlackTeamSideSlot)
			{
				FVector BlackTeamPieceLocation = BlackTeamSideSlot->GetComponentLocation();

				PlayerTag = FText::FromString(TEXT("BLACK"));
				ABaseChessPiece* BlackTeamPiece = InitSpawnChessPiece(BlackTeamSideIndex - i, BlackTeamPieceLocation);
				if (BlackTeamPiece)
				{
					SetChessPieceAppearance(BlackTeamPiece, PlayerTag);
					SetChessPieceProperty(BlackTeamPiece, BlackTeamSideSlot, PlayerTag);
				}
			}
		}
	}
}

void AChessGameMode::SetChessPieceAppearance(ABaseChessPiece* ChessPiece, FText OwnerType)
{
	if (ChessPiece)
	{
		UStaticMeshComponent* ChessPieceMesh = Cast<UStaticMeshComponent>(ChessPiece->FindComponentByClass(UStaticMeshComponent::StaticClass()));

		if (ChessPieceMesh)
		{
			UMaterial* ChessPieceMaterial = nullptr;
			if (OwnerType.EqualTo(FText::FromString(TEXT("WHITE"))))
			{
				ChessPieceMaterial = WhiteTeamMaterial;
				ChessPiece->SetActorRotation(WhiteTeamPieceRotation);
			}
			else if (OwnerType.EqualTo(FText::FromString(TEXT("BLACK"))))
			{
				ChessPieceMaterial = BlackTeamMaterial;
				ChessPiece->SetActorRotation(BlackTeamPieceRotation);
			}

			if (ChessPieceMaterial)
			{
				ChessPieceMesh->SetMaterial(0, ChessPieceMaterial);
			}
		}
	}
}

void AChessGameMode::SetChessPieceProperty(ABaseChessPiece* ChessPiece, UChessBoardSlotComponent* BoardSlot, FText OwnerType)
{
	if (ChessPiece && ChessBoard && BoardSlot)
	{
		ChessPiece->Tags.Add(FName(OwnerType.ToString()));
		ChessPiece->AttachToComponent(BoardSlot, FAttachmentTransformRules::KeepWorldTransform);
		ChessPiece->SetOwner(ChessBoard);
		ChessPiece->SetCurrentPlacedSlot(BoardSlot);
	}
}

ABaseChessPiece* AChessGameMode::InitSpawnChessPiece(int index, FVector SpawnLocation)
{
	ABaseChessPiece* Result = nullptr;

	if (index / BOARD_LINE_NUM == 0 || index / BOARD_LINE_NUM == 7)
	{
		if (index % BOARD_LINE_NUM == 0 || index % BOARD_LINE_NUM == 7)
			Result = SpawnChessPiece(CHESSPIECE_TYPE::ROOK, SpawnLocation);
		else if (index % BOARD_LINE_NUM == 1 || index % BOARD_LINE_NUM == 6)
			Result = SpawnChessPiece(CHESSPIECE_TYPE::KNIGHT, SpawnLocation);
		else if (index % BOARD_LINE_NUM == 2 || index % BOARD_LINE_NUM == 5)
			Result = SpawnChessPiece(CHESSPIECE_TYPE::BISHOP, SpawnLocation);
		else if (index % BOARD_LINE_NUM == 3)
			Result = SpawnChessPiece(CHESSPIECE_TYPE::QUEEN, SpawnLocation);
		else if (index % BOARD_LINE_NUM == 4)
			Result = SpawnChessPiece(CHESSPIECE_TYPE::KING, SpawnLocation);
	}
	else
		Result = SpawnChessPiece(CHESSPIECE_TYPE::PAWN, SpawnLocation);

	return Result;
}

void AChessGameMode::SpawnPromotedChessPiece(const int spawn_slot_index, const FText OwnerType, const int promotion_type)
{
	if (ChessBoard)
	{
		UChessBoardSlotComponent* PromotingSlot = ChessBoard->GetSlotComponent(spawn_slot_index);
		if (PromotingSlot)
		{
			FVector SpawnLocation = PromotingSlot->GetComponentLocation();
			ABaseChessPiece* PromotedChessPiece = SpawnChessPiece(promotion_type, SpawnLocation);
			if (PromotedChessPiece)
			{
				SetChessPieceAppearance(PromotedChessPiece, OwnerType);
				SetChessPieceProperty(PromotedChessPiece, PromotingSlot, OwnerType);
			}
		}
	}
}

ABaseChessPiece* AChessGameMode::SpawnChessPiece(int ChessPieceType, FVector SpawnLocation)
{
	ABaseChessPiece* ChessPiece = nullptr;

	switch (ChessPieceType)
	{
	case CHESSPIECE_TYPE::PAWN:		ChessPiece = GetWorld()->SpawnActor<APawnChessPiece>(PawnClass, SpawnLocation, FRotator::ZeroRotator); break;
	case CHESSPIECE_TYPE::ROOK:		ChessPiece = GetWorld()->SpawnActor<ARookChessPiece>(RookClass, SpawnLocation, FRotator::ZeroRotator); break;
	case CHESSPIECE_TYPE::BISHOP:	ChessPiece = GetWorld()->SpawnActor<ABishopChessPiece>(BishopClass, SpawnLocation, FRotator::ZeroRotator); break;
	case CHESSPIECE_TYPE::KNIGHT:	ChessPiece = GetWorld()->SpawnActor<AKnightChessPiece>(KnightClass, SpawnLocation, FRotator::ZeroRotator); break;
	case CHESSPIECE_TYPE::QUEEN:	ChessPiece = GetWorld()->SpawnActor<AQueenChessPiece>(QueenClass, SpawnLocation, FRotator::ZeroRotator); break;
	case CHESSPIECE_TYPE::KING:		ChessPiece = GetWorld()->SpawnActor<AKingChessPiece>(KingClass, SpawnLocation, FRotator::ZeroRotator); break;
	}

	return ChessPiece;
}

void AChessGameMode::InitProgram()
{
	MyPlayer = Cast<AUserChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	MyController = Cast<AChessPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());

	if (ChessGameInstance && MyPlayer && MyController)
	{
		MyController->bShowMouseCursor = true;

		MyController->SetPlayerActivate(false);
		MyController->SetPlayerState(false);
		MyPlayer->SetPlayerState(false);

		ChessGameInstance->HideAllWidget();
		ChessGameInstance->ShowProgramStartWidget();
	}
}

void AChessGameMode::InitChessGame(bool bFirst)
{
	if (ChessGameInstance && MyPlayer && MyController)
	{
		MyController->SetPlayerActivate(true);
		MyController->SetPlayerState(bFirst);
		MyPlayer->SetPlayerState(bFirst);
	}
}
