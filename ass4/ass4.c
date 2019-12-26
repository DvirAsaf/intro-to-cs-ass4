
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
    int hasSrcCol,hasSrcRow;
} Move;
typedef struct {
    int row,col;
    int isValid;

}Location;

typedef struct
{
    int col,row;
    char player;
}kingSafe;

int isDigit(char digit)
{
    return ('0' <= digit && digit <= '9');
}

int isSrcCol(char pgn[])
{
    int i=0;
    int conter = 0;
    char cure = pgn[i];
    while (cure != '\0')
    {
        if(islower(cure) && cure != CAPTURE)
        {
            conter++;
        }
        i++;
        cure = pgn[i];
    }
    if(conter==2)
    {
        return 1;
    } else
        return 0;
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

int changeCharToIndex(char charToIndex)
{
    int res;
    //convert number to row
    if(isDigit(charToIndex))
    {
        res = charToIndex - '0';
        res = SIZE - res;
    }
        // convert from col char to index : a->0, b->1 ...
    else{
        res = charToIndex - 'a';
    }
    return res;
}

int isEmpty(char board[][SIZE],int row, int col){
    if(board[row][col] == EMPTY)
        return 1;
    else
        return 0;
}

int checkDifferentColors(char board[][SIZE],Move move)
{
    char srcPlayer = board[move.iSrc][move.jSrc];
    char destPlayer = board[move.iDest][move.jDest];
    //White player can't capture white player
    if(isupper(srcPlayer) && isupper(destPlayer))
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

int checkValidKing(char board[][SIZE],Move move)
{
    if(abs(move.iSrc - move.iDest) > 1)
        return 0;
    if(abs(move.jSrc - move.jDest) > 1)
        return 0;
//    if(abs(move.iSrc - move.iDest)==0)
//    {
//        if(!(isEmpty(board,move.iSrc,move.jDest)))
//            return 0;
//    }
//    else if(abs(move.jSrc - move.jDest)==0)
//    {
//        if(!(isEmpty(board,move.iDest,move.jSrc)))
//            return 0;
//    }
//    else
//    {
//        if(!(isEmpty(board,move.iDest,move.jDest)))
//            return 0;
//    }
    return 1;
}

//int checkValidQueen(char board[][SIZE],Move move) {
//    int stepsRow = abs(move.jSrc - move.jDest);
//    int stepsCol = abs(move.iSrc - move.jDest);
//
//    if (stepsCol == stepsRow) {
//        //number of steps that the player move in diagonal line
//        int steps = abs(move.iSrc - move.iDest);
//        //if the player do not move in diagonal line
//        if (abs(move.iSrc - move.iDest) != abs(move.jSrc - move.jDest))
//            return 0;
//        // char side that tell me witch side the player move
//        char side;
//        side = move.jSrc > move.jDest ? 'L' : 'R';
//        // char side that tell me witch direction the player move
//        char direction;
//        direction = move.iSrc > move.iDest ? 'U' : 'D';
//        // i == src row and j== src col
//        int i = move.iSrc;
//        int j = move.jSrc;
//        //the player move left and up in diagonal line, and check that of the way is free (empty) if not error!
//        if (side == 'L' && direction == 'U') {
//            {
//                i = i - 1;
//                j = j - 1;
//                for (int k = 0; k < steps; ++k) {
//                    if (!isEmpty(board, i, j))
//                        return 0;
//                    i--;
//                    j--;
//                }
//            }
//        }
//            //the player move right and up in diagonal line, and check that of the way is free (empty) if not error!
//        else if (side == 'R' && direction == 'U') {
//            i = i - 1;
//            j = j + 1;
//            for (int k = 0; k < steps; k++) {
//                if (!isEmpty(board, i, j))
//                    return 0;
//                i--;
//                j++;
//            }
//        }
//            //the player move left and down in diagonal line, and check that of the way is free (empty) if not error!
//        else if (side == 'L' && direction == 'D') {
//            {
//                i = i + 1;
//                j = j - 1;
//                for (int k = 0; k < steps; ++k) {
//                    if (!isEmpty(board, i, j))
//                        return 0;
//                    i++;
//                    j--;
//                }
//            }
//        }
//            //the player move right and down in diagonal line, and check that of the way is free (empty) if not error!
//        else if (side == 'R' && direction == 'D') {
//            i = i + 1;
//            j = j + 1;
//            for (int k = 0; k < steps; ++k) {
//                if (!isEmpty(board, i, j))
//                    return 0;
//                i++;
//                j++;
//            }
//        }
//        return 1;
//    }//if walking in the same col
//    else if (stepsCol == 0)
//    {
//        //check witch direction the player move U==up and D==down
//        char result = move.iSrc > move.iDest ? 'U' : 'D';
//
//        if (result == 'U') {
//            //check for the next step if the place is empty
//            for (int i = move.iSrc - 1; i >= move.iDest; i--) {
//                if (!(isEmpty(board, i, move.jSrc)))
//                    return 0;
//            }
//        } else {
//            //check for the next step if the place is empty
//            for (int i = move.iSrc + 1; i <= move.iDest; i++) {
//                if (!(isEmpty(board, i, move.jSrc)))
//                    return 0;
//            }
//        }
//        return 1;
//    }
//        //if walking in the same row
//    else if (stepsRow==0) {
//        //check witch direction the player move L==left and R== right
//        char result = move.jSrc > move.jDest ? 'L' : 'R';
//
//        if (result == 'L') {
//            for (int i = move.jSrc - 1; i >= move.jDest; i--) {
//                if (!(isEmpty(board, move.iSrc, i)))
//                    return 0;
//            }
//        }
//            //GO RIGHT
//        else {
//            //check for the next step if the place is empty
//            for (int i = move.jSrc + 1; i <= move.jDest; i++) {
//                if (!(isEmpty(board, move.iSrc, i)))
//                    return 0;
//            }
//
//        }
//    }
//    return 0;
//}

int checkValidKnight(char board[][SIZE],Move move)
{
    //if walking more than 2 step in the same row
    if(abs(move.iSrc - move.iDest) > 2)
        return 0;

    //if walking more than 2 step in the same col
    if(abs(move.jSrc - move.jDest) > 2)
        return 0;

    //If none of them walked 2 steps it is an ERROR !
    if(!(abs(move.jSrc-move.jDest)==2 || abs(move.iSrc - move.iDest) == 2)){
        return 0;
    }

    //if walking 2 step in the same row and walking more than 1 step in col
    if(abs(move.iSrc - move.iDest) == 2)
    {
        if(abs(move.jSrc - move.jDest) != 1)
            return 0;
    }

    //if walking 2 step in the same col and walking more than 1 step in row
    if(abs(move.jSrc - move.jDest) == 2)
    {
        if(abs(move.iSrc - move.iDest) != 1)
            return 0;
    }

    //TODO: תבדוק אם אתה מוריד את הבדיקת הבאות אם עדיין הכל נכון

    // if walking in the same col and not walking to left or right
    if(abs(move.jSrc - move.jDest) == 0)
        return 0;

    // if walking in the same row and not walking up or dawn
    if(abs(move.iSrc - move.iDest) == 0)
        return 0;

    //If we pass all checks it means everything is fine, continue to next checks ...
    return 1;
}

int checkValidBishop(char board[][SIZE],Move move)
{
    //number of steps that the player move in diagonal line
    int steps = abs(move.iSrc - move.iDest);
    if(move.isCapture)
        steps = steps-1;
    //if the player do not move in diagonal line
    if (abs(move.iSrc - move.iDest) != abs(move.jSrc - move.jDest))
        return 0;
    // char side that tell me witch side the player move
    char side;
    side = move.jSrc > move.jDest ? 'L' : 'R';
    // char side that tell me witch side the player move
    char direction;
    direction = move.iSrc > move.iDest ? 'U' : 'D';
    // i == src row and j== src col
    int i = move.iSrc;
    int j = move.jSrc;
    //the player move left and up in diagonal line, and check that of the way is free (empty) if not error!
    if (side == 'L' && direction == 'U') {
        {
            i = i - 1;
            j = j - 1;
            for (int k = 0; k < steps; ++k) {
                if (!isEmpty(board, i, j))
                    return 0;
                i--;
                j--;
            }
        }
    }
        //the player move right and up in diagonal line, and check that of the way is free (empty) if not error!
    else if (side == 'R' && direction == 'U') {
        i = i - 1;
        j = j + 1;
        for (int k = 0; k < steps; k++) {
            if (!isEmpty(board, i, j))
                return 0;
            i--;
            j++;
        }
    }
        //the player move left and down in diagonal line, and check that of the way is free (empty) if not error!
    else if (side == 'L' && direction == 'D') {
        {
            i = i + 1;
            j = j - 1;
            for (int k = 0; k < steps; ++k) {
                if (!isEmpty(board, i, j))
                    return 0;
                i++;
                j--;
            }
        }
    }
        //the player move right and down in diagonal line, and check that of the way is free (empty) if not error!
    else if (side == 'R' && direction == 'D') {
        i = i + 1;
        j = j + 1;
        for (int k = 0; k < steps; ++k) {
            if (!isEmpty(board, i, j))
                return 0;
            i++;
            j++;
        }
    }
    return 1;
}

int checkValidRook(char board[][SIZE],Move move)
{
    // if walking not in the same row or the same col
    if((move.jSrc != move.jDest) && (move.iSrc != move.iDest))
        return 0;

    //if walking in the same row
    if(move.jSrc != move.jDest)
    {
        //check witch direction the player move L==left and R== right
        char result = move.jSrc > move.jDest ? 'L':'R';

        if(result == 'L')
        {
            for (int i = move.jSrc-1; i >=move.jDest+1 ; i--)
            {
                if(!(isEmpty(board,move.iSrc,i)))
                    return 0;
            }
        } else {//GO RIGHT
            //check for the next step if the place is empty
            for (int i = move.jSrc + 1; i <= move.jDest-1; i++) {
                if (!(isEmpty(board, move.iSrc, i)))
                    return 0;
            }
        }
    }
    //if walking in the same col
    if(move.iSrc != move.iDest)
    {
        //check witch direction the player move U==up and D==down
        char result = move.iSrc > move.iDest ? 'U':'D';

        if(result == 'U')
        {
            //check for the next step if the place is empty
            for (int i = move.iSrc-1 ; i >= move.iDest+1 ; i--)
            {
                if(!(isEmpty(board,i,move.jSrc)))
                    return 0;
            }
        } else
        {
            //check for the next step if the place is empty
            for (int i = move.iSrc+1 ; i <= move.iDest-1 ; i++)
            {
                if(!(isEmpty(board,i,move.jSrc)))
                    return 0;
            }
        }
    }
    return 1;
}

//TODO: הפונקצית בדיקה של המלכה שלך לא הייתה נכונה, הבנת נכון את המשמעות של השילוב בין שתי הבדיקות יא גאון, אך מספיק טעות אחת קטנה של אינדקסים ונופלים
//TODO: הרעיון הוא לבדוק האם אחת מהבדחקות עברה בשלום, אחרת טעות
int checkValidQueen(char board[][SIZE],Move move)
{
    if(checkValidBishop(board,move) || checkValidRook(board,move))
        return 1;
    else
        return 0;
}

int checkValidPawn(char board[][SIZE],Move move)
{
    //capture and walking straight - error!!!
    if(move.isCapture && move.jSrc == move.jDest)
        return 0;

    if(!move.isCapture && move.jSrc != move.jDest)//לא ראיתי שעשית שאם זה לא קפצור
        return 0;

    //double step and not im second row - error!!!
    if(abs(move.iSrc-move.iDest)==2 )
    {
        // if white double step is allowed only from second row
        if(move.isWhite && move.iSrc != SIZE-2)
            return 0;
            // if black double step is allowed only from second row
        else if(!move.isWhite && move.iSrc != 1)
            return 0;
    }

    //if trying to move more than 2 steps- error!!!
    if(abs(move.iSrc-move.iDest)>2)
        return 0;

    //if white walking not straight - error!!!
    if(move.isWhite)
    {
        if(!( move.iSrc-move.iDest==1 || move.iSrc-move.iDest==2 ))
            return 0;
    } else{
        //if black walking not straight - error!!!
        if(!(move.iDest-move.iSrc==1 || move.iDest-move.iSrc==2))
            return 0;
    }

    if(move.isCapture)
    {
        if(!(abs(move.iSrc-move.iDest)==1 && abs(move.jSrc-move.jDest)==1))
            return 0;
        if(move.isWhite){
            if(move.iSrc-move.iDest<0)
                return 0;
        }else{
            if(move.iSrc-move.iDest>0)
                return 0;
        }
    }


    if(!move.isCapture)
    {
        if( move.jSrc!=move.jDest)
            return 0;
    }

    return 1;
}

int checkValidCapture(char board[][SIZE],Move move)
{
    if(move.isCapture == 1)
    {
        //trying to move to free space and say it is capture - not valid
        if(board[move.iDest][move.jDest] == EMPTY)
            return 0;
        //capture to your friend is error
        if(checkDifferentColors(board,move) == 0)
        {
            return 0;
        }
        return 1;
    }
        //TODO: לא יודעת למה הורדת את זה זה דפק הכל, תתקשר אלי אם אתה לא מבין מה עשינו פהלאביו דובי
    else if(move.isCapture == 0)
    {
        char digit = board[move.iDest][move.jDest];
        //trying to move to place with enemy without saying it is capture - not valid
//        if((move.isWhite && islower(digit)) || (!move.isWhite && isupper(digit)))
//            return 0;
        if(digit != EMPTY)
            return 0;
//        if(checkDifferentColors(board,move) == 1)
//        {
//            return 0;//Capture without saying it
//        }
        return 1;
    }
}

//int checkValidCheck(char board[][SIZE],Move move,Check check)
//{
//   if(move.isCheck == 0)
//       return 0;
//   else
//       if(move.isWhite)
//       {
//           for (int i = 0 ; i < SIZE ; i++)
//           {
//               for (int j = 0 ; j < SIZE ; j++)
//               {
//                   if(board[i][j] == WHITE_KING)
//                   {
//                       check.col = j;
//                       check.row = i;
//                       check.player = WHITE_KING;
//                       break;
//                   }
//               }
//           }
//           //  מחפש איומים באותה שורה לצד ימין
//           for (int k = check.row ; k < SIZE ; k++)
//           {
//             if(board[check.row][k] == BLACK_QUEEN || board[check.row][k] == BLACK_ROOK )
//
//           }
//       }
//}

int isLastRow(char board[][SIZE],Move move)
{
    if((move.isWhite && move.iSrc==1) || (!(move.isWhite) && move.iSrc==SIZE-2))
        return 1;
    else
        return 0;
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
            for (int l = 0; l < numOfSpace; l++)
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

char whichPromotion(char pgn[])
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
        }
        else{return 1;}
    }
    return 0;

}

