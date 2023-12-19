#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "tools.hpp"
#include "difftree.hpp"
#include "diffTreeDump.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define CreateCmdNode(cmd) DiffCreateCmdNode(cmd, nullptr, nullptr).node
#define ISCMDTOKEN(cmdname) (tokens[*pos]->type == COMMAND && tokens[*pos]->value.cmd == cmdname)


// const char s[] = "1000-7*100/(30+5*10-5*(100/50))+1";

static DiffNode* _ElemParse(char* string, size_t* pos, size_t* nLines);
static DiffNode* _FindKeyWords(char* string, size_t* pos);

DiffNode* GetG(char* string);
DiffNode* GetN(char* string, size_t* pos);
DiffNode* GetE(DiffNode** tokens, size_t* pos);
DiffNode* GetT(DiffNode** tokens, size_t* pos);
DiffNode* GetP(DiffNode** tokens, size_t* pos);
DiffNode* GetName(DiffNode** tokens, size_t* pos);
DiffNode* GetIfWhile(DiffNode** tokens, size_t* pos);
DiffNode* GetOp(DiffNode** tokens, size_t* pos);
DiffNode* GetA(DiffNode** tokens, size_t* pos);



DiffNode** DiffTokenator(char* string, size_t* nLines)
    {
    size_t lenstr = strlen(string);
    DiffNode** tokens = (DiffNode**) calloc(lenstr + 2, sizeof(DiffNode*));
    size_t pos = 0;
    size_t tknpos = 0;

    while (true)
        {
        printf("[%c]\n", string[pos]);
        
        if (string[pos] == '\0')
            {
            tokens[tknpos++] = CreateCmdNode(HLT);
            break;
            }

        else if (isspace(string[pos]))
            pos++;

        else if (isdigit(string[pos]))
            tokens[tknpos++] = GetN(string, &pos);

        else 
            tokens[tknpos++] = _ElemParse(string + pos, &pos, nLines);
        }

    size_t i = 0;
    while(i < tknpos)
        {
        if (tokens[i]->type == COMMAND)
            printf("<%s>, %d\n", getCmdName(tokens[i]->value.cmd), i);
        // NodeGraphicDump(tokens[i]);
        i++;
        }
        
    return tokens;
    }

#define LENGTH(str) sizeof(str) - 1

static DiffNode* _ElemParse(char* string, size_t* pos, size_t* nLines)
    {
    if (*string == '+')
        {
        (*pos)++;
        return CreateCmdNode(ADD);
        }

    else if (*string == '-')
        {   
        (*pos)++;
        return CreateCmdNode(SUB);
        }

    else if (*string == '*')
        {
        (*pos)++;
        return CreateCmdNode(MUL);
        }

    else if (*string == '/')
        {
        (*pos)++;
        return CreateCmdNode(DIV);
        }

    else if (*string == '^')
        {
        (*pos)++;
        return CreateCmdNode(POW);
        }

    else if (strncasecmp(string, ">=", LENGTH(">=")) == 0)
        {
        *pos += LENGTH(">=");
        return CreateCmdNode(AE);
        }
    
    else if (strncasecmp(string, "<=", LENGTH("<=")) == 0)
        {
        *pos += LENGTH("<=");
        return CreateCmdNode(BE);
        }
        
    else if (strncasecmp(string, "==", LENGTH("==")) == 0)
        {
        *pos += LENGTH("==");
        return CreateCmdNode(E);
        }
        
    else if (strncasecmp(string, "!=", LENGTH("!=")) == 0)
        {
        *pos += LENGTH("!=");
        return CreateCmdNode(NE);
        }
    
    else if (*string == '>')
        {
        (*pos)++;
        return CreateCmdNode(A);
        }
    
    else if (*string == '<')
        {
        (*pos)++;
        return CreateCmdNode(B);
        }

    else if (*string == '=')
        {
        (*pos)++;
        return CreateCmdNode(ASN);
        }
        
    else if (*string == '(')
        {
        (*pos)++;
        return CreateCmdNode(OP_BR);
        }

    else if (*string == ')')
        {
        (*pos)++;
        return CreateCmdNode(CL_BR);
        }

    else if (strncasecmp(string, ":(", LENGTH(":(")) == 0)
        {
        *pos += LENGTH(":(");
        return CreateCmdNode(OP_AR);
        }
    
    else if (strncasecmp(string, ":)", LENGTH(":)")) == 0)
        {
        *pos += LENGTH(":)");
        return CreateCmdNode(CL_AR);
        }

    else if (*string == ';')
        {
        (*pos)++;
        (*nLines)++;
        return CreateCmdNode(HLT);
        }

    else if (strncasecmp(string, "sin", LENGTH("sin")) == 0)
        {
        *pos += LENGTH("sin");
        return CreateCmdNode(SIN);
        }

    else if (strncasecmp(string, "cos", LENGTH("cos")) == 0)
        {
        *pos += LENGTH("cos");
        return CreateCmdNode(COS);
        }

    else if (strncasecmp(string, "tg", LENGTH("tg")) == 0)
        {
        *pos += LENGTH("tg");
        return CreateCmdNode(TG);
        }

    else if (strncasecmp(string, "ctg", LENGTH("ctg")) == 0)
        {
        *pos += LENGTH("ctg");
        return CreateCmdNode(CTG);
        }

    else if (strncasecmp(string, "ln", LENGTH("ln")) == 0)
        {
        *pos += LENGTH("ln");
        return CreateCmdNode(LN);
        }

    else if (strncasecmp(string, "arcsin", LENGTH("arcsin")) == 0)
        {
        *pos += LENGTH("arcsin");
        return CreateCmdNode(ARCSIN);
        }

    else if (strncasecmp(string, "arccos", LENGTH("arccos")) == 0)
        {
        *pos += LENGTH("arccos");
        return CreateCmdNode(ARCCOS);
        }

    else if (strncasecmp(string, "arctg", LENGTH("arctg")) == 0)
        {
        *pos += LENGTH("arctg");
        return CreateCmdNode(ARCTG);
        }

    else if (strncasecmp(string, "arcctg", LENGTH("arcctg")) == 0)
        {
        *pos += LENGTH("arcctg");
        return CreateCmdNode(ARCCTG);
        }

    else if (*string == 'x')
        {
        (*pos)++;
        return DiffCreateVarNode('x', nullptr, nullptr).node;
        }

    else
        return _FindKeyWords(string, pos);
    
    return nullptr;
    }

