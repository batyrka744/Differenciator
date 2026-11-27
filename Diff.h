#pragma once

//----------------------------------------------- LIBRARIES AND FILES --------------------------------------------

#include "my_lib.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

//------------------------------------------------------ MACROSES -----------------------------------------------

#define READ_FILE "for_read.txt"
#define NUM_OF_VARS 3
#define MAX_TREE_NODES 1000
#define NOT_FOUNDED -1

#ifdef DIFF_DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif


#define ASSERT_OK(root, vars) \
ON_DEBUG(\
    do {\
    errors err = TreeVerify(root, vars);\
    if (err != ERR_OK) {\
        DiffDump_file(root, vars, err, __FILE__, __LINE__, __func__);\
        assert(0);}\
    } while(0)\
)

#define LEN 256

//-------------------- DSL -------------------

#define _NUM(val) NewNum(val)
#define _0 NewNum(0)
#define _1 NewNum(1)

#define _VAR(name, vars) NewVar(name, vars)

#define _ADD(L, R) NewOp(ADD, L, R)
#define _SUB(L, R) NewOp(SUB, L, R)
#define _MUL(L, R) NewOp(MUL, L, R)
#define _DIV(L, R) NewOp(DIV, L, R)

#define _DL(diffed, var_idx) Diff(diffed->left_son, var_idx)
#define _DR(diffed, var_idx) Diff(diffed->right_son, var_idx)

#define _CL(diffed) CopyNode(diffed->left_son, NULL)
#define _CR(diffed) CopyNode(diffed->right_son, NULL)

#define _FUNC(op, node, var_idx) ((node->left_son) ? (NewOp(op, _CL(node), NULL)) : (NewOp(op, NULL, _CR(node))))

#define LOG_FILE "Dump_files\\log.txt"
#define LOG_GRAPH "Dump_files\\graph_log.html"
#define TEX_FILE "Dump_files\\tex_dump.tex"

//------------------------------------------------ ENUM/STRUCT/CONST/GLOBAL -----------------------------------

enum errors {
    ERR_OK = 0, 
    ERR_NULL_PTR = 1, 
    ERR_NO_MEMORY = 2, 
    ERR_TREE_CYCLE = 3, 
    ERR_INVALID_CHILDREN = 4,
    ERR_BAD_VAR_IDX = 5,
    ERR_UNKNOWN_NODE_TYPE = 6
};


enum node_type {NUM = 1, VAR = 2, OP = 3};

//---------------- OP TYPE ------------

enum op_priority {
    PRIO_NONE = 0,
    PRIO_ADD = 1,
    PRIO_MUL = 2,
    PRIO_POW = 3,
    PRIO_FUNC = 4
};

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

struct op_info {
    op_type type;
    const char* name;
    const char* tex;
    op_priority prio;
};

static const op_info OP_TABLE[] = {
    {NO_OP,   "",       "",                       PRIO_NONE},
    {ADD,     "+",      "+",                      PRIO_ADD},
    {SUB,     "-",      "-",                      PRIO_ADD},
    {MUL,     "*",      "\\cdot ",                PRIO_MUL},
    {DIV,     "/",      "\\frac",                 PRIO_MUL},
    {SQRT,    "sqrt",   "\\sqrt",                 PRIO_FUNC},
    {POWER,   "^",      "^",                      PRIO_POW},
    {LN,      "ln",     "\\ln",                   PRIO_FUNC},
    {LOG,     "log",    "\\log",                  PRIO_FUNC},
    {FACT,    "fact",   "!",                      PRIO_FUNC},
    {SGN,     "sgn",    "\\operatorname{sgn}",    PRIO_FUNC},
    {ABS,     "abs",    "\\left| \\right|",       PRIO_FUNC},
    {SIN,     "sin",    "\\sin",                  PRIO_FUNC},
    {COS,     "cos",    "\\cos",                  PRIO_FUNC},
    {TG,      "tg",     "\\tan",                  PRIO_FUNC},
    {CTG,     "ctg",    "\\cot",                  PRIO_FUNC},
    {ARCSIN,  "arcsin", "\\arcsin",               PRIO_FUNC},
    {ARCCOS,  "arccos", "\\arccos",               PRIO_FUNC},
    {ARCTG,   "arctg",  "\\arctan",               PRIO_FUNC},
    {ARCCTG,  "arcctg", "\\operatorname{arccot}", PRIO_FUNC},
    {SH,      "sh",     "\\sinh",                 PRIO_FUNC},
    {CH,      "ch",     "\\cosh",                 PRIO_FUNC},
    {TH,      "th",     "\\tanh",                 PRIO_FUNC},
    {CTH,     "cth",    "\\coth",                 PRIO_FUNC},
    {ARCSH,   "arcsh",  "\\operatorname{arsinh}", PRIO_FUNC},
    {ARCCH,   "arcch",  "\\operatorname{arcosh}", PRIO_FUNC},
    {ARCTH,   "arcth",  "\\operatorname{artanh}", PRIO_FUNC},
    {ARCCTH,  "arccth", "\\operatorname{arcoth}", PRIO_FUNC},
    {MOD,     "mod",    "\\bmod",                 PRIO_MUL}
};

//---------------------------------

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

errors AddVar(VarTable* table, const char* name, ssize_t* index);

Node_t* NewNode(un_val value, Node_t* left, Node_t* right);

Node_t* NewNum(double num);

Node_t* NewOp(op_type op, Node_t* left, Node_t* right);

Node_t* NewVar(const char* name, VarTable* vars);

errors DeleteNode(Node_t* node);

errors DestroyBranch(Node_t* node);

errors NodeVerify(const Node_t* node, const VarTable* vars, size_t* visited);

errors TreeVerify(const Node_t* root, const VarTable* vars);

const char* ErrCode(errors err);

ssize_t find_op_by_type(op_type type);

ssize_t find_op_by_name(const char* name);

Node_t* CopyNode(const Node_t* ctrl_c, Node_t* parent);

bool is_var_in_branch(const Node_t* node, ssize_t var_idx);

Node_t* Diff(const Node_t* dNode, ssize_t var_idx);

Node_t* FuncDiff(const Node_t* dNode, Node_t* derivative, ssize_t var_idx);

Node_t* PowerDiff(const Node_t* dNode, ssize_t var_idx);