char whichPlayer(char pgn[])
{
    if(isupper(pgn[0]))
    {
        return pgn[0];
    } else
        return PAWN;
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

/*
 * the function check that if this is promotion move than it's valid - the player is pawn in
 * last raw.
 * also it checks the case of a pawn that in last row and there is no promotion which is
 * also not valid
 */


int checkValidPromotion(char board[][SIZE],Move move) {
    if(move.isPromotion == 1){
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

Location searchBlackPawn(char board[][SIZE],Move move)
{
    Location loc;
    loc.isValid = 1;
    if(move.isCapture)
    {
        loc.row = move.iDest-1;
        if(board[loc.row][move.jDest-1] == BLACK_PAWN)
        {

            loc.col = move.jDest - 1 ;
        }
        else
            loc.col = move.jDest + 1;

    } else{
        loc.col = move.jDest;
        //can be double step
        if(move.iDest == 3)
        {
            if(board[move.iDest-1][move.jDest] == BLACK_PAWN)
                loc.row = move.iDest - 1;
            else
                loc.row = move.iDest - 2;
        } else{
            loc.row = move.iDest - 1;
        }
    }
    if(board[loc.row][loc.col] != BLACK_PAWN)
    {
        loc.isValid =0;
    }
    return loc;
}

Location searchWhitePawn(char board[][SIZE],Move move)
{
    Location loc;
    loc.isValid = 1;
    //TODO: הטעות שלנו הייתה פה,שלא אתחלנו את ואליד להיות 1 במקרה שהכל בסדר
    if(move.isCapture)
    {
        loc.row = move.iDest+1;
        if(board[loc.row][move.jDest-1] == WHITE_PAWN)
            //left col
            loc.col = move.jDest - 1 ;
        else
            //right col
            loc.col = move.jDest + 1;

    } else{
        loc.col = move.jDest;
        //can be double step
        if(move.iDest == SIZE - 4)
        {
            if(board[move.iDest+1][move.jDest] == WHITE_PAWN)
                loc.row = move.iDest + 1;
            else if(board[move.iDest+2][move.jDest] == WHITE_PAWN)
                loc.row = move.iDest + 2;

        }
        else{
            loc.row = move.iDest + 1;
        }
    }
    if(board[loc.row][loc.col] != WHITE_PAWN)
    {
        loc.isValid =0;
    }
    return loc;
}

Location searchRook(char board[][SIZE],Move move,char player)
{
    Location loc;
    loc.isValid = 0;
    int flag = 0;
    if(move.hasSrcCol)
    {
        move.jSrc = changeCharToIndex(move.srcCol);
        //טור מהיעד כלפי מעלה
        for (int i = move.iDest -1 ; i >= 0 ; i--)//HERE we should find it
        {
            if(board[i][move.jSrc] == player)
            {
                loc.row = i;
                loc.col = move.jSrc;
                loc.isValid = 1;//we found it
//                break;
                return loc;
            }
            if(board[i][move.jSrc] != EMPTY)
                break;
        }
        //טור מהיעד כלפי מטה
        for (int i = move.iDest; i < SIZE; i++)
        {
            if(board[i][move.jSrc] == player)
            {
                loc.row = i;
                loc.col = move.jSrc;
                loc.isValid = 1;//we found it
//                break;
                return loc;
            }
            if(board[i][move.jSrc]!=EMPTY)
                break;
        }
    }
    else if (move.hasSrcRow)
    {
        move.iSrc = changeCharToIndex(move.srcRow);
        //שורה מהיעד כלפי ימינה
        for (int j = move.jDest+1; j < SIZE; j++)
        {
            if(board[move.iSrc][j] == player)
            {
                loc.col = j;
                loc.row = move.iSrc;
                flag = 1;
                loc.isValid = 1;//we found it
                //break;
                return loc;
            }
            if(board[move.iSrc][j] != EMPTY)
                break;
        }
        //שורה מהיעד כלפי שמאלה
        for (int j = move.jDest-1; j >= 0; j--)
        {
            if(board[move.iSrc][j] == player)
            {
                loc.col = j;
                loc.row = move.iSrc;
                flag = 1;
                loc.isValid = 1;//we found it
//            break;
                return loc;
            }
            if(board[move.iSrc][j] != EMPTY)
                break;
        }
    }
    else
    {
        //טור מהיעד כלפי מעלה
        for (int i = move.iDest-1; i >= 0; i--)//HERE we should find it
        {
            if(board[i][move.jDest] == player)
            {
                loc.row = i;
                loc.col = move.jDest;
                loc.isValid = 1;//we found it
//                break;
                return loc;
            }
            if(board[i][move.jDest] != EMPTY)
                break;
        }
        //טור מהיעד כלפי מטה
        for (int i = move.iDest+1; i < SIZE; i++)
        {
            if(board[i][move.jDest] == player)
            {
                loc.row = i;
                loc.col = move.jDest;
                loc.isValid = 1;//we found it
//                break;
                return loc;
            }
            if(board[i][move.jDest]!=EMPTY)
                break;
        }
        //שורה מהיעד כלפי שמאלה
        for (int j = move.jDest-1; j >= 0; j--)
        {
            if(board[move.iDest][j] == player)
            {
                loc.col = j;
                loc.row = move.iDest;
                flag = 1;
                loc.isValid = 1;//we found it
//            break;
                return loc;
            }
            if(board[move.iDest][j] != EMPTY)
                break;
        }
//שורה מהיעד כלפי ימינה
        for (int j = move.jDest+1; j < SIZE; j++)
        {
            if(board[move.iDest][j] == player)
            {
                loc.col = j;
                loc.row = move.iDest;
                flag = 1;
                loc.isValid = 1;//we found it
                //break;
                return loc;
            }
            if(board[move.iDest][j] != EMPTY)
                break;
        }
    }
    return loc;
}

Location searchKing(char board[][SIZE],Move move)
{
    char playerColor;
    if(move.isWhite)
        playerColor = WHITE_KING;
    else
        playerColor = BLACK_KING;
    Location loc;
    loc.isValid = 0;
    for (int i = move.iDest-1; i <=move.iDest+1 ; i++)
    {
        for (int j = move.jDest-1; j <= move.jDest+1 ; j++)
        {
            if(i<0 || i>=SIZE || j<0 || j>=SIZE)
                continue;
            if(board[i][j]==playerColor)
            {
                loc.col = j;
                loc.row = i;
                loc.isValid = 1;
                break;
            }
        }
    }
    return loc;
}

Location searchBishop(char board[][SIZE],Move move,char player) {
    Location loc;
    loc.row = -1;
    loc.col = -1;
    int flag = 0;
    int j = move.jDest - 1;

//    if (move.hasSrcCol)
//    {
//        move.jSrc = changeCharToIndex(move.srcCol);
//        int steps = 0;
//        steps = abs(move.jSrc - move.jDest);
//        char side;
//        side = move.jSrc > move.jDest ? 'L' : 'R';
//        if(side == 'L')
//            if(board[])
//    }else if(move.hasSrcRow){
//
//    }else
//    {
    //diagnol line from left corner to right down
    for (int i = move.iDest - 1; i >=0 ; i--)
    {
        if(j<0)
            break;
        if(board[i][j] == player)
        {
            loc.col = j;
            loc.row = i;
            loc.isValid = 1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }
            else
                return loc;
            flag = 1;
        }
        if(board[i][j] != EMPTY)
            break;
        j--;
    }
//        if(!flag)
//        {
    j = move.jDest+1;
    //diagnol line up and left
    for (int i = move.iDest + 1; i <SIZE ; i++)
    {
        if(j>=SIZE)
            break;
        if(board[i][j] == player)
        {
            loc.col = j;
            loc.row = i;
            loc.isValid = 1;
            flag = 1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }
            else
                return loc;
        }
        if(board[i][j] != EMPTY)
            break;
        j++;
    }
//        }
//        if(!flag)
//        {
    j = move.jDest-1;
    //diagnol line up and right
    for (int i = move.iDest + 1; i <SIZE ; i++)
    {
        if(j<0)
            break;
        if(board[i][j] == player)
        {
            loc.col = j;
            loc.row = i;
            loc.isValid = 1;
            flag = 1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }

        }
        if(board[i][j] != EMPTY)
            break;
        j--;
    }
//        }
//        if(!flag)
//        {
    j = move.jDest+1;
    //diagnol line down and left
    for (int i = move.iDest - 1; i >= 0 ; i--)
    {
        if(j>=SIZE)
            break;
        if(board[i][j] == player)
        {
            loc.col = j;
            loc.row = i;
            loc.isValid = 1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }
            else
                return loc;
        }
        if(board[i][j] != EMPTY)
            break;
        j++;
    }
