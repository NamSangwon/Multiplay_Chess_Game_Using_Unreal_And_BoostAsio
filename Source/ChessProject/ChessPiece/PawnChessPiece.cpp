// Fill out your copyright notice in the Description page of Project Settings.


#include <ChessProject/ChessPiece/PawnChessPiece.h>

#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessGameMode.h>
#include <ChessProject/ChessGameInstance.h>
#include <ChessProject/Player/ChessPlayer.h>
#include <Kismet/GameplayStatics.h>

TArray<int> APawnChessPiece::CalculateAllPath()
{
	TArray<int> AvailableBoardSlots;
	int CurrentSlotIndex = CurrentPlacedSlot->GetSlotIndex();
	int CurrentRow = CurrentSlotIndex / EChessPieceColumn::SlotNumInLine;
	int CurrentCol = CurrentSlotIndex % EChessPieceColumn::SlotNumInLine;

	AChessBoard* Board = Cast<AChessBoard>(GetOwner());
	if (Board)
	{
		bool bBlock = false;
		int Direction = GetDirection();
		for (int i = 0; i < 2; i++) // Front
		{
			for (int j = -1; j <= 1; j++) // Side
			{
				// Forward Move
				if (j == 0)
				{
					if ((i == 1 && !IsNotMove()) || bBlock) continue;

					int index = (CurrentRow + Direction * (i + 1)) * EChessPieceColumn::SlotNumInLine + CurrentCol;
					if (index >= 0 && index < EChessPieceColumn::TotalSlotInBoard)
					{
						UChessBoardSlotComponent* PathSlot = Board->GetSlotComponent(index);
						if (PathSlot && PathSlot->IsChessPieceExist())
						{
							bBlock = true;
							continue;
						}

						AvailableBoardSlots.Add(index);
					}
				}
				// Attack (Normal(i == 1) or EnPassant(i == 0))
				else if (j != 0)
				{
					int index = (CurrentRow + i * Direction) * EChessPieceColumn::SlotNumInLine + (CurrentCol + j);
					if (index >= 0 && index < EChessPieceColumn::TotalSlotInBoard)
					{
						UChessBoardSlotComponent* TargetSlot = Board->GetSlotComponent(index);
						if (TargetSlot)
						{
							ABaseChessPiece* Target = TargetSlot->GetChessPieceOnSlot();
							if (Target && this->IsEnemy(Target))
							{
								if (IsEnPassant(Target))
								{
									int SlotIndexAfterEnPassant = index + EChessPieceColumn::SlotNumInLine * Direction;

									AvailableBoardSlots.Add(SlotIndexAfterEnPassant);
								}

								if (IsNormalAttack(Target))
								{
									AvailableBoardSlots.Add(index);
								}
							}
						}
					}
				}
			}
		}
	}

	return AvailableBoardSlots;
}

TArray<int> APawnChessPiece::CalculatePathToSlot(int SlotIndex)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard && CurrentPlacedSlot)
	{
		int CurrentRow = CurrentPlacedSlot->GetSlotIndex() / 8;
		int CurrentCol = CurrentPlacedSlot->GetSlotIndex() % 8;

		int TargetRow = SlotIndex / 8;
		int TargetCol = SlotIndex % 8;

		int Direction = GetDirection();
		int RowDist = FMath::Abs(CurrentRow - TargetRow);
		int ColDist = FMath::Abs(CurrentCol - TargetCol);

		// Check Valid Direction (Is Forward)
		if ((Direction == 1 && CurrentRow >= TargetRow) || (Direction == -1 && CurrentRow <= TargetRow))
			return TArray<int>();

		// Move
		TArray<int> Path;
		if (RowDist <= 2 && ColDist == 0)
		{
			for (int forward = 1; forward <= 2; forward++)
			{

				// Check Possible (First Move == Forwarding 2 Slot)
				if (forward == 2)
				{
					if (!(IsNotMove() && ((Direction > 0 && CurrentRow == 1) || (Direction < 0 && CurrentRow == 6))))
						break;
				}

				int path_slot_idx = ((CurrentRow + forward * Direction) * 8) + (CurrentCol);
				if (path_slot_idx >= 0 && path_slot_idx < EChessPieceColumn::TotalSlotInBoard)
				{
					UChessBoardSlotComponent* PathSlot = ChessBoard->GetSlotComponent(path_slot_idx);

					// Is Blocking
					if (PathSlot && PathSlot->IsChessPieceExist()) break;

					Path.Add(path_slot_idx);
				}
			}
		}
		// Attack
		else if (RowDist == 1 && ColDist == 1)
		{
			if (SlotIndex >= 0 && SlotIndex < EChessPieceColumn::TotalSlotInBoard)
			{
				UChessBoardSlotComponent* TargetSlot = ChessBoard->GetSlotComponent(SlotIndex);
				if (TargetSlot)
				{
					ABaseChessPiece* TargetChessPiece = TargetSlot->GetChessPieceOnSlot();
					if (TargetChessPiece)
					{
						if (IsNormalAttack(TargetChessPiece) || IsEnPassant(TargetChessPiece))
							Path.Add(SlotIndex);
					}
				}
			}
		}

		return Path;
	}

	return TArray<int>();
}

