// Fill out your copyright notice in the Description page of Project Settings.


#include "QueenChessPiece.h"
#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/ChessBoard.h>
#include <ChessProject/ChessPiece/KingChessPiece.h>

TArray<int> AQueenChessPiece::CalculateAllPath()
{
	TArray<int> AvailableBoardSlots;
	int CurrentSlotIndex = CurrentPlacedSlot->GetSlotIndex();
	int col = CurrentSlotIndex % EChessPieceColumn::SlotNumInLine;
	int row = CurrentSlotIndex / EChessPieceColumn::SlotNumInLine;

	AChessBoard* Board = Cast<AChessBoard>(GetOwner());
	if (Board)
	{
		// Bishop Path
		for (int i = 0; i < 4; i++)
		{
			// [i = (row, col)] 0 = (-1, -1) && 1 = (1, -1) && 2 = (-1, 1) && 3 = (1, 1) 
			int BishopRowDirection = (i % 2) ? 1 : -1; // 0, 2 => -1 && 1, 3 => 1
			int BishopColDirection = (i / 2) ? 1 : -1; // 0, 1 => -1 && 2, 3 => 1

			for (int j = 1; j < EChessPieceColumn::SlotNumInLine; j++)
			{
				int BishopNextRow = (row + BishopRowDirection * j);
				int BishopNextCol = (col + BishopColDirection * j);
				if ((BishopNextRow < 0 || BishopNextRow >= EChessPieceColumn::SlotNumInLine) || (BishopNextCol < 0 || BishopNextCol >= EChessPieceColumn::SlotNumInLine))
				{
					break;
				}

				int BishopPathIndex = BishopNextRow * EChessPieceColumn::SlotNumInLine + BishopNextCol;
				if (BishopPathIndex >= 0 && BishopPathIndex < EChessPieceColumn::TotalSlotInBoard)
				{
					UChessBoardSlotComponent* PathSlot = Board->GetSlotComponent(BishopPathIndex);
					if (PathSlot && PathSlot->IsChessPieceExist())
					{
						ABaseChessPiece* ChessPiece = PathSlot->GetChessPieceOnSlot();
						if (ChessPiece && ChessPiece->IsEnemy(this))
						{
							AvailableBoardSlots.Add(BishopPathIndex);
						}
						break;
					}

					AvailableBoardSlots.Add(BishopPathIndex);
				}
			}
		}

		// Rook Path
		for (int i = 0; i < 4; i++)
		{
			int RookDirection = (i % 2) ? 1 : -1;

			for (int j = 1; j < EChessPieceColumn::SlotNumInLine; j++)
			{
				int RookPathIndex = INDEX_NONE;
				if (i / 2 == 0) // i == 0, 1 (¡¬øÏ)
				{
					int RookNextCol = (col + RookDirection * j);
					if (RookNextCol < 0 || RookNextCol >= EChessPieceColumn::SlotNumInLine) break;
					RookPathIndex = row * EChessPieceColumn::SlotNumInLine + RookNextCol;
				}
				else // i == 2, 3 (ªÛ«œ)
				{
					int RookNextRow = (row + RookDirection * j);
					if (RookNextRow < 0 || RookNextRow >= EChessPieceColumn::SlotNumInLine) break;
					RookPathIndex = RookNextRow * EChessPieceColumn::SlotNumInLine + col;
				}
				if (RookPathIndex >= 0 && RookPathIndex < EChessPieceColumn::TotalSlotInBoard)
				{
					UChessBoardSlotComponent* PathSlot = Board->GetSlotComponent(RookPathIndex);
					if (PathSlot && PathSlot->IsChessPieceExist())
					{
						ABaseChessPiece* ChessPiece = PathSlot->GetChessPieceOnSlot();
						if (ChessPiece && ChessPiece->IsEnemy(this))
						{
							AvailableBoardSlots.Add(RookPathIndex);
						}
						break;
					}

					AvailableBoardSlots.Add(RookPathIndex);
				}
			}
		}
	}

	return AvailableBoardSlots;
}

TArray<int> AQueenChessPiece::CalculatePathToSlot(int SlotIndex)
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
		else if (RowDist == ColDist)
		{
			Dist = RowDist;
			RowDirection = (CurrentRow > TargetRow) ? -1 : 1;
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