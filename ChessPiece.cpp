#include "ChessPiece.h"

void CHESS_PIECE::Move(int dest)
{
	// Move Piece
	m_cBoard[dest] = m_cBoard[curr_pos];
	
	// Delete Moved Piece
	m_cBoard[curr_pos].reset();

	curr_pos = dest;
}

bool CHESS_PIECE::IsEnemyExistSlot(int slot_idx) const
{
	if (m_cBoard[slot_idx])
	{
		if (team != m_cBoard[slot_idx]->team)
		{
			return true;
		}
	}

	return false;
}

bool PAWN_CHESS_PIECE::IsPossiblePromotion()
{
	// Is Valid Row (in White Team)
	if (team == TEAM::WHITE && curr_pos / 8 == 7)
	{
		return true;
	}
	// Is Valid Row (in Black Team)
	else if (team == TEAM::BLACK && curr_pos / 8 == 0)
	{
		return true;
	}
	// Is Invalid
	else
	{
		return false;
	}
}

std::unordered_set<int> PAWN_CHESS_PIECE::GetPathTo(int dest) const
{
	std::unordered_set<int> path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;
	int dest_row = dest / 8;
	int dest_col = dest % 8;

	int dir = (team == TEAM::WHITE) ? (1) : (-1);

	int row_differ = std::abs(curr_pos_row - dest_row);
	int col_differ = std::abs(curr_pos_col - dest_col);

	// Check Valid Direction (Is Forward)
	if ((dir == 1 && curr_pos_row >= dest_row) || (dir == -1 && curr_pos_row <= dest_row)) 
		return std::unordered_set<int>();

	// Move
	if (col_differ == 0 && row_differ <= 2)
	{
		for (int forward = 1; forward <= 2; forward++)
		{
			// Check Possible (First Move == Forwarding 2 Slot)
			if (forward == 2)
			{
				if (!((dir > 0 && curr_pos_row == 1) || (dir < 0 && curr_pos_row == 6)))
					break;
			}

			int slot_idx = ((curr_pos_row + forward * dir) * 8) + (curr_pos_col);
			if (slot_idx >= 0 && slot_idx < 64)
			{
				// Is Blocking
				if (m_cBoard[slot_idx] != nullptr) break;

				path.insert(slot_idx);
			}
		}
	}
	// Attack
	else if (col_differ == 1 && row_differ == 1) 
	{
		// Enpassant or Normal Attack
		int check_enpassant_slot = (curr_pos_row * 8) + (curr_pos_col + dir * col_differ);
		if (IsEnemyExistSlot(check_enpassant_slot) || IsEnemyExistSlot(dest))
		{
			path.insert(dest);
		}
	}

	return path;
}

std::unordered_set<int> PAWN_CHESS_PIECE::GetAttackPathTo(int dest) const
{
	std::unordered_set<int> path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;
	int dest_row = dest / 8;
	int dest_col = dest % 8;

	int dir = (team == TEAM::WHITE) ? (1) : (-1);

	int row_differ = std::abs(curr_pos_row - dest_row);
	int col_differ = std::abs(curr_pos_col - dest_col);

	// Check Valid Direction (Is Forward)
	if ((dir == 1 && curr_pos_row >= dest_row) || (dir == -1 && curr_pos_row <= dest_row))
		return std::unordered_set<int>();

	// Attack
	if (col_differ == 1 && row_differ == 1)
	{
		// Enpassant or Normal Attack
		int check_enpassant_slot = (curr_pos_row * 8) + (curr_pos_col + dir * col_differ);
		if (IsEnemyExistSlot(check_enpassant_slot) || IsEnemyExistSlot(dest))
		{
			path.insert(dest);
		}
	}

	return path;
}

