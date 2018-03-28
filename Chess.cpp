//
//  main.cpp
//  Chess
//
//  Created by Justin Timberlake on 2018/1/17.
//  Copyright © 2018年 Justin Timberlake. All rights reserved.
//

#include<iostream>
#include<Windows.h>
#include<time.h>
#include "Presetting.h"
#pragma comment(lib, "winmm") 

const LPCSTR cszAbout = "tan90,不存在的";

//Attributes of pic and size
const int WINDOWS_STYLES = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
const int MASK_COLOUR = RGB(0,255,0);
const int SQUARE_SIZE = 56;    //changed
const int BOARD_EDGE = 8;//changed
const int BOARD_WIDTH = BOARD_EDGE + SQUARE_SIZE *9 + BOARD_EDGE;
const int BOARD_LENGTH = BOARD_EDGE + SQUARE_SIZE * 10 + BOARD_EDGE;

//Range of chess board
const int RANGE_TOP = 3;
const int RANGE_BOTTOM = 12;    //length range of board
const int RANGE_LEFT = 3;
const int RANGE_RIGHT = 11;

//Number of chess
const int PIECE_KING = 0;    //王
const int PIECE_ADVISOR = 1;    //士
const int PIECE_BISHOP = 2;    //象
const int PIECE_KNIGHT = 3;    //马
const int PIECE_ROOK = 4;    //车
const int PIECE_CANNON = 5;    //炮
const int PIECE_PAWN = 6;    //兵

//essential attributes
const int MAX_GEN_MOVES = 128;    //max number of strategy produced
const int MAX_MOVES = 256;    //max number of history strategy
const int MAX_DEPTH = 64;    //max search depth
const int MAX_VALUE = 10000; //mate value
const int BAN_VALUE = MAX_VALUE - 100;    //认为可接受的最低分值，当低于该值时则可省去
const int WIN_VALUE = MAX_VALUE - 200;    //可判出胜负的分值，当大于该值时表示有杀棋
const int DRAW_VALUE = 20; //平局时返回给博弈树的值（<0)
const int ADVANCED_VALUE = 3; //先行权分值 doubts
const int RANDOM_MASK = 7;    //随机性分值
const int NULL_MARGIN = 400;    //空步裁剪的子力边界
const int NULL_DEPTH = 2;    //空步裁剪的深度
const int HASH_SIZE = 1 << 20;    //置换表大小
const int HASH_BETA = 2;    //BETA节点置换表项
const int HASH_ALPHA = 1;    //ALPHA节点置换表项
const int HASH_PV = 3;        //PV节点置换表项
const int BOOK_SIZE = 16384;    //开局库大小

