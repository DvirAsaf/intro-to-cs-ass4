#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "ass4.h"
#define SIZE 8

// PGN characters
const char PAWN = 'P';
const char ROOK = 'R';
const char KNIGHT = 'N';
const char BISHOP = 'B';
const char QUEEN = 'Q';
const char KING = 'K';
const char CAPTURE = 'x';
const char PROMOTION = '=';
const char CHECK = '+';
const char MATE = '#';
const char FIRST_COL = 'a';

// FEN & Board characters
const char WHITE_PAWN = 'P';
const char WHITE_ROOK = 'R';
const char WHITE_KNIGHT = 'N';
const char WHITE_BISHOP = 'B';
const char WHITE_QUEEN = 'Q';
const char WHITE_KING = 'K';
const char BLACK_PAWN = 'p';
const char BLACK_ROOK = 'r';
const char BLACK_KNIGHT = 'n';
const char BLACK_BISHOP = 'b';
const char BLACK_QUEEN = 'q';
const char BLACK_KING = 'k';

// FEN separator for strtok()
const char SEP[] = "/";

// Board characters
const char EMPTY = ' ';

// Move logical representation
typedef struct {
    char player, srcRow, srcCol, destRow, destCol;
    int iSrc, jSrc, iDest, jDest;
    int isWhite, isCapture, isPromotion, isCheck, isMate, isLegal;
    char promotionChange;
} Move;
typedef struct {
    int row,col;

}Location;

typedef struct
{
    int startCol;
    int startRow;
    int endCol;
    int endRow;
    char player;
    char isWin;

}piece;

int checkDifferentColors(char board[][SIZE],Move move)
{
    char srcPlayer = board[move.iSrc][move.jSrc];
    char destPlayer = board[move.jSrc][move.jDest];
    //White player can't capture white player
    if(isupper(srcPlayer)&& isupper(destPlayer))
    {
        return 0;
    }
        //black player can't capture black player
    else if (islower(srcPlayer)&&islower(destPlayer))
    {
        return 0;
        //White player can capture black player
    } else return 1;
}

int checkValidPawn(char board[][SIZE],Move move)
{
    //capture and walking straight - error!!!
    if(move.isCapture && move.jSrc != move.jDest)
        return 0;

    //double step and not im second row - error!!!
    if(abs(move.jSrc-move.jDest)==2)
    {
        // if white double step is allowed only from second row
        if(move.isWhite && move.iSrc != SIZE-2)
            return 0;
        // if black double step is allowed only from second row
        else if(!move.isWhite && move.iSrc != 1)
            return 0;
    }

    //if trying to move more than 2 steps- error!!!
    if(abs(move.jSrc-move.jDest)>2)
        return 0;

    //if white walking not straight - error!!!
    if(!(move.isWhite && move.iSrc-move.iDest==1 && move.jSrc==move.jDest))
        return 0;

    //if black walking not straight - error!!!
    if(!(!move.isWhite && move.iDest-move.iSrc==1 && move.jSrc==move.jDest))
        return 0;

    return 1;
}


int checkValidCapture(char board[][SIZE],Move move)
{
    if(move.isCapture == 1)
    {
        //trying to move to free space and say it is capture - not valid
        if(board[move.iDest][move.jDest] == EMPTY)
            return 0;
        //caapture to your friend is error
        if(checkDifferentColors(board,move) == 0)
        {
            return 0;
        }
        return 1;
    }
    else if(move.isCapture == 0)
    {
        char digit = board[move.iDest][move.jDest];
        //trying to move to place with enemy without saying it is capture - not valid
//        if((move.isWhite && islower(digit)) || (!move.isWhite && isupper(digit)))
//            return 0;
        if(checkDifferentColors(board,move) == 1)
        {
            return 0;//Capture without saying it
        }
        return 1;
    }
}



int isLastRow(char board[][SIZE],Move move)
{
    if((move.isWhite && move.iSrc==0) || (!move.isWhite && move.iSrc==SIZE-1))
        return 1;
    else
        return 0;
}

int isEmpty(char board[][SIZE],int row, int col){
    if(board[row][col] == EMPTY)
        return 1;
    else
        return 0;
}

int isDigit(char digit)
{
    return ('0' <= digit && digit <= '9');
}

