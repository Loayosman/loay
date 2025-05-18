import random
import sys

def load_board(filename):
    with open(filename) as f:
        return [list(line.strip()) for line in f.readlines() if len(line.strip()) == 8]

def is_white(piece):
    return piece.isupper()

def is_black(piece):
    return piece.islower()

def piece_value(p):
    values = {'p': 1, 'n': 3, 'b': 3, 'r': 5, 'q': 9, 'k': 100}
    return values.get(p.lower(), 0)

def find_pawn_moves(board, r, c):
    moves = []
    # Move forward
    if r + 1 < 8 and board[r + 1][c] == '.':
        moves.append(((r, c), (r + 1, c)))
        if r == 1 and board[r + 2][c] == '.':
            moves.append(((r, c), (r + 2, c)))
    # Capture diagonally
    for dc in [-1, 1]:
        nr, nc = r + 1, c + dc
        if 0 <= nr < 8 and 0 <= nc < 8 and is_white(board[nr][nc]):
            moves.append(((r, c), (nr, nc)))
    return moves

def find_all_black_moves(board):
    moves = []
    for r in range(8):
        for c in range(8):
            if board[r][c] == 'p':
                moves.extend(find_pawn_moves(board, r, c))
    return moves

def evaluate_move(board, move):
    (_, _), (r2, c2) = move
    target = board[r2][c2]
    return piece_value(target)

def center_bonus(move):
    (_, _), (r2, c2) = move
    return -((r2 - 3.5)**2 + (c2 - 3.5)**2)

def move_to_str(move):
    (r1, c1), (r2, c2) = move
    return f"{chr(c1 + ord('a'))}{8 - r1}{chr(c2 + ord('a'))}{8 - r2}"

def choose_move(moves, board, difficulty):
    if difficulty == "easy":
        return random.choice(moves)
    elif difficulty == "medium":
        scored = sorted(moves, key=lambda m: (evaluate_move(board, m), center_bonus(m)), reverse=True)
        return scored[0]
    elif difficulty == "hard":
        # More advanced: value capture + center + forward move
        def hard_score(move):
            (r1, c1), (r2, c2) = move
            score = evaluate_move(board, move)
            if r2 > r1:  # forward
                score += 0.1
            score += (4 - abs(3.5 - c2)) * 0.05  # center file bonus
            return score
        return max(moves, key=hard_score)
    else:
        return random.choice(moves)

def main():
    if len(sys.argv) < 4:
        print("Usage: python ai.py board.txt move.txt [easy|medium|hard]")
        return

    board = load_board(sys.argv[1])
    output_file = sys.argv[2]
    difficulty = sys.argv[3].lower()

    moves = find_all_black_moves(board)

    if not moves:
        print("No valid moves available for AI.")
        return

    move = choose_move(moves, board, difficulty)
    move_str = move_to_str(move)

    with open(output_file, "w") as f:
        f.write(move_str)

    print(f"AI ({difficulty}) chose move: {move_str}")

if __name__ == "__main__":
    main()