//Judge whether chess is in board
static const char  ChessInBoard[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//for dangerous sitation to king, to judege whether and where
//enemy's chess is in king's home
static const char ChessInFort[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//Judge whether your movement is suitable for rules(for kings,advisors,bishop)
static const char ChessLegal[512] = {
                       0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};

//Judge whether your knights can move in your plan
static const char ChessKnight[512] =  {
                              0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0
};

//move of king(whether other chess is in its path to stop it)
static const char ChessKing[4] = {-16,-1,1,16};
//move of advisor
static const char ChessAdvisor[4] = {-17,-15,15,17};
//move of knight(the distance to its target place)
static const char ChessKnightDelta[4][2] = {{-33, -31}, {-18, 14}, {-14, 18}, {31, 33}};
//move of knight    (doubts)
static const char ChessKnightCheckDelta[4][2] = {{-33, -18}, {-31, -14}, {14, 31}, {18, 33}};

//Initial set of chess board
static const BYTE cucpcStartup[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 20, 19, 18, 17, 16, 17, 18, 19, 20,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0, 21,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,
    0,  0,  0, 22,  0, 22,  0, 22,  0, 22,  0, 22,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 14,  0, 14,  0, 14,  0, 14,  0, 14,  0,  0,  0,  0,
    0,  0,  0,  0, 13,  0,  0,  0,  0,  0, 13,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 12, 11, 10,  9,  8,  9, 10, 11, 12,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


// 子力位置价值表
static const BYTE cucvlPiecePos[7][256] = {
    { // king
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // advisor
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // bishop
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // knight
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
        0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
        0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
        0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
        0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
        0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
        0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
        0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
        0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,
        0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // rook
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
        0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
        0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
        0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
        0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
        0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
        0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
        0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
        0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
        0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // cannon
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
        0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
        0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
        0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
        0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
        0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
        0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
        0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
        0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
        0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }, { // pawn
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
        0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,
        0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,
        0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
        0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
        0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
        0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }
};

inline BOOL IN_BOARD(int sq)    //judge whether chess is in board
{    return ChessInBoard[sq]!=0;}

inline BOOL IN_FORT(int sq)        //判断是否在九宫内
{    return ChessInFort[sq]!=0;}

inline int RANGE_Y(int sq)    //获得棋子的横坐标
{    return sq>>4;}

inline int RANGE_X(int sq)    //获得棋子的纵坐标
{    return sq&15;}

inline int COORD_XY(int x,int y)    //根据纵横坐标获得棋子/格子
{    return x + (y<<4);}

inline int SQUARE_FLIP(int sq)    //反转格子
{    return 254 -sq;}

inline int RANGE_X_FLIP(int x)        //纵坐标水平镜像
{    return 14 - x;}

inline int RANGE_Y_FLIP(int y)        //横坐标水平镜像
{    return 15 - y;}

inline int MIRROR_SQUARE(int sq)    //格子水平镜像
{    return COORD_XY(RANGE_X_FLIP(RANGE_X(sq)),RANGE_Y(sq));}

inline int SQUARE_FORWARD(int sq, int sd)    //格子水平镜像    doubts
{    return sq - 16 + (sd<<5);}

inline BOOL KING_SPAN(int sqSrc, int sqDst){
    return ChessLegal[sqDst-sqSrc +256] == 1; }        //走法是否符合king的规则

inline BOOL ADVISOR_SPAN(int sqSrc, int sqDst)
{    return ChessLegal[sqDst - sqSrc + 256] == 2;}    //是否符合advisor的规则

inline BOOL BISHOP_SPAN(int sqSrc, int sqDst)
{    return ChessLegal[sqDst - sqSrc + 256] == 3;}    //是否符合BIshop的规则

inline int BISHOP_PIN(int sqSrc, int sqDst)        //象眼position
{    return (sqSrc + sqDst) >>1;}

inline int KNIGHT_PIN(int sqSrc, int sqDst)        //马腿position
{    return sqSrc + ChessKnight[sqDst - sqSrc +256];}

inline BOOL HOME_HALF(int sq, int sd)        //judge pass half    这里简略为1个函数
{    return (sq & 0x80) != (sd<<7);}

inline BOOL SAME_HALF(int sqSrc, int sqDst)        //judge same side
{    return ((sqSrc ^ sqDst) & 0x80) == 0;}

inline BOOL SAME_ROW(int sqSrc, int sqDst)        //judge same row
{    return ((sqSrc ^ sqDst) & 0xf0) == 0;}

inline BOOL SAME_COL(int sqSrc, int sqDst)
{    return ((sqSrc ^ sqDst) & 0x0f) == 0;}

inline int SIDE_TAG(int sd)    //获取自身红黑标志
{    return 8 +(sd<<3);}

inline int OPP_SIDE_TAG(int sd)    //获取目标棋子的红黑标志
{    return 16 -(sd<<3);}

inline int SRC(int mv)    //获得走法的起点
{    return mv & 255;}

inline int DST(int mv)    //获得走法终点
{    return mv>>8;}

inline int MOVE( int sqSrc, int sqDst)    //根据上面得到的起点终点获得走法
{    return sqSrc +sqDst * 256; }

inline int MIRROR_MOVE(int mv)        //走法水平镜像
{    return MOVE(MIRROR_SQUARE(SRC(mv)),MIRROR_SQUARE(DST(mv)));}

const BOOL GEN_CAPTURE = TRUE;        // "GenerateMoves"参数

// RC4密码流生成器
struct RC4Struct{
    BYTE s[256];
    int x,y;
    
    void InitZero(void);
    BYTE NextByte(void){    // 生成密码流的下一个字节
        BYTE uc;
        x = (x+1) & 255;
        y = (y +s[x]) & 255;
        uc = s[x];
        s[x] = s[y];
        s[y] = uc;
        return s[(s[x]+s[y]) & 255];
    }
    
    DWORD NextLong(void){    // 生成密码流的下四个字节
        BYTE uc0,uc1,uc2,uc3;
        uc0 = NextByte();
        uc1 = NextByte();
        uc2 = NextByte();
        uc3 = NextByte();
        return uc0 + (uc1<<8) + (uc2<<16) + (uc3 << 24);
    }
};

void RC4Struct::InitZero(void)    // 用空密钥初始化密码流生成器
{    int i,j;
    BYTE uc;
    x = y = j = 0;
    for(i = 0; i < 256; i++)
    {    s[i] = i;}
    for(i = 0; i < 256; i++)
    {
        j = (j+s[i]) & 255;
        uc = s[i];
        s[i] = s[j];
        s[j] = uc;
    }
}

//Zobrist
struct    ZobristStruct{
    DWORD dwKey, dwLock0, dwLock1;//Zobrist值总共96位，放在 dwKey、dwLock0 和 dwLock1 中，其中 dwKey 在检查重复局面时用，也作为置换表的键值，dwLock0 和 dwLock1 用作置换表的校验值，另外，dwLock1 还是查找开局库的依据
    
    void InitZero(void){
        dwKey = dwLock0 = dwLock1 = 0;}
    void InitRC4(RC4Struct & rc4){
        dwKey = rc4.NextLong();
        dwLock0 = rc4.NextLong();
        dwLock1 = rc4.NextLong();
    }
    void Xor(const ZobristStruct &zobr){
        dwKey ^= zobr.dwKey;
        dwLock0 ^= zobr.dwLock0;
        dwLock1 ^= zobr.dwLock1;
    }
    void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2){
        dwKey ^= zobr1.dwKey ^zobr2.dwKey;
        dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
        dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
    }
};

static struct {
    ZobristStruct Player;
    ZobristStruct Table[14][256];
}Zobrist;

static void InitZobrist(void){    //Init Table
    int i,j;
    RC4Struct rc4;
    
    rc4.InitZero();
    Zobrist.Player.InitRC4(rc4);
    for(i = 0; i< 14; i++){
        for(j = 0; j < 256; j++)
        {    Zobrist.Table[i][j].InitRC4(rc4);}
    }
}

struct MoveStruct{    //历史走法信息（4bytes)
    WORD wmv;
    BYTE ucpcCaptured, ucbCheck;
    DWORD dwKey;
    
    void Set(int mv, int pcCaptured, BOOL bCheck, DWORD dwKey_){
        wmv = mv;
        ucpcCaptured = pcCaptured;
        ucbCheck = bCheck;
        dwKey = dwKey_;
    }
};


//局部信息
struct PositionStruct{
    int sdPlayer;    //下一步谁走 0=红 1 = 黑
    BYTE ucpcSquares[256];    //棋子
    int vlWhite,vlBlack;    //红黑子力价值
    int nDistance,nMoveNum;    //距离根节点的步数,历史走法数
    MoveStruct mvsList[MAX_MOVES];    //历史走法列表
    ZobristStruct zobr;
    
    void ClearBoard(void){    //清空棋盘
        sdPlayer = vlWhite = vlBlack = 0;
        memset(ucpcSquares,0,256);
        zobr.InitZero();
    }
    
    void SetIrrev(void){    // 清空(初始化)历史走法信息
        mvsList[0].Set(0,0,Checked(),zobr.dwKey);
        nMoveNum = 1;
    }
    void Startup(void){        // 初始化棋盘
        int sq,pc;
        ClearBoard();
        for(sq = 0; sq < 256; sq++)
        {
            pc = cucpcStartup[sq];
            if(pc!= 0) AddPiece(sq,pc);
        }
        SetIrrev();
    }
    void ChangeSide(void){        // 交换走子方
        sdPlayer = 1 - sdPlayer;
        zobr.Xor(Zobrist.Player);
    }
    
    void AddPiece(int sq,int pc){    //放棋子于目标格点
        ucpcSquares[sq] = pc;
        // 红方加分，黑方减分
        if(pc<16){
            vlWhite += cucvlPiecePos[pc-8][sq];
            zobr.Xor(Zobrist.Table[pc-8][sq]);
        }
        else{
            vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
            zobr.Xor(Zobrist.Table[pc-9][sq]);
        }
    }
    
    void DelPiece(int sq, int pc)    {    //从棋盘上拿走一枚棋子
        ucpcSquares[sq] = 0;
        if(pc < 16){
            vlWhite -= cucvlPiecePos[pc - 8][sq];
            zobr.Xor(Zobrist.Table[pc-8][sq]);
        }
        else {
            vlBlack -= cucvlPiecePos[pc-16][SQUARE_FLIP(sq)];
            zobr.Xor(Zobrist.Table[pc-9][sq]);
             }
    }
                                            
    int Evaluate(void) const{    //评估局面
        return (sdPlayer == 0? vlWhite - vlBlack:vlBlack - vlWhite) + ADVANCED_VALUE;
    }
                                            
    BOOL InCheck(void) const{        //判断是否被将军
        return mvsList[nMoveNum - 1].ucbCheck;
    }
                                            
    BOOL Captured(void) const{    //判断是否吃子
        return mvsList[nMoveNum - 1].ucpcCaptured != 0;
    }
                                            
    int MovePiece(int mv){        //走一步棋后的棋子
        int sqSrc, sqDst, pc, pcCaptured;
        sqSrc = SRC(mv);    //起点位置
        sqDst = DST(mv);    //终点位置
        pcCaptured = ucpcSquares[sqDst];    //被吃掉的棋子的标志
        if(pcCaptured) DelPiece(sqDst,pcCaptured);        //当该位置上有敌方棋子时就要删去该棋子
        pc = ucpcSquares[sqSrc];    //获取起点处的棋子并从起点删去将其加到终点上
        DelPiece(sqSrc, pc);
        AddPiece(sqDst, pc);
        return pcCaptured;
        }
                                            
    void UndoMovePiece(int mv, int pcCaptured){        //撤销前一步的棋子
            int sqSrc, sqDst, pc;
            sqSrc = SRC(mv);
            sqDst = DST(mv);
            pc = ucpcSquares[sqDst];
            DelPiece(sqDst,pc);
            AddPiece(sqSrc,pc);
            if(pcCaptured) AddPiece(sqDst,pcCaptured);
            }
                                            
    BOOL MakeMove(int mv){        //走一步
            int pcCaptured;
            DWORD dwKey;
                                            
            dwKey = zobr.dwKey;
            pcCaptured = MovePiece(mv);
            if(Checked()){        //判断自己走这一步会不会被将军
                UndoMovePiece(mv,pcCaptured);
                return FALSE;
                }
            ChangeSide();
            mvsList[nMoveNum].Set(mv,pcCaptured, Checked(),dwKey);
            nMoveNum++;
            nDistance++;
            return TRUE;
            }
                                            
    void UndoMakeMove(void){    //撤销前一步
            nDistance--;
            nMoveNum--;
            ChangeSide();
            UndoMovePiece(mvsList[nMoveNum].wmv, mvsList[nMoveNum].ucpcCaptured);
            }
                                            
    void NullMove(void){    //当走的是空步
            DWORD dwKey;
            dwKey = zobr.dwKey;
            ChangeSide();
            mvsList[nMoveNum].Set(0,0,FALSE,dwKey);
            nMoveNum++;
            nDistance++;
        }
                                            
    void UndoNullMove(void){    //撤销空步操作
            nDistance--;
            nMoveNum--;
            ChangeSide();
        }
                                            
 
    int GenerateMoves(int *mvs, BOOL bCapture = FALSE) const {    //生成所有走法，当'bCapture' = true时则只生成吃子走法
            int i,j,nGenMoves,nDelta,sqSrc,sqDst;
            int pcSelfSide,pcOppSide,pcSrc,pcDst;
                                            
            nGenMoves = 0;
            pcSelfSide = SIDE_TAG(sdPlayer);    //  标记当前棋子的势力（自己的还是敌方的）
            pcOppSide = OPP_SIDE_TAG(sdPlayer);
            for(sqSrc = 0; sqSrc < 256; sqSrc++)
                {
                    pcSrc = ucpcSquares[sqSrc];
                    if((pcSrc&pcSelfSide) == 0) continue;
                    
                    
                    switch(pcSrc-pcSelfSide){
                        case PIECE_KING:
                            for(i = 0; i < 4; i++)
                            {
                                sqDst = sqSrc + ChessKing[i];
                                if(!IN_FORT(sqDst)) continue;
                                pcDst = ucpcSquares[sqDst];
                                if(bCapture ? ( pcDst & pcOppSide)!= 0 : (pcDst & pcSelfSide) == 0) {
                                    mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                    nGenMoves++;
                                }
                            }
                            break;
                        case PIECE_ADVISOR:
                            for(i = 0; i < 4; i++)
                            {
                                sqDst = sqSrc + ChessAdvisor[i];
                                if(!IN_FORT(sqDst)) continue;
                                pcDst = ucpcSquares[sqDst];
                                if(bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0){
                                    mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                    nGenMoves++;
                                }
                            }
                            break;
                        case PIECE_BISHOP:
                            for(i = 0; i < 4; i++)
                            {
                                sqDst = sqSrc + ChessAdvisor[i];
                                if(!(IN_BOARD(sqDst) && HOME_HALF(sqDst, sdPlayer) && ucpcSquares[sqDst] == 0))continue;
                                sqDst += ChessAdvisor[i];
                                pcDst = ucpcSquares[sqDst];
                                if(bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
                                    mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                    nGenMoves++;
                                }
                            }
                            break;
                        case PIECE_KNIGHT:
                            for(i = 0; i < 4; i++){
                                sqDst = sqSrc + ChessKing[i];
                                if(ucpcSquares[sqDst]) continue;
                                for(j = 0; j < 2; j++)
                                {
                                    sqDst = sqSrc + ChessKnightDelta[i][j];
                                    if(!IN_BOARD(sqDst)) continue;
                                    pcDst = ucpcSquares[sqDst];
                                    if(bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0){
                                        mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                        nGenMoves++;
                                    }
                                }
                            }
                            break;
                        case PIECE_CANNON:
                            for(i = 0 ; i < 4; i++)
                            {
                                nDelta = ChessKing[i];
                                sqDst = sqSrc + nDelta;
                                while(IN_BOARD(sqDst))
                                {
                                    pcDst = ucpcSquares[sqDst];
                                    if(pcDst == 0){
                                        if(!bCapture){
                                            mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                            nGenMoves++;
                                        }
                                    }
                                    else  break;    //表示这条路上有一个可用炮台，break跳至后面的循环中进行射炮操作
                                    sqDst += nDelta;
                                }
                                sqDst += nDelta;
                                while(IN_BOARD(sqDst))
                                {
                                    pcDst = ucpcSquares[sqDst];
                                    if(pcDst)
                                    {
                                        if((pcDst & pcOppSide))
                                        {
                                            mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                            nGenMoves++;
                                        }
                                        break;
                                    }
                                    sqDst += nDelta;
                                }
                            }
                            break;
                        case PIECE_ROOK:
                            for(i = 0; i < 4; i++)
                            {
                                nDelta = ChessKing[i];
                                sqDst = sqSrc + nDelta;
                                while(IN_BOARD(sqDst))
                                {
                                    pcDst = ucpcSquares[sqDst];
                                    if(!pcDst)
                                    {
                                        if(!bCapture)
                                        {
                                            mvs[nGenMoves] = MOVE(sqSrc, sqDst);
                                            nGenMoves++;
                                        }
                                    }
                                    else{
                                        if(pcDst & pcOppSide){
                                            mvs[nGenMoves] = MOVE(sqSrc, sqDst);
                                            nGenMoves++;
                                        }
                                        break;
                                    }
                                    sqDst += nDelta;
            
                                }
                            }
                            break;
                        case PIECE_PAWN:
                            sqDst  = SQUARE_FORWARD(sqSrc, sdPlayer);
                            if(IN_BOARD(sqDst))
                            {
                                pcDst = ucpcSquares[sqDst];
                                if(bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0)
                                {
                                    mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                    nGenMoves++;
                                }
                            }
                            if(!HOME_HALF(sqSrc,sdPlayer))
                            {
                                for(nDelta = -1; nDelta <= 1; nDelta+=2)
                                {
                                    sqDst = sqSrc + nDelta;
                                    if(IN_BOARD(sqDst))
                                    {
                                        pcDst = ucpcSquares[sqDst];
                                        if(bCapture ? (pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0)
                                        {
                                            mvs[nGenMoves] = MOVE(sqSrc,sqDst);
                                            nGenMoves++;
                                        }
                                    }
                                }
                            }
                            break;
                    }
                }
                return nGenMoves;
        }
        
        BOOL LegalMove(int mv) const{   //判断走法是否合理
                int sqSrc,sqDst,sqPin;
                int pcSelfSide, pcSrc,pcDst,nDelta;
                
                sqSrc = SRC(mv);
                pcSrc = ucpcSquares[sqSrc];
                pcSelfSide = SIDE_TAG(sdPlayer);
                if(!(pcSrc & pcSelfSide)) return FALSE;	//judge whther it's my chess 
                
                sqDst = DST(mv);
                pcDst = ucpcSquares[sqDst];
                if(pcDst & pcSelfSide) return FALSE;//judge whther target is my chess 
                
                switch(pcSrc - pcSelfSide){
                    case PIECE_KING:
                        return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
                    case PIECE_ADVISOR:
                        return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
                    case PIECE_BISHOP:
                        return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) && ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
                    case PIECE_KNIGHT:
                        sqPin = KNIGHT_PIN(sqSrc, sqDst);
                        return sqPin != sqSrc && ucpcSquares[sqPin] == 0;
                    case PIECE_ROOK:
                    case PIECE_CANNON:
                        if(SAME_ROW(sqSrc, sqDst)) nDelta = (sqDst < sqSrc ? -1 : 1);
                        else if(SAME_COL(sqSrc, sqDst)) nDelta = (sqDst < sqSrc ? -16 : 16);
                        else return FALSE;
                        sqPin = sqSrc + nDelta;
                        while(sqPin != sqDst && !ucpcSquares[sqPin])    sqPin += nDelta;
                        if(sqPin == sqDst)  return pcDst == 0 || pcSrc - pcSelfSide == PIECE_ROOK; //当遇到棋子时就会先判断是否符合车吃子的规则
                        else if(pcDst  && pcSrc - pcSelfSide == PIECE_CANNON)       //再判断该棋子能否成为炮的炮台，并在炮台后面继续递增找到目的地上是否有可吃的棋子
                        {
                            sqPin += nDelta;
                            while(sqPin != sqDst && ucpcSquares[sqPin] == 0) sqPin += nDelta;
                            return sqPin == sqDst;
                        }
                        else return FALSE;
                    case PIECE_PAWN:
                        if(!HOME_HALF(sqDst, sdPlayer) && (sqDst  == sqSrc - 1 || sqDst == sqSrc + 1 ))   return TRUE;    //过河后增加的左右移动情况判断
                        return sqDst == SQUARE_FORWARD(sqSrc, sdPlayer);
                    default:
                        return FALSE;
                }
            }
        BOOL Checked(void) const{        //判断是否被将军
                    int i,j,sqSrc,sqDst;
                    int pcSelfSide,pcOppSide,pcDst,nDelta;
                    pcSelfSide = SIDE_TAG(sdPlayer);
                    pcOppSide = OPP_SIDE_TAG(sdPlayer);
                    for(sqSrc = 0; sqSrc < 256; sqSrc ++)
                    {
                        if(ucpcSquares[sqSrc] != pcSelfSide + PIECE_KING)   continue;   //找将军,sqSrc为将军坐标
                    
                        if(ucpcSquares[SQUARE_FORWARD(sqSrc, sdPlayer)] == pcOppSide + PIECE_PAWN) return TRUE;
                        for(nDelta = -1; nDelta <= 1; nDelta += 2)
                        {
                            if(ucpcSquares[sqSrc + nDelta] == pcOppSide + PIECE_PAWN) return TRUE;  //ucpcSquares[sqSrc + nDelta]在将军的左右的位置检测其位置上的棋子是不是敌方的小兵
                        }
                        
                        for(i = 0; i < 4; i++)
                        {
                            if(ucpcSquares[sqSrc + ChessAdvisor[i]])    continue;   //判断哪些地方有马腿，有马腿的地方就不用去检测那边有没有马
                        
                            for(j = 0; j < 2; j++)  //一个时候只会有一个马将军
                            {
                                pcDst = ucpcSquares[sqSrc + ChessKnightDelta[i][j]];
                                if(pcDst == pcOppSide + PIECE_KNIGHT) return TRUE;
                            }
                        }
                        
                        for(i = 0; i < 4; i++)  //检测会不会被敌方的king cannon rook将军
                        {
                            nDelta = ChessKing[i];
                            sqDst = sqSrc + nDelta;
                            while(IN_BOARD(sqDst))
                            {
                                pcDst = ucpcSquares[sqDst];
                                if(pcDst)
                                {
                                    if(pcDst == pcOppSide + PIECE_ROOK || pcDst == pcOppSide + PIECE_KING) return TRUE;
                                    break;
                                }
                                sqDst += nDelta;
                            }
                        
                        sqDst += nDelta;    //遇到障碍物且其不是敌方的车与将，则从这个障碍物后找有没有敌方的炮存在
                        while(IN_BOARD(sqDst))
                        {
                            int pcDst = ucpcSquares[sqDst];
                            if(pcDst)
                            {
                                if(pcDst == pcOppSide + PIECE_CANNON) return TRUE;
                            }
                            break;
                            sqDst += nDelta;
                        }
                        }
                        return FALSE;
                    }
                    return FALSE;
                }
                
        BOOL IsMate(void){               //判断是否被杀
            int i,nGenMoveNum,pcCaptured;
            int mvs[MAX_GEN_MOVES];
            
            nGenMoveNum = GenerateMoves(mvs);
            for(i = 0;i < nGenMoveNum ; i++)
            {
                pcCaptured = MovePiece(mvs[i]);
                if(!Checked())
                {
                    UndoMovePiece(mvs[i], pcCaptured);
                    return FALSE;
                }
                else UndoMovePiece(mvs[i], pcCaptured);
            }
            return TRUE;
        
        }
                
        int DrawValue(void) const        //和棋分值
        {	return (nDistance & 1) == 0 ? -DRAW_VALUE :DRAW_VALUE;}

		int RepStatus(int nRecur = 1) const{    //检测重复局面
			BOOL bSelfSide;
			BOOL bPerpCheck;
			BOOL bOppPerpCheck;
			const MoveStruct *lpmvs;
            
			bSelfSide = FALSE;
			bPerpCheck = bOppPerpCheck = TRUE;
			lpmvs = mvsList + nMoveNum - 1;
			while(lpmvs->wmv && lpmvs->ucpcCaptured == 0)
			{
				if(bSelfSide)
				{
					bPerpCheck = bPerpCheck && lpmvs->ucbCheck;
					if(lpmvs->dwKey == zobr.dwKey)
					{
						nRecur--;
						if(!nRecur) return 1+(bPerpCheck ? 2:0) + (bOppPerpCheck ? 4 : 0);
					}
				}
				else bOppPerpCheck = bOppPerpCheck && lpmvs->ucbCheck;
				bSelfSide = !bSelfSide;
				lpmvs--;
			}
			return 0;
		}
        int RepValue(int nRepStatus) const        //重复局面分值
        {    int vlReturn;
             vlReturn = ((nRepStatus & 2) == 0 ? 0 : nDistance - BAN_VALUE) + ((nRepStatus &4) == 0 ? 0 : BAN_VALUE - nDistance);
             return vlReturn == 0 ? DrawValue() : vlReturn;
        }
                                                
        BOOL NullOkay(void) const{                //判断是否允许空步裁剪
             return (sdPlayer == 0? vlWhite : vlBlack) > NULL_MARGIN;}
                                                
        void Mirror(PositionStruct &posMirror) const{    //对局面镜像
            int sq,pc;
            posMirror.ClearBoard();
            for(sq = 0; sq < 256; sq++)
            {
                pc = ucpcSquares[sq];
                if(pc)  posMirror.AddPiece(MIRROR_SQUARE(sq), pc);
            }
            if(sdPlayer == 1) posMirror.ChangeSide();
            posMirror.SetIrrev();
        }
};


static PositionStruct pos;    //局面实例
static struct {
    HINSTANCE hInst;        //应用程序句柄实例
    HWND hWnd;              //主窗口句柄
    HDC hdc,hdcTmp;         //设备句柄，只有在“ClickSquare"过程中有效
    HBITMAP bmpBoard, bmpSelected, bmpPieces[24];       //资源图片句柄
    int sqSelected, mvLast;             //选中的格子与上一步棋
    BOOL bFlipped, bGameOver;           //判断是否反转棋盘与结束游戏
}Xqwl;

struct HashItem{    //置换表项结构
    BYTE ucDepth, ucFlag;
    short svl;
    WORD wmv,wReserved;
    DWORD dwLock0,dwLock1;
};

struct BookItem{       //开局库结构
    DWORD dwLock;
    WORD wmv,wvl;
};
                
static struct {         //与搜索有关的全局变量
    int mvResult;       //电脑走的棋
    int nHistoryTable[65536];       //历史表
    int mvKillers[MAX_DEPTH][2];      //杀棋表
    HashItem HashTable[HASH_SIZE];      //置换表
    int nBookSize;                      //开局库大小
    BookItem BookTable[BOOK_SIZE];      //开局库
}Search;
                
static void LoadBook(void){             //装入开局库
    HRSRC hrsrc;            //WINAPI中的句柄类型
    hrsrc = FindResource(Xqwl.hInst,"BOOK_DATA",RT_RCDATA);             //WINAPI中的寻找资源函数，返回被找到资源的句柄
    Search.nBookSize = SizeofResource(Xqwl.hInst,hrsrc)/sizeof(BookItem);
    if(Search.nBookSize > BOOK_SIZE)    Search.nBookSize = BOOK_SIZE;
    memcpy(Search.BookTable, LockResource(LoadResource(Xqwl.hInst,hrsrc)), Search.nBookSize * sizeof(BookItem));    //将搜索的开局库复制
}

static int CompareBook(const void * lpbk1, const void *lpbk2)       //比较局面是否不同，一样时返回值为0
{
    DWORD dw1,dw2;
    dw1 = ((BookItem * ) lpbk1)->dwLock;
    dw2 = ((BookItem * ) lpbk2)->dwLock;
    return dw1 > dw2 ? 1:(dw1<dw2 ? -1 : 0);
}

static int SearchBook(void){    //搜索开局库
    int i,vl,nBookMoves, mv;
    int mvs[MAX_GEN_MOVES], vls[MAX_GEN_MOVES];
    BOOL bMirror;
    BookItem bkToSearch, *lpbk;
    PositionStruct posMirror;
    
    if(!Search.nBookSize)   return 0;   //当开局库大小=0时则立即返回
    
    //搜索当前局面
    bMirror = FALSE;
    bkToSearch.dwLock = pos.zobr.dwLock1;
    lpbk = (BookItem *) bsearch(&bkToSearch, Search.BookTable, Search.nBookSize, sizeof(BookItem), CompareBook);
    
    if(!lpbk)   //没有找到当前局面则寻找当前局面的镜像局面
    {
        bMirror = TRUE;
        pos.Mirror(posMirror);
        bkToSearch.dwLock = posMirror.zobr.dwLock1;
        lpbk = (BookItem *) bsearch(&bkToSearch, Search.BookTable, Search.nBookSize, sizeof(BookItem), CompareBook);
    }
    
    if(!lpbk)   return 0;       //原局面与其镜像局面均没有找到时就马上返回0
    
    //若找到当前局面就向前找第一个开局库项
    while(lpbk >= Search.BookTable && lpbk->dwLock == bkToSearch.dwLock) lpbk--;
    lpbk++;
    //将走法与其对应分值写入mvs 与 vls数组中
    vl = nBookMoves = 0;
    while(lpbk < Search.BookTable + Search.nBookSize && lpbk->dwLock == bkToSearch.dwLock)
    {
        mv = (bMirror ? MIRROR_MOVE(lpbk->wmv) : lpbk->wmv);
        if(pos.LegalMove(mv)){
            mvs[nBookMoves] = mv;
            vls[nBookMoves] = lpbk->wvl;
            vl += vls[nBookMoves];
            nBookMoves++;	
            if(nBookMoves == MAX_GEN_MOVES) break;	//  及时停止以防“BOOK.DAT"中有异常数据
        }
        lpbk++;
    }
    
    if(!vl) return 0;   //  及时停止以防“BOOK.DAT"中有异常数据
    
    //根据权重随机选择一个走法
    vl = rand()%vl;

    for(i = 0; i < nBookMoves; i++)
    {
        vl -= vls[i];
        if(vl < 0) break;
    }
    return mvs[i];
}
            
//提取置换表项
static int ProbeHash(int vlAlpha, int vlBeta, int nDepth, int &mv)
{
    BOOL bMate;     //  杀棋标志
    HashItem hsh;
    
    hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
    if(hsh.dwLock0 != pos.zobr.dwLock0 || pos.zobr.dwLock1)
    {
        mv = 0;
        return -MAX_VALUE;
    }
    mv = hsh.wmv;
    bMate = FALSE;
	if(hsh.svl > WIN_VALUE) 
	{
		if(hsh.svl < BAN_VALUE) return -MAX_VALUE;
		hsh.svl -= pos.nDistance;
		bMate = TRUE;
	}
	else if(hsh.svl < -WIN_VALUE)
	{
		if(hsh.svl > -BAN_VALUE) return -MAX_VALUE;
		hsh.svl += pos.nDistance;
		bMate = TRUE;
	}
	if(hsh.ucDepth >= nDepth || bMate)
	{
		if(hsh.ucFlag == HASH_BETA) return (hsh.svl >= vlBeta ? hsh.svl : -MAX_VALUE);
		else if(hsh.ucFlag == HASH_ALPHA) return (hsh.svl <= vlAlpha ? hsh.svl : -MAX_VALUE);
		return hsh.svl;
	}
    return -MAX_VALUE;
};


static void RecordHash(int nFlag, int vl, int nDepth, int mv)	//保存置换表项
{
	HashItem hsh;
	hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
	if(hsh.ucDepth > nDepth) return;
	hsh.ucFlag = nFlag;
	hsh.ucDepth = nDepth;
	if(vl > WIN_VALUE) 
	{
		if(!mv && vl <= BAN_VALUE) return;  //当判断条件为真时，则表示可能会出现反复搜索或超出搜索界限且无最佳步法，则退出
		hsh.svl = vl + pos.nDistance;
	}
	else if(vl < -WIN_VALUE)
	{
		if(!mv && vl >= -BAN_VALUE) return;
		hsh.svl = vl - pos.nDistance;
	}
	else hsh.svl = vl;
	hsh.wmv = mv;
	hsh.dwLock0 = pos.zobr.dwLock0;
	hsh.dwLock1 = pos.zobr.dwLock1;
	Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)] = hsh;
};

