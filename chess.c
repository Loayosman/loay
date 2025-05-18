#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <windows.h>

#define BOARD_SIZE 8
#define MAX_MOVES 100

char board[BOARD_SIZE][BOARD_SIZE];
char ai_difficulty[16] = "easy";
char move_history[MAX_MOVES][5]; // Store moves in history
int move_count = 0; // Count of moves made

void init_board() {
    char *start[] = {"rnbqkbnr", "pppppppp", "        ", "        ",
                     "        ", "        ", "PPPPPPPP", "RNBQKBNR"};
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = start[i][j];
}

void print_board() {
    printf("  a b c d e f g h\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            char piece = board[i][j];
            switch (piece) {
                case 'k': printf("♔ "); break;
                case 'q': printf("♕ "); break;
                case 'r': printf("♖ "); break;
                case 'b': printf("♗ "); break;
                case 'n': printf("♘ "); break;
                case 'p': printf("♙ "); break;
                case 'K': printf("♚ "); break;
                case 'Q': printf("♛ "); break;
                case 'R': printf("♜ "); break;
                case 'B': printf("♝ "); break;
                case 'N': printf("♞ "); break;
                case 'P': printf("♟ "); break;
                default: printf(". "); break;
            }
        }
        printf("\n");
    }
    printf("  a b c d e f g h\n");
}

int in_bounds(int r, int c) {
    return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
}

int is_valid_move(int r1, int c1, int r2, int c2, int white_turn) {
    char piece = board[r1][c1];
    char dest = board[r2][c2];
    if (piece == ' ' || (white_turn && islower(piece)) || (!white_turn && isupper(piece)))
        return 0;
    if (dest != ' ' && ((white_turn && isupper(dest)) || (!white_turn && islower(dest))))
        return 0;
    return 1;
}

void make_move(int r1, int c1, int r2, int c2) {
    board[r2][c2] = board[r1][c1];
    board[r1][c1] = ' ';
    // Store the move in history
    snprintf(move_history[move_count], sizeof(move_history[move_count]), "%c%d%c%d", c1 + 'a', 8 - r1, c2 + 'a', 8 - r2);
    move_count++;
}

void undo_move() {
    if (move_count == 0) {
        printf("No moves to undo.\n");
        return;
    }
    move_count--;
    char last_move[5];
    strcpy(last_move, move_history[move_count]);
    
    int r1 = 8 - (last_move[1] - '0');
    int c1 = last_move[0] - 'a';
    int r2 = 8 - (last_move[3] - '0');
    int c2 = last_move[2] - 'a';

    // Restore the previous state
    board[r1][c1] = board[r2][c2];
    board[r2][c2] = ' ';
    printf("Undid move: %s\n", last_move);
}

int check_king_status() {
    int white_king = 0, black_king = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 'K') white_king = 1;
            if (board[i][j] == 'k') black_king = 1;
        }
    if (!white_king) { printf("Black wins! White king was captured.\n"); return -1; }
    if (!black_king) { printf("White wins! Black king was captured.\n"); return -1; }
    return 0;
}

int parse_position(const char *pos, int *row, int *col) {
    if (strlen(pos) != 2) return 0;
    *col = pos[0] - 'a';
    *row = 8 - (pos[1] - '0');
    return in_bounds(*row, *col);
}

void export_board(const char *filename) {
    FILE *f = fopen(filename, "w");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fputc(board[i][j] == ' ' ? '.' : board[i][j], f);
        }
        fputc('\n', f);
    }
    fclose(f);
}

int read_ai_move(const char *filename, int *r1, int *c1, int *r2, int *c2) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    char move[5];
    if (fscanf(f, "%4s", move) != 1) {
        fclose(f);
        return 0;
    }
    fclose(f);
    *c1 = move[0] - 'a';
    *r1 = 8 - (move[1] - '0');
    *c2 = move[2] - 'a';
    *r2 = 8 - (move[3] - '0');
    return in_bounds(*r1, *c1) && in_bounds(*r2, *c2);
}

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    init_board();

    printf("Choose AI difficulty (easy, medium, hard): ");
    scanf("%15s", ai_difficulty);

    int white_turn = 1;
    char input[4];
    char from[3], to[3];
    int r1, c1, r2, c2;

    while (1) {
        print_board();

        if (white_turn) {
            printf("White's move (e.g. e2 e4) or 'u' to undo double move: ");
            scanf("%3s", input);

            if (strcmp(input, "u") == 0) {
                int undo_times = 2;
                for (int i = 0; i < undo_times; i++) {
                    undo_move();
                    white_turn = !white_turn; 
                }
                continue;
            } 

            strcpy(from, input);
            printf("to: ");
            scanf("%2s", to);

            if (!parse_position(from, &r1, &c1) || !parse_position(to, &r2, &c2)) {
                printf("Invalid input. Use format like e2 e4.\n");
                continue;
            }

            if (!is_valid_move(r1, c1, r2, c2, white_turn)) {
                printf("Invalid move.\n");
                continue;
            }

            make_move(r1, c1, r2, c2);
        } else {
            printf("AI (%s) is thinking...\n", ai_difficulty);
            export_board("board.txt");

            char command[128];
            snprintf(command, sizeof(command), "python ai.py board.txt move.txt %s", ai_difficulty);
            system(command);

            if (!read_ai_move("move.txt", &r1, &c1, &r2, &c2)) {
                printf("AI move failed.\n");
                break;
            }

            make_move(r1, c1, r2, c2);
        }

        if (check_king_status() == -1)
            break;

        white_turn = !white_turn;
    }

    system("pause");
    return 0;
}
