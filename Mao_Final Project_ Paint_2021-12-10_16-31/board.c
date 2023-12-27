#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "board.h"

char** setup_board(int num_rows, int num_cols, char blank_char){
	char** board = (char**)calloc(num_rows, sizeof(*board));
	for(int i = 0; i < num_rows; ++i){
		board[i] = (char*)calloc(num_cols, sizeof(*board[i]));
		for(int j = 0; j < num_cols; ++j){
			board[i][j] = blank_char;
		}
	}
	return board;
}


void print_board(char** board, int num_rows, int num_cols){
	if(num_cols >= 10 || num_rows >= 10){
		for(int i = 0; i < num_rows; ++i){
			printf("%2d ", num_rows - 1 - i); //print the row header
			for(int j = 0; j < num_cols; ++j){
				printf(" %c ", board[num_rows - 1 - i][j]); //print the contents
			}
			printf("\n");
		}

		printf("   ");
		for(int i = 0; i < num_cols;++i){ //print column headers
			printf("%2d ", i);
		}
		printf("\n");
	}
	else{
		for(int i = 0; i < num_rows; ++i){
			printf("%d ", num_rows - 1 - i); //print the row header
			for(int j = 0; j < num_cols; ++j){
				printf("%c ", board[num_rows - 1 - i][j]); //print the contents
			}
			printf("\n");
		}

		printf("  ");
		for(int i = 0; i < num_cols;++i){ //print column headers
			printf("%d ", i);
		}
		printf("\n");
	}

}

bool is_inside_board(int row, int col, int num_rows, int num_cols){
	return is_between(row, 0, num_rows - 1) &&
				 is_between(col, 0, num_cols - 1);
}

bool is_between(int val, int min_val, int max_val){
	return val >= min_val && val <= max_val;
}

bool spot_is_empty(char** board, int row, int col, char blank_char){
	return board[row][col] == blank_char;
}

bool all_row_same(int row, char** board, int num_rows, int num_cols){
	for(int i = 0; i < num_cols; ++i){
		if(board[row][i] != board[row][0]){
			return false;
		}
	}
	return true;
}


bool all_col_same(int col, char** board, int num_rows, int num_cols){
	for(int i = 0; i < num_rows; ++i){
		if(board[i][col] != board[0][col]){
			return false;
		}
	}
	return true;
}

bool is_board_full(char** board,  int dimensions, int blank_char){
	for(int i = 0; i < dimensions; ++i){
		for (int j = 0; j < dimensions; j++) {
			if(board[i][j] == blank_char){
				return false;
			}
		}
	}
	return true;
}
