#include "ChessGameManager.h"

#include "ChessGamePacket.hpp"

void ChessGameManager::GameStart()
{
	InitBoard();

	game_round = 1;
}

void ChessGameManager::InitGame()
{
	std::fill(m_cBoard.begin(), m_cBoard.end(), nullptr);

	game_round = 0;
}

void ChessGameManager::InitBoard()
{
	std::fill(m_cBoard.begin(), m_cBoard.end(), nullptr);

	enum ROW : uint8_t {WHITE_FIRST_ROW = 0, WHITE_SECOND_ROW = 1, BLACK_SECOND_ROW = 6, BLACK_FIRST_ROW = 7};
	enum COL : uint8_t {LEFT_ROOK, LEFT_KNIGHT, LEFT_BISHOP, QUEEN, KING, RIGHT_BISHOP, RIGHT_KNIGHT, RIGHT_ROOK};

	for (int col = 0; col < 8; col++)
	{
		int pos = -1;

		pos = WHITE_SECOND_ROW * 8 + col;
		m_cBoard[pos] = std::make_shared<PAWN_CHESS_PIECE>(m_cBoard, TEAM::WHITE, pos);

		pos = BLACK_SECOND_ROW * 8 + col;
		m_cBoard[pos] = std::make_shared<PAWN_CHESS_PIECE>(m_cBoard, TEAM::BLACK, pos);

		switch (col)
		{
		case LEFT_ROOK: case RIGHT_ROOK:
			pos = WHITE_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<ROOK_CHESS_PIECE>(m_cBoard, TEAM::WHITE, pos);

			pos = BLACK_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<ROOK_CHESS_PIECE>(m_cBoard, TEAM::BLACK, pos);
			break;
		case LEFT_KNIGHT: case RIGHT_KNIGHT:
			pos = WHITE_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<KNIGHT_CHESS_PIECE>(m_cBoard, TEAM::WHITE, pos);

			pos = BLACK_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<KNIGHT_CHESS_PIECE>(m_cBoard, TEAM::BLACK, pos);
			break;
		case LEFT_BISHOP: case RIGHT_BISHOP:
			pos = WHITE_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<BISHOP_CHESS_PIECE>(m_cBoard, TEAM::WHITE, pos);

			pos = BLACK_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<BISHOP_CHESS_PIECE>(m_cBoard, TEAM::BLACK, pos);
			break;
		case QUEEN:
			pos = WHITE_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<QUEEN_CHESS_PIECE>(m_cBoard, TEAM::WHITE, pos);

			pos = BLACK_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<QUEEN_CHESS_PIECE>(m_cBoard, TEAM::BLACK, pos);
			break;
		case KING:
			pos = WHITE_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<KING_CHESS_PIECE>(m_cBoard, TEAM::WHITE, pos);

			pos = BLACK_FIRST_ROW * 8 + col;
			m_cBoard[pos] = std::make_shared<KING_CHESS_PIECE>(m_cBoard, TEAM::BLACK, pos);
			break;
		}
	}
}

bool ChessGameManager::Move(int src, int dest)
{
	std::unordered_set<int> available_path = m_cBoard[src]->GetPathTo(dest);

	bool bCanReach = available_path.find(dest) != available_path.end();
	if (bCanReach)
	{
		m_cBoard[src]->Move(dest);

		// Not Update Round (When pawn can Promote)
		std::shared_ptr<PAWN_CHESS_PIECE> pawn = std::dynamic_pointer_cast<PAWN_CHESS_PIECE>(m_cBoard[dest]);
		if (pawn)
		{
			if (pawn->IsPossiblePromotion()) 
				return true;
		}

		NextRound();

		return true;
	}

	return false;
}

bool ChessGameManager::Castling(int king_src, int rook_src)
{
	std::shared_ptr<KING_CHESS_PIECE> king = std::dynamic_pointer_cast<KING_CHESS_PIECE>(m_cBoard[king_src]);
	if (king)
	{
		if (king->IsPossibleCastling(rook_src))
		{
			int king_dest = -1;
			int rook_dest = -1;

			// Left Side Castling
			if (king_src > rook_src)
			{
				rook_dest = king_src - 1;
				king_dest = rook_dest - 1;
			}
			// Right Side Castling
			else
			{
				king_dest = rook_src - 1;
				rook_dest = king_dest - 1;
			}

			// Process Castling
			if (king_dest >= 0 && rook_dest >= 0)
			{
				m_cBoard[king_src]->Move(king_dest);
				m_cBoard[rook_src]->Move(rook_dest);

				NextRound();

				return true;
			}
		}
	}

	return false;
}

bool ChessGameManager::Promotion(int src, int promotion_type)
{
	std::shared_ptr<PAWN_CHESS_PIECE> pawn = std::dynamic_pointer_cast<PAWN_CHESS_PIECE>(m_cBoard[src]);
	if (pawn)
	{
		if (pawn->IsPossiblePromotion())
		{
			TEAM team = m_cBoard[src]->team;
			m_cBoard[src].reset();

			// Is Valid Promotion Type
			TYPE PromotionType = static_cast<TYPE>(promotion_type);
			switch (PromotionType)
			{
			case TYPE::ROOK: m_cBoard[src] = std::make_shared<ROOK_CHESS_PIECE>(m_cBoard, team, src); break;
			case TYPE::KNIGHT: m_cBoard[src] = std::make_shared<KNIGHT_CHESS_PIECE>(m_cBoard, team, src); break;
			case TYPE::BISHOP: m_cBoard[src] = std::make_shared<BISHOP_CHESS_PIECE>(m_cBoard, team, src); break;
			case TYPE::QUEEN: m_cBoard[src] = std::make_shared<QUEEN_CHESS_PIECE>(m_cBoard, team, src); break;
			default: return false;
			}

			NextRound();

			return true;
		}
	}

	return false;
}

