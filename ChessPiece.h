#include <iostream>
#include <vector>
#include <unordered_set>

enum class TEAM : uint8_t { WHITE, BLACK };
enum class TYPE : uint8_t { PAWN, ROOK, BISHOP, KNIGHT, QUEEN, KING };

struct CHESS_PIECE
{
	CHESS_PIECE(std::vector<std::shared_ptr<CHESS_PIECE>>& chess_board, TEAM _team, int _pos)
		: m_cBoard(chess_board), team(_team), curr_pos(_pos) {}

	virtual std::unordered_set<int> GetPathTo(int dest) const = 0;
	virtual std::unordered_set<int> GetAttackPathTo(int dest) const { return GetPathTo(dest); }
	virtual std::unordered_set<int> GetAllPath() const = 0;
	virtual void Move(int dest);
	virtual bool TypeCheck(TYPE type) const = 0;
	inline bool IsEnemyExistSlot(int slot_idx) const;

	TEAM team;
	int curr_pos;
	std::vector<std::shared_ptr<CHESS_PIECE>>& m_cBoard;
};

struct PAWN_CHESS_PIECE : virtual CHESS_PIECE
{
	PAWN_CHESS_PIECE(std::vector<std::shared_ptr<CHESS_PIECE>>& chess_board, TEAM _team, int _pos) : CHESS_PIECE(chess_board, _team, _pos) {}

	bool IsPossiblePromotion();
	std::unordered_set<int> GetPathTo(int dest) const override;
	std::unordered_set<int> GetAttackPathTo(int dest) const override;
	std::unordered_set<int> GetAllPath() const override;
	void Move(int dest) override;
	bool TypeCheck(TYPE type) const override { return (type == TYPE::PAWN); }

	int move_cnt = 0;
};

struct ROOK_CHESS_PIECE : virtual CHESS_PIECE
{
	ROOK_CHESS_PIECE(std::vector<std::shared_ptr<CHESS_PIECE>>& chess_board, TEAM _team, int _pos) : CHESS_PIECE(chess_board, _team, _pos) {}

	std::unordered_set<int> GetPathTo(int dest) const override;
	std::unordered_set<int> GetAllPath() const override;
	bool TypeCheck(TYPE type) const override { return (type == TYPE::ROOK); }
};

struct KNIGHT_CHESS_PIECE : virtual CHESS_PIECE
{
	KNIGHT_CHESS_PIECE(std::vector<std::shared_ptr<CHESS_PIECE>>& chess_board, TEAM _team, int _pos) : CHESS_PIECE(chess_board, _team, _pos) {}

	std::unordered_set<int> GetPathTo(int dest) const override;
	std::unordered_set<int> GetAllPath() const override;
	bool TypeCheck(TYPE type) const override { return (type == TYPE::KNIGHT); }
};

struct BISHOP_CHESS_PIECE : virtual CHESS_PIECE
{
	BISHOP_CHESS_PIECE(std::vector<std::shared_ptr<CHESS_PIECE>>& chess_board, TEAM _team, int _pos) : CHESS_PIECE(chess_board, _team, _pos) {}

	std::unordered_set<int> GetPathTo(int dest) const override;
	std::unordered_set<int> GetAllPath() const override;
	bool TypeCheck(TYPE type) const override { return (type == TYPE::BISHOP); }
};

struct QUEEN_CHESS_PIECE : virtual CHESS_PIECE, virtual ROOK_CHESS_PIECE, virtual BISHOP_CHESS_PIECE
{
	QUEEN_CHESS_PIECE(std::vector<std::shared_ptr<CHESS_PIECE>>& chess_board, TEAM _team, int _pos) 
		: CHESS_PIECE(chess_board, _team, _pos), ROOK_CHESS_PIECE(chess_board, _team, _pos), BISHOP_CHESS_PIECE(chess_board, _team, _pos) {}

	std::unordered_set<int> GetPathTo(int dest) const override;
	std::unordered_set<int> GetAllPath() const override;
	bool TypeCheck(TYPE type) const override { return (type == TYPE::QUEEN); }
};

struct KING_CHESS_PIECE : virtual CHESS_PIECE
{
	KING_CHESS_PIECE(std::vector<std::shared_ptr<CHESS_PIECE>>& chess_board, TEAM _team, int _pos) : CHESS_PIECE(chess_board, _team, _pos) {}

	bool IsPossibleCastling(int rook_pos);
	std::unordered_set<int> GetPathTo(int dest) const override;
	std::unordered_set<int> GetAllPath() const override;
	bool TypeCheck(TYPE type) const override { return (type == TYPE::KING); }
};