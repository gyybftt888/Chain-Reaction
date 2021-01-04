#include <iostream>
#include <stdlib.h>
#include <time.h>
//#include "../include/algorithm.h"
#include "algorithm.h"
//#include "../include/rules.h"
#include "rules.h"

using namespace std;

/******************************************************
 * In your algorithm, you can just use the the funcitons
 * listed by TA to get the board information.(functions 
 * 1. ~ 4. are listed in next block)
 * 
 * The STL library functions is not allowed to use.
******************************************************/

/*************************************************************************
 * 1. int board.get_orbs_num(int row_index, int col_index)
 * 2. int board.get_capacity(int row_index, int col_index)
 * 3. char board.get_cell_color(int row_index, int col_index)
 * 4. void board.print_current_board(int row_index, int col_index, int round)
 * 
 * 1. The function that return the number of orbs in cell(row, col)
 * 2. The function that return the orb capacity of the cell(row, col)
 * 3. The function that return the color fo the cell(row, col)
 * 4. The function that print out the current board statement
*************************************************************************/


#define ROW 5
#define COL 6

typedef struct Position {
    int x, y;
    void Set(int i, int j) {
        x = i;
        y = j;
    }
} Grid;

class myboard {
public:
    Cell cells[ROW][COL];                       // The 5*6 board whose index (0,0) is start from the upper left corner
    void cell_reaction_marker();                // After the explosion, mark all the cell that  will explode in next iteration
    bool cell_is_full(Cell* cell);              // Check wether the cell is full of orbs and set the explosion variable to be true
    void add_orb(int i, int j, char color);     // Add orb into the cell (i, j)
    void cell_reset(int i, int j);              // Reset the cell to the initial state (color = 'w', orb_num = 0)
    void cell_explode(int i, int j);            // The explosion of cell (i, j). It will add the orb into neighbor cells
    void cell_chain_reaction(Player player);    // If there is aa explosion, check wether the chain reaction is active

public:
    myboard();
    myboard(Board board);

    // The basic functions of the M_Board
    int get_orbs_num(int i, int j);
    int get_capacity(int i, int j);
    char get_cell_color(int i, int j);
    bool place_orb(int i, int j, Player* player);      // Use this function to place a orb into a cell
    bool win_the_game(Player player);                  // The function that is used to check wether the player wins the game after his/her placemnet operation
};

myboard::myboard() {

    ////// Initialize the borad with correct capacity //////
    // The corners of the board
    cells[0][0].set_capacity(3), cells[0][5].set_capacity(3),
    cells[4][0].set_capacity(3), cells[4][5].set_capacity(3);

    // The edges of the board
    cells[0][1].set_capacity(5), cells[0][2].set_capacity(5), cells[0][3].set_capacity(5), cells[0][4].set_capacity(5),
    cells[1][0].set_capacity(5), cells[2][0].set_capacity(5), cells[3][0].set_capacity(5),
    cells[1][5].set_capacity(5), cells[2][5].set_capacity(5), cells[3][5].set_capacity(5),
    cells[4][1].set_capacity(5), cells[4][2].set_capacity(5), cells[4][3].set_capacity(5), cells[4][4].set_capacity(5);

}

myboard::myboard(Board board) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            cells[i][j].set_color(board.get_cell_color(i, j));
            cells[i][j].set_capacity(board.get_capacity(i, j));
            cells[i][j].set_orbs_num(board.get_orbs_num(i, j));
        }
    }
}

bool myboard::place_orb(int i, int j, Player* player) {

    if (!index_range_illegal(i, j) && !placement_illegal(*player, cells[i][j])) {
        int temp = cells[i][j].get_orbs_num();
        temp += 1;
        cells[i][j].set_orbs_num(temp);
        cells[i][j].set_color(player->get_color());
    }
    else {
        player->set_illegal();
        return false;
    }

    if (cell_is_full(&cells[i][j])) {
        cell_explode(i, j);
        cell_reaction_marker();
        cell_chain_reaction(*player);
    }

    return true;
}

bool myboard::cell_is_full(Cell* cell) {
    if (cell->get_orbs_num() >= cell->get_capacity()) {
        cell->set_explode(true);
        return true;
    }
    else return false;
}

void myboard::add_orb(int i, int j, char color) {
    int orb_num = cells[i][j].get_orbs_num();
    orb_num++;
    cells[i][j].set_orbs_num(orb_num);
    cells[i][j].set_color(color);
}

