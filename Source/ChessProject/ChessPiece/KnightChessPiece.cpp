// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightChessPiece.h"
#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>

TArray<int> AKnightChessPiece::CalculateAllPath()
{
	TArray<int> AvailableBoardSlots;
	int CurrentSlotIndex = CurrentPlacedSlot->GetSlotIndex();
	int col = CurrentSlotIndex % EChessPieceColumn::SlotNumInLine;
	int row = CurrentSlotIndex / EChessPieceColumn::SlotNumInLine;

	AChessBoard* Board = Cast<AChessBoard>(GetOwner());
	if (Board)
	{
		for (int i = -2; i <= 2; i++)
		{
			if (i == 0) continue;

			for (int j = -2; j <= 2; j++)
			{
				if (j == 0 || abs(i) == abs(j)) continue;

				int NextRow = row + i;
				int NextCol = col + j;

				if ((NextRow >= 0 && NextRow < EChessPieceColumn::SlotNumInLine) && (NextCol >= 0 && NextCol < EChessPieceColumn::SlotNumInLine))
				{
					int index = NextRow * EChessPieceColumn::SlotNumInLine + NextCol;

					UChessBoardSlotComponent* PathSlot = Board->GetSlotComponent(index);
					if (PathSlot && PathSlot->IsChessPieceExist())
					{
						ABaseChessPiece* ChessPiece = PathSlot->GetChessPieceOnSlot();
						if (ChessPiece && ChessPiece->IsEnemy(this))
						{
							AvailableBoardSlots.Add(index);
						}
						continue;
					}

					AvailableBoardSlots.Add(index);
				}
			}
		}
	}

	return AvailableBoardSlots;
}

TArray<int> AKnightChessPiece::CalculatePathToSlot(int SlotIndex)
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

		if ((RowDist == 2 && ColDist == 1) || (RowDist == 1 && ColDist == 2))
		{
			if (SlotIndex >= 0 && SlotIndex < EChessPieceColumn::TotalSlotInBoard)
			{
				UChessBoardSlotComponent* DestSlot = ChessBoard->GetSlotComponent(SlotIndex);
				if (DestSlot)
				{
					TArray<int> Path;

					ABaseChessPiece* ChessPiece = DestSlot->GetChessPieceOnSlot();
					if (IsEnemy(ChessPiece) || ChessPiece != nullptr)
						Path.Add(SlotIndex);

					return Path;
				}
			}
		}
	}

	return TArray<int>();
}