//        }
//    }

    return loc;
}

Location searchKnight(char board[][SIZE],Move move)
{
    char playerColor;
    if(move.isWhite)
        playerColor = WHITE_KNIGHT;
    else
        playerColor = BLACK_KNIGHT;
    Location loc;
    int flag = 0;
    loc.isValid = 0;
    int i = move.iDest,j=move.jDest;

    //2 steps row one col
    if(i-2>=0 && j-1>=0)
    {
        if(board[i-2][j-1] == playerColor)
        {
            loc.row = i-2;
            loc.col = j-1;
            loc.isValid = 1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
            flag=1;
        }
    }
//    if(!flag)
//    {
    if(i-2>=0 && j+1<SIZE)
    {
        if(board[i-2][j+1] == playerColor)
        {
            loc.row = i-2;
            loc.col = j+1;
            loc.isValid = 1;
            flag=1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
        }
    }
//    }
//    if(!flag)
//    {
    if(i+2<SIZE && j+1<SIZE)
    {
        if(board[i+2][j+1] == playerColor)
        {
            loc.row = i+2;
            loc.col = j+1;
            loc.isValid = 1;
            flag=1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
        }
    }
//    }
//    if(!flag)
//    {
    if(i+2<SIZE && j-1>=0)
    {
        if(board[i+2][j-1] == playerColor)
        {
            loc.row = i+2;
            loc.col = j-1;
            loc.isValid = 1;
            flag=1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
        }
    }
//    }

    //2 steps col one row
//    if(!flag)
//    {
    if(i+1<SIZE && j-2>=0)
    {
        if(board[i+1][j-2] == playerColor)
        {
            loc.row = i+1;
            loc.col = j-2;
            loc.isValid = 1;
            flag=1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
        }
    }
//    }

//    if(!flag)
//    {
    if(i+1<SIZE && j+2< SIZE)
    {
        if(board[i+1][j+2] == playerColor)
        {
            loc.row = i+1;
            loc.col = j+2;
            loc.isValid = 1;
            flag=1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
        }
    }
//    }

//    if(!flag)
//    {
    if(i-1>=0 && j-2>=0)
    {
        if(board[i-1][j-2] == playerColor)
        {
            loc.row = i-1;
            loc.col = j-2;
            loc.isValid = 1;
            flag=1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
        }
    }
//    }

//    if(!flag)
//    {
    if(i-1>=0 && j+2<SIZE)
    {
        if(board[i-1][j+2] == playerColor)
        {
            loc.row = i-1;
            loc.col = j+2;
            loc.isValid = 1;
            if(move.hasSrcCol){
                int colFound = changeCharToIndex(move.srcCol);
                if(loc.col == colFound){
                    return loc;
                }
            }else if(move.hasSrcRow){
                int rowFound = changeCharToIndex(move.srcRow);
                if(loc.row == rowFound){
                    return loc;
                }
            }else{
                return loc;
            }
        }
    }
//    }
    return loc;
}

