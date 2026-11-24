#include "Diff_dump.h"


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


    system("dot -Tpng dump.dot -o dump.png");
}


void RecOutput(Node_t* node, VarTable* vars, FILE* file) {
    my_assert(node != NULL,);
    my_assert(vars != NULL,);
    my_assert(file != NULL,);
    
    switch(node->val.type) {
        case NUM:
            fprintf(file, "\tnode%p [label=\" %lg\"];\n", node, node->val.num);
            break;

        case VAR:
            fprintf(file, "\tnode%p [label=\" %s\"];\n", node, vars->names[node->val.index]);
            break;

        case OP:
            fprintf(file, "\tnode%p [label=\" %s\"];\n", node, OP_TABLE[(node->val.op)].name);
            break;

        default:
            fprintf(file, "\tnode%p [fillcolor=\"#D3D3D3\", label=\"unknown value_type: %d\"];\n", node, node->val.type);
            break;
    }


    if (node->left_son != NULL) {
        fprintf(file, "\tnode%p -> node%p [color=\"#B22222\"];\n", node, node->left_son);
        RecOutput(node->left_son, vars, file);
    }     
    if (node->right_son != NULL) {
        fprintf(file, "\tnode%p -> node%p [color=\"#2E8B57\"];\n", node, node->right_son);
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