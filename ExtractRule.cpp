#include "ExtractRule.h"
using namespace std;


void AlignedRel::LoadAlignedSentence(const string &s)
{
using namespace boost;
    stringstream ss(s);
    int key(0),value(0);
    size_t pos;string word;
    ss>>word;
    sizeC = lexical_cast<int>(word);
    ss>>word;
    sizeE = lexical_cast<int>(word);
    while(ss>>word)
    {
        if((pos=word.find("-"))!=string::npos)
        {
            key=lexical_cast<int>(word.substr(0,pos));
            value=lexical_cast<int>(word.substr(pos+1));
        }
        C2EMap.insert(make_pair(key,value));
        E2CMap.insert(make_pair(value,key));
    }
}
int AlignedRel::GetChSize()
{
    return C2EMap.rbegin()->first+1;
}

int AlignedRel::GetEnSize()
{
    return E2CMap.rbegin()->first+1;
}
vector<int> AlignedRel::GetCSpanGivenESpan(int start,int back)
{
    vector<int> res;
    multimap<int,int>::iterator it,it1;
    for(size_t i(start);i<=back;++i)
    {
        if(E2CMap.find(i)==E2CMap.end())continue;
        it=E2CMap.lower_bound(i);it1=E2CMap.upper_bound(i);
        while(it!=it1)
        {
            res.push_back(it->second);
            it++;
        }
    }
    sort(res.begin(),res.end());
   // copy(res.begin(),res.end(),ostream_iterator<int>(cerr," "));cerr<<endl;
    return res;
}
vector<int> AlignedRel::GetESpanGivenCSpan(int start,int back)
{
    vector<int> res;
    if (back<start){cerr<<"back less than start\n";return res;}
     multimap<int,int>::iterator it,it1;
    for(size_t i(start);i<=back;++i)
    {
        if(C2EMap.find(i)==C2EMap.end())continue;
        it=C2EMap.lower_bound(i);it1=C2EMap.upper_bound(i);
        while(it!=it1)
        {
            res.push_back(it->second);
            it++;
        }
    }
    sort(res.begin(),res.end());
   // copy(res.begin(),res.end(),ostream_iterator<int>(cerr," "));cerr<<endl;
    return res;
}
int AlignedRel::CountOfAPreceedB(vector<int> &avec,vector<int>&bvec)
{
    int countAll(0),countIter;
   // cerr<<avec.size()<<" "<<bvec.size()<<endl;
    for(size_t i(0);i<avec.size();++i)
    {
        countIter=0;

        for(size_t j(0);j<bvec.size();++j)
        {
        //    cerr<<avec[i]<<' '<<bvec[j]<<endl;
            if(avec[i]> bvec[j])continue;
            countIter = bvec.size()-j; break;
        }
        countAll += countIter;
    }
    return countAll;
}
bool AlignedRel::IsReverse(int a,int b,int c)
{
    return a*2 < b*c;
}



