#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>

#include "ChessPiece.h"

class ChessGameManager
{
public:
	ChessGameManager()
	{
		m_cBoard.assign(64, nullptr);
	}
	~ChessGameManager() = default;

public:
	void GameStart();
	void InitGame();

private:
	void InitBoard();

public:
	bool Move(int src, int dest);
	bool Castling(int king_src, int rook_src);
	bool Promotion(int src, int promotion_type);

public:
	uint8_t IsGameOver(int before_round);

private:
	int FindKing(TEAM team);
	std::unordered_set<int> FindAllAttackPath(int enemy_king_idx, std::vector<int> AllyChessPieces) const;
	std::vector<int> FindAllChessPieces(TEAM team);

public:
	bool CheckSynchronize(int round) const { return (game_round == round); }
	void NextRound() { game_round++; }
	uint8_t GetGameRound() { return game_round; }

private:
	std::vector<std::shared_ptr<CHESS_PIECE>> m_cBoard;

	uint8_t game_round = 0;
};
