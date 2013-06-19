#include "SynTree.h"

using namespace std;

int LeftBranket(const string& s, string & str_flw_lb)
{
	if (s.length() < 2 )
        return -1;
//    else if (s[1] == ')')
//    {
//        str_flw_lb = s;return 0;
//    }
    else{ str_flw_lb = s.substr(1);return 1;}

}
int RightBranket(const string& s, string & str_bf_rb, int count , vector<string>sentenceVec)
{
//    int startPos = s.find(")");
//    if (startPos == 0){
//        str_bf_rb = ")";startPos +=1;
//    }
//    else{
//        str_bf_rb = s.substr(0,startPos);
//    }
//    for(size_t i(0);i<s.length()-startPos;++i)
//    {
//        if(s[i+startPos]==')')continue;
//        else{
//            startPos = s.find(")",i+startPos);i=0;
//            str_bf_rb = s.substr(0,startPos);
//        }
//    }
    if (count >= sentenceVec.size())
        return -1;
    str_bf_rb=sentenceVec[count];
    if(s.find(str_bf_rb)==string::npos)return -1;
    int startPos=str_bf_rb.length();
    if(s[startPos]!=')')return -1;
    return s.length()-startPos;
}
int SynTree::ParseTree(const string&s, int mode)
{
    stringstream ss(s);
    string word,str_flw_lb,str_bf_rb;
    TreeNode *currentNode(&root);
    TreeNode *tmpNode = new TreeNode("",NULL);

    int emptyLB=0,check,brkNum;
    while(ss>>word)
    {
            check = LeftBranket(word,str_flw_lb);
            if(check==-1){emptyLB++;continue;}
            else break;
    }
    if(check == 0 || check == -1){cerr<<"wrong start\n";cerr<<s<<endl;exit(1);}
    tmpNode = new TreeNode(str_flw_lb,currentNode);
    currentNode->children.push_back(tmpNode);
    currentNode = tmpNode;
    while(ss>>word)
    {
        if (word[0]=='('&&word[word.size()-1]!=')'){ //check word starting with (
            check = LeftBranket(word,str_flw_lb);
            if(check==1)
            {
                tmpNode = new TreeNode(str_flw_lb,currentNode);tmpNode->Begin = mode;
                currentNode->children.push_back(tmpNode);
                currentNode = tmpNode; continue;
            }
            else if(check == 0)
                brkNum = RightBranket(word,str_bf_rb,mode,sentenceVec);
            }
        else if (word==")")continue; //the last )
        else if(word[word.length()-1]==')')brkNum = RightBranket(word,str_bf_rb,mode,sentenceVec);
        else {cerr<<"wrong right brankets\n\n";exit(1);}
        if (brkNum == -1)return -1; // 句法分析异常
        tmpNode = new TreeNode(str_bf_rb,currentNode);
        if(mode!=-1) {tmpNode->id=mode;tmpNode->Begin=mode;tmpNode->Back=mode;nodeIdMap[mode++]= tmpNode;}  //for node id map
        currentNode->children.push_back(tmpNode);currentNode->Back = mode-1;
        if (brkNum==0)continue;
        for (;brkNum>0;brkNum--)
        {

           currentNode = currentNode->parent;
           currentNode->Back = mode-1;
        }
    }
  //  PrintTree(&root);
    return 0;
}

int SynTree::ClearTree()
{
    TreeNode * currentNode = &root;
    while(currentNode->parent != NULL || !currentNode->children.empty())
    {
        while(!currentNode->children.empty())
            currentNode = currentNode->children.back();

        currentNode = currentNode->parent;
        for(size_t i(0);i<currentNode->children.size();++i)
            delete currentNode->children[i];
        vector<TreeNode *>().swap(currentNode->children);
        currentNode = currentNode->parent;
        delete currentNode->children.back();
        currentNode->children.pop_back();
    }
    return 0;
}

void SynTree::PrintTree(TreeNode *node)
{
    TreeNode *currentNode = node;

    if (currentNode->children[0]->children.empty())
        {cerr<<' '<<currentNode->children[0]->value;return;}
    for(size_t i(0);i< node->children.size();++i)
    {
        currentNode = node->children[i];
        cerr<<" ("<<currentNode->value/*<<"//"<<currentNode->Begin<<"//"<<currentNode->Back*/;
        PrintTree( currentNode); cerr<<")";
    }
}
int SynTree::GetSpanStartWithNode(TreeNode *node)
{
    TreeNode *currentNode = node;
    while(!currentNode->children.empty())currentNode=currentNode->children[0];
    return currentNode->id;
}
int SynTree::GetSpanEndWithNode(TreeNode *node)
{
    TreeNode *currentNode = node;
    while(!currentNode->children.empty())currentNode=currentNode->children.back();
    return currentNode->id;
}
TreeNode *SynTree::GetNodeWithId(int id)
{
    return nodeIdMap[id];
}


void SynTree::GetStrVec(int begin, int back,vector<string> &result)
{
    if(back>nodeIdMap.size()-1 || back == -1)back = nodeIdMap.size()-1;
    for(;begin<=back;++begin)
        result.push_back(nodeIdMap[begin]->value);
}






