#ifndef FEATEXTRACT_H_INCLUDED
#define FEATEXTRACT_H_INCLUDED

#include"Initialization.h"
#include"RSTTree.h"

struct FeatUnit
{
    int relationValue;
    int firstNucleiValue;
    string firstKeyWord;
    int secondNucleiValue;
    string secondKeyWord;
  //  bool FirstBoundary;
  //  bool SecondBoundary;
    bool ShareSameParent;
    bool FirstSyntactic ;
    bool SecondSyntactic;
    int FirstSubClause;
    int SecondSubClause;
    string firstSynSymbol;
    string secondSynSymbol;
    FeatUnit(int m_rv,int m_fnv,const string &m_fkw,int m_snv,const string & m_skw):relationValue(m_rv),firstNucleiValue(m_fnv),firstKeyWord(m_fkw),\
    secondNucleiValue(m_snv),secondKeyWord(m_skw){};
};
struct SegmentationUnit
{
  string unitStr;
  int keyLabel;
 // SegmentationUnit(const string & m_s){unitStr(m_s);}
};
struct SegCharUnit
{
  int Beg;//word id not the character id
  int Back;//word id
  int Beg_Without_Conn;//word id
  int Back_Without_Conn;//word id
  string keyWordOrPhrase;
  int keyPhIndex;// phrase index
  SegCharUnit(int m_beg,int m_back,const string m_key,int kpI):Beg(m_beg),Back(m_back),keyWordOrPhrase(m_key),keyPhIndex(kpI),Beg_Without_Conn(-1),Back_Without_Conn(-1){};
  SegCharUnit():Beg_Without_Conn(-1),Back_Without_Conn(-1),Beg(-1),Back(-1),keyPhIndex(-1){};
};
struct RelAndProc
{
  int rel;
  int count;
  float proc;
};
struct Choice
{
  int pos;//1:pre, 2:suffix
  int syn_sub_0;//cur 1:syn 2:sub
  int syn_sub_1;//1:syn 2:sub
  int sub_pos_0;//cur 1:in 2:just 3:over
  int sub_pos_1;//1:in 2:just 3:over
  bool share;//true:share false:not share
  bool equal;//give the second choice;
  set<string> syn_0;//syntax symbol
  set<string> syn_1;//syntax symbol
  string keyValue;
  Choice():pos(0),syn_sub_0(0),syn_sub_1(0),sub_pos_0(0),sub_pos_1(0){};

};
struct TestRes
{
  int relationValue;
  int firstNucleiValue;
  int secondNucleiValue;
  SegCharUnit firstUnit;
  SegCharUnit secondUnit;
  TestRes():firstNucleiValue(-1),secondNucleiValue(-1){};
};

class FeatExt
{
public:

    int FeatIntial(RSTTree & );
    void GetTrainFeat_1(const string&s, SynTree *mytree,DataItem &item);//Get Conjunction words

    void PrintTrainFeatures();

    void GetTestFeat_1(const string&s, SynTree *mytree, DataItem &item, Resource &resource);//Get test features;
    void DecisionLearn(const char*file);
    void LoadLM(const char*file);
    vector<TestRes> DecideForEachSegCharUnit(const string&s,DataItem &item,Resource &resource,SynTree *mytree);
    void DecideConnForNewSegCharUnit(vector<TestRes> &resVec,DataItem &item,Resource&resource);

    //as feature results
    vector<FeatUnit> featItems;
    vector<FeatUnit> testFeatItems;
    void Clear(){vector<FeatUnit>().swap(featItems);vector<RSTNode *>().swap(relationNodeVec);}
    //make statistics
    bool IsinBiLM(const string&s){return biLm.find(s)!=biLm.end();}

private:
    RSTTree *treeForLine;
    vector<RSTNode*> relationNodeVec;
    multimap<string,RelAndProc>uniLm;
    multimap<string,RelAndProc>biLm;
    vector<Choice> decisionVec;
    vector<string> subSentenceVec;

private:
    void UnitSegmentation(const string&s,DataItem &item,Resource &resource,vector<vector<SegCharUnit> >&segCharUnitMatrix);
    bool GetPreSyntacticStr(SynTree *mytree,SegCharUnit &unit,int subSenBeg,Choice &choiceUnit,vector<int>&result);
    bool GetSufSyntacticStr(SynTree *mytree,SegCharUnit &unit,int subSenBeg,Choice &choiceUnit,vector<int>&result);

    bool GetCurSyntaticStr(SynTree *mytree, SegCharUnit &unit,Choice &choiceUnit,vector<int> &result);
    void GetCurSyntacticStr_withConn(SynTree *mytree,SegCharUnit &unit,vector<int> &result);
    void Deal_Sub_Over_Condition(vector<TestRes> &resVec,vector<size_t> &bufferSatelliteVec,int wordLen);
    vector<TestRes> ReorderSegUnit(vector<TestRes> &resVec,vector<vector<int> >&);
    void AddConn(vector<TestRes> &resVec,DataItem&item,Resource&resource,vector<RSTNode *>&nodeVec);
    void ReConstructed(vector<RSTNode*>&nodeVec);
    int AddConn_1(vector<TestRes> &resVec,DataItem&item,Resource&resource,vector<RSTNode *>&nodeVec);
    int JudgeBasicConn(vector<string>&avec,vector<string>&bvec,RSTNode*p,int mode);
};

void MakeStOfBiLm(int index,vector<RSTNode*> &nodeVec,map<string,vector<int> > &biLmIndexMap);







#endif // FEATEXTRACT_H_INCLUDED
