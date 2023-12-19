#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "tools.hpp"
#include "difftree.hpp"
#include "differentiator.hpp"
#include "diffTreeDump.hpp"

#define difL DifferentiateTree(node->left)
#define difR DifferentiateTree(node->right)

#define copyL DiffCopyNode(node->left)
#define copyR DiffCopyNode(node->right)

#define ADD(left, right) DiffCreateCmdNode(ADD, left, right).node
#define SUB(left, right) DiffCreateCmdNode(SUB, left, right).node
#define MUL(left, right) DiffCreateCmdNode(MUL, left, right).node
#define DIV(left, right) DiffCreateCmdNode(DIV, left, right).node
#define POW(left, right) DiffCreateCmdNode(POW, left, right).node

#define SIN(val)         DiffCreateCmdNode(SIN,  nullptr, val).node
#define COS(val)         DiffCreateCmdNode(COS,  nullptr, val).node
#define TG(val)          DiffCreateCmdNode(TG,  nullptr, val).node
#define CTG(val)         DiffCreateCmdNode(CTG,  nullptr, val).node
#define LN(val)          DiffCreateCmdNode(LN,  nullptr, val).node

#define CNST(val)        DiffCreateCnstNode(val, nullptr, nullptr).node

size_t _recDiffOptimise(DiffNode* node);
bool _DiffOptimiseDelNeutral(DiffNode* node);
bool _DiffOptimiseCountCnst(DiffNode* node);
bool _OptMulDiv(DiffNode* node);
bool _OptAddSub(DiffNode* node);

DiffNode* diffAdd(DiffNode* node);
DiffNode* diffSub(DiffNode* node);
DiffNode* diffMul(DiffNode* node);
DiffNode* diffDiv(DiffNode* node);
DiffNode* diffPow(DiffNode* node);
DiffNode* diffSin(DiffNode* node);
DiffNode* diffCos(DiffNode* node);
DiffNode* diffTg(DiffNode* node);
DiffNode* diffCtg(DiffNode* node);
DiffNode* diffLn(DiffNode* node);
DiffNode* diffArcSin(DiffNode* node);
DiffNode* diffArcCos(DiffNode* node);
DiffNode* diffArcTg(DiffNode* node);
DiffNode* diffArcCtg(DiffNode* node);

DiffNode* DifferentiateTree(DiffNode* curNode)
    {
    if (!curNode)
        return nullptr;

    switch (curNode->type)
        {
        case CONST:
            return DiffCreateCnstNode(0, nullptr, nullptr).node;
            break;
        case VAR:
            return DiffCreateCnstNode(1, nullptr, nullptr).node;
            break;
        case COMMAND:
            switch (curNode->value.cmd)
                {
                case ADD:       return diffAdd(curNode);
                case SUB:       return diffSub(curNode);
                case MUL:       return diffMul(curNode);
                case DIV:       return diffDiv(curNode);
                case POW:       return diffPow(curNode);
                case SIN:       return diffSin(curNode);
                case COS:       return diffCos(curNode);
                case TG:        return diffTg(curNode);
                case CTG:       return diffCtg(curNode);
                case LN:        return diffLn(curNode);
                case ARCSIN:    return diffArcSin(curNode);
                case ARCCOS:    return diffArcCos(curNode);
                case ARCTG:     return diffArcTg(curNode);
                case ARCCTG:    return diffArcCtg(curNode);
                default: printf("cmd unknown\n");
                }
            break;
        }
    ERR_PRINT();
    return nullptr;
    }

error_t DiffOptimise(DiffTree* tree)
    {
    size_t ischange = _recDiffOptimise(tree->root);

    while (ischange)
        ischange = _recDiffOptimise(tree->root);
    
    return RecountNodes(tree);
    }

size_t _recDiffOptimise(DiffNode* node)
    {
    if (node == nullptr)
        return false;
    
    size_t ischange = 0;
    
    ischange += _recDiffOptimise(node->left);
    ischange += _recDiffOptimise(node->right);

    if (_DiffOptimiseCountCnst(node))
        ischange++;

    // DiffTree tree = {};
    // size_t s = 10;
    // tree.root = node;
    // tree.size = &s;
    // TreeGraphicDump(&tree);

    if (_DiffOptimiseDelNeutral(node))
        ischange++;

    // TreeGraphicDump(&tree);
    return ischange;
    }

