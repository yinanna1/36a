#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include <string.h>
#include <ctype.h>

struct canvas_size
{
    int num_rows;
    int num_columns;
};

struct rc_label{
    char rc;
    int index;
};

struct load_info{
    int num_rows;
    int num_columns;
    char **board;
};

struct canvas_size get_canvas_size(int argc, char *argv[]);
struct canvas_size get_resize_size(char*cmd, int rows, int columns);
struct rc_label get_row_col_label(char*cmd,int rows,int columns);
void print_help();
int is_int(char *s);
struct canvas_size commands_loop(char **board, int rows, int columns);
int write_command(char*cmd, char **board, int rows, int columns);
int erase_command(char*cmd, char **board, int rows, int columns);
char **resize_command(struct canvas_size new_size, char **board, int rows, int columns);
char **add_command(struct rc_label info, char **board, int rows, int columns);
char **delete_command(struct rc_label info, char **board, int rows, int columns);
int save_command(char*cmd, char **board, int rows, int columns);
struct load_info load_command(char*cmd, char **board, int rows, int columns);
void free_board(char **board,int num_rows, int num_cols);

int main(int argc, char *argv[]) {
    struct canvas_size cv_size = get_canvas_size(argc,argv);
    char **board = setup_board(cv_size.num_rows,cv_size.num_columns,'*');
    struct canvas_size size_res = commands_loop(board,cv_size.num_rows,cv_size.num_columns);
    return 0;
}

void free_board(char **board,int num_rows, int num_cols){
	for(int i = 0; i < num_rows; i++){
		free(board[i]);
	}
	free(board);
}

int write_command(char *cmd, char **board, int rows, int columns){
    //split command
    int arr[4] = {-1,-1,-1,-1};
    int num_ct = 0;
    const char *split = " ";
    char *p;
    p = strtok(cmd,split); 
    p = strtok(NULL,split); //skip w
    while(p!=NULL) {
        if(!is_int(p) || num_ct >= 4){
            //Too many arguments or not number
            return -1;
        }
        else{
            arr[num_ct] = atoi(p);
            num_ct++;
        }
        p = strtok(NULL,split); 
    }
    //Out of bounds/Negative(write)
    for(int i = 0; i < 4; i++){
        if(arr[i] < 0) return -1;
    }
    if(!is_inside_board(arr[0],arr[1],rows,columns) || 
        !is_inside_board(arr[2],arr[3],rows,columns))
        return -1;
    //start draw
    //point
    if(arr[0] == arr[2] && arr[1] == arr[3]){
        board[arr[0]][arr[1]] = '-';
    }
    //Horizontal line
    else if(arr[0] == arr[2]){
        if(arr[1] > arr[3]){
            //swap 2 points
            int tmp = arr[1];
            arr[1] = arr[3];
            arr[3] = tmp;
        }
        for(int i = arr[1]; i <= arr[3]; i++){
            if(board[arr[0]][i] == '*' || board[arr[0]][i] == '-'){
                board[arr[0]][i] = '-';
            }
            else{
                board[arr[0]][i] = '+';
            }
        }
    }
    //Vertical line
    else if(arr[1] == arr[3]){
        if(arr[0] > arr[2]){
            //swap 2 points
            int tmp = arr[0];
            arr[0] = arr[2];
            arr[2] = tmp;
        }
        for(int i = arr[0]; i <= arr[2]; i++){
            if(board[i][arr[1]] == '*' || board[i][arr[1]] == '|'){
                board[i][arr[1]] = '|';
            }
            else{
                board[i][arr[1]] = '+';
            }
        }
    }
    //Left diagonal
    else if(arr[0] - arr[2] == arr[1] - arr[3]){
        int start_row = arr[0] < arr[2] ? arr[0]:arr[2]; 
        int start_col = arr[1] < arr[3] ? arr[1]:arr[3];
        for(int i = 0;i <= abs(arr[0] - arr[2]); i++){
            if(board[start_row + i][start_col + i] == '/' || 
            board[start_row + i][start_col + i] == '*'){
                board[start_row + i][start_col + i] = '/';
            }
            else{
                board[start_row + i][start_col + i] = '+';
            }
        }
    }
    //Right diagonal
    else if(arr[0] - arr[2] == -arr[1] + arr[3]){
        int start_row = arr[0] < arr[2] ? arr[0]:arr[2]; 
        int start_col = arr[1] > arr[3] ? arr[1]:arr[3];
        for(int i = 0;i <= abs(arr[0] - arr[2]); i++){
            if(board[start_row + i][start_col - i] == '\\' || 
            board[start_row + i][start_col - i] == '*'){
                board[start_row + i][start_col - i] = '\\';
            }
            else{
                board[start_row + i][start_col - i] = '+';
            }
        }
    }
    else{
        printf("Cannot draw the line as it is not straight.\n");
    }
    return 0;
}

