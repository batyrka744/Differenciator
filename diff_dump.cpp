#include "Diff_dump.h"
#define FULL_DUMP

void DiffDumpGraph(Node_t* root, VarTable* vars, const char* reason) {
    my_assert(root != NULL,);
    my_assert(vars != NULL,);
    my_assert(reason != NULL,);

    FILE* file = fopen("Dump_files\\graph_dump.dot", "w");
    my_assert(file != NULL,);

    fprintf(file, "digraph Tree {\n");
    fprintf(file, "\trankdir=TB;\n");
    fprintf(file, "\tnode [style=filled, fillcolor=\"#00FFFF\"];\n\n");

    fprintf(file, "\tlabelloc=\"t\";\n");
    fprintf(file, "\tlabeljust=\"l\";\n");
    fprintf(file, "\tlabel=\"Dump: %s \npointer to root: %p | pointer to VarTable: %p | pointer to array with var names: %p "
                    "| VarTable capacity: %zu | num_of_vars: %zu\";\n",
            reason, root, vars, vars->names, vars->capacity, vars->size);
    fprintf(file, "\tfontsize=16;\n\n");
    

    RecOutput(root, vars, file);
    

    fprintf(file, "}\n");
    fclose(file);


    system("dot -Tpng Dump_files\\graph_dump.dot -o Dump_files\\graph.png");
}


void RecOutput(Node_t* node, VarTable* vars, FILE* file) {
    my_assert(node != NULL,);
    my_assert(vars != NULL,);
    my_assert(file != NULL,);
    
    switch(node->val.type) {
        case NUM:
#ifdef FULL_DUMP
            fprintf(file, "\tnode%p [shape=Mrecord, label=\"{ node: %p | parent: %p | NUM | %lg | { <l> L: %p | <r> R: %p } }\"];\n",
                    node, node, node->parent, node->val.num, node->left_son, node->right_son);
#else
            fprintf(file, "\tnode%p [shape=circle, label=\" %lg\"];\n", node, node->val.num);
#endif
            break;

        case VAR:
#ifdef FULL_DUMP
            fprintf(file, "\tnode%p [shape=Mrecord, fillcolor=\"#FFFF00\", label=\"{ node: %p | parent: %p | VAR [%zd] | %s | { <l> L: %p | <r> R: %p } }\"];\n",
                    node, node, node->parent, node->val.index, vars->names[node->val.index], node->left_son, node->right_son);
#else
            fprintf(file, "\tnode%p [shape=square, fillcolor = \"#FFFF00\", label=\" %s\"];\n", node, vars->names[node->val.index]);
#endif
            break;

        case OP: {
            char color[256] = "";
            switch(node->val.op) {
                case ADD:
                    strcpy(color, "#90EE90");
                    break;
                case SUB:
                    strcpy(color, "#87CEFA");
                    break;
                case MUL:
                    strcpy(color, "#FFA500");
                    break;
                case DIV:
                    strcpy(color, "#0000FF");
                    break;  
                default:
                    strcpy(color, "#4B0082");
                    break;
            }
#ifdef FULL_DUMP
            fprintf(file, "\tnode%p [shape=Mrecord, fillcolor=\"%s\", label=\"{ node: %p | parent: %p | OP [%d] | %s | { <l> L: %p | <r> R: %p } }\"];\n",
                    node, color, node, node->parent, node->val.op, OP_TABLE[(node->val.op)].name, node->left_son, node->right_son);
#else
            fprintf(file, "\tnode%p [shape=invtriangle, fillcolor=\"%s\", label=\" %s\"];\n", node, color, OP_TABLE[(node->val.op)].name);
#endif
            break;
        }
        default:
#ifdef FULL_DUMP
            fprintf(file, "\tnode%p [shape=Mrecord, fillcolor=\"#D3D3D3\", label=\"{ node: %p | parent: %p | UNKNOWN [%d] | num: %lg | idx: %zd | op: %d | { <l> L: %p | <r> R: %p } }\"];\n",
                    node, node, node->parent, node->val.type, node->val.num, node->val.index, node->val.op, node->left_son, node->right_son);
#else
            fprintf(file, "\tnode%p [fillcolor=\"#D3D3D3\", label=\"unknown value_type: %d\"];\n", node, node->val.type);
#endif
            break;
    }

    if (node->left_son != NULL) {
#ifdef FULL_DUMP
        fprintf(file, "\tnode%p:l -> node%p [color=\"#B22222\"];\n", node, node->left_son);
#else
        fprintf(file, "\tnode%p -> node%p [color=\"#B22222\"];\n", node, node->left_son);
#endif
        RecOutput(node->left_son, vars, file);
    }     
    if (node->right_son != NULL) {
#ifdef FULL_DUMP
        fprintf(file, "\tnode%p:r -> node%p [color=\"#2E8B57\"];\n", node, node->right_son);
#else
        fprintf(file, "\tnode%p -> node%p [color=\"#2E8B57\"];\n", node, node->right_son);
#endif
        RecOutput(node->right_son, vars, file);
    }
}