static BYTE cucMvvLva[24] = {		//子力价值
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 1, 1, 3, 4, 3, 2, 0,
	5, 1, 1, 3, 4, 3, 2, 0
};

inline int MvvLva(int mv)	//求MVV/LVA值
{ return (cucMvvLva[pos.ucpcSquares[DST(mv)]] << 3) - cucMvvLva[pos.ucpcSquares[SRC(mv)]];}

static int CompareMvvLva(const void *lpmv1, const void *lpmv2){	//按MVV/LVA值排序的比较函数
	return MvvLva(*(int *) lpmv2) - MvvLva(*(int *) lpmv1);
}

static int CompareHistory(const void *lpmv1, const void * lpmv2){	//按历史走法表排序的比较函数
	return Search.nHistoryTable[*(int *) lpmv2] - Search.nHistoryTable[*(int *) lpmv1];
}


//对走法进行排序
const int PHASE_HASH = 0;
const int PHASE_KILLER_1 = 1;
const int PHASE_KILLER_2 = 2;
const int PHASE_GEN_MOVES = 3;
const int PHASE_REST = 4;

struct SortStruct {	//走法排序结构函数及参数
	int mvHash, mvKiller1, mvKiller2;	//一种置换表走法及两种杀手走法
	int nPhase,nIndex, nGenMoves;		//当前阶段，当前走法，走法迭代总数
	int mvs[MAX_GEN_MOVES];				//所有走法（最多128种）