error_t _UpdateNode(DiffNode* node)
    {
    node->type = CONST;
    node->subtreeLen = 1;

    NodeDestruct(node->left);
    NodeDestruct(node->right);

    node->left = nullptr;
    node->right = nullptr;
    return NO_ERROR;
    }

bool _DiffOptimiseCountCnst(DiffNode* node)
    {
    if (node->right == nullptr || node->left == nullptr)
        return 0;
    
    if (node->type == COMMAND && node->left->type == CONST && node->right->type == CONST)
        {
        switch (node->value.cmd)
            {
            case ADD: 
                node->value.cnst = node->left->value.cnst + node->right->value.cnst;
                _UpdateNode(node);
                return true;
            case SUB: 
                node->value.cnst = node->left->value.cnst - node->right->value.cnst;
                _UpdateNode(node);
                return true;
            case MUL: 
                node->value.cnst = node->left->value.cnst * node->right->value.cnst;
                _UpdateNode(node);
                return true;
            case DIV: 
                node->value.cnst = node->left->value.cnst / node->right->value.cnst;
                _UpdateNode(node);
                return true;
            case POW: 
                node->value.cnst = pow(node->left->value.cnst, node->right->value.cnst);
                _UpdateNode(node);
                return true;
            default: return false;
            }
        }

    return false;
    }

bool _DiffOptimiseDelNeutral(DiffNode* node)
    {
    if (node->right == nullptr || node->left == nullptr)
        return 0;
    
    bool ischange = false;
    ischange += _OptMulDiv(node);
    ischange += _OptAddSub(node);
    return false;
    }

bool _OptMulDiv(DiffNode* node)
    {
    if (node->type == COMMAND && (node->value.cmd == MUL || node->value.cmd == DIV))
        {
        if ((node->left->type == CONST && node->left->value.cnst == 0) || (node->right->type == CONST && node->right->value.cnst == 0))
            {
            node->type = CONST;
            node->value.cnst = 0;
            node->subtreeLen = 0;

            NodeDestruct(node->left);
            NodeDestruct(node->right);

            node->left = nullptr;
            node->right = nullptr;

            return true;
            }

        else if (node->value.cmd == MUL && node->left->type == CONST && node->left->value.cnst == 1 && node->left->subtreeLen == 1)
            {
            node->type = node->right->type;
            node->value = node->right->value;
            node->subtreeLen = node->right->subtreeLen;
            
            DiffNode* oldrn = node->right;
            DiffNode* oldln = node->left;

            node->right = nullptr;
            node->left = nullptr;

            if (oldrn->left)
                {
                node->left = oldrn->left;
                node->left->parent = node;
                }
            
            if (oldrn->right)
                {
                node->right = oldrn->right;
                node->right->parent = node;
                }

            free(oldln);
            free(oldrn);
            return true;
            }
        
        else if (node->right->type == CONST && node->right->value.cnst == 1 && node->right->subtreeLen == 1)
            {
            node->type = node->left->type;
            node->value = node->left->value;
            node->subtreeLen = node->left->subtreeLen;

            DiffNode* oldrn = node->right;
            DiffNode* oldln = node->left;
            
            node->right = nullptr;
            node->left = nullptr;

            if (oldln->right)
                {
                node->right = oldln->right;
                node->right->parent = node;
                }

            if (oldln->left)
                {
                node->left = oldln->left;
                node->left->parent = node;
                }
            
            free(oldrn);
            free(oldln);
            return true;
            }
        }
    return false;
    }