std::unordered_set<int> PAWN_CHESS_PIECE::GetAllPath() const
{
	std::unordered_set<int> all_path;

	int dir = (team == TEAM::WHITE) ? (1) : (-1);
	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;

	int normal_move_slot = ((curr_pos_row + dir) * 8) + (curr_pos_col);
	if (normal_move_slot >= 0 && normal_move_slot < 64)
	{
		std::unordered_set<int> normal_move_path = GetPathTo(normal_move_slot);
		all_path.insert(normal_move_path.begin(), normal_move_path.end());
	}

	int first_move_double_slot = ((curr_pos_row + 2 * dir) * 8) + (curr_pos_col);
	if (first_move_double_slot >= 0 && first_move_double_slot < 64)
	{
		std::unordered_set<int> first_move_path = GetPathTo(first_move_double_slot);
		all_path.insert(first_move_path.begin(), first_move_path.end());
	}

	int left_attack_slot = ((curr_pos_row + dir) * 8) + (curr_pos_col - 1);
	if (left_attack_slot >= 0 && left_attack_slot < 64)
	{
		std::unordered_set<int> left_attack_path = GetPathTo(left_attack_slot);
		all_path.insert(left_attack_path.begin(), left_attack_path.end());
	}

	int right_attack_slot = ((curr_pos_row + dir) * 8) + (curr_pos_col + 1);
	if (right_attack_slot >= 0 && right_attack_slot < 64)
	{
		std::unordered_set<int> right_attack_path = GetPathTo(right_attack_slot);
		all_path.insert(right_attack_path.begin(), right_attack_path.end());
	}

	return all_path;
}

void PAWN_CHESS_PIECE::Move(int dest)
{
	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;
	int dest_col = dest % 8;

	// pawn move count (for Enpassant condition)
	move_cnt++;

	// Is Attack
	if (curr_pos_col != dest_col)
	{
		// Is Enpassant
		if (curr_pos_row == 3 || curr_pos_row == 4)
		{
			int check_enpassant_slot = (curr_pos_row * 8) + dest_col;
			if (m_cBoard[check_enpassant_slot] != nullptr)
			{
				if (IsEnemyExistSlot(check_enpassant_slot))
				{
					std::shared_ptr<PAWN_CHESS_PIECE> enpassnat_target = std::dynamic_pointer_cast<PAWN_CHESS_PIECE>(m_cBoard[check_enpassant_slot]);
					if (enpassnat_target && enpassnat_target->move_cnt > 1)
					{
						// Delete Enpassant Target
						m_cBoard[check_enpassant_slot].reset();
					}
				}
			}
		}
	}

	CHESS_PIECE::Move(dest);
}

std::unordered_set<int> ROOK_CHESS_PIECE::GetPathTo(int dest) const
{
	std::unordered_set<int> path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;

	int dest_row = dest / 8;
	int dest_col = dest % 8;

	int row_dir = (curr_pos_row < dest_row) ? (1) : (-1);
	int col_dir = (curr_pos_col < dest_col) ? (1) : (-1);

	// Is Same Col
	if (curr_pos_col == dest_col)
	{
		int row_differ = std::abs(curr_pos_row - dest_row);

		for (int i = 1; i <= row_differ; i++)
		{
			int slot_idx = ((curr_pos_row + i * row_dir) * 8) + (curr_pos_col);
			// Block
			if (m_cBoard[slot_idx])
			{
				// Path Block
				if (slot_idx != dest) break;

				// Enemy exist at Destination
				if (IsEnemyExistSlot(slot_idx))
					path.insert(slot_idx);
			}
			// Empty
			else
			{
				path.insert(slot_idx);
			}
		}
	}
	// Is Same Row
	else if (curr_pos_row == dest_row)
	{
		int col_differ = std::abs(curr_pos_col - dest_col);

		// Is Empty Between Src & Dest
		for (int i = 1; i <= col_differ; i++)
		{
			int slot_idx = (curr_pos_row * 8) + (curr_pos_col + col_dir * i);
			// Block
			if (m_cBoard[slot_idx])
			{
				// Path Block
				if (slot_idx != dest) break;

				// Enemy exist at Destination
				if (IsEnemyExistSlot(slot_idx))
					path.insert(slot_idx);
			}
			// Empty
			else
			{
				path.insert(slot_idx);
			}
		}
	}

	return path;
}