Location searchQueen(char board[][SIZE],Move move)
{
    char playerColor;
    if(move.isWhite)
        playerColor = WHITE_QUEEN;
    else
        playerColor = BLACK_QUEEN;
    Location loc;
    loc = searchBishop(board,move,playerColor);
    //we didnt find this in bishop path, so let try to find it in rook path
    if(loc.row==-1 || loc.col==-1)
    {
        //search the same whay so use this function
        loc = searchRook(board,move,playerColor);
    }
    return loc;
}

Location searchSrc(char board[][SIZE],Move move)
{
    //TODO: לבדוק מה תשובת המרצה ואם לא חוקי אז תמיד לבדוק שהתו הנבדק אם הוא לא החייל המבוקש שהוא תו ריק לעשות ברייק  ולהמשיך לחפש בכיוונים השונים
    Location loc;
    switch (move.player) {
        case 'P':
        {
            if(move.isWhite)
                loc = searchWhitePawn(board,move);
            else
                loc = searchBlackPawn(board,move);
            break;
        }
        case 'R':
        {
            char playerColor;
            if(move.isWhite)
                playerColor = WHITE_ROOK;
            else
                playerColor = BLACK_ROOK;

            loc = searchRook(board,move,playerColor);
            break;
        }
        case 'Q':
        {
            loc = searchQueen(board,move);
            break;
        }
        case 'K':
        {
            loc = searchKing(board,move);
            break;
        }
        case 'B':
        {
            char playerColor;
            if(move.isWhite)
                playerColor = WHITE_BISHOP;
            else
                playerColor = BLACK_BISHOP;
            loc = searchBishop(board,move,playerColor);
            break;
        }
        case 'N':
        {
            loc = searchKnight(board,move);
            break;
        }
    }
    return loc;
}

