#include<iostream>
#include<algorithm>
#include<vector>
#include<set>
#include<map>
#include<iterator>
#include<fstream>
#include<sstream>
#include<boost/regex.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/lexical_cast.hpp>


using namespace std;


struct TreeNode
{
  string value;
  TreeNode *parent;
  int id;
  int Begin;
  int Back;
  vector<TreeNode*> children;
  TreeNode(const string&s, TreeNode* m_parent):value(s),parent(m_parent),id(0){};
};

class SynTree
{
public:
    TreeNode root;
  
        SynTree(const string&ori_s):root("",NULL){
        stringstream ss(ori_s);
        copy(istream_iterator<string,char>(ss),istream_iterator<string,char>(),back_insert_iterator<vector<string> >(sentenceVec));
        ss.str("");
    }
        SynTree():root("",NULL){};
    int ParseTree(const string& s, int mode=0);
    int ClearTree();
    void PrintTree(TreeNode *node);
    TreeNode* GetNodeWithId(int id);
    int GetSpanStartWithNode(TreeNode *node);
    int GetSpanEndWithNode(TreeNode *node);
    void GetStrVec(int begin, int back,vector<string>&result);
    int GetSize(){return nodeIdMap.size();}
    vector<string> sentenceVec;

private:
    map<int, TreeNode*> nodeIdMap;

};

/*******************************************************************************/
int LeftBranket(const string& s, string & str_flw_lb)
{
	if (s.length() < 2 )
        return -1;
    else{ str_flw_lb = s.substr(1);return 1;}

}
int RightBranket(const string& s, string & str_bf_rb, int count)
{
        int startPos = s.find(")");
        if (startPos == 0){
        startPos += 1;str_bf_rb=")";
        }
        else{
            str_bf_rb = s.substr(0,startPos);
        }
        for(size_t i(0);i<s.length()-startPos;++i)
        {
            if(s[i+startPos]==')')continue;
            else{
                startPos = s.find(")",i+startPos);i=0;
                str_bf_rb = s.substr(0,startPos);
            }
        }
    
   
    return s.length()-startPos;
}
int NONEfunction(TreeNode *node)
{
    while(node!=NULL)
        if(node->parent!=NULL)
            if(node->parent->children.size() ==1)
            {
                node = node->parent;
                delete node->children.back();
            }
            else node=node->parent;
    return 0;
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
                brkNum = RightBranket(word,str_bf_rb,mode);
            }
        else if (word==")")continue; //the last )
        else if(word[word.length()-1]==')')brkNum = RightBranket(word,str_bf_rb,mode);
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
    
    //PrintTree(&root,0);

   // cerr<<"parsing has been done"<<endl;
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
void OverTree(TreeNode *node, bool &backTrace)
{
    TreeNode *currentNode = node;
    if(currentNode->value=="-NONE-"){backTrace=true;return;}
    size_t pos=currentNode->value.find("-");
    if(pos!=string::npos && pos != 0)
        currentNode->value = currentNode->value.substr(0,pos);
    if (currentNode->children[0]->children.empty())
        {return;}


    for(size_t i(0);i< node->children.size();)
    {
       // cerr<<node->children[i]->value<<endl;
        currentNode = node->children[i];
        OverTree(currentNode,backTrace);
        if(backTrace)
        {
            delete currentNode;
            if(node->children.size()==1)
            {    
                node->children.clear();
                return;
            }
            else
            {
                node->children.erase(node->children.begin()+i);
                backTrace=false;
            }

        }
        else
            ++i;
    }

}
void SynTree::PrintTree(TreeNode *node)
{
    TreeNode *currentNode = node;
    if (currentNode->children[0]->children.empty())
        {cout<<' '<<currentNode->children[0]->value;return;}
    for(size_t i(0);i< node->children.size();++i)
    {
        currentNode = node->children[i];
       // cout<<'\n';
        cout<<" ("<<currentNode->value/*<<"//"<<currentNode->Begin<<"//"<<currentNode->Back*/;
        PrintTree( currentNode); cout<<")";
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
vector<string> Readfid(const char*file)
{
    using namespace boost;
    ifstream in(file);
    string line,result; bool start=false;
    boost::match_results<string::const_iterator> mr;
    vector<string> vec;
    while(getline(in,line))
    {
        //cerr<<line<<endl;
        if(starts_with(line,"<"))
            continue;
        else if(starts_with(line,"("))
        {
            if(result.compare("")!=0)
                vec.push_back(result);
            result=line;continue;
        }
        else if(starts_with(line," ") || starts_with(line,"\t"))
        {
            result=result+" "+line;
        }
        else
        {
            cerr<<"wrong\n";exit(1);
        }
        //vec.push_back(line);
    }
    vec.push_back(result);
    return vec;
}

int main(int argc,char *argv[])
{
    using namespace boost;
   /* string s="( (FRAG (NR 新华社) (NR 上海) (NT 12月) (NT 13日) (NN 电) (NR () (NN 记者) (NR 刘丹)) (NT 13日) (NP (CP (IP (VP (PP (P 在) (NP (NR 上海))) (VP (VV 闭幕)))) (DEC 的)) (NP (NR \")) (NP (NP (NR 中国) (NN 青少年) (NN 社会) (NN 教育) (NN 论坛)) (PRN (PU ──) (NP (QP (CD 2004・)) (NP (NN 媒体) (CC 与) (NN 未成年人)))) (NP (NN 发展))) (NP (NN \"))) (NN 倡议) (PU :) (NP (NR 中国)) (VP (VP (VV 应) (VP (VV 建立) (NP (DNP (PP (P 有关) (NP (NN 未成年人) (NN 状况))) (DEG 的)) (NP (NN 新闻) (NN 发言人)) (NP (NN 制度))))) (PU ，) (VP (ADVP (AD 定期)) (PP (P 就) (NP (NP (NN 未成年人)) (CP (IP (VP (VV 发展) (NP (NN 现状) (CC 和) (NN 存在)))) (DEC 的)) (NP (NN 问题)))) (VP (VV 发布) (NP (NN 消息)))) (PU 。))) )";

    SynTree *mytree=new SynTree;
    mytree->ParseTree(s,0);*/
    if(argc!=2){cerr<<argv[0]<<"<synfile> \n";exit(1);}
   
   vector<string> vec = Readfid(argv[1]);
   cerr<<argv[1]<<" size: "<<vec.size()<<endl;
   int count=1;
   bool backtrace;
   
  /* for(size_t i(0);i<vec.size();++i)
   {
    cout<<vec[i]<<endl;
   }
   return 0;*/
    for(size_t i(0);i<vec.size();++i)
    {
        backtrace =false;
        //cerr<<count++<<":";
        while(vec[i].find(" )")!=string::npos)
            {replace_all(vec[i]," )",")");}
       
        trim(vec[i]);
        if(vec[i].substr(vec[i].length()-2,1)==")")
            vec[i]=vec[i].substr(0,vec[i].length()-1)+" )";
        //cerr<<vec[i]<<endl;
        SynTree *mytree=new SynTree();
        mytree->ParseTree(vec[i],0);
        OverTree(&mytree->root,backtrace);
        //cerr<<"over tree has been done\n";
        mytree->PrintTree(&mytree->root);
        cout<<endl;
    }
    return 0;
}



