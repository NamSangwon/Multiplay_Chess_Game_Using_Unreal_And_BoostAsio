
// Fill out your copyright notice in the Description page of Project Settings.


#include <ChessProject/ChessPiece/RookChessPiece.h>
#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>

TArray<int> ARookChessPiece::CalculateAllPath()
{
	TArray<int> AvailableBoardSlots;
	int CurrentSlotIndex = CurrentPlacedSlot->GetSlotIndex();
	int col = CurrentSlotIndex % EChessPieceColumn::SlotNumInLine;
	int row = CurrentSlotIndex / EChessPieceColumn::SlotNumInLine;

	AChessBoard* Board = Cast<AChessBoard>(GetOwner());
	if (Board)
	{
		for (int i = 0; i < 4; i++)
		{
			int Direction = (i % 2) ? 1 : -1;
			for (int j = 1; j < EChessPieceColumn::SlotNumInLine; j++)
			{
				int index = INDEX_NONE;
				// i == 0, 1 (¡¬øÏ)
				if (i / 2 == 0)
				{
					int NextCol = (col + Direction * j);
					if (NextCol < 0 || NextCol >= EChessPieceColumn::SlotNumInLine) break;
					index = row * EChessPieceColumn::SlotNumInLine + NextCol;

				}
				// i == 2, 3 (ªÛ«œ)
				else
				{
					int NextRow = (row + Direction * j);
					if (NextRow < 0 || NextRow >= EChessPieceColumn::SlotNumInLine) break;
					index = NextRow * 8 + col;
				}

				if (index >= 0 && index < EChessPieceColumn::TotalSlotInBoard)
				{
					UChessBoardSlotComponent* PathSlot = Board->GetSlotComponent(index);
					if (PathSlot && PathSlot->IsChessPieceExist())
					{
						ABaseChessPiece* ChessPiece = PathSlot->GetChessPieceOnSlot();
						if (ChessPiece && ChessPiece->IsEnemy(this))
						{
							AvailableBoardSlots.Add(index);
						}
						break;
					}

					AvailableBoardSlots.Add(index);
				}
			}
		}
	}

	return AvailableBoardSlots;
}

TArray<int> ARookChessPiece::CalculatePathToSlot(int SlotIndex)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard && CurrentPlacedSlot)
	{
		int CurrentRow = CurrentPlacedSlot->GetSlotIndex() / 8;
		int CurrentCol = CurrentPlacedSlot->GetSlotIndex() % 8;

		int TargetRow = SlotIndex / 8;
		int TargetCol = SlotIndex % 8;

		int RowDist = (CurrentRow > TargetRow) ? (CurrentRow - TargetRow) : (TargetRow - CurrentRow);
		int ColDist = (CurrentCol > TargetCol) ? (CurrentCol - TargetCol) : (TargetCol - CurrentCol);

		int Dist = 0;
		int RowDirection = 0;
		int ColDirection = 0;
		if (RowDist > 0 && ColDist == 0)
		{
			Dist = RowDist;
			RowDirection = (CurrentRow > TargetRow) ? -1 : 1;
		}
		else if (RowDist == 0 && ColDist > 0)
		{
			Dist = ColDist;
			ColDirection = (CurrentCol > TargetCol) ? -1 : 1;
		}
		else
		{
			return TArray<int>();
		}

		TArray<int> Path;
		for (int i = 1; i <= Dist; i++)
		{
			int PathSlotIndex = (CurrentRow + i * RowDirection) * EChessPieceColumn::SlotNumInLine + (CurrentCol + i * ColDirection);

			if (PathSlotIndex >= 0 && PathSlotIndex < EChessPieceColumn::TotalSlotInBoard)
			{
				UChessBoardSlotComponent* PathSlot = ChessBoard->GetSlotComponent(PathSlotIndex);
				if (PathSlot)
				{
					ABaseChessPiece* ChessPiece = PathSlot->GetChessPieceOnSlot();
					if (ChessPiece)
					{
						if (IsEnemy(ChessPiece))
							Path.Add(PathSlotIndex);

						break;
					}
					else
					{
						Path.Add(PathSlotIndex);
					}
				}
			}
		}	

		return Path;
	}

	return TArray<int>();
}