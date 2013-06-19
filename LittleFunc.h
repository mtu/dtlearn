#ifndef LITTLEFUNC_H_INCLUDED
#define LITTLEFUNC_H_INCLUDED
#include "RSTTree.h"

class LittleFuncClass
{
public:
    vector<string> &LoadRSTFile(const char*file);
    void PrintRelAndCount()
    {
        map<int,vector<int> >::iterator it=mmap.begin();
        while(it!=mmap.end()){cout<<it->first<<"    "<<it->second.size()<<endl;
        ++it;}
        map<string,vector<int> >::iterator iter=mmap2.begin();
        while(iter!=mmap2.end()){cout<<iter->first<<"    "<<iter->second.size()<<endl;
        ++iter;}
    };
    void HierarchicalSample(int n);
    void HierarchicalSample_forConn(int n,const char*);
    float RateComputing(int n,int N){return float(n)/float(N);}
    void ComputeAccAndRecall(const char*autofile,const char*reffile);
    float FScore(float a,float r){return 2*a*r/(a+r);}
    void ComputeRSTFileDistribution();
    void ComputeLengthAvg(const char*rstfile);
    RSTNode *Convert2CompleteRSTTree(RSTNode *node);
private:
    map<int, vector<int> > mmap;
    map<string,vector<int> >mmap2;
    vector<string> vecStr;
    int MatchNodeCount(vector<RSTNode *>&autoVec,vector<RSTNode *>&refVec);
    int MatchSegCount(vector<RSTNode *>&autoVec,vector<RSTNode *>&refVec);
};
#endif // LITTLEFUNC_H_INCLUDED
