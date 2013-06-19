#ifndef POSTDECODE_H_INCLUDED
#define POSTDECODE_H_INCLUDED

#include"Initialization.h"
#include"RSTTree.h"

class PostDecodeClass
{
public:
    void LoadAndPrintRSTTree(const char *filename);
    void PrintFlatRSTTree(DataItem &item, vector<RSTNode>& nodeVec);
    void PrintFlatRSTTree_1(DataItem &item, vector<RSTNode>& nodeVec);
};


#endif // POSTDECODE_H_INCLUDED
