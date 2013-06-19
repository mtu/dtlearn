#ifndef EXTRACTRULE_H_INCLUDED
#define EXTRACTRULE_H_INCLUDED

#include"Initialization.h"
#include"RSTTree.h"
class AlignedRel
{
public:
    void LoadAlignedSentence(const string &s);
    int GetChSize();
    int GetEnSize();
    vector<int> GetESpanGivenCSpan(int start,int back);
    vector<int> GetCSpanGivenESpan(int start,int back);
    int CountOfAPreceedB(vector<int> &avec,vector<int>&bvec);
    bool IsReverse(int a,int b,int c);
    void Clear(){C2EMap.clear();E2CMap.clear();}
    int GetSizeOfC(){return sizeC;}
    int GetSizeOfE(){return sizeE;}
private:
    multimap<int,int> C2EMap;
    multimap<int,int> E2CMap;
    int sizeC;
    int sizeE;
};
struct CountStruct
{
  int count;
  int revCount;
};

typedef pair<string,CountStruct> VCPair;

struct UDGreater
{
  bool operator()(const VCPair &a,const VCPair&b)
  {
      return a.second.count <= b.second.count;
  }
};
typedef priority_queue<VCPair,vector<VCPair>,UDGreater> NewRow;

typedef map<string,CountStruct> ValueAndCount;
typedef vector< ValueAndCount > Row;

class TransRule
{
public:
    bool LoadItemAndRST(const string&aligns,const string&sRST,const string&ens);
    int ExtractOrder(RSTNode *node);
    void UpdateOrderVec();
    void Clear(){align.Clear();vector<RSTNode*>().swap(singleTreeVec);}
    void GetEnWordVec(const string&s){
        vector<string>().swap(enWordVec);
        stringstream ss(s);copy(istream_iterator<string,char>(ss),istream_iterator<string,char>(),back_insert_iterator<vector<string> >(enWordVec));
        ss.str("");
        }
    string GetEnStrWithChId(vector<int> &vec);
    string GetStrWithId(unsigned start_word, unsigned end_word);
    int UnitExtractTransPairs(RSTNode *node,string &res);
    int ExtractTransPairs(RSTNode *node,string &res);
    int GetSpanGiveSrc(RSTNode *);


    vector<int> orderVec;
    vector<RSTNode *> singleTreeVec;
private:
    RSTTree rstTree;

    AlignedRel align;
    vector<string>enWordVec;

    vector<int> leftVec;
    vector<int> rightVec;


};
float SumRow(ValueAndCount &row);
bool GetKeyWordId(int startId,const string&key,const string&s,vector<int> &resVec);
void MakeStOfBiLm_1(int index,TransRule &rule,map<string,int>&biIndex,vector<vector<int> > &biLmIndexMap, vector<vector<int> >&orderIndexMap,Row &transSet);
//biIndex: A/B : mapIndex ; biLmIndexMap : mapIndex : 1,3,5...(sentence numbers)
//orderIndexMap: mapIndex : not reversed, reversed
//transSet: mapIndex : a/b/count, a0/b0/count0, a1/b1/count1...
#endif // EXTRACTRULE_H_INCLUDED
