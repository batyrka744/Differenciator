#include "Diff_dump.h"





void TreeDumpGraph(Node_t* root, VarTable* vars, const char* reason) {
    my_assert(root != NULL,);
    my_assert(vars != NULL,);
    my_assert(reason != NULL,);

    FILE* file = fopen("dump.dot", "w");
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
    my_assert(file != NULL,);

    /*if (node->parent == NULL)
        fprintf(file, "\tnode%p [label=\" %lg\", fillcolor = \"#FF00FF\"];\n", node, node->data);*/
    
    switch(node->val.type) {
        case NUM:
            fprintf(file, "\tnode%p [label=\" %lg\"];\n", node, node->val.num);
            break;

        case VAR:
            fprintf(file, "\tnode%p [label=\" %s\"];\n", node, vars->names[node->val.index]);
            break;

        case OP:
            fprintf(file, "\tnode%p [label=\" %d\"];\n", node, node->val.op);//not done
            break;
    }
    fprintf(file, "\tnode%p [label=\" %lg\"];\n", node, node->val.num);

    if (node->left_son != NULL) {
        fprintf(file, "\tnode%p -> node%p [color=\"#B22222\"];\n", node, node->left_son);
        RecOutput(node->left_son, vars, file);
    }     
    if (node->right_son != NULL) {
        fprintf(file, "\tnode%p -> node%p [color=\"#2E8B57\"];\n", node, node->right_son);
        RecOutput(node->right_son, vars, file);
    }
}


/*
void TreeDump_file(const Tree* tree, const TreeNode_t* node, errors err_code, const char* file, int line, const char* func) {
    FILE* log_file = fopen(LOG_FILE, "w"); 
    my_assert_tell(log_file != NULL, "Open-file err",);

    fprintf(log_file, "<pre>\n");
    fprintf(log_file, "========================================\n");
    fprintf(log_file, "TREE DUMP\n");
    fprintf(log_file, "Error code: %d (%s)\n", err_code, ErrCode(err_code));
    fprintf(log_file, "Called from: %s() at %s:%d\n\n", func, file, line);

    if (tree != NULL) {
        fprintf(log_file, "Tree address: [%p]\n", (void*)tree);
        fprintf(log_file, "Tree size:    %zu\n", tree->num_of_nodes);
        fprintf(log_file, "Root node:    [%p]\n", (void*)tree->root);

    } else {
        fprintf(log_file, "Tree address: NULL (Tree pointer is invalid!)\n");
    }

    fprintf(log_file, "\nCurrent node: [%p]\n", (void*)node);
    fprintf(log_file, "========================================\n");
    fprintf(log_file, "</pre>\n");

    TreeDumpGraph(tree, "In TreeDump_file");
    fprintf(log_file, "<img src=\"dump.png\">\n<hr>\n");

    fclose(log_file);
}*/