TArray<int> APawnChessPiece::CalculateAttackPathToSlot(int SlotIndex)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard && CurrentPlacedSlot)
	{
		int CurrentRow = CurrentPlacedSlot->GetSlotIndex() / 8;
		int CurrentCol = CurrentPlacedSlot->GetSlotIndex() % 8;

		int TargetRow = SlotIndex / 8;
		int TargetCol = SlotIndex % 8;

		int Direction = GetDirection();
		int RowDist = FMath::Abs(CurrentRow - TargetRow);
		int ColDist = FMath::Abs(CurrentCol - TargetCol);

		// Check Valid Direction (Is Forward)
		if ((Direction == 1 && CurrentRow >= TargetRow) || (Direction == -1 && CurrentRow <= TargetRow))
			return TArray<int>();

		// Attack
		if (RowDist == 1 && ColDist == 1)
		{
			if (SlotIndex >= 0 && SlotIndex < EChessPieceColumn::TotalSlotInBoard)
			{
				UChessBoardSlotComponent* TargetSlot = ChessBoard->GetSlotComponent(SlotIndex);
				if (TargetSlot)
				{
					TArray<int> Path;

					ABaseChessPiece* TargetChessPiece = TargetSlot->GetChessPieceOnSlot();
					if (TargetChessPiece)
					{
						if (IsNormalAttack(TargetChessPiece) || IsEnPassant(TargetChessPiece))
							Path.Add(SlotIndex);
					}

					return Path;
				}
			}
		}
	}

	return TArray<int>();
}

bool APawnChessPiece::IsEnPassant(const ABaseChessPiece* OtherChessPiece)
{
	APawnChessPiece* TargetPawn = Cast<APawnChessPiece>(const_cast<ABaseChessPiece*>(OtherChessPiece));

	if (TargetPawn)
	{
		if (CheckEnPassantCondition(TargetPawn))
		{
			int AttackerPosition = CurrentPlacedSlot->GetSlotIndex();
			int AttackerRow = AttackerPosition / EChessPieceColumn::SlotNumInLine;
			int AttackerCol = AttackerPosition % EChessPieceColumn::SlotNumInLine;

			int TargetPawnPosition = TargetPawn->GetCurrentPlacedSlotIndex();
			int TargetPawnRow = TargetPawnPosition / EChessPieceColumn::SlotNumInLine;
			int TargetPawnCol = TargetPawnPosition % EChessPieceColumn::SlotNumInLine;

			if (TargetPawnCol == AttackerCol - 1 || TargetPawnCol == AttackerCol + 1)
			{
				if (TargetPawnRow == AttackerRow) {
					return true;
				}
			}
		}
	}

	return false;
}

bool APawnChessPiece::IsNormalAttack(const ABaseChessPiece* OtherChessPiece)
{
	if (OtherChessPiece)
	{
		int AttackerPosition = CurrentPlacedSlot->GetSlotIndex();
		int AttackerRow = AttackerPosition / EChessPieceColumn::SlotNumInLine;
		int AttackerCol = AttackerPosition % EChessPieceColumn::SlotNumInLine;
		int AttackerDirection = GetDirection();

		int OtherChessPiecePosition = OtherChessPiece->GetCurrentPlacedSlotIndex();
		int OtherChessPieceRow = OtherChessPiecePosition / EChessPieceColumn::SlotNumInLine;
		int OtherChessPieceCol = OtherChessPiecePosition % EChessPieceColumn::SlotNumInLine;

		if (OtherChessPieceCol == AttackerCol - 1 || OtherChessPieceCol == AttackerCol + 1)
		{
			if (OtherChessPieceRow == AttackerRow + AttackerDirection) {
				return true;
			}
		}
	}

	return false;
}

