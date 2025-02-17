// Fill out your copyright notice in the Description page of Project Settings.


#include <ChessProject/ChessPiece/KingChessPiece.h>
#include <ChessProject/ChessBoardSlotComponent.h>
#include <ChessProject/ChessBoard.h>

#include <ChessProject/ChessPiece/RookChessPiece.h>

#include <Kismet/GameplayStatics.h>

bool AKingChessPiece::IsCheckedBy(ABaseChessPiece* Attacker) const
{
	// Can not check each king
	if (Attacker && !Attacker->IsA(AKingChessPiece::StaticClass()))
	{
		if (CurrentPlacedSlot)
		{
			int TargetSlotIndex = CurrentPlacedSlot->GetSlotIndex();

			if (Attacker->IsInRoute(TargetSlotIndex))
			{
				return true;
			}
		}
	}

	return false;
}

bool AKingChessPiece::IsCheckState()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);
		for (int i = 0; i < AllChessPieces.Num(); i++)
		{
			if (this->IsEnemy(AllChessPieces[i]) && this->IsCheckedBy(AllChessPieces[i]))
			{
				UE_LOG(LogTemp, Warning, TEXT("[ %s is Checking ]"), *AllChessPieces[i]->GetName());
				return true;
			}
		}
	}

	return false;
}

bool AKingChessPiece::IsCheckmate()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);

		// 체크 상태인 킹이 움직이지 못하면, 체크메이트
		if (IsCheckState())
		{
			// 킹 피하기 가능 여부 && 체크 중인 기물 (잡기 or 진로 방해) 가능 여부 
			bool bAvoidCheck = false;
			for (int i = 0; i < AllChessPieces.Num(); i++)
			{
				if (!this->IsEnemy(AllChessPieces[i]))
				{
					TArray<int> AvailablePath = AllChessPieces[i]->CalculatePathToAvoidCheck();
					if (!AvailablePath.IsEmpty())
					{
						bAvoidCheck = true;
						break;
					}
				}
			}

			return !bAvoidCheck;
		}
	}

	return false;
}

bool AKingChessPiece::IsStalemate()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);

		if (!IsCheckState())
		{
			// 킹 움직임 가능 여부 확인
			TArray<int> AvoidCheckPath = this->CalculatePathToAvoidCheck();
			bool bKingMovable = (!AvoidCheckPath.IsEmpty());

			// 킹을 제외한 나머지 움직임 가능 여부 확인
			bool bExistMovableChessPiece = false;
			for (int i = 0; i < AllChessPieces.Num(); i++)
			{
				if (!this->IsEnemy(AllChessPieces[i]) && this != AllChessPieces[i])
				{
					TArray<int> Path = AllChessPieces[i]->CalculateAllPath();
					if (!Path.IsEmpty())
					{
						bExistMovableChessPiece = true;
						break;
					}
				}
			}

			return (!bKingMovable && !bExistMovableChessPiece);
		}
	}

	return false;
}

TArray<int> AKingChessPiece::CalculateAllPath()
{
	TArray<int> AvailableBoardSlots;
	int CurrentSlotIndex = CurrentPlacedSlot->GetSlotIndex();
	int col = CurrentSlotIndex % EChessPieceColumn::SlotNumInLine;
	int row = CurrentSlotIndex / EChessPieceColumn::SlotNumInLine;

	AChessBoard* Board = Cast<AChessBoard>(GetOwner());
	if (Board)
	{
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0) continue;

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
					}
					// Except Ally ChessPiece Slot
					else
					{
						AvailableBoardSlots.Add(index);
					}
				}
			}
		}

		if (!IsCheckState())
		{
			if (IsPossibleCastling(EChessPieceColumn::RightRook))
			{
				int index = row * EChessPieceColumn::SlotNumInLine + EChessPieceColumn::RightRook;
				AvailableBoardSlots.Add(index);
			}

			if (IsPossibleCastling(EChessPieceColumn::LeftRook))
			{
				int index = row * EChessPieceColumn::SlotNumInLine + EChessPieceColumn::LeftRook;
				AvailableBoardSlots.Add(index);
			}
		}
	}

	return AvailableBoardSlots;
}

TArray<int> AKingChessPiece::CalculatePathToAvoidCheck()
{
	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		// Find Enemy Chess Pieces
		TArray<ABaseChessPiece*> AllChessPieces;
		ChessBoard->GetAllChessPieces(AllChessPieces);

		TArray<int> MovableSlots = this->CalculateAllPath();
		for (int i = 0; i < MovableSlots.Num(); i++)
		{
			for (int j = 0; j < AllChessPieces.Num(); j++)
			{
				if (this->IsEnemy(AllChessPieces[j]))
				{
					TArray<int> AttackerPath = AllChessPieces[j]->CalculatePathToSlot(MovableSlots[i]);

					if (AttackerPath.Find(MovableSlots[i]) != INDEX_NONE)
					{
						MovableSlots.RemoveAt(i);
						i--;
						break;
					}
				}
			}
		}

		return MovableSlots;
	}

	return TArray<int>();
}

