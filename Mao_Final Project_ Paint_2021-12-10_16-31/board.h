#ifndef BOARD_H
  #define BOARD_H
	#include <stdbool.h>
  char** setup_board(int num_rows, int num_cols, char blank_char);
	void print_board(char** board, int num_rows, int num_cols);
	bool is_inside_board(int row, int col, int num_rows, int num_cols);
	bool is_between(int val, int min_val, int max_val);
	bool spot_is_empty(char** board, int row, int col, char blank_char);
	bool all_row_same(int row, char** board, int num_rows, int num_cols);
	bool all_col_same(int col, char** board, int num_rows, int num_cols);
	bool is_board_full(char** board,  int dimensions, int blank_char);



#endif
