#include "SynTree.h"
#include"Initialization.h"
#include"FeatExtract.h"
#include "ExtractRule.h"
#include "LittleFunc.h"
using namespace std;

vector<string>& LittleFuncClass::LoadRSTFile(const char*file)
{
    ifstream in(file);
    int count(1);
    if(!in ){cerr<<"wrong to open file\n";exit(1);}
    string line,mark;int pos; string tmpstr;
     while(getline(in,line))
    {
        line=half2full(line);
        vecStr.push_back(line);
        pos=line.find(" ");
        mark=line.substr(0,pos);
        pos=line.find_first_not_of(" ",pos+1);
        line = line.substr(pos);

        RSTTree rstTree;
        rstTree.MyConstructRSTTree(line);
       // cerr<<"rst tree has been parsed\n";

        if(rstTree.RSTNodeVec[0].leftChild!=NULL)
        {
            vector<RSTNode *>vec;
            vec = rstTree.GetAllRSTTree();
            for(size_t i(0);i<vec.size();++i)
            {
                if(mmap.find(vec[i]->relationValue)==mmap.end())
                    mmap[vec[i]->relationValue]=vector<int>(1,count-1);
                else mmap[vec[i]->relationValue].push_back(count-1);
                tmpstr = vec[i]->leftChild->keyWord+"/"+vec[i]->rightChild->keyWord;
                if(mmap2.find(tmpstr)==mmap2.end())
                    mmap2[tmpstr]=vector<int>(1,count-1);
                else mmap2[tmpstr].push_back(count-1);
            }
        }
        ++count;
    }
    return vecStr;
}
void LittleFuncClass::HierarchicalSample(int n)
{
    map<int,vector<int> >::iterator it=mmap.begin();
    set<int> mset;
    while(it!=mmap.end()){
        for(int i(0);i<n && i<(*it).second.size();++i)
            mset.insert((*it).second.at(i));
        ++it;}
    set<int>::iterator iter=mset.begin();
    while(iter!=mset.end()){cout<<vecStr[(*iter++)]<<endl;}
}
void LittleFuncClass::HierarchicalSample_forConn(int n,const char*file_lm)
{
    FeatExt lmobj;
    lmobj.LoadLM(file_lm);

    map<string,vector<int> >::iterator it=mmap2.begin();
    set<int> mset;
    while(it!=mmap2.end()){
       // if(lmobj.IsinBiLM((*it).first))
            for(int i(0);i<n && i<(*it).second.size();++i)
                mset.insert((*it).second.at(i));
        ++it;}
    set<int>::iterator iter=mset.begin();
    while(iter!=mset.end()){cout<<vecStr[(*iter++)]<<endl;}
}
int LittleFuncClass::MatchNodeCount(vector<RSTNode *>&autoVec,vector<RSTNode *>&refVec)
{
   int count=0;
   for (size_t i(0);i<autoVec.size();++i)
        for(size_t j(0); j<refVec.size();++j)
        {
           // if(autoVec[i]->relationValue == refVec[j]->relationValue && autoVec[i]->Begin == refVec[j]->Begin && autoVec[i]->Back==refVec[j]->Back)
            if( autoVec[i]->leftChild->Begin == refVec[j]->leftChild->Begin && autoVec[i]->leftChild->Back==refVec[j]->leftChild->Back)
                count++;
            if( autoVec[i]->rightChild->Begin == refVec[j]->rightChild->Begin && autoVec[i]->rightChild->Back==refVec[j]->rightChild->Back)
                count++;
            if(autoVec[i]->relationValue == refVec[j]->relationValue && autoVec[i]->Begin == refVec[j]->Begin && autoVec[i]->Back==refVec[j]->Back)
                count++;
        }
    return count;
}
int LittleFuncClass::MatchSegCount(vector<RSTNode *>&autoVec,vector<RSTNode *>&refVec)
{
   int count=0;
   for (size_t i(0);i<autoVec.size();++i)
        for(size_t j(0); j<refVec.size();++j)
        {
           // if(autoVec[i]->relationValue == refVec[j]->relationValue && autoVec[i]->Begin == refVec[j]->Begin && autoVec[i]->Back==refVec[j]->Back)
            if( autoVec[i]->leftChild->Begin == refVec[j]->leftChild->Begin && autoVec[i]->leftChild->Back==refVec[j]->leftChild->Back)
                count++;
            if( autoVec[i]->rightChild->Begin == refVec[j]->rightChild->Begin && autoVec[i]->rightChild->Back==refVec[j]->rightChild->Back)
                count++;
        }
    return count;
}
void LittleFuncClass::ComputeAccAndRecall(const char*autofile,const char*reffile)
{//autofile: 自动分析结果，0和1的句子都保留，不保留编号或者0 |||, 1 |||

    ifstream in(autofile);ifstream in1(reffile);
    int count(1);
    if(!in || !in1 ){cerr<<"wrong to open file\n";exit(1);}
    string line,line1,mark;int pos; string tmpstr;
    int correctNum(0),autoNum(0),refNum(0);
    int correctSegNum(0),autoSegNum(0),refSegNum(0);
    map<int,int> m1,m2,m3,m4,m5,m6;
     while(getline(in,line)&& getline(in1,line1))
    {
       // cerr<<count<<' ';
        RSTTree rstTree;
        RSTTree rstTree1;
        rstTree.MyConstructRSTTree(line);
        rstTree1.MyConstructRSTTree(line1);

       // cerr<<"rst tree has been parsed\n";

        vector<RSTNode *>vec,vec1;
        if(rstTree.RSTNodeVec[0].leftChild!=NULL)
        {
            vec = rstTree.GetAllRSTTree();
            autoNum += 3*vec.size();
            autoSegNum += 2*vec.size();

        }
        if(rstTree1.RSTNodeVec[0].leftChild!=NULL)
        {
            vec1 = rstTree1.GetAllRSTTree();
            refNum += 3*vec1.size();
            refSegNum += 2*vec1.size();
        }
        correctNum += MatchNodeCount(vec,vec1);
        correctSegNum += MatchSegCount(vec,vec1);
      //  cerr<<"match node has been count\n";
        ++count;
    }
    float acc =RateComputing(correctNum,autoNum);
    float rec =RateComputing(correctNum,refNum);
    float accSeg=RateComputing(correctSegNum,autoSegNum);
    float recSeg = RateComputing(correctSegNum,refSegNum);
    cout<<"auto parse node: "<<autoNum<<endl;
    cout<<"ref parse node: "<<refNum<<endl;
    cout<<"correct parse node: "<<correctNum<<endl;
    cout<<"accuracy = "<<acc<<endl;
    cout<<"recall = "<<rec<<endl;
    cout<<"F-score = "<<FScore(acc,rec)<<endl<<"==========================\n";
    cout<<"auto seg node: "<<autoSegNum<<endl;
    cout<<"ref seg node: "<<refSegNum<<endl;
    cout<<"correct seg node: "<<correctSegNum<<endl;
    cout<<"accuracy = "<<accSeg<<endl;
    cout<<"recall = "<<recSeg<<endl;
    cout<<"F-score = "<<FScore(accSeg,recSeg)<<endl;

}
void LittleFuncClass::ComputeRSTFileDistribution()
{
    map<int,vector<int> >::iterator it=mmap.begin();
    while(it!=mmap.end())
    {
        cout<<"### "<<(*it).first<<" ### "<<(*it).second.size()<<endl;
        copy(it->second.begin(),it->second.end(),ostream_iterator<int>(cout,"\n"));
        ++it;
    }
}
void LittleFuncClass::ComputeLengthAvg(const char*rstfile)
{
    ifstream in(rstfile);
    int count(1);
    if(!in ){cerr<<"wrong to open file\n";exit(1);}
    string line,mark;int pos; string tmpstr;
    int lengthCount(0);
     while(getline(in,line))
    {
        line=half2full(line);
        vecStr.push_back(line);
        pos=line.find(" ");
        mark=line.substr(0,pos);
        pos=line.find_first_not_of(" ",pos+1);
        line = line.substr(pos);

        RSTTree rstTree;
        rstTree.MyConstructRSTTree(line);
       // cerr<<"rst tree has been parsed\n";

        if(rstTree.RSTNodeVec[0].leftChild!=NULL)
        {
            vector<RSTNode *>vec;
            vec = rstTree.GetAllRSTTree();
            for(size_t i(0);i<vec.size();++i)
            {



            }
        }
    }
}
RSTNode *LittleFuncClass::Convert2CompleteRSTTree(RSTNode *node)
{

}