void DiffDump_file(Node_t* node, VarTable* vars, errors err_code, const char* file, int line, const char* func) {
    FILE* log_file = fopen(LOG_GRAPH, "w"); 
    my_assert_tell(log_file != NULL, "Open-file err",);

    fprintf(log_file, "<pre>\n");
    fprintf(log_file, "========================================\n");
    fprintf(log_file, "DIFF DUMP\n");
    fprintf(log_file, "Error code: %d (%s)\n", err_code, ErrCode(err_code));
    fprintf(log_file, "Called from: %s() at %s:%d\n\n", func, file, line);

    fprintf(log_file, "capacity = %d\n", vars->capacity);
    fprintf(log_file, "size = %d\n", vars->size);
    fprintf(log_file, "vars [%p]\n{\n", vars->names);
    for (size_t i = 0; i < vars->size; i++) {
        fprintf(log_file, "\tvar%d: %p - \"%s\"\n", i, vars->names[i], vars->names[i]);
    }         
    fprintf(log_file, "}\n");

    fprintf(log_file, "\nCurrent node: [%p]\n", (void*)node);
    fprintf(log_file, "========================================\n");
    fprintf(log_file, "</pre>\n");

    DiffDumpGraph(node, vars, "In DiffDump_file");
    fprintf(log_file, "<img src=\"graph.png\">\n<hr>\n");

    fclose(log_file);
}

const char* ErrCode(errors err) {
    switch(err) {
        case ERR_OK:
            return "ERR_OK";
        case ERR_NULL_PTR:
            return "ERR_NULL_PTR";
        case ERR_NO_MEMORY:
            return "ERR_NO_MEMORY";
        case ERR_TREE_CYCLE:
            return "ERR_TREE_CYCLE";
        case ERR_BAD_VAR_IDX:
            return "ERR_BAD_VAR_IDX";
        case ERR_INVALID_CHILDREN:
            return "ERR_INVALID_CHILDREN";
        case ERR_UNKNOWN_NODE_TYPE:
            return "ERR_UNKNOWN_NODE_TYPE";
        default:
            return "UNKNOWN ERROR";
    }
    return "SWITCH ERROR ERROR";
}

void TexDumpStart(Node_t* root, VarTable* vars) {
    FILE* file = fopen(TEX_FILE, "w");
    my_assert(file,);

    fprintf(file, "\\documentclass[a4paper,12pt]{article}\n"
                    "\\usepackage[utf8]{inputenc}\n"
                    "\\usepackage[russian]{babel}\n"
                    "\\usepackage{amsmath}\n\n"
                    "\\begin{document}\n");


    fprintf(file, "\\[ ");
    TexPrintNode(root, vars, file);
    fprintf(file, " \\]\n");


    fprintf(file, "\\end{document}");
    fclose(file);
    system("pdflatex -quiet -output-directory=Dump_files Dump_files\\tex_dump.tex");
}

void TexPrintNode(const Node_t* node, VarTable* vars, FILE* file) {
    my_assert(vars,);
    my_assert(file,);
    if (!node) return;

    switch(node->val.type) {
        
        case NUM:
            fprintf(file, "%lg", node->val.num);
            break;

        case VAR:
            fprintf(file, "%s", vars->names[node->val.index]);
            break;

        case OP: {
            ssize_t i = find_op_by_type(node->val.op);
            my_assert(i >= 0,);

            switch(node->val.op) {

                case DIV:

                    fprintf(file, "%s{", OP_TABLE[i].tex);
                    TexPrintNode(node->left_son, vars, file);

                    fprintf(file, "}{");
                    TexPrintNode(node->right_son, vars, file);
                    fprintf(file, "}");

                    break;


                case ADD:
                case SUB:
                case MUL:
                    TexPrintChild(node, node->left_son, false, vars, file);
                    fprintf(file, "%s", OP_TABLE[i].tex);
                    TexPrintChild(node, node->right_son, true, vars, file);

                    break;


                case POWER:
                    fprintf(file, "{");
                    TexPrintChild(node, node->left_son, false, vars, file);
                    fprintf(file, "}^{");
                    TexPrintChild(node, node->right_son, true, vars, file);
                    fprintf(file, "}");

                    break;


                case SQRT:
                    fprintf(file, "%s{", OP_TABLE[i].tex);
                    TexPrintNode(node->left_son, vars, file);
                    TexPrintNode(node->right_son, vars, file);
                    fprintf(file, "}");

                    break;


                default:
                    fprintf(file, "%s(", OP_TABLE[i].tex);
                    TexPrintNode(node->left_son, vars, file);
                    TexPrintNode(node->right_son, vars, file);
                    fprintf(file, ")");

                    break;
            }

            break;
        }
            
    }
}

bool NeedBrackets(const Node_t* parent, const Node_t* child, bool is_right) {
    if (!parent || !child) return false;
    if (parent->val.type != OP || child->val.type != OP) return false;
    if (parent->val.op == DIV) return false;

    op_priority parent_prio = OP_TABLE[parent->val.op].prio;
    op_priority child_prio  = OP_TABLE[child->val.op].prio;

    if (child_prio < parent_prio) return true;
    if (child_prio == parent_prio && is_right && parent->val.op == SUB) return true;
    if (child_prio == parent_prio && !is_right && parent->val.op == POWER) return true;

    return false;
}

void TexPrintChild(const Node_t* parent, const Node_t* child, bool is_right, VarTable* vars, FILE* file) {
    bool brackets = NeedBrackets(parent, child, is_right);
    if (brackets) fprintf(file, "\\left(");
    TexPrintNode(child, vars, file);
    if (brackets) fprintf(file, "\\right)");
}