	void Init(int mvHash_)	//初始化，设定各信号参数的初始值
	{
		mvHash = mvHash_;
		mvKiller1 = Search.mvKillers[pos.nDistance][0];
		mvKiller2 = Search.mvKillers[pos.nDistance][1];
		nPhase = PHASE_HASH;
	}
	int Next(void)		//迭代生成下一个走法
	{
		int mv;
		switch(nPhase){		//走法启发的若干阶段，不设置break，从hash到杀棋再到迭代走法
		case PHASE_HASH:	//第一个是置换表走法启发
			nPhase = PHASE_KILLER_1;
			if(mvHash) return mvHash;

		case PHASE_KILLER_1:	//第一个杀棋走法启发
			nPhase = PHASE_KILLER_2;
			if(mvKiller1 != mvHash && mvKiller1 && pos.LegalMove(mvKiller1)) return mvKiller1;

		case PHASE_KILLER_2:	//第二个杀棋走法启发
			nPhase = PHASE_GEN_MOVES;
			if(mvKiller2 != mvHash && mvKiller2 && pos.LegalMove(mvKiller2)) return mvKiller2;

		case PHASE_GEN_MOVES:	//生成所有走法并进行排序
			nPhase = PHASE_REST;
			nGenMoves = pos.GenerateMoves(mvs);
			qsort(mvs,nGenMoves,sizeof(int),CompareHistory);
			nIndex = 0;

		case PHASE_REST:	//针对剩余未选用的走法放入历史表中进行启发
			while(nIndex < nGenMoves) 
			{
				mv = mvs[nIndex];
				nIndex++;	//追踪当前走法数
				if(mv != mvHash && mv != mvKiller1 && mv != mvKiller2) return mv;	//当mv不是以上三种时则停止，表明当前走法为无用走法。
			}
		default: //走法不符合以上条件，则表示没有新走法了。
			return 0;

		}
	}
};