char capitalToLower(char capitalLetter)
{
    char lowerLetter = capitalLetter + ('a' - 'A');
    return lowerLetter;
}

int isValidMove(char board[][SIZE],Move move)
{
    switch (move.player)
    {
        case 'P':
        case 'p':
        {
            if(!checkValidPawn(board,move))
                return  0;
            break;
        }
        case 'R':
        case 'r':
        {
            if(checkValidRook(board,move) == 0)
                return 0;
            break;
        }
        case 'Q':
        case 'q':
        {
            if(checkValidQueen(board,move) == 0)
                return 0;
            break;
        }
        case 'K':
        case 'k':
        {
            if(checkValidKing(board,move)==0)
                return 0;
            break;
        }
        case 'B':
        case 'b':
        {
            //TODO: נופל פה
            if(checkValidBishop(board,move)==0)
                return 0;
            break;
        }
        case 'N':
        case 'n':
        {
            if(checkValidKnight(board,move) == 0)
                return 0;
            break;
        }
    }

    if(checkValidPromotion(board,move) == 0)
        return 0;

    if(checkValidCapture(board,move) == 0)
        return 0;

//    if(isKingSafe2(board,move) == 0)
////        return 0;

//    //TODO: Add check for shach and mat
//    if(checkValidCheck(board,move) == 0)
//        return 0;

    return 1;
}

int isKingSafe2(char board[][SIZE],Move move)
{
    Move moveKing;
    char kingColor;
    if(move.isWhite)
        kingColor = WHITE_KING;
    else
        kingColor = BLACK_KING;
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if(board[i][j] == kingColor)
            {
                moveKing.iDest = i;
                moveKing.jDest = j;
                break;
            }
        }
    }

    if(move.isWhite)
    {
        for (int i = 0; i < SIZE ; ++i)
        {
            for (int j = 0; j < SIZE ; ++j)
            {
                char player = board[i][j];
                if(player == WHITE_KING)
                    continue;
                if(player == BLACK_KING)
                {
                    moveKing.player = BLACK_KING;
                }else if(player == BLACK_KNIGHT){
                    moveKing.player = BLACK_KNIGHT;
                }else if(player == BLACK_QUEEN){
                    moveKing.player = BLACK_QUEEN;
                }else if(player == BLACK_BISHOP){
                    moveKing.player = BLACK_BISHOP;
                }else if(player == BLACK_PAWN){
                    moveKing.player = BLACK_PAWN;
                }else if(player == BLACK_ROOK){
                    moveKing.player = BLACK_ROOK;
                }

                moveKing.iSrc = i;
                moveKing.jSrc = j;
                moveKing.isCapture = 1;
                moveKing.isWhite = 0;
                if(isValidMove(board,moveKing) == 1){
                    //המלך מאויים ולכן אפשר לעצור את הסריקה ולהחזיר אפס
                    return 0;
                }
            }
        }
        return 1;
    }
    else{

        for (int i = 0; i < SIZE ; ++i)
        {
            for (int j = 0; j < SIZE ; ++j)
            {
                char player = board[i][j];
                if(player == BLACK_KING)
                    continue;
                if(player == WHITE_KING)
                {
                    moveKing.player = WHITE_KING;
                }else if(player == WHITE_KNIGHT){
                    moveKing.player = WHITE_KNIGHT;
                }else if(player == WHITE_QUEEN){
                    moveKing.player = WHITE_QUEEN;
                }else if(player == WHITE_BISHOP){
                    moveKing.player = WHITE_BISHOP;
                }else if(player == WHITE_PAWN){
                    moveKing.player = WHITE_PAWN;
                }else if(player == WHITE_ROOK){
                    moveKing.player = WHITE_ROOK;
                }

                moveKing.iSrc = i;
                moveKing.jSrc = j;
                moveKing.isCapture = 1;
                moveKing.isWhite = 1;
                if(isValidMove(board,moveKing) == 1){
                    //המלך מאויים ולכן אפשר לעצור את הסריקה ולהחזיר אפס
                    return 0;
                }
            }
        }
        return 1;
    }




}