bool TransRule::LoadItemAndRST(const string&aligns,const string&sRST,const string&ens)
{
    //cerr<<sRST<<endl;

    rstTree.MyConstructRSTTree(sRST);
    cerr<<"rst tree has been parsed\n";
    singleTreeVec = rstTree.GetAllRSTTree();
   // cerr<<singleTreeVec.size()<<endl;
    GetEnWordVec(ens);
    align.LoadAlignedSentence(aligns);
    //if(align.GetChSize() > rstTree.wordVec.size()){cerr<<"ch in align unequal to rst vec size\n"<<align.GetChSize()<<" "<<rstTree.wordVec.size();exit(1);}
    if(align.GetSizeOfC()!=rstTree.wordVec.size()){cerr<<"ch in align unequal to rst vec size\n"<<align.GetSizeOfC()<<" "<<rstTree.wordVec.size();return false;}
    if(align.GetSizeOfE()!=enWordVec.size()){cerr<<"en in align unequal to data vec size\n"<<align.GetSizeOfE()<<" "<<enWordVec.size();return false;}
    vector<int>().swap(orderVec);
    return true;
}
std::string TransRule::GetEnStrWithChId(vector<int> &vec)
{
    string res;
    set<int> mset;
    vector<int> mvec;
    vector<int> enVec;
    for(size_t i(0);i<vec.size();++i)
    {
        enVec = align.GetESpanGivenCSpan(vec[i],vec[i]);
        mvec.insert(mvec.end(),enVec.begin(),enVec.end());
    }

    copy(mvec.begin(),mvec.end(),insert_iterator<set<int> >(mset,mset.begin()));
    set<int>::iterator it=mset.begin();

    while(it!=mset.end())
    {
    //    cerr<<(*it)<<" ";
        res += enWordVec[(*it)]+" ";it++;
    }
    return res.substr(0,res.length()-1);
}
std::string TransRule::GetStrWithId(unsigned start_word,unsigned end_word)
{
    stringstream ss;
    if((int) start_word < 0){cerr<<"wrong start-word : "<<int (start_word)<<endl;return "";}
    copy(rstTree.wordVec.begin()+start_word,rstTree.wordVec.begin()+end_word+1, ostream_iterator<string,char>(ss," "));
    string r=ss.str();
    return r.substr(0,r.length()-1);
}
int TransRule::GetSpanGiveSrc(RSTNode *node)
{
    vector<int>().swap(leftVec);vector<int>().swap(rightVec);
    if(node->leftChild==NULL || node->rightChild==NULL)return -1;
    leftVec=align.GetESpanGivenCSpan(node->leftChild->Begin,node->leftChild->Back);
    rightVec=align.GetESpanGivenCSpan(node->rightChild->Begin,node->rightChild->Back);
    if(leftVec.empty()||rightVec.empty())return -1;
    return 1;
}
int TransRule::ExtractOrder(RSTNode *node)
{
    int countLPreceed=align.CountOfAPreceedB(leftVec,rightVec);
    if(align.IsReverse(countLPreceed,leftVec.size(),rightVec.size()))
    return 1;
    else return 0;
}
int CountWordNum(const string &s)
{
    if(s=="")return 0;
    stringstream ss(s);int count=0;string word;
    while(ss>>word)count++;
    return count;
}
bool GetKeyWordId(int startId,const string&key,const string&s,vector<int> &resVec)
{
    using namespace boost;
    boost::match_results<string::const_iterator> mr;
    vector<int>().swap(resVec);
    string tmpkey = boost::replace_all_copy(key,"\\s"," ");
    boost::replace_all(tmpkey,"[X]"," (.*) ");
    stringstream ss(s);
    int count(0);
    if(tmpkey.find(" ")==string::npos)
    {
        string word;
        while(ss>>word)
        {
            resVec.push_back(count+startId);
            if (word == tmpkey){return true;}
            count++;
        }
        return false;
    }
    else
    {
        int pre(0),cur,preCount(0);string prefix;
       // boost::replace(tmpkey,"X"," (.*) ");
        tmpkey="( |^)"+tmpkey+"( |$)";
        if(!regex_search(s,mr,regex(tmpkey)))return false;
        prefix = s.substr(0,mr.position());count  = CountWordNum(prefix);
        pre=mr.position();preCount = count;
        for(int i(2);i<mr.size();++i)
        {
            prefix = s.substr(pre,mr.position(i)-pre);
            count  = preCount+CountWordNum(prefix);
            for(int i(preCount);i<count;++i)resVec.push_back(i+startId);
            pre = mr.position(i)+mr.length(i);
            preCount = count+CountWordNum(mr.str(i));
        }
        if(pre!=mr.position()+mr.length())
        {
            prefix=s.substr(pre,mr.position()+mr.length()-pre);
            count = preCount+CountWordNum(prefix);
            for(int i(preCount);i<count;++i)resVec.push_back(i+startId);
        }
        return true;
    }
}