inline void SetBestMove(int mv, int nDepth)	//对最佳走法的处理
{
	int * lpmvKillers;
	Search.nHistoryTable[mv] += nDepth * nDepth;	//在历史表该走法对应的项上加上该走法对应的深度		
	lpmvKillers = Search.mvKillers[pos.nDistance];
	if(lpmvKillers[0] != mv)
	{
		lpmvKillers[0] = mv;
		lpmvKillers[1] = lpmvKillers[0];
	}
}

static int SearchQuiesc(int vlAlpha, int vlBeta)	//静态搜索
{
	int i, nGenMoves;
	int vl, vlBest;
	int mvs[MAX_GEN_MOVES];	

	vl = pos.RepStatus();	//先检测重复局面
	if(vl) return pos.RepStatus(); //doubts 需要调试其中有无参数
	
	if(pos.nDistance == MAX_DEPTH) return pos.Evaluate();	//当搜索深度达到最大深度时，则返回评估局面函数

	vlBest = -MAX_VALUE;	//类似于求最大值时先将目标值设为最小。

	if(pos.InCheck())	//先判断有没有被将军，若有则生成所有走法并进行排序。
	{
		nGenMoves = pos.GenerateMoves(mvs);
		qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
	}
	else {	//当没有被将军时，则表示我们可以先评估局面再选取最佳走法
		vl = pos.Evaluate();
		if(vl > vlBest) 
		{
			vlBest = vl;
			if(vl >= vlBeta) return vl;
			if(vl > vlAlpha) vlAlpha = vl;
		}

	//当评估值小于beta界限时，则表示我们可以生成吃子走法。
	nGenMoves = pos.GenerateMoves(mvs, GEN_CAPTURE);
	qsort(mvs, nGenMoves, sizeof(int),CompareMvvLva);
	}

	for( i = 0; i < nGenMoves; i++)	//迭代生成前面在界限内的走法
	{
		if(pos.MakeMove(mvs[i]))
		{
			vl = -SearchQuiesc(-vlBeta,-vlAlpha);	//在下面的if中不断缩小A-B界限并进行递归从而提高准确度
			pos.UndoMakeMove();

			if(vl > vlBest)
			{
				vlBest = vl;	//获得最大值
				if(vl >= vlBeta) return vl;		//表示找到Beta走法，这个时候停止即可。
				if(vl >= vlAlpha) vlAlpha = vl;	//Alpha走法，缩小界限并继续寻找
			}
		}
	}
	//迭代生成所有走法后返回最佳值
	return vlBest == -MAX_VALUE ? pos.nDistance - MAX_VALUE : vlBest;		//doubts

}

