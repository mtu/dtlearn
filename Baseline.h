#ifndef BASELINE_H_INCLUDED
#define BASELINE_H_INCLUDED
#include"SynTree.h"
class BaselineClass
{
public:
  int Divide(const char*orifile, const char*synfile);

private:
    int CheckDivision(int spanbeg,int comma, int spanend,SynTree *tree);
    vector<string> MakeDivision(vector<SynTree *>&treeVec,vector<int> &vec);
    vector<int> SegByPunc(SynTree *tree);

};


#endif // BASELINE_H_INCLUDED