std::unordered_set<int> ROOK_CHESS_PIECE::GetAllPath() const
{
	std::unordered_set<int> all_path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;

	int left_end_slot = (curr_pos_row * 8) + 0;
	if (left_end_slot >= 0 && left_end_slot < 64)
	{
		std::unordered_set<int> left_end_path = GetPathTo(left_end_slot);
		all_path.insert(left_end_path.begin(), left_end_path.end());
	}

	int right_end_slot = (curr_pos_row * 8) + 7;
	if (right_end_slot >= 0 && right_end_slot < 64)
	{
		std::unordered_set<int> right_end_path = GetPathTo(right_end_slot);
		all_path.insert(right_end_path.begin(), right_end_path.end());
	}

	int top_end_slot = (7 * 8) + curr_pos_col;
	if (top_end_slot >= 0 && top_end_slot < 64)
	{
		std::unordered_set<int> top_end_path = GetPathTo(top_end_slot);
		all_path.insert(top_end_path.begin(), top_end_path.end());
	}

	int bottom_end_slot = (0 * 8) + curr_pos_col;
	if (bottom_end_slot >= 0 && bottom_end_slot < 64)
	{
		std::unordered_set<int> bottom_end_path = GetPathTo(bottom_end_slot);
		all_path.insert(bottom_end_path.begin(), bottom_end_path.end());
	}

	return all_path;
}

std::unordered_set<int> KNIGHT_CHESS_PIECE::GetPathTo(int dest) const
{
	std::unordered_set<int> path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;
	int dest_row = dest / 8;
	int dest_col = dest % 8;

	int row_differ = std::abs(curr_pos_row - dest_row);
	int col_differ = std::abs(curr_pos_col - dest_col);

	if ((row_differ == 2 && col_differ == 1) || (row_differ == 1 && col_differ == 2))
	{
		if (IsEnemyExistSlot(dest) || m_cBoard[dest] == nullptr)
			path.insert(dest);
	}

	return path;
}

std::unordered_set<int> KNIGHT_CHESS_PIECE::GetAllPath() const
{
	std::unordered_set<int> all_path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;

	for (int row = -2; row <= 2; row++)
	{
		if (row == 0) continue;

		for (int col = -2; col <= 2; col++)
		{
			if (col == 0) continue;
			if (std::abs(row) == std::abs(col)) continue;

			int slot_idx = ((curr_pos_row + row) * 8) + (curr_pos_col + col);
			if (slot_idx >= 0 && slot_idx < 64)
			{
				if (IsEnemyExistSlot(slot_idx) || m_cBoard[slot_idx] == nullptr)
					all_path.insert(slot_idx);
			}
		}
	}

	return all_path;
}

std::unordered_set<int> BISHOP_CHESS_PIECE::GetPathTo(int dest) const
{
	std::unordered_set<int> path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;

	int dest_row = dest / 8;
	int dest_col = dest % 8;

	int row_differ = std::abs(curr_pos_row - dest_row);
	int col_differ = std::abs(curr_pos_col - dest_col);

	// Is Moved Diagonal
	if (row_differ == col_differ)
	{
		int row_dir = (curr_pos_row < dest_row) ? (1) : (-1);
		int col_dir = (curr_pos_col < dest_col) ? (1) : (-1);

		for (int i = 1; i <= row_differ; i++)
		{
			int slot_idx = ((curr_pos_row + i * row_dir) * 8) + (curr_pos_col + i * col_dir);
			// Block
			if (m_cBoard[slot_idx])
			{
				// Path Block
				if (slot_idx != dest) break;

				// Enemy exist at Destination
				if (IsEnemyExistSlot(slot_idx))
					path.insert(slot_idx);
			}
			// Empty
			else
			{
				path.insert(slot_idx);
			}
		}
	}

	return path;
}

std::unordered_set<int> BISHOP_CHESS_PIECE::GetAllPath() const
{
	std::unordered_set<int> all_path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;

	int differ;
	int top_differ = 7 - curr_pos_row;
	int bottom_differ = curr_pos_row;
	int left_differ = curr_pos_col;
	int right_differ = 7 - curr_pos_col;

	differ = (left_differ < top_differ) ? (left_differ) : (top_differ);
	int left_top_slot = ((curr_pos_row + differ) * 8) + (curr_pos_col - differ);
	if (left_top_slot >= 0 && left_top_slot < 64)
	{
		std::unordered_set<int>left_top_path = GetPathTo(left_top_slot);
		all_path.insert(left_top_path.begin(), left_top_path.end());
	}

	differ = (right_differ < top_differ) ? (right_differ) : (top_differ);
	int right_top_slot = ((curr_pos_row + differ) * 8) + (curr_pos_col + differ);
	if (right_top_slot >= 0 && right_top_slot < 64)
	{
		std::unordered_set<int> right_top_path = GetPathTo(right_top_slot);
		all_path.insert(right_top_path.begin(), right_top_path.end());
	}

	differ = (left_differ < bottom_differ) ? (left_differ) : (bottom_differ);
	int left_bottom_slot = ((curr_pos_row - differ) * 8) + (curr_pos_col - differ);
	if (left_bottom_slot >= 0 && left_bottom_slot < 64)
	{
		std::unordered_set<int> left_bottom_path = GetPathTo(left_bottom_slot);
		all_path.insert(left_bottom_path.begin(), left_bottom_path.end());
	}

	differ = (right_differ < bottom_differ) ? (right_differ) : (bottom_differ);
	int right_bottom_slot = ((curr_pos_row - differ) * 8) + (curr_pos_col + differ);
	if (right_bottom_slot >= 0 && right_bottom_slot < 64)
	{
		std::unordered_set<int> right_bottom_path = GetPathTo(right_bottom_slot);
		all_path.insert(right_bottom_path.begin(), right_bottom_path.end());
	}

	return all_path;
}