const BOOL NO_NULL = TRUE;	//SearchFull函数的参数

static int SearchFull(int vlAlpha, int vlBeta, int nDepth, BOOL bNoNull = FALSE)	//当超出边界时的搜索过程
{
	int nHashFlag,vl, vlBest;
	int mv,mvBest, mvHash, nNewDepth;
	SortStruct Sort;

	if(nDepth <= 0) return SearchQuiesc(vlAlpha,vlBeta);	//当深度为0时则开始调用静态搜索获取走法。当空步裁剪太多时则深度会小于0

	vl = pos.RepStatus();	//先检测重复局面
	if(vl) return pos.RepValue(vl);

	if(pos.nDistance == MAX_DEPTH) return pos.Evaluate();	//当深度到最大值时则直接返回评估函数

	vl = ProbeHash(vlAlpha,vlBeta, nDepth, mvHash);	//置换表裁剪
	if(vl > -MAX_VALUE) return vl;	//得到置换表走法

	if(!bNoNull && !pos.InCheck() && pos.NullOkay())	//恐怖裁剪
	{
		pos.NullMove();
		vl = -SearchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
		pos.UndoMakeMove();
		if(vl >= vlBeta) return vl;	//beta是获取该走法的下界
	}

	nHashFlag = HASH_ALPHA;		//初始化
	vlBest = -MAX_VALUE;		//同上一个函数中
	mvBest = 0;

	Sort.Init(mvHash);			//初始化排序结构

	while((mv = Sort.Next()))	//迭代每一个走法
	{
		if(pos.MakeMove(mv))
		{
			nNewDepth = pos.InCheck()? nDepth : nDepth - 1;	//若将军则延伸长度
			if(vlBest == -MAX_VALUE) vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
			else
			{
				vl = -SearchFull(-vlAlpha - 1,-vlAlpha,nNewDepth);
				if(vl > vlAlpha && vl < vlBeta) vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
			}
			pos.UndoMakeMove();

			//进行A-B边界判断与截断
			if(vl > vlBest) 
			{
				vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
				if (vl >= vlBeta) { // 找到一个Beta走法
					nHashFlag = HASH_BETA;
					mvBest = mv;      // Beta走法要保存到历史表里面
					break;            // Beta截断
				}
				if (vl > vlAlpha) { // 找到一个PV走法
					nHashFlag = HASH_PV;
					mvBest = mv;      // PV走法要保存到历史表
					vlAlpha = vl;     // 缩小Alpha-Beta边界
				}
			}
		}
	}

	//搜索完所有走法后将最佳走法保存到历史表并返回最佳值
	if(vlBest == -MAX_VALUE) return pos.nDistance - MAX_VALUE;	//当其为杀棋时则返回步数来评价该走法的好坏
	RecordHash(nHashFlag, vlBest, nDepth, mvBest);
	if(mvBest) SetBestMove(mvBest,nDepth);				//当其不是Alpha走法时则将最佳走法保存到历史表里
	return vlBest;
}

//对根节点的A-B搜索
static int SearchRoot(int nDepth)
{
	int vl,vlBest,mv,nNewDepth;
	SortStruct Sort;

	vlBest = -MAX_VALUE;
	Sort.Init(Search.mvResult);
	while((mv = Sort.Next()))
	{
		if(pos.MakeMove(mv))
		{
			nNewDepth = pos.InCheck() ? nDepth : nDepth - 1;
			if(vlBest == -MAX_VALUE) vl = -SearchFull(-MAX_VALUE, MAX_VALUE, nNewDepth, NO_NULL);
			else 
			{
				vl = -SearchFull(-vlBest - 1, -vlBest, nNewDepth);
				if(vl > vlBest) vl = -SearchFull(-MAX_VALUE, -vlBest, nNewDepth, NO_NULL);
			}
			pos.UndoMakeMove();
			if(vl > vlBest)
			{
				vlBest = vl;
				Search.mvResult = mv;
				if(vlBest > - WIN_VALUE && vlBest < WIN_VALUE) vlBest += (rand() & RANDOM_MASK) - (rand() & RANDOM_MASK);
			}
		}
	}
	RecordHash(HASH_PV, vlBest, nDepth, Search.mvResult);
	SetBestMove(Search.mvResult, nDepth);
	return vlBest;
}