void myboard::cell_reset(int i, int j) {
    cells[i][j].set_orbs_num(0);
    cells[i][j].set_explode(false);
    cells[i][j].set_color('w');
}

void myboard::cell_explode(int i, int j) {
    int orb_num;
    char color = cells[i][j].get_color();

    cell_reset(i, j);

    if (i + 1 < ROW) {
        add_orb(i + 1, j, color);
    }

    if (j + 1 < COL) {
        add_orb(i, j + 1, color);
    }

    if (i - 1 >= 0) {
        add_orb(i - 1, j, color);
    }

    if (j - 1 >= 0) {
        add_orb(i, j - 1, color);
    }
    if (i + 1 < ROW && j - 1 >= 0) {
        add_orb(i + 1, j - 1, color);
    }
    if (i - 1 >= 0 && j - 1 >= 0) {
        add_orb(i - 1, j - 1, color);
    }
    if (i + 1 < ROW && j + 1 < COL) {
        add_orb(i + 1, j + 1, color);
    }
    if (i - 1 >= 0 && j + 1 < COL) {
        add_orb(i - 1, j + 1, color);
    }
}

void myboard::cell_reaction_marker() {

    // Mark the next cell whose number of orbs is equal to the capacity
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            cell_is_full(&cells[i][j]);
        }
    }
}

void myboard::cell_chain_reaction(Player player) {

    bool chain_reac = true;

    while (chain_reac) {

        chain_reac = false;

        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (cells[i][j].get_explode()) {
                    cell_explode(i, j);
                    chain_reac = true;
                }
            }
        }

        if (win_the_game(player)) {
            return;
        }

        cell_reaction_marker();
    }
}

bool myboard::win_the_game(Player player) {

    char player_color = player.get_color();
    bool win = true;

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (cells[i][j].get_color() == player_color || cells[i][j].get_color() == 'w') continue;
            else {
                win = false;
                break;
            }
        }
        if (!win) break;
    }
    return win;
}

int myboard::get_orbs_num(int i, int j) {
    return cells[i][j].get_orbs_num();
}

int myboard::get_capacity(int i, int j) {
    return cells[i][j].get_capacity();
}

char myboard::get_cell_color(int i, int j) {
    return cells[i][j].get_color();
}

Grid* SearchNeigbors(int i, int j);
myboard move(myboard board, Grid pos, Player player);
int score(myboard board, Player player);
pair<Grid, int> minimax(myboard board, int depth, int breadth, Player player);

void algorithm_A(Board board, Player player, int index[]) {
    myboard m_board(board);
    pair<Grid, int> best_move = minimax(m_board, 3, 10, player);
    index[0] = best_move.first.x;
    index[1] = best_move.first.y;
};

int* bestn(myboard board, Player player) {
    int* conf = new int[100];
    for (int i = 0; i < 100; i++) {
        conf[i] = -10000;
    }
    char color = player.get_color();

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (board.get_cell_color(i, j) == color || board.get_cell_color(i, j) == 'w') {
                Grid pos;
                pos.Set(i, j);
                conf[i * 10 + j] = score(move(board, pos, player), player);
            }
        }
    }
    return conf;
    delete[]conf;
}

pair<Grid, int> minimax(myboard board, int depth, int breadth, Player player) {
    char color = player.get_color();
    int* best_moves = bestn(board, player);
    Grid* best_pos = new Grid[breadth];
    int* best_val = new int[breadth];
    int val = -10000;
    Grid best_next_pos;
    int best_next_val;

    for (int i = 0; i < breadth; i++) {
        best_pos[i].Set(0, 0);
        best_val[i] = -10000;
    }

    for (int i = 0; i < 100; i++) {
        if (i / 10 >= ROW || i % 10 >= COL)continue;
        if (board.get_cell_color(i / 10, i % 10) != color && board.get_cell_color(i / 10, i % 10) != 'w')continue;
        if (best_moves[i] > val) {
            val = best_moves[i];
            best_next_pos.Set(i / 10, i % 10);
        }
        /*for (int j = 0; j < breadth; j++) {
            if (best_moves[i] > best_val[j]) {
                /*for (int k = breadth - 1; k >= j + 1; k--) {
                    best_val[k] = best_val[k - 1];
                    best_pos[k].Set(best_pos[k - 1].x, best_pos[k - 1].y);
                }*/
                /*for (int k = j + 1; k < breadth; k++) {
                    best_val[k] = best_val[k - 1];
                    best_pos[k].Set(best_pos[k - 1].x, best_pos[k - 1].y);
                }*/
         /*       best_val[j] = best_moves[i];
                best_pos[j].Set(i / 10, i % 10);
                break;
            }
        }*/
    }
    delete[]best_moves;
    //best_next_pos = best_pos[0];
    best_next_val = score(move(board, best_next_pos, player), player);

    if (depth == 1) {
        return make_pair(best_next_pos, best_next_val);
    }

    for (int i = 0; i < breadth; i++) {
        myboard b_new(move(board, best_pos[i], player));
        pair <Grid, int> best_move = minimax(b_new, depth - 1, breadth, player);
        int val = best_move.second;
        if (val > best_next_val) {
            best_next_val = val;
            best_next_pos = best_pos[i];
        }
    }
    delete[]best_pos;
    delete[]best_val;
    return make_pair(best_next_pos, best_next_val);
}