void createBoard(char board[][SIZE], char fen[])
{
    // cureDigit = the value of fen
    int k=0;
    char cureDigit = fen[k];
    //i = row and j = col
    int i = 0, j = 0;
    while (cureDigit != '\0')
    {
        if (cureDigit == '/')
        {
           i++;
           j=0;
        }
        else if('0' <= cureDigit && cureDigit <= '9')
        {
            int numOfSpace = (cureDigit - '0');
            for (int l = 0; l < cureDigit; l++)
            {
                board[i][j] = EMPTY;
                j++;
            }
        }else
        {
            board [i][j] = cureDigit;
            j++;
        }
        //before next iteration update values
        k++;
        cureDigit = fen[k];
    }
//    for (int i = 0; i < SIZE; ++i) {
        //print all row
//        for (int j = 0; j < SIZE; ++j) {
//            printf("%c",board[i][j]);
//        }
//        print enter
//        printf("\n");
//    }
}

void printBoard(char board[][SIZE])
{
    printf("* |");
    char col = 'A';
    for (int i = 0; i < SIZE; i++)
    {
        if (i == SIZE - 1)
            printf("%c", col);
        else
            printf("%c ", col);
        col++;
    }
    printf("| *\n");
    printf("* -");
    for (int j = 0; j < (2 * SIZE - 1); j++)
    {
        printf("-");
    }
    int row = SIZE;
    printf("- *\n");
    for (int i = 0; i < SIZE ; i++)
    {
        printf("%d ",row);
        //print all row
        for (int j = 0; j < SIZE; ++j) {
            printf("|%c",board[i][j]);
        }
        printf("|");
        printf(" %d",row);
//        print enter
        printf("\n");
        row--;
    }
    printf("* -");
    for (int j = 0; j < (2 * SIZE - 1); j++) {
        printf("-");
    }
    printf("- *\n");
    printf("* |");
    col = 'A';
    for (int i = 0; i < SIZE; i++) {
        if (i == SIZE - 1)
            printf("%c", col);
        else
            printf("%c ", col);
        col++;
    }
    printf("| *\n");
}

int isCapture(char pgn[])
{
    int i=0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(cure != 'x')
        {
            i++;
            cure = pgn[i];
        }else return 1;
    }
    return 0;

}

int isPromotion(char pgn[])
{
    int i=0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(cure != '=')
        {
            i++;
            cure = pgn[i];
        }else return 1;
    }
    return 0;

}

char whichPromoion(char pgn[])
{
    int i=0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(cure != '=')
        {
            i++;
            cure = pgn[i];
        }else{
            i++;
            cure= pgn[i];
            return cure;
        }
}
    //never reach here
    return cure;
}

int isCheck(char pgn[])
{
    int i=0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(cure != '+')
        {
            i++;
            cure = pgn[i];
        }else return 1;
    }
    return 0;

}

int isMate(char pgn[])
{
    int i=0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(cure != '#')
        {
            i++;
            cure = pgn[i];
        }else return 1;
    }
    return 0;

}

int isSrcCol(char pgn[])
{
    int i=0;
    int conter = 0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(islower(cure)&& cure != CAPTURE)
        {
            conter++;
        }
        i++;
        cure = pgn[i];
    }
    if(conter==2)
    {
        return 1;
    } else return 0;
}

int isSrcRow(char pgn[])
{
    int i=0;
    int conter = 0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(isDigit(cure))
        {
            conter++;
        }
        i++;
        cure = pgn[i];
    }
    if(conter==2)
    {
        return 1;
    } else return 0;
}

char whichPlayer(char pgn[])
{
    if(isupper(pgn[0]))
    {
        return pgn[0];
    } else return PAWN;
}

char findSrcCol(char pgn[])
{
    int i=0;
    while (pgn[i] != '\0')
    {
        if(islower(pgn[i])  && pgn[i] != CAPTURE)
        {
            return pgn[i];
        } else i++;
    }
    //never happens
    return 'y';
}

char findSrcRow(char pgn[])
{
    int i=0;
    while (pgn[i] != '\0')
    {
        if(isDigit(pgn[i]))
        {
            return pgn[i];
        } else i++;
    }
    //never happens
    return 'y';
}

char findDstCol(char pgn[],int isSrcCol)
{
    int i=0;
    if(isSrcCol){
        //return the second lower char
        int isFoundSrc = 0;
        while (pgn[i] != '\0')
        {
            if(islower(pgn[i]) && pgn[i] != CAPTURE)
            {
                if(!isFoundSrc){
                    isFoundSrc = 1;
                } else{
                    //this is second time we find lower char so return dst
                    return pgn[i];
                }
            }
            i++;
        }
    }else{
        //return first lower char
        while (pgn[i] != '\0')
        {
            if(islower(pgn[i])  && pgn[i] != CAPTURE)
            {
                return pgn[i];
            } else i++;
        }
    }
    //never happens
    return 'y';
}

