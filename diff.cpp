#include "Diff.h"

int main() {

    FILE* file_read = fopen(READ_FILE, "r");
    my_assert(file_read, -1);





    fclose(file_read);
    return 0;
}

//--------------------------------------------------- VAR_TABLE FUNCTIONS -----------------------------------------------

errors VarTableInit(VarTable* table, size_t capacity) {
    my_assert(table != NULL, ERR_NULL_PTR);

    table->names = (char**)calloc(capacity, sizeof(char*));
    my_assert(table->names, ERR_NO_MEMORY);

    table->capacity = capacity;
    table->size = 0;

    return ERR_OK;
}

errors VarTableDtor(VarTable* table) {
    my_assert(table != NULL, ERR_NULL_PTR);

    free(table->names);
    table->capacity = 0;
    table->size = 0;

    return ERR_OK;
}

errors AddVar(VarTable* table, const char* name, size_t* index) {
    my_assert(table, ERR_NULL_PTR);
    my_assert(name, ERR_NULL_PTR);
    my_assert(index, ERR_NULL_PTR);

    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(name, table->names[i]) == 0) {
            (*index) = i;
            return ERR_OK;
        }
    }

    if (table->capacity <= table->size) {
        char** temp = 0;

        temp = (char**)realloc(table->names, sizeof(char*)*(table->capacity*2 + 1));
        my_assert(temp, ERR_NO_MEMORY);

        table->capacity = table->capacity*2 + 1;

        table->names = temp;
    }

    table->names[table->size] = strdup(name);
    my_assert(table->names[table->size], ERR_NO_MEMORY);

    (*index) = table->size;
    table->size++;

    return ERR_OK;
}

//-------------------------------------------------- NODE FUNCTIONS ------------------------------------------

Node_t* NewNode(un_val value, Node_t* left, Node_t* right) {
    //некоторые узлы могут быть без сыновей, поэтому нет ассертов
    //лучше скидывай свою почту и пароль, Артёмчик, будем открывать с тобой кейсики

    Node_t* node = NULL;

    switch(value.type) {

        case OP:
            NewOp(value.op);
            break;

        case VAR:
            NewVar(value.index);
            break;

        case NUM:
            NewNum(value.num);
            break;
    }

    node->left_son = left;
    node->right_son = right;
    return node;
}

Node_t* NewNum(double num) {

    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    my_assert(node, NULL);
    
    node->val.num = num;
    node->val.op = NO_OP;
    node->val.index = -1;

    return node;
}

Node_t* NewVar(ssize_t index) {
    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    my_assert(node, NULL);
    
    node->val.num = NAN;
    node->val.op = NO_OP;
    node->val.index = index;

    return node;
}

Node_t* NewOp(op_type op) {
    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    my_assert(node, NULL);
    
    node->val.num = NAN;
    node->val.op = op;
    node->val.index = -1;

    return node;
}