uint8_t ChessGameManager::IsGameOver(int before_round)
{
	TEAM ally_team = (before_round % 2 == 1) ? (TEAM::WHITE) : (TEAM::BLACK);
	TEAM enemy_team = (ally_team == TEAM::WHITE) ? (TEAM::BLACK) : (TEAM::WHITE);

	// Find King
	int enemy_king_idx = FindKing(enemy_team);

	// Find Enemy Chess Pieces
	std::vector<int> AllyChessPieces = FindAllChessPieces(ally_team);
	std::vector<int> EnemyChessPieces = FindAllChessPieces(enemy_team);

	std::unordered_set<int> all_check_path = FindAllAttackPath(enemy_king_idx, AllyChessPieces);
	bool bIsChecked = (all_check_path.find(enemy_king_idx) != all_check_path.end());

	// Is Check State
	if (bIsChecked)
	{
		// Is Checkmate State

		// Check Blockable Check State (Block Path or Catch Checker)
		for (int& enemy_slot : EnemyChessPieces)
		{
			if (m_cBoard[enemy_slot]->TypeCheck(TYPE::KING)) continue;

			// Check Unmovable Chess Piece (== Aleady Blocking) ==> Can Nothing
			bool bIsBlockingCheck = all_check_path.find(enemy_slot) != all_check_path.end();
			if (bIsBlockingCheck)
				continue;

			// Is Movable Chess Piece Blockable
			for (int checker_path_slot : all_check_path)
			{
				std::unordered_set<int> enemy_path_to_block_check = m_cBoard[enemy_slot]->GetAttackPathTo(checker_path_slot);

				bool bCanBlock = (enemy_path_to_block_check.find(checker_path_slot) != enemy_path_to_block_check.end());
				if (bCanBlock)
				{
					return PACKET_DATA::CHESS_EVENT::EVENT_TYPE::CHECK;
				}
			}
		}

		// Check Avoid Check State
		for (int row = -1; row <= 1; row++)
		{
			for (int col = -1; col <= 1; col++)
			{
				int avoid_check_slot = (enemy_king_idx + (row * 8)) + (col);
				if (avoid_check_slot >= 0 && avoid_check_slot < 64)
				{
					// Find Safe Slot (except Ally ChessPiece Slot)
					if (m_cBoard[avoid_check_slot] != nullptr && m_cBoard[avoid_check_slot]->team == m_cBoard[enemy_king_idx]->team) continue;

					std::unordered_set<int> all_ally_attack_path = FindAllAttackPath(avoid_check_slot, AllyChessPieces);

					// Check Can Move to Safe Slot
					bool bCanAvoid = (all_ally_attack_path.find(avoid_check_slot) == all_ally_attack_path.end());
					if (bCanAvoid)
					{
						return PACKET_DATA::CHESS_EVENT::EVENT_TYPE::CHECK;
					}
				}
			}
		}

		return PACKET_DATA::CHESS_EVENT::EVENT_TYPE::CHECKMATE;
	}
	else
	{
		// Is Stalemate State

		// Check Movable
		for (int& enemy_slot : EnemyChessPieces)
		{
			// Check Unmovable Chess Piece (== Aleady Blocking) ==> Can Nothing
			bool bIsBlockingCheck = all_check_path.find(enemy_slot) != all_check_path.end();
			if (bIsBlockingCheck)
				continue;

			// Find enemy_piece can be placed slot
			std::unordered_set<int> enemy_movable_path = m_cBoard[enemy_slot]->GetAllPath();

			// Check King Movable
			if (m_cBoard[enemy_slot]->TypeCheck(TYPE::KING)) 
			{
				for (int king_movable_slot_idx : enemy_movable_path)
				{
					std::unordered_set<int> all_ally_attack_path = FindAllAttackPath(king_movable_slot_idx, AllyChessPieces);

					bool bIsMovableKing = all_ally_attack_path.empty();
					if (bIsMovableKing)
					{
						return PACKET_DATA::CHESS_EVENT::EVENT_TYPE::NONE;
					}
				}
			}
			// Check Others Movable
			else 
			{
				bool bIsMovable = !enemy_movable_path.empty();
				if (bIsMovable)
				{
					return PACKET_DATA::CHESS_EVENT::EVENT_TYPE::NONE;
				}
			}
		}

		return PACKET_DATA::CHESS_EVENT::EVENT_TYPE::STALEMATE;
	}

	return PACKET_DATA::CHESS_EVENT::EVENT_TYPE::NONE;
}

int ChessGameManager::FindKing(TEAM team)
{
	for (int i = 0; i < 64; i++)
	{
		if (m_cBoard[i])
		{
			if (m_cBoard[i]->TypeCheck(TYPE::KING) && m_cBoard[i]->team == team)
			{
				return i;
			}
		}
	}

	return -1;
}

std::unordered_set<int> ChessGameManager::FindAllAttackPath(int enemy_king_idx, std::vector<int> AllyChessPieces) const
{
	// Find Checker 
	std::unordered_set<int> all_attacker_path;
	for (int& ally_slot : AllyChessPieces)
	{
		std::unordered_set<int> attacker_path = m_cBoard[ally_slot]->GetAttackPathTo(enemy_king_idx);

		all_attacker_path.insert(attacker_path.begin(), attacker_path.end());
	}

	return all_attacker_path;
}

std::vector<int> ChessGameManager::FindAllChessPieces(TEAM team)
{
	std::vector<int> chess_pieces;

	for (int i = 0; i < 64; i++)
	{
		if (m_cBoard[i] != nullptr)
		{
			if (m_cBoard[i]->team == team)
			{
				chess_pieces.push_back(i);
			}
		}
	}

	return chess_pieces;
}