int erase_command(char*cmd, char **board, int rows, int columns){
    //split command
    int arr[2] = {-1,-1};
    int num_ct = 0;
    const char *split = " ";
    char *p;
    p = strtok(cmd,split); 
    p = strtok(NULL,split); //skip e
    while(p!=NULL) {
        if(!is_int(p) || num_ct >= 2){
            //Too many arguments or not number
            return -1;
        }
        else{
            arr[num_ct] = atoi(p);
            num_ct++;
        }
        p = strtok(NULL,split); 
    }
    //Out of bounds/Negative(erase)
    for(int i = 0; i < 2; i++){
        if(arr[i] < 0) return -1;
    }
    if(!is_inside_board(arr[0],arr[1],rows,columns))
        return -1;
    //start erase
    board[arr[0]][arr[1]] = '*';
    return 0;
}

struct canvas_size get_resize_size(char*cmd, int rows, int columns){
    //split command
    struct canvas_size new_size;
    new_size.num_rows = -1;
    new_size.num_columns = -1;
    int arr[2] = {-1,-1};
    int num_ct = 0;
    const char *split = " ";
    char *p;
    p = strtok(cmd,split); 
    p = strtok(NULL,split); //skip r
    while(p!=NULL) {
        if(!is_int(p) || num_ct >= 2){
            //Too many arguments or not number
            return new_size;
        }
        else{
            arr[num_ct] = atoi(p);
            num_ct++;
        }
        p = strtok(NULL,split); 
    }
    //min size: 1
    for(int i = 0; i < 2; i++){
        if(arr[i] < 1) return new_size;
    }
    new_size.num_rows = arr[0];
    new_size.num_columns = arr[1];
    return new_size;
}

char **resize_command(struct canvas_size new_size, char **board, int rows, int columns){
    //start resize
    char **new_board = setup_board(new_size.num_rows,new_size.num_columns,'*');
    for(int i = 0;i < new_size.num_rows && i < rows; i++){
        for(int j = 0; j < new_size.num_columns && j < columns; j++){
            new_board[i][j] = board[i][j];
        }
    }
    // free board
    free_board(board,rows,columns);
    return new_board;
}

struct rc_label get_row_col_label(char*cmd,int rows,int columns){
    //split command
    struct rc_label res;
    res.rc = ' ';
    res.index = -1;
    int index = -1;
    int num_ct = 0;
    const char *split = " ";
    char *p;
    p = strtok(cmd,split);  //skip a
    p = strtok(NULL,split); 
    if(strlen(p) == 1){
        char lb = p[0];
        p = strtok(NULL,split); // skip r or c
        if(lb == 'r' || lb == 'c'){
            while(p!=NULL) {
                if(!is_int(p) || num_ct >= 1){
                    return res;
                }
                else{
                    index = atoi(p);
                    num_ct++;
                }
                p = strtok(NULL,split); 
            }
            if(lb == 'r' && (index < 0 || index > rows)){
                return res;
            }
            if(lb == 'c' && (index < 0 || index > columns)){
                return res;
            }
            res.rc = lb;
            res.index = index;
            return res;
        }
    }
    return res;
}

char **add_command(struct rc_label info, char **board, int rows, int columns){
    // start add row
    if(info.rc == 'r'){
        char **new_board = setup_board(rows + 1,columns,'*');
        for(int i = 0; i < info.index; i++){
            for(int j = 0; j < columns; j++){
                new_board[i][j] = board[i][j];
            }
        }
        for(int i = info.index + 1; i < rows + 1; i++){
            for(int j = 0; j < columns; j++){
                new_board[i][j] = board[i-1][j];
            }
        }
        free_board(board,rows,columns);
        return new_board;
    }else{
        char **new_board = setup_board(rows,columns+1,'*');
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < info.index; j++){
                new_board[i][j] = board[i][j];
            }
        }
        for(int i = 0; i < rows; i++){
            for(int j = info.index + 1; j < columns + 1; j++){
                new_board[i][j] = board[i][j-1];
            }
        }
        free_board(board,rows,columns);
        return new_board;
    }
}