int isEnemyKingSafe(char board[][SIZE],int isWhite)
{
    Move moveKing;
    char enemyKingColor;
    if(isWhite)
        enemyKingColor = BLACK_KING;
    else
        enemyKingColor = WHITE_KING;
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if(board[i][j] == enemyKingColor)
            {
                moveKing.iDest = i;
                moveKing.jDest = j;
                break;
            }
        }
    }

    if(!isWhite)
    {
        for (int i = 0; i < SIZE ; ++i)
        {
            for (int j = 0; j < SIZE ; ++j)
            {
                char player = board[i][j];
//                if(player == WHITE_KING)
//                    continue;
                if(player == BLACK_KING)
                {
                    moveKing.player = BLACK_KING;
                }else if(player == BLACK_KNIGHT){
                    moveKing.player = BLACK_KNIGHT;
                }else if(player == BLACK_QUEEN){
                    moveKing.player = BLACK_QUEEN;
                }else if(player == BLACK_BISHOP){
                    moveKing.player = BLACK_BISHOP;
                }else if(player == BLACK_PAWN){
                    moveKing.player = BLACK_PAWN;
                }else if(player == BLACK_ROOK){
                    moveKing.player = BLACK_ROOK;
                }
                if(player == BLACK_KING || player == BLACK_KNIGHT || player == BLACK_QUEEN || player == BLACK_BISHOP ||
                   player == BLACK_ROOK || player == BLACK_PAWN )
                {
                    moveKing.iSrc = i;
                    moveKing.jSrc = j;
                    moveKing.isCapture = 1;
                    moveKing.isWhite = 0;
                    if(isValidMove(board,moveKing) == 1){
                        //המלך מאויים ולכן אפשר לעצור את הסריקה ולהחזיר אפס
                        return 0;
                    }
                }

            }
        }
        return 1;
    }
    else{

        for (int i = 0; i < SIZE ; ++i)
        {
            for (int j = 0; j < SIZE ; ++j)
            {
                char player = board[i][j];
//                if(player == BLACK_KING)
//                    continue;
                if(player == WHITE_KING)
                {
                    moveKing.player = WHITE_KING;
                }else if(player == WHITE_KNIGHT){
                    moveKing.player = WHITE_KNIGHT;
                }else if(player == WHITE_QUEEN){
                    moveKing.player = WHITE_QUEEN;
                }else if(player == WHITE_BISHOP){
                    moveKing.player = WHITE_BISHOP;
                }else if(player == WHITE_PAWN){
                    moveKing.player = WHITE_PAWN;
                }else if(player == WHITE_ROOK){
                    moveKing.player = WHITE_ROOK;
                }

                if(player == WHITE_KING || player == WHITE_KNIGHT || player == WHITE_QUEEN || player == WHITE_BISHOP ||
                   player == WHITE_PAWN || player == WHITE_ROOK ){
                    moveKing.iSrc = i;
                    moveKing.jSrc = j;
                    moveKing.isCapture = 1;
                    moveKing.isWhite = 1;
                    if(isValidMove(board,moveKing) == 1){
                        //המלך מאויים ולכן אפשר לעצור את הסריקה ולהחזיר אפס
                        return 0;
                    }
                }

            }
        }
        return 1;
    }




}