static DiffNode* _FindKeyWords(char* string, size_t* pos)
    {
    if (strncasecmp(string, "че", LENGTH("че")) == 0 || strncasecmp(string, "чё", LENGTH("чё")) == 0)
        {
        *pos += LENGTH("че");
        return CreateCmdNode(IF);
        }

    else if (strncasecmp(string, "пиздани", LENGTH("пиздани")) == 0)
        {
        *pos += LENGTH("пиздани");
        return CreateCmdNode(PRINT);
        }

    else if (strncasecmp(string, "не тормози", LENGTH("не тормози")) == 0)
        {
        *pos += LENGTH("не тормози");
        return CreateCmdNode(WHILE);
        }

    else 
        return nullptr;
    }

DiffNode* GetN(char* string, size_t* pos)
    {
    double val = 0;

    while ('0' <= string[*pos] && string[*pos] <= '9')
        {
        val = val * 10 + string[*pos] - '0';
        (*pos)++;
        }
        
    return DiffCreateCnstNode(val, nullptr, nullptr).node;
    }

DiffNode* GetG(char* string)
    {
    size_t pos = 0;

    printf("beforetok\n");
    size_t nLines = 0;
    DiffNode** tokens = DiffTokenator(string, &nLines);
    printf("aftertok\n");

    size_t curLine = 1;

    DiffNode* node = GetOp(tokens, &pos);

    free(tokens[pos]);
    free(tokens);
    return node;
    }

DiffNode* GetE(DiffNode** tokens, size_t* pos)
    {
    DiffNode* node1 = GetT(tokens, pos);

    while (tokens[*pos]->type == COMMAND && tokens[*pos]->value.cmd >= E && tokens[*pos]->value.cmd <= SUB)
        {
        difCommands op = tokens[*pos]->value.cmd;
        // NodeGraphicDump(tokens[*pos]);
        free(tokens[*pos]); 
        (*pos)++;

        DiffNode* node2 = GetT(tokens, pos);

        node1 = DiffCreateCmdNode(op, node1, node2).node;
        }

    return node1;
    }