char **delete_command(struct rc_label info, char **board, int rows, int columns){
    // start add row
    if(info.rc == 'r'){
        char **new_board = setup_board(rows - 1,columns,'*');
        for(int i = 0; i < info.index; i++){
            for(int j = 0; j < columns; j++){
                new_board[i][j] = board[i][j];
            }
        }
        for(int i = info.index + 1; i < rows; i++){
            for(int j = 0; j < columns; j++){
                new_board[i-1][j] = board[i][j];
            }
        }
        free_board(board,rows,columns);
        return new_board;
    }else{
        char **new_board = setup_board(rows,columns-1,'*');
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < info.index; j++){
                new_board[i][j] = board[i][j];
            }
        }
        for(int i = 0; i < rows; i++){
            for(int j = info.index + 1; j < columns; j++){
                new_board[i][j-1] = board[i][j];
            }
        }
        free_board(board,rows,columns);
        return new_board;
    }
}

int save_command(char*cmd, char **board, int rows, int columns){
    const char *split = " ";
    char *p;
    p = strtok(cmd,split);  //skip s
    p = strtok(NULL,split); 
    char *fname = p;//filename
    if(fname== NULL){
        return -1;
    }
    //save to file
    FILE *fp = NULL;
    fp = fopen(fname,"w+");
    if(fp == NULL){
        printf("Failed to open file: %s\n",fname);
        return 0;
    }
    fprintf(fp,"%d %d\n",rows,columns);
    for(int i = 0;i < rows; i++){
        for(int j = 0; j < columns; j++){
            fprintf(fp,"%c ",board[i][j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return 0;
}

struct load_info load_command(char*cmd, char **board, int rows, int columns){
    struct load_info tmp_info;
    tmp_info.num_columns = 0;
    tmp_info.num_rows = 0;
    tmp_info.board = NULL;
    const char *split = " ";
    char *p;
    p = strtok(cmd,split);  //skip s
    p = strtok(NULL,split); 
    char *fname = p;//filename
    if(fname == NULL){
        return tmp_info;
    }
    //load from file
    FILE *fp = NULL;
    fp = fopen(fname,"r");
    if(fp == NULL){
        printf("Failed to open file: %s",fname);
        tmp_info.num_rows = -1;
        return tmp_info;
    }
    free_board(board,rows,columns);
    fscanf(fp, "%d %d", &tmp_info.num_rows, &tmp_info.num_columns);
    char **new_board = setup_board(tmp_info.num_rows,tmp_info.num_columns,'*');
    char c;
    fscanf(fp, "\n");
    for(int i = 0;i < tmp_info.num_rows; i++){
        for(int j = 0; j < tmp_info.num_columns; j++){
            fscanf(fp, "%c ",&c);
            new_board[i][j] = c;
        }
        if(feof(fp)) break;
        fscanf(fp,"\n");
    }
    tmp_info.board = new_board;
    fclose(fp);
    return tmp_info;
}

int is_int(char *s){
    int len = strlen(s);
    for(int i = 0;i<len;i++){
        if(i == 0 && s[i] =='-') continue;
        if(!isdigit(s[i]))
            return 0;
    }
    return 1;
}

struct canvas_size commands_loop(char **board, int rows, int columns){
    int flag = 1;
    while(flag){
        print_board(board,rows,columns);
        printf("Enter your command: ");
        // read a line of command
        char *str = (char*)malloc(sizeof(char)*255);
        fgets(str,255,stdin);
        char *p = strchr(str,'\n');
        if(p!=NULL) *p = '\0';
        // deal with cmd
        if(strlen(str) == 1){
            // q and h
            switch(str[0]){
                // quit
                case 'q':
                if(strlen(str) == 1){
                    flag = 0;
                }
                else{
                    printf("Unrecognized command. Type h for help.\n");
                }
                break;

                // help
                case 'h':
                if(strlen(str) == 1){
                    print_help();
                }
                else{
                    printf("Unrecognized command. Type h for help.\n");
                }
                break;
                case 'w':printf("Improper draw command.\n");break;
                case 'e':printf("Improper erase command.\n");break;
                case 'r':printf("Improper resize command.\n");break;
                case 'a':printf("Improper add command.\n");break;
                case 'd':printf("Improper delete command.\n");break;
                case 's':printf("Improper save command or file could not be opened.\n");break;
                case 'l':printf("Improper load command or file could not be opened.\n");break;
                default:
                printf("Unrecognized command. Type h for help.\n");
                break;
            }
        }
        else if(strlen(str) > 1 && str[1] == ' '){
            switch(str[0]){
                case 'w':
                if(write_command(str,board,rows,columns) == -1){
                    printf("Improper draw command.\n");
                }
                break;
                case 'e':
                if(erase_command(str,board,rows,columns) == -1){
                    printf("Improper erase command.\n");
                }
                break;
                case 'r':{
                struct canvas_size new_size = get_resize_size(str,rows,columns);
                if(new_size.num_rows == -1){
                    printf("Improper resize command.\n");
                }
                else{
                    board = resize_command(new_size,board,rows,columns);
                    rows = new_size.num_rows;
                    columns = new_size.num_columns;
                }
                }
                break;
                case 'a':
                {
                struct rc_label tmp_lb = get_row_col_label(str,rows,columns);
                if(tmp_lb.rc == ' '){
                    printf("Improper add command.\n");
                }
                else{
                    board = add_command(tmp_lb,board,rows,columns);
                    if(tmp_lb.rc == 'r'){
                        rows++;
                    }
                    else{
                        columns++;
                    }
                }
                }
                break;

                case 'd':
                {
                struct rc_label tmp_lb = get_row_col_label(str,rows,columns);
                if(tmp_lb.rc == ' '){
                    printf("Improper delete command.\n");
                }
                else if((tmp_lb.rc == 'r' && tmp_lb.index == rows) || 
                        (tmp_lb.rc == 'c' && tmp_lb.index == columns)){
                    printf("Improper delete command.\n");
                }
                else{
                    board = delete_command(tmp_lb,board,rows,columns);
                    if(tmp_lb.rc == 'r'){
                        rows--;
                    }
                    else{
                        columns--;
                    }
                }
                }
                break;

                case 's':
                if(save_command(str,board,rows,columns) == -1){
                    printf("Improper save command or file could not be opened.\n");
                }
                else{

                }
                break;

                case 'l':
                {
                    struct load_info info = load_command(str,board,rows,columns);
                    if(info.num_rows == 0){
                        printf("Improper load command or file could not be opened.\n");
                    }
                    else if(info.num_rows != -1){
                        board = info.board;
                        rows = info.num_rows;
                        columns = info.num_columns;
                    }
                }
                break;

                default:
                printf("Unrecognized command. Type h for help.\n");
                break;
            }
        }
        else{
            printf("Unrecognized command. Type h for help.\n");
        }
        free(str);
    }
    free_board(board,rows,columns);
    struct canvas_size res;
    res.num_rows = rows;
    res.num_columns = columns;
    return res;
}

struct canvas_size get_canvas_size(int argc, char *argv[]){
    int num_rows;
    int num_columns;
    int default_flag = 1;
    if (argc == 1){
        default_flag = 0;
        num_rows = 10;
        num_columns = 10;
    }
    else if(argc != 3){
        // Command-line argument count
        printf("Wrong number of command line arguements entered.\n");
        printf("Usage: ./paint.out [num_rows num_cols]\n");
    }
    else {
        if(!is_int(argv[1])){
            printf("The number of rows is not an integer.\n");
        }
        else if(!is_int(argv[2])){
            printf("The number of columns is not an integer.\n");
        }
        else{
            num_rows = atoi(argv[1]);
            num_columns = atoi(argv[2]);
            if(num_rows < 1){
                printf("The number of rows is less than 1.\n");
            }
            else if(num_columns < 1){
                printf("The number of columns is less than 1.\n");
            }
            else{
                default_flag = 0;
            }
        }
    }
    if(default_flag){
        printf("Making default board of 10 X 10.\n");
        num_rows = 10;
        num_columns = 10;
    }
    struct canvas_size tmp;
    tmp.num_rows = num_rows;
    tmp.num_columns = num_columns;
    return tmp;
}

void print_help(){
    printf("Commands:\n");
    printf("Help: h\n");
    printf("Quit: q\n");
    printf("Draw line: w row_start col_start row_end col_end\n");
    printf("Resize: r num_rows num_cols\n");
    printf("Add row or column: a [r | c] pos\n");
    printf("Delete row or column: d [r | c] pos\n");
    printf("Erase: e row col\n");
    printf("Save: s file_name\n");
    printf("Load: l file_name\n");
}