char findDstRow(char pgn[],int isSrcRow)
{
    int i=0;
    if(isSrcRow){
        //return the second digit
        int isFoundSrc = 0;
        while (pgn[i] != '\0')
        {
            if(isDigit(pgn[i]))
            {
                if(!isFoundSrc){
                    isFoundSrc = 1;
                } else{
                    //this is second time we find digit so return dst
                    return pgn[i];
                }
            }
            i++;
        }
    }else{
        //return first digit
        while (pgn[i] != '\0')
        {
            if(isDigit(pgn[i]))
            {
                return pgn[i];
            } else i++;
        }
    }
    //never happens
    return 'y';
}

/*
 * the function check that if this is promotion move than it's valid - the player is pawn in
 * last raw.
 * also it checks the case of a pawn that in last row and there is no promotion which is
 * also not valid
 */
int checkValidPromotion(char board[][SIZE],Move move) {
    if(move.isPromotion){
        //validate the player is pawn in last raw.
        if(isLastRow(board,move) && move.player==PAWN){
            return 1;
        }
        else
            return 0;
    } else{
        if(isLastRow(board,move) && move.player==PAWN)
            return 0;
        //if it is pawn not in last row it is ok, or other player in last row also ok
        else
            return 1;
    }
}

int isValidMove(char board[][SIZE], Move move){
    if(move.player == PAWN){
        if(checkValidPawn(board,move) == 0)
            return  0;
    }

    if(checkValidCapture(board,move) == 0)
        return 0;

    if(checkValidPromotion(board,move) == 0)
        return 0;


//    return 1;
}

int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn)
{
    Move move;
    move.player = whichPlayer(pgn);
    move.isMate = isMate(pgn);
    move.isCheck = isCheck(pgn);
    move.isCapture = isCapture(pgn);
    move.isPromotion = isPromotion(pgn);
    if(move.isPromotion)
    {
        move.promotionChange = whichPromoion(pgn);
    }
    move.isWhite = isWhiteTurn;
    //if there is a src col
    if(isSrcCol(pgn))
    {
        move.srcCol = findSrcCol(pgn);
        move.destCol = findDstCol(pgn,1);
    } else{
        move.destCol = findDstCol(pgn,0);
    }

    if(isSrcRow(pgn))
    {
        move.srcRow = findSrcRow(pgn);
        move.destRow = findDstRow(pgn,1);
    }else {
        move.destCol = findDstCol(pgn,0);
    }

}

//int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn)
//{
//    char player;
//    char colDst;
//    char colSrc;
//    char rowDst;
//    char rowSrc;
//    int i=0;
//    int numOfDigit = numOfDigit(pgn);
//    if(numOfDigit == 2){
//        int foundSrc =0;
//        if(isupper(pgn[0]))
//        {
//            player = pgn[0];
//            i++;
//        }else {
//            player = PAWN;
//        }
//
//        char d = pgn[i];
//        while (d != '\0'){
//            if(islower(d) && d!='x'){
//                if(foundSrc){
//                    colDst = d;
//                } else{
//                    colSrc = d;
//                    foundSrc = 1; //Set flag to true
//                }
//            }else if(d == 'x'){
//                //capture
//            }else if(isdigit(d)){
//                if(foundSrc){
//                    rowDst = d;
//                } else{
//                    rowSrc = d;
//                }
//            }
//        }
//
//        //src abd dst
//    }else{
//        //dst
//    }
//
////if the move is legal return 1 if not return 0.
//
//}
//void printBoardFromFEN(char fen[]){}



int isClear(char board[][SIZE],char piece,Location loc)
{
    char target = (board[loc.row][loc.col]);
    if (target!= EMPTY )
    {
        //White piece cant eats friend
       if(isupper(target)&&isupper(piece))
       {
           return 0;

           //Black piece cant eats friend
       } else if(islower(target)&& islower(piece))
       {
           return 0;
       } else return 2;
    }
    return 1;
}
int isMove(char board[][SIZE],char piece,Location src, Location dest)
{
    if (isClear(board,piece,dest))
    {
        if(piece=='P')
        {
            if (src.col==dest.col)
            {
                if(dest.row-src.row==1)
                {
                    return 1;
                }
            } else if ((dest.row-src.row==2)&& src.row==1)
            {
                return 1;
            } else return 0;
        }
    }
    return 0;
}