int isKingSafe(char board[][SIZE],Move move)
{
    //חיפוש המלך הלבן
    if(move.isWhite == 1)
    {
        for (int i = 0; i < SIZE ; i++)
        {
            for (int j = 0 ; j < SIZE ; j++)
            {
                if(board[i][j]== WHITE_KING)
                {
                    move.iDest = i;
                    move.jDest = j;
                    break;
                }
            }
        }
        //בדיקה שאין מעלי רגלי שיכול לתקוף את המלך והוא משבצת ממנו
        if(board[move.iDest-1][move.jDest-1]==BLACK_PAWN || board[move.iDest-1][move.jDest+1]==BLACK_PAWN)
            return 0;
        // בדיקה שבאותו שורה אין מלכה או צריח לכיוון ימין
        for (int k = move.jDest+1 ; k < SIZE ; k++)
        {
            if(board[move.iDest][k] == BLACK_ROOK || board[move.iDest][k] == BLACK_QUEEN)
            {
                if(board[move.iDest][k] == BLACK_ROOK)
                    move.player = BLACK_ROOK;
                else move.player = BLACK_QUEEN;
                move.jSrc = k;
                move.iSrc = move.iDest;
                move.isWhite = 0;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        // בדיקה שבאותו שורה אין מלכה או צריח לכיוון שמאל
        for (int k = move.jDest-1 ; k >= 0 ; k--)
        {
            if(board[move.iDest][k] == BLACK_ROOK || board[move.iDest][k] == BLACK_QUEEN)
            {
                if(board[move.iDest][k] == BLACK_ROOK)
                    move.player = BLACK_ROOK;
                else move.player = BLACK_QUEEN;
                move.jSrc = k;
                move.iSrc = move.iDest;
                move.isWhite = 0;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        // בדיקה שבאותו תור אין מלכה או צריח לכיוון מטה
        for (int k = move.iDest+1 ; k < SIZE ; k++)
        {
            if(board[k][move.jDest] == BLACK_ROOK || board[k][move.jDest] == BLACK_QUEEN)
            {
                if(board[k][move.jDest] == BLACK_ROOK)
                    move.player = BLACK_ROOK;
                else move.player = BLACK_QUEEN;
                move.iSrc = k;
                move.jSrc = move.jDest;
                move.isWhite = 0;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        // בדיקה שבאותו תור אין מלכה או צריח לכיוון מעלה
        for (int k = move.iDest-1 ; k >= 0 ; k--)
        {
            if(board[k][move.jDest] == BLACK_ROOK || board[k][move.jDest] == BLACK_QUEEN)
            {
                if(board[k][move.jDest] == BLACK_ROOK)
                    move.player = BLACK_ROOK;
                else move.player = BLACK_QUEEN;
                move.iSrc = k;
                move.jSrc = move.jDest;
                move.isWhite = 0;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        //בדיקה שאין באלכסון לכיון שמאל למעלה כלים עויינים
        for (int l = move.iDest-1; l >= 0 ; l--)
        {
            for (int t = move.jDest-1; t >=0 ; t--)
            {
                if(board[l][t] == BLACK_QUEEN || board[l][t] == BLACK_BISHOP)
                {
                    if(board[l][t] == BLACK_BISHOP)
                        move.player = BLACK_BISHOP;
                    else move.player = BLACK_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 0;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שאין אלכסון לכיוןן שמאל למעלה כלים עויינים
        for (int l = move.iDest-1; l >= 0 ; l--)
        {
            for (int t = move.jDest+1; t <SIZE ; t++)
            {
                if(board[l][t] == BLACK_QUEEN || board[l][t] == BLACK_BISHOP)
                {
                    if(board[l][t] == BLACK_BISHOP)
                        move.player = BLACK_BISHOP;
                    else move.player = BLACK_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 0;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שאין באלכסון לכיוון שמאל למטה כלים עוינים
        for (int l = move.iDest+1; l < SIZE ; l++)
        {
            for (int t = move.jDest-1; t >=0 ; t--)
            {
                if(board[l][t] == BLACK_QUEEN || board[l][t] == BLACK_BISHOP)
                {
                    if(board[l][t] == BLACK_BISHOP)
                        move.player = BLACK_BISHOP;
                    else move.player = BLACK_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 0;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שאין באלכסון לכיון ימין למטה כלים עויינים
        for (int l = move.iDest-1; l <SIZE ; l++)
        {
            for (int t = move.jDest-1; t <SIZE ; t++)
            {
                if(board[l][t] == BLACK_QUEEN || board[l][t] == BLACK_BISHOP)
                {
                    if(board[l][t] == BLACK_BISHOP)
                        move.player = BLACK_BISHOP;
                    else move.player = BLACK_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 0;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שהפרש לא יכול לתקוף את המלך
        if(board[move.iDest-2][move.jDest-1]==BLACK_KNIGHT)
            return 0;
        if(board[move.iDest-2][move.jDest+1]==BLACK_KNIGHT)
            return 0;
        if(board[move.iDest+2][move.jDest-1]==BLACK_KNIGHT)
            return 0;
        if(board[move.iDest+2][move.jDest+1]==BLACK_KNIGHT)
            return 0;
        if(board[move.iDest-1][move.jDest-2]==BLACK_KNIGHT)
            return 0;
        if(board[move.iDest+1][move.jDest-2]==BLACK_KNIGHT)
            return 0;
        if(board[move.iDest-1][move.jDest+2]==BLACK_KNIGHT)
            return 0;
        if(board[move.iDest+1][move.jDest+2]==BLACK_KNIGHT)
            return 0;
    }
        //חיפוש המלך השחור
    else if (move.isWhite == 0)
    {
        for (int i = 0; i < SIZE ; i++)
        {
            for (int j = 0 ; j < SIZE ; j++)
            {
                if(board[i][j]== BLACK_KING)
                {
                    move.iDest = i;
                    move.jDest = j;
                    break;
                }
            }
        }
        //בדיקה שאין מעלי רגלי שיכול לתקוף את המלך והוא משבצת ממנו
        if(board[move.iDest+1][move.jDest-1]==WHITE_PAWN || board[move.iDest+1][move.jDest+1]==WHITE_PAWN)
            return 0;

        // בדיקה שבאותו שורה אין מלכה או צריח לכיוון ימין
        for (int k = move.jDest+1 ; k < SIZE ; k++)
        {
            if(board[move.iDest][k] == WHITE_ROOK || board[move.iDest][k] == WHITE_QUEEN)
            {
                if(board[move.iDest][k] == WHITE_ROOK)
                    move.player = WHITE_ROOK;
                else move.player = WHITE_QUEEN;
                move.jSrc = k;
                move.iSrc = move.iDest;
                move.isWhite = 1;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        // בדיקה שבאותו שורה אין מלכה או צריח לכיוון שמאל
        for (int k = move.jDest-1 ; k >= 0 ; k--)
        {
            if(board[move.iDest][k] == WHITE_ROOK || board[move.iDest][k] == WHITE_QUEEN)
            {
                if(board[move.iDest][k] == WHITE_ROOK)
                    move.player = WHITE_ROOK;
                else move.player = WHITE_QUEEN;
                move.jSrc = k;
                move.iSrc = move.iDest;
                move.isWhite = 1;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        // בדיקה שבאותו תור אין מלכה או צריח לכיוון מטה
        for (int k = move.iDest+1 ; k < SIZE ; k++)
        {
            if(board[k][move.jDest] == WHITE_ROOK || board[k][move.jDest] == WHITE_QUEEN)
            {
                if(board[k][move.jDest] == WHITE_ROOK)
                    move.player = WHITE_ROOK;
                else move.player = WHITE_QUEEN;
                move.iSrc = k;
                move.jSrc = move.jDest;
                move.isWhite = 1;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        // בדיקה שבאותו תור אין מלכה או צריח לכיוון מעלה
        for (int k = move.iDest-1 ; k >= 0 ; k--)
        {
            if(board[k][move.jDest] == WHITE_ROOK || board[k][move.jDest] == WHITE_QUEEN)
            {
                if(board[k][move.jDest] == WHITE_ROOK)
                    move.player = WHITE_ROOK;
                else move.player = WHITE_QUEEN;
                move.iSrc = k;
                move.jSrc = move.jDest;
                move.isWhite = 1;
                move.isCapture = 1;
                if(isValidMove(board,move))
                    return 0;
            }
        }
        //בדיקה שאין באלכסון לכיון שמאל למעלה כלים עויינים
        for (int l = move.iDest-1; l >= 0 ; l--)
        {
            for (int t = move.jDest-1; t >=0 ; t--)
            {
                if(board[l][t] == WHITE_QUEEN || board[l][t] == WHITE_BISHOP)
                {
                    if(board[l][t] == WHITE_BISHOP)
                        move.player = WHITE_BISHOP;
                    else move.player = WHITE_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 1;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שאין אלכסון לכיוןן שמאל למעלה כלים עויינים
        for (int l = move.iDest-1; l >= 0 ; l--)
        {
            for (int t = move.jDest+1; t <SIZE ; t++)
            {
                if(board[l][t] == WHITE_QUEEN || board[l][t] == WHITE_BISHOP)
                {
                    if(board[l][t] == WHITE_BISHOP)
                        move.player = WHITE_BISHOP;
                    else move.player = WHITE_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 1;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שאין באלכסון לכיוון שמאל למטה כלים עוינים
        for (int l = move.iDest+1; l < SIZE ; l++)
        {
            for (int t = move.jDest-1; t >=0 ; t--)
            {
                if(board[l][t] == WHITE_QUEEN || board[l][t] == WHITE_BISHOP)
                {
                    if(board[l][t] == WHITE_BISHOP)
                        move.player = WHITE_BISHOP;
                    else move.player = WHITE_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 1;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שאין באלכסון לכיון ימין למטה כלים עויינים
        for (int l = move.iDest-1; l <SIZE ; l++)
        {
            for (int t = move.jDest-1; t <SIZE ; t++)
            {
                if(board[l][t] == WHITE_QUEEN || board[l][t] == WHITE_BISHOP)
                {
                    if(board[l][t] == WHITE_BISHOP)
                        move.player = WHITE_BISHOP;
                    else move.player = WHITE_QUEEN;
                    move.jSrc = t;
                    move.iSrc = l;
                    move.isWhite = 1;
                    move.isCapture = 1;
                    if(isValidMove(board,move))
                        return 0;
                }
            }
        }
        //בדיקה שהפרש לא יכול לתקוף את המלך
        if(board[move.iDest-2][move.jDest-1]==WHITE_KNIGHT)
            return 0;
        if(board[move.iDest-2][move.jDest+1]==WHITE_KNIGHT)
            return 0;
        if(board[move.iDest+2][move.jDest-1]==WHITE_KNIGHT)
            return 0;
        if(board[move.iDest+2][move.jDest+1]==WHITE_KNIGHT)
            return 0;
        if(board[move.iDest-1][move.jDest-2]==WHITE_KNIGHT)
            return 0;
        if(board[move.iDest-1][move.jDest+2]==WHITE_KNIGHT)
            return 0;
        if(board[move.iDest+1][move.jDest-2]==WHITE_KNIGHT)
            return 0;
        if(board[move.iDest+1][move.jDest+2]==WHITE_KNIGHT)
            return 0;
    }
    return 1;
}

int isValidCheck(char board[][SIZE],Move move)
{
    if(move.isCheck || move.isMate)
    {
        //אם כן ואין ואיום איום על המלך היריב אז טעות
        if(isEnemyKingSafe(board,move.isWhite) == 1)
            return 0;
    } else{
        if(isEnemyKingSafe(board,move.isWhite) == 0)
            return 0;
    }
    return 1;
}

//void backupBoard(char board[][SIZE],Move storage)
//{
//
//}

void updateBoard(char board[][SIZE],Move storage)
{
    //clean src location
    board[storage.iSrc][storage.jSrc] = EMPTY;

    //update dest location
    char player;

    if(!storage.isWhite)
        board[storage.iDest][storage.jDest] = capitalToLower(storage.player);
    else
        board[storage.iDest][storage.jDest] = storage.player;

}

int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn) {
    Move move;
    Move storage;
    move.hasSrcCol = 0;
    move.hasSrcRow = 0;
    move.player = whichPlayer(pgn);
    move.isMate = isMate(pgn);
    move.isCheck = isCheck(pgn);
    move.isCapture = isCapture(pgn);
    move.isPromotion = isPromotion(pgn);
    move.isWhite = isWhiteTurn;

    if(move.isPromotion)
    {
        move.promotionChange = whichPromotion(pgn);
    }

    //if there is a src col set it
    if (isSrcCol(pgn)) {
        move.srcCol = findSrcCol(pgn);
        move.destCol = findDstCol(pgn, 1);
        move.hasSrcCol = 1;

    } else {
        move.destCol = findDstCol(pgn, 0);
    }

    if (isSrcRow(pgn)) {
        move.srcRow = findSrcRow(pgn);
        move.destRow = findDstRow(pgn, 1);
        move.hasSrcRow = 1;
    } else {
        move.destRow = findDstRow(pgn, 0);
    }
    move.jDest = changeCharToIndex(move.destCol);
    move.iDest = changeCharToIndex(move.destRow);

    if (!move.hasSrcRow || !move.hasSrcCol) {
        Location loc = searchSrc(board, move);
        if (loc.isValid == 0)
            return 0;
        if (move.hasSrcRow) {
            move.iSrc = changeCharToIndex(move.srcRow);
            move.jSrc = loc.col;
        } else if (move.hasSrcCol) {
            move.jSrc = changeCharToIndex(move.srcCol);
            move.iSrc = loc.row;
        } else {
            move.iSrc = loc.row;
            move.jSrc = loc.col;
        }
    } else {
        //TODO: אחרי שמוצאים את נקודת המוצא אפשר לעדכן את האינדקסים של המוצא
        //update index values:
        move.jSrc = changeCharToIndex(move.srcCol);
        move.iSrc = changeCharToIndex(move.srcRow);
    }

    if (isValidMove(board,move) == 0)
        return 0;

    if (move.isPromotion)
    {
//        if(isValidMove(board,move))
//        {
        move.player = move.promotionChange;
//        }
    }



    storage.iDest = move.iDest;
    storage.jDest = move.jDest;
    storage.iSrc = move.iSrc;
    storage.jSrc = move.jSrc;
    storage.player = move.player;
    storage.isWhite = move.isWhite;
    storage.isPromotion = move.isPromotion;
    storage.promotionChange = move.promotionChange;

//    if(move.isCheck)
//    {
//        if(isKingSafe(board,move) == 0)
//            return 0;
//    }
//    else if(move.isMate)
//    {
//        if(isKingSafe(board,move)==1)
//            updateBoard(board,storage);
//    }

//    if(isKingSafe2(board,move) == 0)
//        return 0;

    char board2[SIZE][SIZE];
    for (int i = 0; i < SIZE ; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            board2[i][j] = board[i][j];
        }
    }

    updateBoard(board,storage);


    if(isValidCheck(board,move) == 0)
    {
        for (int i = 0; i < SIZE ; ++i)
        {
            for (int j = 0; j < SIZE; ++j)
            {
                board[i][j] = board2[i][j];
            }
        }
        return  0;
    }





    return 1;
}



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