static void SearchMain(void)	//用于迭代加深搜索
{
	int i,t,vl,nGenMoves;
	int mvs[MAX_GEN_MOVES];

	//对三个表进行初始化
	memset(Search.nHistoryTable, 0 , 65536 * sizeof(int));
	memset(Search.mvKillers, 0 , MAX_DEPTH * 2 * sizeof(int));
	memset(Search.HashTable, 0, HASH_SIZE * sizeof(HashItem));
	t = clock();
	pos.nDistance = 0;  //初始化步数

	//搜索开局库来选择一个开局走法
	Search.mvResult = SearchBook();
	if(Search.mvResult)
	{
		pos.MakeMove(Search.mvResult);
		if(!pos.RepStatus(3))
		{
			pos.UndoMakeMove();
			return;
		}
		pos.UndoMakeMove();
	}

	//检查走法是否唯一，若唯一则直接返回，若不唯一则在后面迭代可能的走法
	vl = 0;
	nGenMoves = pos.GenerateMoves(mvs);
	for(i = 0; i < nGenMoves; i++)
	{
		if(pos.MakeMove(mvs[i]))
		{
			pos.UndoMakeMove();
			Search.mvResult = mvs[i];
			vl++;
		}
	}
	if(vl == 1) return ;

	//加深迭代可能的走法
	for(i = 1; i <= MAX_DEPTH; i++)
	{
		vl = SearchRoot(i);
		if(vl > WIN_VALUE || vl < -WIN_VALUE) break;	//当有任意一方有绝杀时则终止搜索
		if(clock() - t > CLOCKS_PER_SEC) break;
	}
}

// TransparentBlt 的替代函数，用来修正原函数在 Windows 98 下资源泄漏的问题	(CSDN上的一段代码,非原创)
static void TransparentBlt2(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent) {
  HDC hImageDC, hMaskDC;
  HBITMAP hOldImageBMP, hImageBMP, hOldMaskBMP, hMaskBMP;

  hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest);
  hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);
  hImageDC = CreateCompatibleDC(hdcDest);
  hMaskDC = CreateCompatibleDC(hdcDest);
  hOldImageBMP = (HBITMAP) SelectObject(hImageDC, hImageBMP);
  hOldMaskBMP = (HBITMAP) SelectObject(hMaskDC, hMaskBMP);

  if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc) {
    BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
        hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
  } else {
    StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
        hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
  }
  SetBkColor(hImageDC, crTransparent);
  BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);
  SetBkColor(hImageDC, RGB(0,0,0));
  SetTextColor(hImageDC, RGB(255,255,255));
  BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);
  SetBkColor(hdcDest, RGB(255,255,255));
  SetTextColor(hdcDest, RGB(0,0,0));
  BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
      hMaskDC, 0, 0, SRCAND);
  BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
      hImageDC, 0, 0, SRCPAINT);

  SelectObject(hImageDC, hOldImageBMP);
  DeleteDC(hImageDC);
  SelectObject(hMaskDC, hOldMaskBMP);
  DeleteDC(hMaskDC);
  DeleteObject(hImageBMP);
  DeleteObject(hMaskBMP);
}

// 绘制透明图片
inline void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp) {
  SelectObject(hdcTmp, bmp);
  TransparentBlt2(hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, hdcTmp, 0, 0, SQUARE_SIZE, SQUARE_SIZE, MASK_COLOUR);
}

// 绘制棋盘
static void DrawBoard(HDC hdc) {
  int x, y, xx, yy, sq, pc;
  HDC hdcTmp;

  // 画棋盘
  hdcTmp = CreateCompatibleDC(hdc);
  SelectObject(hdcTmp, Xqwl.bmpBoard);
  BitBlt(hdc, 0, 0, BOARD_WIDTH, BOARD_LENGTH, hdcTmp, 0, 0, SRCCOPY);
  // 画棋子
  for (x = RANGE_LEFT; x <= RANGE_RIGHT; x ++) {
    for (y = RANGE_TOP; y <= RANGE_BOTTOM; y ++) {
      if (Xqwl.bFlipped) 
	  {	xx = BOARD_EDGE + (RANGE_X_FLIP(x) - RANGE_LEFT) * SQUARE_SIZE;
        yy = BOARD_EDGE + (RANGE_Y_FLIP(y) - RANGE_TOP) * SQUARE_SIZE;
      } else {
        xx = BOARD_EDGE + (x - RANGE_LEFT) * SQUARE_SIZE;
        yy = BOARD_EDGE + (y - RANGE_TOP) * SQUARE_SIZE;
      }
      sq = COORD_XY(x, y);
      pc = pos.ucpcSquares[sq];
      if (pc)	DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);

      if (sq == Xqwl.sqSelected || sq == SRC(Xqwl.mvLast) || sq == DST(Xqwl.mvLast)) DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpSelected);
     
    }
  }
  DeleteDC(hdcTmp);
}

// 播放资源声音
inline void PlayResWav(int nResId) {
  PlaySound(MAKEINTRESOURCE(nResId), Xqwl.hInst, SND_ASYNC | SND_NOWAIT | SND_RESOURCE);
}

// 弹出不带声音的提示框
static void MessageBoxMute(LPCSTR lpszText) {
  MSGBOXPARAMS mbp;
  mbp.cbSize = sizeof(MSGBOXPARAMS);
  mbp.hwndOwner = Xqwl.hWnd;
  mbp.hInstance = NULL;
  mbp.lpszText = lpszText;
  mbp.lpszCaption = "中国花样输棋冠军";
  mbp.dwStyle = MB_USERICON;
  mbp.lpszIcon = MAKEINTRESOURCE(IDI_INFORMATION);
  mbp.dwContextHelpId = 0;
  mbp.lpfnMsgBoxCallback = NULL;
  mbp.dwLanguageId = 0;
  if (MessageBoxIndirect(&mbp) == 0) 
  {
    mbp.hInstance = Xqwl.hInst;
    mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
    MessageBoxIndirect(&mbp);
  }
}

// "DrawSquare"参数
const BOOL DRAW_SELECTED = TRUE;

// 画格子
static void DrawSquare(int sq, BOOL bSelected = FALSE) {
  int sqFlipped, xx, yy, pc;

  sqFlipped = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq;
  xx = BOARD_EDGE + (RANGE_X(sqFlipped) - RANGE_LEFT) * SQUARE_SIZE;
  yy = BOARD_EDGE + (RANGE_Y(sqFlipped) - RANGE_TOP) * SQUARE_SIZE;
  SelectObject(Xqwl.hdcTmp, Xqwl.bmpBoard);
  BitBlt(Xqwl.hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, Xqwl.hdcTmp, xx, yy, SRCCOPY);
  pc = pos.ucpcSquares[sq];
  if (pc != 0) {
    DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);
  }
  if (bSelected) {
    DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpSelected);
  }
}


static void ResponseMove(void)		//电脑回应
{	
	//电脑走一步棋
	int vlRep;
	SetCursor((HCURSOR) LoadImage(NULL, IDC_WAIT, IMAGE_CURSOR,0,0,LR_DEFAULTSIZE|LR_SHARED));
	SearchMain();
	SetCursor((HCURSOR) LoadImage(NULL,IDC_WAIT,IMAGE_CURSOR,0,0,LR_DEFAULTSIZE | LR_SHARED));
	pos.MakeMove(Search.mvResult);
	//先覆盖起点与终点棋子
	DrawSquare(SRC(Xqwl.mvLast));
	DrawSquare(DST(Xqwl.mvLast));
	//将变化的棋子画上去
	Xqwl.mvLast = Search.mvResult;
	DrawSquare(SRC(Xqwl.mvLast),DRAW_SELECTED);
	DrawSquare(DST(Xqwl.mvLast), DRAW_SELECTED);
	//检测局面是否有重复
	vlRep = pos.RepStatus(3);
	if(pos.IsMate()) 
	{
		PlayResWav(IDR_LOSS);
		MessageBoxMute("你输了要帮老妈洗碗噢(⊙o⊙)");
		Xqwl.bGameOver = TRUE;
	}
	else if(vlRep > 0) 
	{
		vlRep = pos.RepValue(vlRep);
		PlayResWav(vlRep < - WIN_VALUE ? IDR_LOSS : vlRep > WIN_VALUE ? IDR_WIN : IDR_DRAW);
		MessageBoxMute(vlRep < -WIN_VALUE ? "Yeah！打的好累啊。。 不过还是我赢啦！" : vlRep > WIN_VALUE ? "/(ㄒoㄒ)/~~下那么久我累死了，还是让给你赢好了" : "这次打平！下次再来！");
		Xqwl.bGameOver = TRUE;
	}
	else if(pos.nMoveNum > 100)
	{
		PlayResWav(IDR_DRAW);
		MessageBoxMute("下了那么久，累死我了！ 这次当作打平好了~");
		Xqwl.bGameOver = TRUE;
	}
	else 
	{
		PlayResWav(pos.InCheck() ? IDR_CHECK2 : pos.Captured() ? IDR_CAPTURE2 : IDR_MOVE2);
		if(pos.Captured()) pos.SetIrrev();
	}

}

