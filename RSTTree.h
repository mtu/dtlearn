#ifndef RSTTREE_H_INCLUDED
#define RSTTREE_H_INCLUDED
#include "Common.h"
#include "Initialization.h"

using namespace std;
struct RSTNode
{
    int relationValue;
    int Begin;
    int Back;
    int nucleiValue;
    std::string keyWord;
    int keyWordId;
    RSTNode *parent;
    RSTNode *leftChild;
    RSTNode *rightChild;
    RSTNode(RSTNode *m_parent):relationValue(-1),Begin(-1),Back(-1),nucleiValue(-1),keyWord(""),keyWordId(-1),parent(m_parent),leftChild(NULL),rightChild(NULL){}
};
void ConstructRSTTree(const string&s, vector<string>& sentenceVec, vector<RSTNode>& result);
void ConstructRSTTree_1(const string&s, vector<string>& sentenceVec, vector<RSTNode>& result);
void PrintRSTTree(DataItem &item, vector<RSTNode>& nodeVec);

class RSTTree
{
public:
    void MyConstructRSTTree(const string&s){
        ConstructRSTTree_1(s, wordVec, RSTNodeVec);}
    vector<RSTNode *> GetAllRSTTree();
    vector<string> wordVec;
    vector<int> wordIdForSubSen;
    void RSTTreeClear();
    vector<RSTNode> RSTNodeVec;
private:


    vector<RSTNode *> relationVec;
    void UnitRSTTree(RSTNode *node);
public:


};
extern RSTRelation relationMap;

#endif // RSTTREE_H_INCLUDED