TArray<int> AKingChessPiece::CalculatePathToSlot(int SlotIndex)
{

	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard && CurrentPlacedSlot)
	{
		int CurrentRow = CurrentPlacedSlot->GetSlotIndex() / 8;
		int CurrentCol = CurrentPlacedSlot->GetSlotIndex() % 8;

		int TargetRow = SlotIndex / 8;
		int TargetCol = SlotIndex % 8;

		int RowDist = CurrentRow - TargetRow;
		int ColDist = CurrentCol - TargetCol;

		if ((RowDist >= -1 && RowDist <= 1) && (ColDist >= -1 && ColDist <= 1))
		{
			if (SlotIndex >= 0 && SlotIndex < EChessPieceColumn::TotalSlotInBoard)
			{
				UChessBoardSlotComponent* DestSlot =  ChessBoard->GetSlotComponent(SlotIndex);
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

bool AKingChessPiece::IsPossibleCastling(EChessPieceColumn RookColumn)
{
	bool bCanCastling = false;

	AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
	if (ChessBoard)
	{
		int StartIndex, EndIndex, Direction;
		switch (RookColumn)
		{
		case EChessPieceColumn::LeftRook:
			StartIndex = EChessPieceColumn::LeftRook;
			EndIndex = EChessPieceColumn::King;
			Direction = -1;
			break;
		case EChessPieceColumn::RightRook:
			StartIndex = EChessPieceColumn::King;
			EndIndex = EChessPieceColumn::RightRook;
			Direction = 1;
			break;
		default:
			return false;
		}

		if (IsNotMove())
		{
			int CurrentPosition = GetCurrentPlacedSlotIndex();
			int CurrentRow = CurrentPosition / EChessPieceColumn::SlotNumInLine;
			int CurrentCol = CurrentPosition % EChessPieceColumn::SlotNumInLine;

			// 킹과 룩이 한 번도 움직이지 않음 && 킹과 룩 사이가 비어 있음
			if (CurrentCol == EChessPieceColumn::King)
			{
				for (int i = StartIndex; i <= EndIndex; i++)
				{
					if (i == EChessPieceColumn::King) continue;

					int index = CurrentRow * EChessPieceColumn::SlotNumInLine + i;
					if (i == EChessPieceColumn::LeftRook || i == EChessPieceColumn::RightRook)
					{
						UChessBoardSlotComponent* RookExistSlot = ChessBoard->GetSlotComponent(index);
						if (RookExistSlot && RookExistSlot->IsChessPieceExist())
						{
							ARookChessPiece* TargetRook = Cast<ARookChessPiece>(RookExistSlot->GetChessPieceOnSlot());
							if (TargetRook && !this->IsEnemy(TargetRook) && TargetRook->IsNotMove())
							{
								bCanCastling = true;
							}
						}
					}
					else
					{
						UChessBoardSlotComponent* RookNotExistSlot = ChessBoard->GetSlotComponent(index);
						if (RookNotExistSlot && RookNotExistSlot->IsChessPieceExist())
						{
							return false;
						}
					}
				}
			}

			// (원래 위치 & 거쳐가야 할 칸 & 목적지) 3칸이 공격받고 있으면, 캐슬링 불가
			if (bCanCastling)
			{
				TArray<ABaseChessPiece*> AllChessPieces;
				ChessBoard->GetAllChessPieces(AllChessPieces);

				for (int i = 0; i <= 2; i++)
				{
					int CastlingSlotIndex = CurrentRow * EChessPieceColumn::SlotNumInLine + (EChessPieceColumn::King + i * Direction);

					for (int j = 0; j < AllChessPieces.Num(); j++)
					{
						if (this->IsEnemy(AllChessPieces[j]))
						{
							TArray<int> CastlingCheckablePath = AllChessPieces[j]->CalculatePathToSlot(CastlingSlotIndex);
							if (CastlingCheckablePath.Find(CastlingSlotIndex) != INDEX_NONE)
							{
								return false;
							}
						}
					}
				}
			}
		}
	}

	return bCanCastling;
}

void AKingChessPiece::Castling(class ARookChessPiece* CastlingTarget)
{
	if (CastlingTarget)
	{
		UChessBoardSlotComponent* SrcRookSlot = CastlingTarget->GetCurrentPlacedSlot();

		if (CurrentPlacedSlot && SrcRookSlot)
		{
			int Row = CurrentPlacedSlot->GetSlotIndex() / 8;
			int KingExistedCol = CurrentPlacedSlot->GetSlotIndex() % 8;
			int CastlingTargetExistedCol = SrcRookSlot->GetSlotIndex() % 8;

			// Calculate Destination Position
			int DestKingCol = INDEX_NONE;
			int DestRookCol = INDEX_NONE;
			if (CastlingTargetExistedCol > KingExistedCol) // (Queen Side Castling)
			{
				DestRookCol = KingExistedCol + 1;
				DestKingCol = DestRookCol + 1;
			}
			else // (King Side Castling)
			{
				DestRookCol = KingExistedCol - 1;
				DestKingCol = DestRookCol - 1;
			}

			AChessBoard* ChessBoard = Cast<AChessBoard>(GetOwner());
			if (ChessBoard)
			{
				// King Move
				if (DestKingCol >= 0 && DestKingCol < EChessPieceColumn::SlotNumInLine)
				{
					int index = Row * BOARD_LINE_NUM + DestKingCol;
					UChessBoardSlotComponent* DestSlot = ChessBoard->GetSlotComponent(index);
					if (DestSlot && !DestSlot->IsChessPieceExist())
					{
						this->Move(DestSlot);
					}
				}

				// Rook Move
				if (DestRookCol >= 0 && DestRookCol < EChessPieceColumn::SlotNumInLine)
				{
					int index = Row * BOARD_LINE_NUM + DestRookCol;
					UChessBoardSlotComponent* DestSlot = ChessBoard->GetSlotComponent(index);
					if (DestSlot && !DestSlot->IsChessPieceExist())
					{
						CastlingTarget->Move(DestSlot);
					}
				}
			}
		}
	}
}