bool _OptAddSub(DiffNode* node)
    {
    if (node->type == COMMAND && (node->value.cmd == ADD || node->value.cmd == SUB))
        {
        if (node->value.cmd == ADD && node->left->type == CONST && node->left->value.cnst == 0)
            {
            node->type = node->right->type;
            node->value = node->right->value;
            node->subtreeLen = node->right->subtreeLen;

            NodeDestruct(node->left);
            DiffNode* oldrn = node->right;

            node->left = node->right->left;
            if (node->right->left)
                node->right->left->parent = node;

            node->right = node->right->right;
            if (node->right->right)
                node->right->right->parent = node;

            free(oldrn);
            return true;
            }

        else if (node->right->type == CONST && node->right->value.cnst == 0)
            {
            node->type = node->left->type;
            node->value = node->left->value;
            node->subtreeLen = node->left->subtreeLen;

            NodeDestruct(node->right);
            DiffNode* oldln = node->left;

            node->right = node->left->right;
            if (node->left->right)
                node->left->right->parent = node;

            node->left = node->left->left;
            if (node->left->left)
                node->left->left->parent = node;

            free(oldln);
            return true;
            }
        }
    return false;
    }

DiffNode* diffAdd(DiffNode* node)
    {
    DiffNode* newNode = ADD(difL, difR);
    return newNode;
    }

DiffNode* diffSub(DiffNode* node)
    {
    DiffNode* newNode = SUB(difL, difR);
    return newNode;
    }

DiffNode* diffMul(DiffNode* node)
    {
    DiffNode* newNode = ADD(MUL(difL, copyR), MUL(copyL, difR));
    return newNode;
    }

DiffNode* diffDiv(DiffNode* node)
    {
    DiffNode* newNode = DIV(SUB(MUL(difL, copyR), MUL(copyL, difR)), POW(copyR, CNST(2)));
    return newNode;
    }

DiffNode* difPowVarUp(DiffNode* node)
    {
    DiffNode* newNode = MUL(MUL(POW(copyL, copyR), LN(copyL)), difR);
    return newNode;
    }

DiffNode* difPowVarUpDown(DiffNode* node)
    {
    DiffNode* newNode = MUL(POW(copyL, copyR), ADD(MUL(LN(copyL), difR), MUL(copyR, DIV(difL, copyL))));
    return newNode;
    }

DiffNode* diffPow(DiffNode* node)
    {
    if (IsVarInSubtree(node->right))
        {
        if (IsVarInSubtree(node->left))
            return difPowVarUpDown(node);
        return difPowVarUp(node);
        }

    DiffNode* newNode = MUL(MUL(POW(copyL, SUB(copyR, CNST(1))), copyR), difL);
    return newNode;
    }

DiffNode* diffSin(DiffNode* node)
    {
    DiffNode* newNode = MUL(COS(copyR), difR);
    return newNode;
    }

DiffNode* diffCos(DiffNode* node)
    {
    DiffNode* newNode = MUL(MUL(CNST(-1), SIN(copyR)), difR);
    return newNode;
    }

DiffNode* diffTg(DiffNode* node)
    {
    DiffNode* newNode = MUL(DIV(CNST(1), POW(COS(copyR), CNST(2))), difR);
    return newNode;
    }

DiffNode* diffCtg(DiffNode* node)
    {
    DiffNode* newNode = MUL(DIV(CNST(-1), POW(SIN(copyR), CNST(2))), difR);
    return newNode;
    }

DiffNode* diffLn(DiffNode* node)
    {
    DiffNode* newNode = MUL(DIV(CNST(1), copyR), difR);
    return newNode;
    }

DiffNode* diffArcSin(DiffNode* node)
    {
    DiffNode* newNode = MUL(DIV(CNST(1), POW(SUB(CNST(1), POW(copyR, CNST(2))), CNST(0.5))), difR);
    return newNode;
    }

DiffNode* diffArcCos(DiffNode* node)
    {
    DiffNode* newNode = MUL(DIV(CNST(-1), POW(SUB(CNST(1), POW(copyR, CNST(2))), CNST(0.5))), difR);
    return newNode;
    }

DiffNode* diffArcTg(DiffNode* node)
    {
    DiffNode* newNode = MUL(DIV(CNST(1), ADD(CNST(1), POW(copyR, CNST(2)))), difR);
    return newNode;
    }

DiffNode* diffArcCtg(DiffNode* node)
    {
    DiffNode* newNode = MUL(DIV(CNST(-1), ADD(CNST(1), POW(copyR, CNST(2)))), difR);
    return newNode;
    }
