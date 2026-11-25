#include "Diff_dump.h"

int main() {

    FILE* file_read = fopen(READ_FILE, "r");
    my_assert(file_read, -1);

    VarTable vars = {0};
    VarTableInit(&vars, 10);

    Node_t* node1 = _NUM(2);
    Node_t* node2 = _VAR("x", &vars);
    Node_t* node3 = _NUM(5);
    Node_t* node4 = _VAR("y", &vars);

    Node_t* node = _MUL(_ADD(node1, node2), _SUB(node3, node4));



    TexDumpStart(node, &vars);
    DiffDump_file(node, &vars, ERR_OK, LOG_GRAPH, __LINE__, __func__);

    DestroyBranch(node);
    VarTableDtor(&vars);
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

errors AddVar(VarTable* table, const char* name, ssize_t* index) {
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


/**
 * @brief finds op in the table by it's number
 * 
 * @param[in] type number of op in enum op_type
 * @return ssize_t index in OP_TABLE, -1 if not_founded
 */
ssize_t find_op_by_type(op_type type) {
    size_t size = sizeof(OP_TABLE) / sizeof(OP_TABLE[0]);
    for (size_t i = 0; i < size; i++) {
        if (type == OP_TABLE[i].type)
            return i;
    }
    return NOT_FOUNDED;
}

/**
 * @brief finds op in the table by it's name 
 * 
 * @param[in] name name of op 
 * @return ssize_t index in OP_TABLE, -1 if not_founded
 */
ssize_t find_op_by_name(const char* name) {
    my_assert(name, NOT_FOUNDED);

    size_t size = sizeof(OP_TABLE) / sizeof(OP_TABLE[0]);
    for (size_t i = 0; i < size; i++) {
        if (strcmp(name, OP_TABLE[i].name) == 0)
            return i;
    }
    return NOT_FOUNDED;
}

//-------------------------------------------------- NODE FUNCTIONS ------------------------------------------

Node_t* NewNode(un_val value, Node_t* left, Node_t* right) {
    //некоторые узлы могут быть без сыновей, поэтому нет ассертов
    //лучше скидывай свою почту и пароль, Артёмчик, будем открывать с тобой кейсики

    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    my_assert(node, NULL);

    switch(value.type) {

        case OP:
            node->val.num = NAN;
            node->val.op = value.op;
            node->val.index = -1;
            node->val.type = OP;
            break;

        case VAR:
            node->val.num = NAN;
            node->val.op = NO_OP;
            node->val.index = value.index;
            node->val.type = VAR;
            break;

        case NUM:
            node->val.num = value.num;
            node->val.op = NO_OP;
            node->val.index = -1;
            node->val.type = NUM;
            break;
    }
    node->left_son = left;
    node->right_son = right;

    if(node->left_son) {
        node->left_son->parent = node;
    }
    if(node->right_son) {
        node->right_son->parent = node;
    }

    return node;
}

Node_t* NewNum(double num) {

    un_val value = {};
    value.type = NUM;
    value.num = num;

    return NewNode(value, NULL, NULL);
}


Node_t* NewVar(const char* name, VarTable* vars) {
    my_assert(name, NULL);
    my_assert(vars, NULL);

    un_val value = {};
    value.type = VAR;

    size_t index = 0;
    errors err = AddVar(vars, name, &value.index);
    
    return NewNode(value, NULL, NULL);
}


Node_t* NewOp(op_type op, Node_t* left, Node_t* right) {

    un_val value = {};
    value.type = OP;
    value.op = op;

    return NewNode(value, left, right);
}

errors DeleteNode(Node_t* node) {
    my_assert(node, ERR_NULL_PTR);

    if (node->parent != NULL) {
        if (node->parent->left_son == node) node->parent->left_son = NULL;
        if (node->parent->right_son == node) node->parent->right_son = NULL;
    }
        
    if (node->left_son != NULL) 
        node->left_son->parent = NULL;
    
    if (node->right_son != NULL) 
        node->right_son->parent = NULL;


    node->val.index = -1;
    node->val.num = NAN;
    node->val.op = NO_OP;

    node->left_son = NULL;
    node->right_son = NULL;
    node->parent = NULL;

    free(node);

    return ERR_OK;
}

errors DestroyBranch(Node_t* node) {
    my_assert(node != NULL, ERR_NULL_PTR);

    errors err = ERR_OK;
    if (node->left_son != NULL)
        err = DestroyBranch(node->left_son);
    if (node->right_son != NULL)
        err = DestroyBranch(node->right_son);

    err = DeleteNode(node);

    return err;
}

//------------------------------------------------- VERIFY --------------------------------------------

errors NodeVerify(const Node_t* node, const VarTable* vars, size_t* visited) {
    if (node == NULL) 
        return ERR_OK;

    if (visited != NULL) {
        (*visited)++;
        if (*visited > MAX_TREE_NODES)
            return ERR_TREE_CYCLE;
    }

    switch(node->val.type) {

        case NUM:
            if (node->left_son || node->right_son)
                return ERR_INVALID_CHILDREN;
            break;

        case VAR:
            if (node->left_son || node->right_son)
                return ERR_INVALID_CHILDREN;
            if (!vars || node->val.index >= vars->size) {
                return ERR_BAD_VAR_IDX;
            }
            break;

        case OP:
            if (!node->left_son && !node->right_son)
                return ERR_INVALID_CHILDREN;
            break;

        default:
            return ERR_UNKNOWN_NODE_TYPE;
    }
    errors err_left = NodeVerify(node->left_son, vars, visited);

    if (err_left != ERR_OK) 
        return err_left;

    return NodeVerify(node->right_son, vars, visited);
}

errors TreeVerify(const Node_t* root, const VarTable* vars) {
    if (!root)
        return ERR_OK;

    size_t visited = 0;
    return NodeVerify(root, vars, &visited);
}

