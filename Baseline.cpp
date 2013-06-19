#include"Baseline.h"
#include"Initialization.h"



using namespace std;
int BaselineClass::Divide(const char*orifile, const char*synfile)
{
    ifstream in(orifile);
    ifstream in1(synfile);
    if(!in || !in1){cerr<<"wrong to open\n";exit(1);}
    int count(1);
    string line,line1;vector<string> vec;
    vector<SynTree *>treeVec;
    while(getline(in,line) )
    {
        vector<SynTree *>().swap(treeVec);
         line=half2full(line);
         for(size_t i(0);i<11;++i)
         {
             getline(in1,line1);
             if(line1 == "")break;
             SynTree *mytree=new SynTree(line);
             line1=half2full(line1);
             if(mytree->ParseTree(line1,0)<0){cout<<line<<endl;continue;}
             if (mytree->GetSize()==0){cout<<line<<endl;continue;}
             treeVec.push_back(mytree);
         }
         cerr<<count++<<endl;
//         getline(in1,line1);
//         if(line1.compare("")>0){cerr<<"wrong in parse file\n";exit(1);}
         if(treeVec.empty()){cout<<line<<endl<<endl;continue;}

         vector<string>().swap(vec);
         vector<int> vecIndex = SegByPunc(treeVec[0]);
         if(vecIndex.size()<=2)
         {
             cout<<line<<endl<<endl;
             continue;
         }
         vec = MakeDivision(treeVec,vecIndex);
         copy(vec.begin(),vec.end(),ostream_iterator<string,char>(cout,"\n"));
         cout<<endl;
    }
}
bool IsPunctuation(const string&s)
{
    return (s=="，" || s=="。"||s=="；" ||s=="？");
}
vector<int> BaselineClass::SegByPunc(SynTree *tree)
{
    vector<int> vecRes;
    for(size_t i(0);i<tree->sentenceVec.size();++i)
    {
        if(IsPunctuation(tree->sentenceVec[i]))vecRes.push_back(i);
    }
    if(vecRes.size()>0)
    {
        if(vecRes.front()>0)vecRes.insert(vecRes.begin(),0);
        if(vecRes.back()==tree->sentenceVec.size()-1)vecRes.back()=tree->sentenceVec.size();
        else if(vecRes.back()<tree->sentenceVec.size()-1)vecRes.push_back(tree->sentenceVec.size());
    }
    return vecRes;
}
string Join(int beg, int back,vector<string>&vec)
{
    using namespace boost;
    string res;
    for(int i(beg);i<=back;++i)res += vec[i]+" ";
    return trim_copy(res);
}
vector<string> BaselineClass::MakeDivision(vector<SynTree *>&treeVec,vector<int> &vec)
{
    using namespace boost;
    vector<string> vecRes;
    vector<int> dividableVec;
    int beg(0),back;
    for(size_t i(1);i<vec.size()-1;++i)
    {
        int count(0);
        for(size_t j(0);j<treeVec.size();++j)
        {
            if(CheckDivision(beg,vec[i],vec[i+1]-1,treeVec[j])>0)
                count++;
        }
        if (count >= 5)
            dividableVec.push_back(vec[i]);
        beg = vec[i]+1;
    }
    if(dividableVec.size()>0)
    {
        if(dividableVec.front() > 0)dividableVec.insert(dividableVec.begin(),-1);
        if(dividableVec.back() < treeVec[0]->sentenceVec.size()-1)dividableVec.push_back(treeVec[0]->sentenceVec.size()-1);
    }
    else {vecRes.push_back(Join(0,treeVec[0]->sentenceVec.size()-1,treeVec[0]->sentenceVec)); return vecRes;}
    for(size_t k(1);k<dividableVec.size()-1;++k)
    {
        vecRes.push_back(Join(dividableVec[k-1]+1,dividableVec[k]-1,treeVec[0]->sentenceVec));
        vecRes.push_back(treeVec[0]->sentenceVec[dividableVec[k]]);
        back = dividableVec[k]+1;
    }
    if(back <= treeVec[0]->sentenceVec.size()-1)vecRes.push_back(Join(back,dividableVec.back(),treeVec[0]->sentenceVec));
    return vecRes;
}
int BaselineClass::CheckDivision(int spanbeg,int comma, int spanend,SynTree *tree)
{
    if (tree->sentenceVec[comma]=="；" || tree->sentenceVec[comma]==";")return 1;
    TreeNode *nodeA = tree->GetNodeWithId(spanbeg);
    TreeNode *nodeComma = tree->GetNodeWithId(comma);
    while(nodeComma->parent->children.size()==1)nodeComma=nodeComma->parent;
    TreeNode *nodeB = tree->GetNodeWithId(spanend);
    bool isDivide=true;
    while(nodeA != NULL)
    {
        if(nodeA->Back<comma ){nodeA=nodeA->parent;}
        else if(nodeA->Back >= comma){break;}
        else{isDivide=false;break;}
    }
    if(!isDivide)return -1;
    else if(nodeA != nodeComma->parent)return -1;
     while(nodeB != NULL)
    {
        if(nodeB->Begin > comma ){nodeB=nodeB->parent;}
        else if(nodeB->Begin <= comma){break;}
        else{isDivide=false;break;}
    }
    if(!isDivide)return -1;
    else if(nodeB != nodeComma->parent)return -1;
    return 1;
}