std::unordered_set<int> QUEEN_CHESS_PIECE::GetPathTo(int dest) const
{
	std::unordered_set<int> path;

	std::unordered_set<int> rook_path = ROOK_CHESS_PIECE::GetPathTo(dest);
	path.insert(rook_path.begin(), rook_path.end());

	std::unordered_set<int> bishop_path = BISHOP_CHESS_PIECE::GetPathTo(dest);
	path.insert(bishop_path.begin(), bishop_path.end());

	return path;
}

std::unordered_set<int> QUEEN_CHESS_PIECE::GetAllPath() const
{
	std::unordered_set<int> all_path;

	std::unordered_set<int> rook_path = ROOK_CHESS_PIECE::GetAllPath();
	all_path.insert(rook_path.begin(), rook_path.end());

	std::unordered_set<int> bishop_path = BISHOP_CHESS_PIECE::GetAllPath();
	all_path.insert(bishop_path.begin(), bishop_path.end());

	return all_path;
}

bool KING_CHESS_PIECE::IsPossibleCastling(int rook_pos)
{
	// Is Valid Type
	if ((m_cBoard[curr_pos]->TypeCheck(TYPE::KING) == false) || (m_cBoard[rook_pos]->TypeCheck(TYPE::ROOK) == false))
		return false;

	// Is Same Team
	if (m_cBoard[curr_pos]->team != m_cBoard[rook_pos]->team)
		return false;

	// Is Existed in Same Row
	if ((curr_pos / 8) != (rook_pos / 8))
		return false;

	// Is Valid Row (in White Team)
	if (m_cBoard[curr_pos]->team == TEAM::WHITE && curr_pos / 8 != 0)
		return false;

	// Is Valid Row (in Black Team)
	if (m_cBoard[curr_pos]->team == TEAM::BLACK && curr_pos / 8 != 7)
		return false;

	// Is Empty Between King & Rook
	int first = std::min(curr_pos, rook_pos); // Left or Right Castling
	int last = first + std::abs(curr_pos - rook_pos); // Left or Right Castling
	for (int i = first + 1; i < last; i++)
	{
		if (m_cBoard[i] != nullptr)
			return false;
	}

	return true;
}

std::unordered_set<int> KING_CHESS_PIECE::GetPathTo(int dest) const
{
	std::unordered_set<int> path;

	int row_differ = ((curr_pos / 8) - (dest / 8));
	int col_differ = ((curr_pos % 8) - (dest % 8));

	// Is Moved (¡¾1, ¡¾1) or (¡¾1, 0) or (0, ¡¾1)
	if ((-1 <= row_differ && row_differ <= 1) && (-1 <= col_differ && col_differ <= 1))
	{
		if (IsEnemyExistSlot(dest) || m_cBoard[dest] == nullptr)
			 path.insert(dest);
	}

	return path;
}

std::unordered_set<int> KING_CHESS_PIECE::GetAllPath() const
{
	std::unordered_set<int> all_path;

	int curr_pos_row = curr_pos / 8;
	int curr_pos_col = curr_pos % 8;

	for (int row = -1; row <= 1; row++)
	{
		for (int col = -1; col <= 1; col++)
		{
			if (row == 0 && col == 0) continue;

			int slot_idx = ((curr_pos_row + row) * 8) + (curr_pos_col + col);
			if (slot_idx >= 0 && slot_idx < 64)
			{
				std::unordered_set<int> path = GetPathTo(slot_idx);
				all_path.insert(path.begin(), path.end());
			}
		}
	}

	return all_path;
}
