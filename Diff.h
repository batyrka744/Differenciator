#pragma once

//----------------------------------------------- LIBRARIES AND FILES --------------------------------------------

#include "my_lib.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
// #include <assert.h>
// #include <ctype.h>
#include <string.h>

//------------------------------------------------------ MACROSES -----------------------------------------------

#define READ_FILE "for_read.txt"
#define NUM_OF_VARS 3


//--------------------------------------------------------- DSL ------------------------------------------------------

#define _NUM(val) NewNum(val, NULL, NULL)
#define _0 NewNum(0, NULL, NULL)
#define _1 NewNum(1, NULL, NULL)

#define _VAR(index) NewVar(index, NULL, NULL)

#define _ADD(L, R) NewOp(ADD, L, R)
#define _SUB(L, R) NewOp(SUB, L, R)
#define _MUL(L, R) NewOp(MUL, L, R)
#define _DIV(L, R) NewOp(DIV, L, R)










//------------------------------------------------ ENUM/STRUCT/CONST/GLOBAL -----------------------------------

enum errors {ERR_OK = 0, ERR_NULL_PTR = 1, ERR_NO_MEMORY = 2};

enum node_type {NUM = 1, VAR = 2, OP = 3};

enum op_type {
    NO_OP = 0, 

    ADD = 1, 
    SUB = 2, 
    MUL = 3, 
    DIV = 4, 

    SQRT = 5,
    POWER = 6,
    LN = 7,
    LOG = 8,

    FACT = 9,
    SGN = 10,
    ABS = 11,

    SIN = 12,
    COS = 13,
    TG = 14,
    CTG = 15,

    ARCSIN = 16, 
    ARCCOS = 17,
    ARCTG = 18,
    ARCCTG = 19,

    SH = 20,
    CH = 21,
    TH = 22,
    CTH = 23,

    ARCSH = 24,
    ARCCH = 25,
    ARCTH = 26,
    ARCCTH = 27,

    MOD = 28
};

struct un_val {
    node_type type;

    double num;
    ssize_t index;
    op_type op;
};

struct Node_t {
    Node_t* parent;

    un_val val;

    Node_t* left_son;
    Node_t* right_son;
};

struct VarTable {
    char** names;
    size_t size;
    size_t capacity;
};







//------------------------------------------------- FUNCTION DECLARATIONS -----------------------------------------------

errors VarTableInit(VarTable* table, size_t capacity);

errors VarTableDtor(VarTable* table);

errors AddVar(VarTable* table, const char* name, size_t* index);

Node_t* NewNode(un_val value, Node_t* left, Node_t* right);

Node_t* NewNum(double num, Node_t* left, Node_t* right);

Node_t* NewOp(op_type op, Node_t* left, Node_t* right);

Node_t* NewVar(ssize_t index, Node_t* left, Node_t* right);

errors DeleteNode(Node_t* node);

errors DestroyBranch(Node_t* node);
