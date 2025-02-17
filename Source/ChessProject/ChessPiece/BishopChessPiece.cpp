// Fill out your copyright notice in the Description page of Project Settings.


#include "BishopChessPiece.h"

#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>

TArray<int> ABishopChessPiece::CalculateAllPath()
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
			// [i = (row, col)] 0 = (-1, -1) && 1 = (1, -1) && 2 = (-1, 1) && 3 = (1, 1) 
			int RowDirection = (i % 2) ? 1 : -1; // 0, 2 => -1 && 1, 3 => 1
			int ColDirection = (i / 2) ? 1 : -1; // 0, 1 => -1 && 2, 3 => 1

			for (int j = 1; j < EChessPieceColumn::SlotNumInLine; j++)
			{
				int NextRow = (row + RowDirection * j);
				int NextCol = (col + ColDirection * j);

				if ((NextRow < 0 || NextRow >= EChessPieceColumn::SlotNumInLine) || (NextCol < 0 || NextCol >= EChessPieceColumn::SlotNumInLine))
				{
					break;
				}

				int index = NextRow * EChessPieceColumn::SlotNumInLine + NextCol;
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

TArray<int> ABishopChessPiece::CalculatePathToSlot(int SlotIndex)
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard && CurrentPlacedSlot)
	{
		int CurrentRow = CurrentPlacedSlot->GetSlotIndex() / 8;
		int CurrentCol = CurrentPlacedSlot->GetSlotIndex() % 8;

		int TargetRow = SlotIndex / 8;
		int TargetCol = SlotIndex % 8;

		int RowDist = (CurrentRow > TargetRow) ? (CurrentRow - TargetRow) : (TargetRow - CurrentRow);
		int ColDist = (CurrentCol > TargetCol) ? (CurrentCol - TargetCol) : (CurrentCol - CurrentRow);

		if (RowDist == ColDist)
		{
			int Dist = RowDist;
			int RowDirection = (CurrentRow > TargetRow) ? -1 : 1;
			int ColDirection = (CurrentCol > TargetCol) ? -1 : 1;

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
						if (ChessPiece != nullptr)
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
	}

	return TArray<int>();
}