int TransRule::UnitExtractTransPairs(RSTNode *node, string &res)
{
    vector<int>resVec;
    string key=node->keyWord;
    string s=GetStrWithId(node->Begin,node->Back);
    if(s=="")return -1;
    bool IsKeyId=GetKeyWordId(node->Begin,key,s,resVec);
    if(!IsKeyId)return -1;
    copy(resVec.begin(),resVec.end(),ostream_iterator<int>(cerr," "));
    cerr<<endl;
    res = GetEnStrWithChId(resVec);
    vector<int>().swap(resVec);
    return 1;
}
int TransRule::ExtractTransPairs(RSTNode *node,string &res)
{
    string leftStr,rightStr;res="";
    if(UnitExtractTransPairs(node->leftChild,leftStr)==-1)leftStr="NULL";
    if(UnitExtractTransPairs(node->rightChild,rightStr)==-1)rightStr="NULL";
    if(leftStr=="NULL" && rightStr=="NULL")return -1;
    res=leftStr+"/"+rightStr;
    return 1;
}
void TransRule::UpdateOrderVec()
{
    int reverse;
    for(size_t i(0);i<singleTreeVec.size();++i)
    {
        if(GetSpanGiveSrc(singleTreeVec[i])==-1){reverse=-1;continue;}
        reverse = ExtractOrder(singleTreeVec[i]);
        orderVec.push_back(reverse);
    }
}
float SumRow(ValueAndCount &row)
{
    int sum(0);
    map<string,CountStruct>::iterator it=row.begin();
    while(it!=row.end())
    {
        sum += (*it).second.count;
        it++;
    }
    return float(sum);
}

float SumNewRow(vector<VCPair> &row)
{
    int sum(0);

    for(size_t i(0);i<row.size();++i)sum += row[i].second.count;
    return float(sum);
}
void MakeStOfBiLm_1(int index,TransRule &rule,map<string,int>&biIndex,vector<vector<int> > &biLmIndexMap,\
                     vector<vector<int> >&orderIndexMap, Row &transSet)
{
    string key,fk,sk,tranStr;
    int indexMap,reverse,isExtracPair;bool IsSpanLegal;
    for (size_t i(0);i<rule.singleTreeVec.size();++i)
    {
        if(rule.singleTreeVec[i]->rightChild!=NULL)
        {
            if(rule.singleTreeVec[i]->leftChild->keyWord=="")fk="NULL";
            else fk=rule.singleTreeVec[i]->leftChild->keyWord;
            if(rule.singleTreeVec[i]->rightChild->keyWord=="")sk="NULL";
            else sk=rule.singleTreeVec[i]->rightChild->keyWord;
            if(fk=="NULL" && sk=="NULL")continue;
            key=fk+'/'+sk;
        }
        else continue;
        IsSpanLegal=(rule.GetSpanGiveSrc(rule.singleTreeVec[i])==1)?true:false;
        if(IsSpanLegal)
        {
            reverse=rule.ExtractOrder(rule.singleTreeVec[i]);
            isExtracPair=rule.ExtractTransPairs(rule.singleTreeVec[i],tranStr);

            if(biIndex.find(key)!=biIndex.end()){
                indexMap = biIndex[key];
                biLmIndexMap[indexMap].push_back(index);
//                if(reverse==1)orderIndexMap[indexMap][1]+=1;
//                else if(reverse==0)orderIndexMap[indexMap][0]+=1;

                if(isExtracPair==1)
                {
                    if(transSet[indexMap].find(tranStr)!=transSet[indexMap].end())
                    {
                         transSet[indexMap][tranStr].count += 1;
                         if(reverse==1)transSet[indexMap][tranStr].revCount+=1;
                    }
                    else
                    {
                        transSet[indexMap][tranStr].count=1;
                        if(reverse==1)transSet[indexMap][tranStr].revCount=1;
                        else transSet[indexMap][tranStr].revCount=0;
                    }
                }
            }
            else{
                //cerr<<biIndex.size()<<endl;
                biIndex.insert(make_pair(key,biIndex.size()));
                biLmIndexMap.push_back(vector<int>(1,index));
//                orderIndexMap.push_back(vector<int>(2,0));
//                if(reverse==1)orderIndexMap.back()[1]+=1;
//                else if(reverse==0)orderIndexMap.back()[0]+=1;


                if(isExtracPair==1)
                {
                    map<string,CountStruct> tmpMap;tmpMap[tranStr].count=1;
                    if(reverse==1)tmpMap[tranStr].revCount=1;
                    else tmpMap[tranStr].revCount=0;
                    transSet.push_back(tmpMap);
                }
                else transSet.push_back(map<string,CountStruct>());
            }
        }//is span legal

    }
}




