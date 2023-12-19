#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "tools.hpp"
#include "difftree.hpp"
#include "diffLatexDump.hpp"
#include "differentiator.hpp"
#include "diffTreeDump.hpp"

size_t TEX_ID = 1;

error_t texAdd(DiffNode* node, FILE* filedest);
error_t texSub(DiffNode* node, FILE* filedest);
error_t texMul(DiffNode* node, FILE* filedest);
error_t texDiv(DiffNode* node, FILE* filedest);
error_t texPow(DiffNode* node, FILE* filedest);
error_t texSin(DiffNode* node, FILE* filedest);
error_t texCos(DiffNode* node, FILE* filedest);
error_t texTg(DiffNode* node, FILE* filedest);
error_t texCtg(DiffNode* node, FILE* filedest);
error_t texLn(DiffNode* node, FILE* filedest);
error_t texArcsin(DiffNode* node, FILE* filedest);
error_t texArccos(DiffNode* node, FILE* filedest);
error_t texArctg(DiffNode* node, FILE* filedest);
error_t texArcctg(DiffNode* node, FILE* filedest);

error_t _recDiffTexDump(DiffNode* node, FILE* filedest);

error_t beginEquation(FILE* filedest);
error_t endEquation(FILE* filedest);


const char* texBegin =  "\\documentclass[a4paper,12pt]{article}\n"
                        "\\usepackage{cmap}\n"
                        "\\usepackage[T2A]{fontenc}\n"
                        "\\usepackage[utf8]{inputenc}\n"
                        "\\usepackage[english, russian]{babel}\n"
                        "\\usepackage{mathtext}\n"
                        "\\usepackage{amsmath, amsfonts, amssymb, amsthm, mathtools}\n"
                        "\\usepackage{textcomp}\n"
                        "\\usepackage{euscript}\n"
                        "\\usepackage{xcolor}\n"
                        "\\usepackage{hyperref}\n"
                        "\\definecolor{urlcolor}{HTML}{799B03}\n"
                        "\\title{\\textbf{Дичь}}\n"
                        "\\date{2023}\n"
                        "\\begin{document}\n"
                        "\\maketitle\n"; 

const char* texEnd = "\\end{document}\n";

error_t TexAndDiff(DiffTree* tree)
    {
    MY_ASSERT_SOFT(tree, ERR_NULL_PTR);

    char filedestname[CMD_MAX_LEN] = "";
    sprintf(filedestname, "%s/TreeTex%zu.tex", TREE_TEX_FOLDER, TEX_ID);

    FILE* filedest = fileopenerW(filedestname);
    
    fprintf(filedest, texBegin);

    beginEquation(filedest);

    _recDiffTexDump(tree->root, filedest);

    endEquation(filedest);
    
    DiffTree newTree = {};

    newTree.root = DifferentiateTree(tree->root);
    newTree.size = &newTree.root->subtreeLen;
    
    RecountNodes(&newTree);
    
    TreeGraphicDump(&newTree);
    
    DiffOptimise(&newTree);

    TreeGraphicDump(&newTree);

    beginEquation(filedest);

    _recDiffTexDump(newTree.root, filedest);

    endEquation(filedest);

    fprintf(filedest, texEnd);

    fclose(filedest);

    char command[CMD_MAX_LEN] = "";
    printf("tex dump\n%s", command);
    sprintf(command, "pdflatex --output-directory=%s %s", TREE_PDF_FOLDER, filedestname);

    system(command);

    TEX_ID++;

    NodeDestruct(tree->root);
    tree->root = newTree.root;
    tree->size = newTree.size;

    return NO_ERROR;
    }

error_t DiffTexDump(DiffTree* tree)
    {
    MY_ASSERT_SOFT(tree, ERR_NULL_PTR);

    char filedestname[CMD_MAX_LEN] = "";
    sprintf(filedestname, "%s/TreeTex%zu.tex", TREE_TEX_FOLDER, TEX_ID);

    FILE* filedest = fileopenerW(filedestname);
    
    fprintf(filedest, texBegin);

    beginEquation(filedest);

    _recDiffTexDump(tree->root, filedest);

    endEquation(filedest);

    fprintf(filedest, texEnd);
    fclose(filedest);

    char command[CMD_MAX_LEN] = "";
    printf("tex dump\n%s", command);
    sprintf(command, "pdflatex --output-directory=%s %s", TREE_PDF_FOLDER, filedestname);

    system(command);

    TEX_ID++;
    
    return NO_ERROR;
    }


error_t _recDiffTexDump(DiffNode* node, FILE* filedest)
    {
    if (!node)
        return NO_ERROR;

    switch (node->type)
        {
        case (CONST):
            fprintf(filedest, "%lg", node->value.cnst); break;
        case (VAR):
            fprintf(filedest, "%c", node->value.var); break;
        case (COMMAND):
            switch (node->value.cmd)
                {
                case (ADD):     texAdd(node, filedest); break;
                case (SUB):     texSub(node, filedest); break;
                case (MUL):     texMul(node, filedest); break;
                case (DIV):     texDiv(node, filedest); break;
                case (POW):     texPow(node, filedest); break;
                case (SIN):     texSin(node, filedest); break;
                case (COS):     texCos(node, filedest); break;
                case (TG):      texTg(node, filedest); break;
                case (CTG):     texCtg(node, filedest); break;
                case (LN):      texLn(node, filedest); break;
                case (ARCSIN):  texArcsin(node, filedest); break;
                case (ARCCOS):  texArccos(node, filedest); break;
                case (ARCTG):   texArctg(node, filedest); break;
                case (ARCCTG):  texArcctg(node, filedest); break;
                default: fprintf(filedest, "unknown"); break;
                }
             break;
        default: fprintf(filedest, "unknown");  break;
        }

    return NO_ERROR;
    }

error_t beginEquation(FILE* filedest)
    {
    fprintf(filedest, "\n\n"
                      "\\begin{center}\n"
                      "\\begin{equation}\n");

    return NO_ERROR;
    }

error_t endEquation(FILE* filedest)
    {
    fprintf(filedest, "\n\\end{equation}\n"
                      "\\end{center}"
                      "\n\n");

    return NO_ERROR;
    }

error_t texAdd(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "(");

    _recDiffTexDump(node->left, filedest);

    fprintf(filedest, " + ");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texSub(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "(");

    _recDiffTexDump(node->left, filedest);

    fprintf(filedest, " - ");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texMul(DiffNode* node, FILE* filedest)
    {
    _recDiffTexDump(node->left, filedest);

    fprintf(filedest, " \\cdot ");

    _recDiffTexDump(node->right, filedest);

    return NO_ERROR;
    }

error_t texDiv(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, " \\frac{");

    _recDiffTexDump(node->left, filedest);

    fprintf(filedest, "}{");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, "}");

    return NO_ERROR;
    }

error_t texPow(DiffNode* node, FILE* filedest)
    {
    // fprintf(filedest, "(");

    _recDiffTexDump(node->left, filedest);

    fprintf(filedest, "^{");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, "}");
    // fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texSin(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\sin (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texCos(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\cos (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texTg(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\tan (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texCtg(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\cot (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texLn(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\ln (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texArcsin(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\arcsin (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texArccos(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\arccos (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texArctg(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\arctan (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }

error_t texArcctg(DiffNode* node, FILE* filedest)
    {
    fprintf(filedest, "\\arccot (");

    _recDiffTexDump(node->right, filedest);

    fprintf(filedest, ")");

    return NO_ERROR;
    }