bool APawnChessPiece::CheckEnPassantCondition(const APawnChessPiece* TargetPawn)
{
	AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	if (ChessGameMode)
	{
		int CurrentPosition = CurrentPlacedSlot->GetSlotIndex();
		int CurrentRow = CurrentPosition / EChessPieceColumn::SlotNumInLine;

		int TargetPosition = TargetPawn->GetCurrentPlacedSlotIndex();
		int TargetRow = TargetPosition / EChessPieceColumn::SlotNumInLine;

		// 첫번째 이동
		bool bFirstMove = (TargetPawn->MoveCount == 1);

		// 같은 위치
		bool bSameRow = (CurrentRow == TargetRow);

		// 두 칸 이동 (Player 1 Side or Player 2 Side)
		bool bForwardTwoSlot = (TargetRow == 3 || TargetRow == 4);

		// 바로 전 라운드에 이동
		bool bIsTargetLastMove = TargetPawn->IsLastMoved();

		return (bFirstMove && bSameRow && bForwardTwoSlot && bIsTargetLastMove);
	}

	return false;
}

int APawnChessPiece::GetDirection() const
{
	if (Tags.Find(FName(TEXT("WHITE"))) != INDEX_NONE)
	{
		return 1;
	}
	else if (Tags.Find(FName(TEXT("BLACK"))) != INDEX_NONE)
	{
		return -1;
	}

	return 0;
}

bool APawnChessPiece::CheckPossiblePromotion()
{
	int CurrentSlotIndex = CurrentPlacedSlot->GetSlotIndex();
	int CurrentRow = CurrentSlotIndex / 8;

	if (Tags.Find(FName(TEXT("WHITE"))) != INDEX_NONE)
	{
		return (CurrentRow >= 7);
	}
	else if (Tags.Find(FName(TEXT("BLACK"))) != INDEX_NONE)
	{
		return (CurrentRow <= 0);
	}

	return false;
}

void APawnChessPiece::EnPassant()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard && CurrentPlacedSlot)
	{
		int Position = CurrentPlacedSlot->GetSlotIndex();
		int Row = Position / EChessPieceColumn::SlotNumInLine;
		int Col = Position % EChessPieceColumn::SlotNumInLine;

		int Direction = GetDirection();

		// Check (When Player == Player 1) && Only Can Try (When Row == 5)
		// Check (When Player == Player 2) && Only Can Try (When Row == 2)
		if ((Direction == 1 && Row == 5) || (Direction == -1 && Row == 2))
		{
			// 앙파상을 당하는 폰의 존재 유무 확인
			int EnpassantTargetSlotIndex = (Row - Direction) * EChessPieceColumn::SlotNumInLine + Col;

			UChessBoardSlotComponent* TargetPawnSlot = ChessBoard->GetSlotComponent(EnpassantTargetSlotIndex);
			if (TargetPawnSlot && TargetPawnSlot->IsChessPieceExist())
			{
				APawnChessPiece* TargetPawn = Cast<APawnChessPiece>(TargetPawnSlot->GetChessPieceOnSlot());
				if (TargetPawn)
				{
					TargetPawn->Death();
				}
			}
		}
	}
}

void APawnChessPiece::Promote(const int promotion_type)
{
	AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	AChessPlayer* ChessPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (ChessPlayer && ChessGameMode && CurrentPlacedSlot)
	{
		int slot_index = CurrentPlacedSlot->GetSlotIndex();
		FText owner_type = FText::FromName(this->GetOwnerType());

		if (!owner_type.IsEmpty() && 0 <= slot_index && slot_index < EChessPieceColumn::TotalSlotInBoard)
		{
			ChessPlayer->SelectChessPiece(nullptr);

			this->Death();

			ChessGameMode->SpawnPromotedChessPiece(slot_index, owner_type, promotion_type);
		}
	}
}

void APawnChessPiece::Move(UChessBoardSlotComponent* Destination)
{
	Super::Move(Destination);

	AChessPlayer* ChessPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance && ChessPlayer)
	{
		EnPassant();

		if (ChessPlayer->IsMine(this) && CheckPossiblePromotion())
		{
			ChessPlayer->SelectChessPiece(this);
			ChessGameInstance->ShowPromotionWidget();
		}
	}
}
