#include "Diff_dump.h"

int main() {

    FILE* file_read = fopen(READ_FILE, "r");
    my_assert(file_read, -1);

    VarTable vars = {0};
    VarTableInit(&vars, 10);

    Node_t* node = NewOp(POWER, _VAR("x", &vars), _VAR("x", &vars));


    TexDumpStart(node, &vars);
    DiffDump_file(node, &vars, ERR_OK, LOG_GRAPH, __LINE__, __func__);

    Node_t* diff_node = Diff(node, 0);
    TexDumpStart(diff_node, &vars);

    DestroyBranch(diff_node);
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

    for (size_t i = 0; i < table->size; i++)
        free(table->names[i]);

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

//----------------------------------------------- DIFFERENTIATION -----------------------------------

Node_t* CopyNode(const Node_t* ctrl_c, Node_t* parent) {
    if (!ctrl_c) return NULL;

    Node_t* ctrl_v = (Node_t*)calloc(1, sizeof(Node_t));
    my_assert(ctrl_v, NULL);

    ctrl_v->val.index = ctrl_c->val.index;
    ctrl_v->val.num = ctrl_c->val.num;
    ctrl_v->val.op = ctrl_c->val.op;
    ctrl_v->val.type = ctrl_c->val.type;

    ctrl_v->parent = parent;

    ctrl_v->left_son = CopyNode(ctrl_c->left_son, ctrl_v);
    ctrl_v->right_son = CopyNode(ctrl_c->right_son, ctrl_v);

    return ctrl_v;
}


Node_t* Diff(const Node_t* dNode, ssize_t var_idx) {

    un_val value = {};


    switch(dNode->val.type) {

        case NUM:
            value.type = NUM;
            value.num = 0;
            return NewNode(value, NULL, NULL);

        case VAR:

            if (dNode->val.index == var_idx) 
                return _1;
            else
                return _0;
            
        case OP:

            switch(dNode->val.op) {

                case ADD:
                    return _ADD(_DL(dNode, var_idx), _DR(dNode, var_idx));

                case SUB:
                    return _SUB(_DL(dNode, var_idx), _DR(dNode, var_idx));

                case MUL:
                    return _ADD(_MUL(_DL(dNode, var_idx), _CR(dNode)), _MUL(_CL(dNode), _DR(dNode, var_idx)));

                case DIV:
                    return _DIV(_SUB(_MUL(_DL(dNode, var_idx), _CR(dNode)), _MUL(_DR(dNode, var_idx), _CL(dNode))),
                             _MUL(_CR(dNode), _CR(dNode)));

                case POWER:
                    return PowerDiff(dNode, var_idx);

                case SQRT:
                    return FuncDiff(dNode, _DIV(_1, _MUL(_NUM(2), NewOp(SQRT, _CL(dNode), NULL))), var_idx);

                case LOG:
                    return FuncDiff(dNode, _DIV(_1, _MUL(_CR(dNode), NewOp(LN, _CL(dNode), NULL))), var_idx);

                case LN:
                    if(dNode->left_son) return FuncDiff(dNode, _DIV(_1, _CL(dNode)), var_idx);
                    return FuncDiff(dNode, _DIV(_1, _CR(dNode)), var_idx);

                case SIN:
                    return FuncDiff(dNode, _FUNC(COS, dNode, var_idx), var_idx);

                case COS:
                    return FuncDiff(dNode, _MUL(_NUM(-1), _FUNC(SIN, dNode, var_idx)), var_idx);

                case TG:
                    return FuncDiff(dNode, _DIV(_1, NewOp(POWER, _FUNC(COS, dNode, var_idx), _NUM(2))), var_idx);

                case CTG:
                    return FuncDiff(dNode, _DIV(_NUM(-1), NewOp(POWER, _FUNC(SIN, dNode, var_idx), _NUM(2))), var_idx);

                case ARCSIN:
                    return FuncDiff(dNode, _DIV(_1, NewOp(SQRT, _SUB(_1, NewOp(POWER, _CL(dNode), _NUM(2))), NULL)), var_idx);

                case ARCCOS:
                    return FuncDiff(dNode, _DIV(_NUM(-1), NewOp(SQRT, _SUB(_1, NewOp(POWER, _CL(dNode), _NUM(2))), NULL)), var_idx);

                case ARCTG:
                    return FuncDiff(dNode, _DIV(_1, _ADD(_1, NewOp(POWER, _CL(dNode), _NUM(2)))), var_idx);

                case ARCCTG:
                    return FuncDiff(dNode, _DIV(_NUM(-1), _ADD(_1, NewOp(POWER, _CL(dNode), _NUM(2)))), var_idx);

                case SH:
                    return FuncDiff(dNode, _FUNC(CH, dNode, var_idx), var_idx);

                case CH:
                    return FuncDiff(dNode, _FUNC(SH, dNode, var_idx), var_idx);

                case TH:
                    return FuncDiff(dNode, _DIV(_1, NewOp(POWER, _FUNC(CH, dNode, var_idx), _NUM(2))), var_idx);

                case CTH:
                    return FuncDiff(dNode, _DIV(_NUM(-1), NewOp(POWER, _FUNC(SH, dNode, var_idx), _NUM(2))), var_idx);
                
                case ARCSH:
                    return FuncDiff(dNode, _DIV(_1, NewOp(SQRT, _ADD(_1, NewOp(POWER, _CL(dNode), _NUM(2))), NULL)), var_idx);

                case ARCCH:
                    return FuncDiff(dNode, _DIV(_1, NewOp(SQRT, _SUB(NewOp(POWER, _CL(dNode), _NUM(2)), _1), NULL)), var_idx);

                case ARCTH:
                case ARCCTH:
                    return FuncDiff(dNode, _DIV(_1, _SUB(_1, NewOp(POWER, _CL(dNode), _NUM(2)))), var_idx);

                default:
                    printf(RED "I can't count this derivative yet\n" RESET);
                    break;
            }
        break;

        default:
            printf(RED "Unknown type of node\n" RESET);
            break;
    }   
    return NULL;
}

Node_t* FuncDiff(const Node_t* dNode, Node_t* derivative, ssize_t var_idx) {
    my_assert(dNode, NULL);

    bool in_x_l = is_var_in_branch(dNode->left_son, var_idx);
    bool in_x_r = is_var_in_branch(dNode->right_son, var_idx);

    if (in_x_l)
        return _MUL(derivative, _DL(dNode, var_idx));
    else if (in_x_r)
        return _MUL(derivative, _DR(dNode, var_idx));

    DestroyBranch(derivative);
    return _0;
}



Node_t* PowerDiff(const Node_t* dNode, ssize_t var_idx) {
    my_assert(dNode, NULL);

    bool var_in_left = is_var_in_branch(dNode->left_son, var_idx);
    bool var_in_right = is_var_in_branch(dNode->right_son, var_idx);

    if (var_in_left && var_in_right) {
        return _MUL(NewOp(POWER, _CL(dNode), _CR(dNode)), 
        _ADD(_MUL(_DR(dNode, var_idx), NewOp(LN, _CL(dNode), NULL)), 
        _MUL(_CR(dNode), _DIV(_DL(dNode, var_idx), _CL(dNode)))));

    } else if (var_in_left) {
        return _MUL(_MUL(_CR(dNode), NewOp(POWER, _CL(dNode), _SUB(_CR(dNode), _1))), _DL(dNode, var_idx));

    } else if (var_in_right) {
        return _MUL(_MUL(CopyNode(dNode, NULL), _DR(dNode, var_idx)), NewOp(LN, _CL(dNode), NULL));

    } else {
        return _0;
    }

}

bool is_var_in_branch(const Node_t* node, ssize_t var_idx) {
    if (!node) return false;
    
    if (node->val.type == VAR && node->val.index == var_idx)
        return true;
    
    bool in_left = is_var_in_branch(node->left_son, var_idx);
    if (in_left)
        return in_left;
    
    return is_var_in_branch(node->right_son, var_idx);   
}