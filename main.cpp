#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "tools.hpp"
#include "difftree.hpp"
#include "differentiator.hpp"
#include "diffTreeDump.hpp"
#include "recDes.hpp"
#include "diffLatexDump.hpp"

int main()
    {
    // DiffTree tree = {};
    // DiffTreeRead(&tree, "data.txt");
    // TreeGraphicDump(&tree);

    DiffTree tree = {};
    char* string = FileToBuf("data.txt");
    DiffNode* root = GetG(string);
    free(string);

    tree.root = root;
    RecountNodes(&tree);

    TreeGraphicDump(&tree);

    // TreeGraphicDump(&tree);
    
    NodeDestruct(tree.root);

    return 0;
    }