static void ClickSquare(int sq)
{
	int pc,mv,vlRep;
	Xqwl.hdc = GetDC(Xqwl.hWnd);
	Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
	sq = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq ;
	pc = pos.ucpcSquares[sq];

	if(pc & SIDE_TAG(pos.sdPlayer))
	{
		if(Xqwl.sqSelected) DrawSquare(Xqwl.sqSelected);
		Xqwl.sqSelected = sq;
		DrawSquare(sq, DRAW_SELECTED);
		if(Xqwl.mvLast)
		{
			DrawSquare(SRC(Xqwl.mvLast));
			DrawSquare(DST(Xqwl.mvLast));
		}
		PlayResWav(IDR_CLICK);
	}

	else if(Xqwl.sqSelected && !Xqwl.bGameOver)
	{
		mv = MOVE(Xqwl.sqSelected, sq);
		if(pos.LegalMove(mv))
		{
			if(pos.MakeMove(mv))
			{
				Xqwl.mvLast = mv;
				DrawSquare(Xqwl.sqSelected, DRAW_SELECTED);
				DrawSquare(sq, DRAW_SELECTED);
				Xqwl.sqSelected = 0;
				vlRep = pos.RepStatus(3);
				if(pos.IsMate())
				{
					PlayResWav(IDR_WIN);
					MessageBoxMute("哇，这都被你赢了。");
					Xqwl.bGameOver = TRUE;
				}
				else if(vlRep > 0)
				{
					vlRep = pos.RepValue(vlRep);
					PlayResWav(vlRep > WIN_VALUE ? IDR_LOSS : vlRep < -WIN_VALUE ? IDR_WIN : IDR_DRAW);
					MessageBoxMute(vlRep > WIN_VALUE ? "Yeah！打的好累啊。。 不过还是我赢啦！" : vlRep < -WIN_VALUE ? "/(ㄒoㄒ)/~~下那么久我累死了，还是让给你赢好了" : "这次打平！下次再来！");
					Xqwl.bGameOver = TRUE;
				}
				else if(pos.nMoveNum > 100) 
				{
					PlayResWav(IDR_DRAW);
					MessageBoxMute("下了那么久，累死我了！ 这次当作打平好了~");
					Xqwl.bGameOver = TRUE;
				}
				else 
				{
					PlayResWav(pos.InCheck() ? IDR_CHECK : pos.Captured() ? IDR_CAPTURE : IDR_MOVE);
					if(pos.Captured()) pos.SetIrrev();
					ResponseMove();
				}
			}
			else PlayResWav(IDR_ILLEGAL);
		}

	}
	DeleteDC(Xqwl.hdcTmp);
	ReleaseDC(Xqwl.hWnd, Xqwl.hdc);
}

//初始化棋盘
static void Startup(void)
{
	pos.Startup();
	Xqwl.sqSelected = Xqwl.mvLast = 0;
	Xqwl.bGameOver = FALSE;
}

//窗体事件捕捉过程
static LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int x ,y ;
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;
	MSGBOXPARAMS mbp;

	switch(uMsg){
	
	case WM_CREATE:
		GetWindowRect(hWnd, &rect);
		x = rect.left;
		y = rect.top;
		rect.right = rect.left + BOARD_WIDTH;
		rect.bottom = rect.top + BOARD_LENGTH;
		AdjustWindowRect(&rect, WINDOWS_STYLES, TRUE);
		MoveWindow(hWnd, x,y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDM_FILE_RED:
		case IDM_FILE_BLACK:
			Xqwl.bFlipped = (LOWORD(wParam) == IDM_FILE_BLACK);
			Startup();
			hdc = GetDC(Xqwl.hWnd);
			DrawBoard(hdc);
			if(Xqwl.bFlipped)
			{
				Xqwl.hdc = hdc;
				Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
				ResponseMove();
				DeleteDC(Xqwl.hdcTmp);
			}
			ReleaseDC(Xqwl.hWnd, hdc);
			break;
		case IDM_FILE_EXIT:
			DestroyWindow(Xqwl.hWnd);
			break;
		case IDM_HELP_HOME:
			ShellExecute(NULL, NULL, "http://www.baidu.com/", NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDM_HELP_ABOUT:
			MessageBeep(MB_ICONINFORMATION);
			mbp.cbSize = sizeof(MSGBOXPARAMS);
			mbp.hwndOwner = hWnd;
			mbp.hInstance = Xqwl.hInst;
			mbp.lpszText = cszAbout;
			mbp.lpszCaption = "关于如何战胜您的儿子";
			mbp.dwStyle = MB_USERICON;
			mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
			mbp.dwContextHelpId = 0;
			mbp.lpfnMsgBoxCallback = NULL;
			mbp.dwLanguageId = 0;
			MessageBoxIndirect(&mbp);
			break;
	}
		break;
	case WM_PAINT:
		hdc = BeginPaint(Xqwl.hWnd, &ps);
		DrawBoard(hdc);
		EndPaint(Xqwl.hWnd, &ps);
	case WM_LBUTTONDOWN:
    x = RANGE_LEFT + (LOWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
    y = RANGE_TOP + (HIWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
    if (x >= RANGE_LEFT && x <= RANGE_RIGHT && y >= RANGE_TOP && y <= RANGE_BOTTOM) {
      ClickSquare(COORD_XY(x, y));
    }
    break;
  // 其他事件
  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return FALSE;
}

inline HBITMAP LoadResBmp(int nResId)
{	return (HBITMAP) LoadImage(Xqwl.hInst, MAKEINTRESOURCE(nResId), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  int i;
  MSG msg;
  WNDCLASSEX wce;

  // 初始化全局变量
  srand((DWORD) time(NULL));
  InitZobrist();
  Xqwl.hInst = hInstance;
  LoadBook();
  Xqwl.bFlipped = FALSE;
  Startup();

  // 装入图片
  Xqwl.bmpBoard = LoadResBmp(IDB_BOARD);
  Xqwl.bmpSelected = LoadResBmp(IDB_SELECTED);
  for (i = PIECE_KING; i <= PIECE_PAWN; i ++) {
    Xqwl.bmpPieces[SIDE_TAG(0) + i] = LoadResBmp(IDB_RK + i);
    Xqwl.bmpPieces[SIDE_TAG(1) + i] = LoadResBmp(IDB_BK + i);
  }

  // 设置窗口
  wce.cbSize = sizeof(WNDCLASSEX);
  wce.style = 0;
  wce.lpfnWndProc = (WNDPROC) WndProc;
  wce.cbClsExtra = wce.cbWndExtra = 0;
  wce.hInstance = hInstance;
  wce.hIcon = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, LR_SHARED);
  wce.hCursor = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
  wce.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  wce.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
  wce.lpszClassName = "Family Chess";
  wce.hIconSm = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, LR_SHARED);
  RegisterClassEx(&wce);

  // 打开窗口
  Xqwl.hWnd = CreateWindow("Family Chess", "家庭象棋", WINDOWS_STYLES,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
  if (Xqwl.hWnd == NULL) {
    return 0;
  }
  ShowWindow(Xqwl.hWnd, nCmdShow);
  UpdateWindow(Xqwl.hWnd);

  // 接收消息
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}