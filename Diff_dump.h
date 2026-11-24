#pragma once

#include "Diff.h"


void DiffDumpGraph(Node_t* root, VarTable* vars, const char* reason);

void RecOutput(Node_t* node, VarTable* vars, FILE* file);

void DiffDump_file(Node_t* node, VarTable* vars, errors err_code, const char* file, int line, const char* func);

const char* ErrCode(errors err);