DiffNode* GetT(DiffNode** tokens, size_t* pos)
    {
    DiffNode* node1 = GetP(tokens, pos);

    while (tokens[*pos]->type == COMMAND && (tokens[*pos]->value.cmd == MUL || tokens[*pos]->value.cmd == DIV || tokens[*pos]->value.cmd == POW))
        {
        difCommands op = tokens[*pos]->value.cmd;
        free(tokens[*pos]);
        (*pos)++;
        DiffNode* node2 = GetP(tokens, pos);

        node1 = DiffCreateCmdNode(op, node1, node2).node;
        }

    return node1;
    }

DiffNode* GetIfWhile(DiffNode** tokens, size_t* pos)
    {
    if (ISCMDTOKEN(IF) || ISCMDTOKEN(WHILE))
        {
        DiffNode* OPnode = tokens[*pos];
        (*pos)++;
        if (ISCMDTOKEN(OP_BR))
            {
            free(tokens[*pos]);
            (*pos)++;
            DiffNode* node = GetE(tokens, pos);
            OPnode->left = node;
            if (ISCMDTOKEN(CL_BR))
                {
                free(tokens[*pos]);
                (*pos)++;
                OPnode->right = GetOp(tokens, pos);
                return OPnode;
                }
            return nullptr;
            }
        return nullptr;
        }
    return nullptr;
    }

DiffNode* GetOp(DiffNode** tokens, size_t* pos)
    {

    if (ISCMDTOKEN(IF) || ISCMDTOKEN(WHILE))
        {
        return GetIfWhile(tokens, pos);
        }

    else if (tokens[*pos]->type == VAR)
        {
        return GetA(tokens, pos);
        }
    
    else if (ISCMDTOKEN(OP_AR))
        {
        free(tokens[*pos]);
        (*pos)++;
        DiffNode* node1 = GetOp(tokens, pos);
        DiffNode* node2 = {};

        DiffNode* root = node1;

        while (!ISCMDTOKEN(CL_AR))
            {
            node2 = GetOp(tokens, pos);
            node1->right = node2;
            node1 = node2;
            }

        free(tokens[*pos]);
        (*pos)++;
        return root;
        }

    else 
        return nullptr;
    }

DiffNode* GetA(DiffNode** tokens, size_t* pos)
    {
    if (tokens[*pos]->type == VAR)
        {
        DiffNode* varnode = tokens[*pos];
        (*pos)++;
        if (ISCMDTOKEN(ASN))
            {
            DiffNode* assign = tokens[*pos];
            (*pos)++;
            assign->left = varnode;
            DiffNode* value = GetE(tokens, pos);
            assign->right = value;
            if (ISCMDTOKEN(HLT))
                {
                DiffNode* halt = tokens[*pos];
                (*pos)++;
                halt->left = assign;
                return halt;
                }
            return nullptr;
            }

        return nullptr;
        }

    return nullptr;
    }

DiffNode* GetName(DiffNode** tokens, size_t* pos)
    {
    if (tokens[*pos]->type == COMMAND && tokens[*pos]->value.cmd >= SIN)
        {
        difCommands op = tokens[*pos]->value.cmd;
        (*pos)++;
        if (tokens[*pos]->type == COMMAND && tokens[*pos]->value.cmd == OP_BR)
            {
            free(tokens[*pos]);
            (*pos)++;
            DiffNode* node = GetE(tokens, pos);
            node = DiffCreateCmdNode(op, nullptr, node).node;
            if (tokens[*pos]->type == COMMAND && tokens[*pos]->value.cmd == CL_BR)
                {
                free(tokens[*pos]);
                (*pos)++;
                return node;
                }

            printf("noclbr %d\n", tokens[*pos]->type);
            return nullptr;
            }

        printf("noopbr\n");
        return nullptr;
        }

    return nullptr;
    }

DiffNode* GetP(DiffNode** tokens, size_t* pos)
    {
    if (tokens[*pos]->type == COMMAND && tokens[*pos]->value.cmd == OP_BR)
        {
        free(tokens[*pos]);
        (*pos)++;
        DiffNode* val = GetE(tokens, pos);
        if (tokens[*pos]->type == COMMAND && tokens[*pos]->value.cmd == CL_BR)
            {
            free(tokens[*pos]);
            (*pos)++;
            return val;
            }

        return nullptr;
        }

    else if (tokens[*pos]->type == CONST)
        {
        (*pos)++;
        return tokens[*pos - 1];
        }

    else if (tokens[*pos]->type == VAR)
        {
        (*pos)++;
        return tokens[*pos - 1];        
        }

    else 
        return GetName(tokens, pos);
    }