myboard move(myboard board, Grid pos, Player player) {
    char color = player.get_color();

    board.place_orb(pos.x, pos.y, &player);
    return board;
}

int score(myboard board, Player player) {
    int sc = 0;
    int my_orbs = 0, enemy_orbs = 0;
    char color = player.get_color();
    char enemycolor = (color == 'r') ? 'b' : 'r';
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (board.get_cell_color(i, j) == color) {
                my_orbs += board.get_orbs_num(i, j);
                bool flag_not_vulnerable = true;
                Grid* Neighbors = SearchNeigbors(i, j);
                for (int k = 0; k < 8; k++) {
                    if (Neighbors[k].x == -1 && Neighbors[k].y == -1) {
                        break;
                    }
                    else {
                        if (board.get_cell_color(Neighbors[k].x, Neighbors[k].y) == enemycolor && board.get_orbs_num(Neighbors[k].x, Neighbors[k].y) == board.get_capacity(Neighbors[k].x, Neighbors[k].y) - 1) {
                            sc -= (5 - board.get_capacity(Neighbors[k].x, Neighbors[k].y));
                            flag_not_vulnerable = false;
                        }
                    }
                }
                if (flag_not_vulnerable) {
                    // #The edge Heuristic
                    if (board.get_capacity(i, j) == 3) {
                        sc += 2;
                    }
                    // #The corner Heuristic
                    else if (board.get_capacity(i, j) == 2) {
                        sc += 3;
                    }
                    // #The unstability Heuristic
                    if (board.get_orbs_num(i, j) == board.get_capacity(i, j) - 1) {
                        sc += 2;
                    }
                }
                delete[]Neighbors;
            }
            else {
                enemy_orbs += board.get_orbs_num(i, j);
            }
        }
    }
    // #The number of Orbs Heuristic
    sc += my_orbs;
    sc -= enemy_orbs;

    if (enemy_orbs == 0 && my_orbs > 1) {
        return 10000;
    }
    else if (my_orbs == 0 && enemy_orbs > 1) {
        return -10000;
    }

    return sc;
}

Grid* SearchNeigbors(int i, int j) {
    Grid* Neighbors = new Grid[8];
    int num = 0;
    for (int k = 0; k < 8; k++) {
        Neighbors[k].Set(-1, -1);
    }

    if (i + 1 < ROW) {
        Neighbors[num++].Set(i + 1, j);
    }

    if (j + 1 < COL) {
        Neighbors[num++].Set(i, j + 1);
    }

    if (i - 1 >= 0) {
        Neighbors[num++].Set(i - 1, j);
    }

    if (j - 1 >= 0) {
        Neighbors[num++].Set(i, j - 1);
    }
    if (i + 1 < ROW && j - 1 >= 0) {
        Neighbors[num++].Set(i + 1, j - 1);
    }
    if (i - 1 >= 0 && j - 1 >= 0) {
        Neighbors[num++].Set(i - 1, j - 1);
    }
    if (i + 1 < ROW && j + 1 < COL) {
        Neighbors[num++].Set(i + 1, j + 1);
    }
    if (i - 1 >= 0 && j + 1 < COL) {
        Neighbors[num++].Set(i - 1, j + 1);
    }

    return Neighbors;
    delete[]Neighbors;
}