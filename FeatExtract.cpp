
#include"FeatExtract.h"

/*==============================================================================================*/
//void GetFeat_1(const string&s,vector<int>&keyWordIdVec)

int FeatExt::FeatIntial(RSTTree &rstTree)
{
    treeForLine = &rstTree;
    relationNodeVec=treeForLine->GetAllRSTTree();

    return 1;
}

TreeNode *DirectParent(SynTree *mytree, RSTNode *node)
{
   // cerr<<"enter DirectParent\n";
    int beg(node->Begin),back(node->Back);
 //   cerr<<beg<<" / "<<back<<endl;
    while (mytree->GetNodeWithId(beg)->value == "，")beg += 1;
    while(mytree->GetNodeWithId(back)->value=="，" || mytree->GetNodeWithId(back)->value=="。")back -=1;
    if(beg > back){cerr<<"wrong in ShareParent\n";exit(1);}
//    cerr<<beg<<" // "<<back<<endl;
    TreeNode *leftP = mytree->GetNodeWithId(beg)->parent;
//    cerr<<leftP->value<<" "<<leftP->Begin<<' '<<leftP->Back<<" "\
//        <<leftP->parent->value<<" "<<leftP->parent->Begin<<" "<<leftP->parent->Back<<endl;

    while(leftP->Back < back){leftP = leftP->parent;
//    cerr<<leftP->value<<" "<<leftP->Begin<<" "<<leftP->Back<<endl;
    }
//    cerr<<leftP->value<<" "<<leftP->Begin<<' '<<leftP->Back<<endl;

    return leftP;
}
TreeNode *ZeroParent(TreeNode *leftP)
{
    while(leftP->parent!=NULL && leftP->parent->children.size()==1)leftP = leftP->parent;
    return leftP;
}
bool IsSyntactic(SynTree *mytree, int syn_a, int rst_a, int syn_b,int rst_b)
{
    //cerr<<"enter IsSyntactic\n";
    //cerr<<syn_a<<' '<<rst_a<<" "<<syn_b<<' '<<rst_b<<endl;
    if(syn_a == rst_a)
    {
        if(syn_b == rst_b)return true;
        else if(syn_b == rst_b +1 && (mytree->GetNodeWithId(syn_b)->value =="，"|| mytree->GetNodeWithId(syn_b)->value =="。"))return true;
        else if(syn_b == rst_b -1 && (mytree->GetNodeWithId(rst_b)->value=="，" ||mytree->GetNodeWithId(rst_b)->value=="。"))
            return true;
        else return false;
    }
    else return false;
}
bool ShareParent_with_IsSyntactic(SynTree *mytree,DataItem &item, RSTNode *node,FeatUnit &unit)
{
    //cerr<<"enter ShareParent_with_IsSyntactic\n";
    TreeNode *P0 = DirectParent(mytree,node->leftChild);
    TreeNode *P = ZeroParent(P0);

    TreeNode *PP = P->parent==NULL?P:P->parent;
    //cerr<<P->value<<" \\ "<<P->parent->value<<endl;
    unit.FirstSyntactic = IsSyntactic(mytree,P->Begin,node->leftChild->Begin,P->Back,node->leftChild->Back);
    if(unit.FirstSyntactic){unit.firstSynSymbol=P0->value;}else{unit.firstSynSymbol="N/A";}
    //cerr<<"out of firstsyntatic\n";
    if(node->rightChild == NULL){unit.ShareSameParent = true;unit.SecondSyntactic=true;unit.secondSynSymbol="N/A";}
    else{
        int beg = node->rightChild->Begin; int back = node->rightChild->Back;
        while(mytree->GetNodeWithId(back)->value=="，" || mytree->GetNodeWithId(back)->value=="。")back -=1;
        if(beg > back){cerr<<"wrong in ShareParent\n";exit(1);}
        if(PP->Back >= back)unit.ShareSameParent=true;
        else unit.ShareSameParent=false;

    P0 = DirectParent(mytree,node->rightChild);
    unit.SecondSyntactic = IsSyntactic(mytree,P0->Begin,beg,P0->Back,back);
    if(unit.SecondSyntactic){unit.secondSynSymbol=P0->value;}else{unit.secondSynSymbol="N/A";}
    }
    return true;
}
int IsSubClause(RSTNode *node,DataItem &item)
{
   // cerr<<"enter IsSubClause\n";
   // cerr<<node->Begin<<" "<<node->Back<<" || "<<node->keyWord<<endl;
    if(item.sepVec.size()==0)
    {
        if(node->Begin==0 &&  node->Back== (item.sentenceVec.size()-1))return 1;
        else if(node->Begin>=0 &&  node->Back<= (item.sentenceVec.size()-1))return 0;
       // else if(node->Begin==0 &&  node->Back>= (int (item.sepVec[0])))return 2;
       else return 2;
    }
    if(node->Begin==0 &&  node->Back== (int (item.sepVec[0])))return 1;
    else if(node->Begin>=0 &&  node->Back<= (int (item.sepVec[0])))return 0;
    else if(node->Begin==0 &&  node->Back>= (int (item.sepVec[0])))return 2;
    else if(node->Back==item.sentenceVec.size()-1 && node->Begin == (int (item.sepVec.back()+1)))return 1;
    else if(node->Back<=item.sentenceVec.size()-1 && node->Begin >= (int (item.sepVec.back()+1)))return 0;
    else if(node->Back==item.sentenceVec.size()-1 && node->Begin < (int (item.sepVec.back()+1)))return 2;
    else if(node->Begin >0 && node->Back < item.sentenceVec.size()-1){
        for(size_t i(0);i<item.sepVec.size()-1;++i)
            {if(node->Begin==(int (item.sepVec[i]+1)) && node->Back==(int (item.sepVec[i+1])))
                return 1;
            if(node->Begin <= (int (item.sepVec[i]+1)) && node->Back >=(int (item.sepVec[i+1])))
                return 2;
            if(node->Begin >= (int (item.sepVec[i]+1)) && node->Back <=(int (item.sepVec[i+1])))
                return 0;}
            //else cerr<<node->Begin<<" "<<node->Back<<" || "<<item.sepVec[i]+1<<" "<<item.sepVec[i+1]<<endl;
    }
  //  cerr<<node->Begin<<" "<<node->Back<<" || "<<node->keyWord<<endl;
    return -1;
}

void FeatExt::GetTrainFeat_1(const string&s, SynTree *mytree,DataItem &item)
{
    for(size_t i(0);i<relationNodeVec.size();++i)
    {
        // cerr<<relationNodeVec[i]->Begin<<" rnv "<<relationNodeVec[i]->Back<<endl;
         if(relationNodeVec[i]->rightChild==NULL){
            FeatUnit itemFeat(relationNodeVec[i]->relationValue,relationNodeVec[i]->leftChild->nucleiValue \
                              ,relationNodeVec[i]->leftChild->keyWord,-1,"NULL");
            itemFeat.ShareSameParent=true;itemFeat.SecondSyntactic=true;
            itemFeat.SecondSubClause=-2;
            itemFeat.FirstSyntactic=true;itemFeat.FirstSubClause = IsSubClause(relationNodeVec[i]->leftChild,item);
            featItems.push_back(itemFeat);
         }
         else{
         FeatUnit itemFeat(relationNodeVec[i]->relationValue,relationNodeVec[i]->leftChild->nucleiValue \
                           ,relationNodeVec[i]->leftChild->keyWord,relationNodeVec[i]->rightChild->nucleiValue \
                           ,relationNodeVec[i]->rightChild->keyWord);
         ShareParent_with_IsSyntactic(mytree,item,relationNodeVec[i],itemFeat);
         itemFeat.FirstSubClause = IsSubClause(relationNodeVec[i]->leftChild,item);
         itemFeat.SecondSubClause =IsSubClause(relationNodeVec[i]->rightChild,item);featItems.push_back(itemFeat);
         }
    }
    //copy(item.sepVec.begin(),item.sepVec.end(),ostream_iterator<unsigned,char>(cout, " "));cout<<endl;
}
void MakeStOfBiLm(int index,vector<RSTNode *>&nodeVec,map<string,vector<int> > &biLmIndexMap)
{
    string key,fk,sk;
    for (size_t i(0);i<nodeVec.size();++i)
    {
        if(nodeVec[i]->rightChild!=NULL)
        {
            if(nodeVec[i]->leftChild->keyWord=="")fk="NULL";
            else fk=nodeVec[i]->leftChild->keyWord;
            if(nodeVec[i]->rightChild->keyWord=="")sk="NULL";
            else sk=nodeVec[i]->rightChild->keyWord;
            if(fk=="NULL" && sk=="NULL")continue;
            key=fk+'/'+sk;
        }
        else continue;
        if(biLmIndexMap.find(key)!=biLmIndexMap.end())biLmIndexMap[key].push_back(index);
        else{biLmIndexMap.insert(make_pair(key,vector<int>(1,index)));}
    }
}
void FeatExt::PrintTrainFeatures()
{
    vector<int> binaryVec(5,0);
    for (size_t i(0);i<featItems.size();++i)
    {
        binaryVec[0]=featItems[i].ShareSameParent?1:0;
        binaryVec[1]=featItems[i].FirstSyntactic?1:0;
        binaryVec[2]=featItems[i].FirstSubClause;
        binaryVec[3]=featItems[i].SecondSyntactic?1:0;
        binaryVec[4]=featItems[i].SecondSubClause;
        cout<<featItems[i].relationValue<<' '<<featItems[i].firstNucleiValue<<' ' \
        <<featItems[i].secondNucleiValue<<' ' \
        <<featItems[i].firstKeyWord<<' '\
        <<featItems[i].secondKeyWord<<' ' ;
        copy(binaryVec.begin(),binaryVec.end(),ostream_iterator<int,char>(cout," "));
        cout<<" ";
        cout<<featItems[i].firstSynSymbol<<' ' \
            <<featItems[i].secondSynSymbol;
        cout<<endl;
    }
}
bool PosLess(SegCharUnit a,SegCharUnit b)
{
    if( a.Beg < b.Beg)return true;
    else if(a.Beg==b.Beg && a.Back > b.Beg)return true;
    else return false;
}

void FeatExt::UnitSegmentation(const string&s,DataItem &item,Resource &resource,vector<vector<SegCharUnit> >&segCharUnitMatrix)
{
    boost::match_results<string::const_iterator> mr;
    vector<string>().swap(subSentenceVec);
    item.GetSubStrVec(subSentenceVec);
    vector<SegCharUnit> segCharUnitVec;

    //cerr<<s<<endl;
   // copy(subSentenceVec.begin(),subSentenceVec.end(),ostream_iterator<string,char>(cout,"\n"));

    segCharUnitMatrix.resize(subSentenceVec.size(),segCharUnitVec );
    int count(0),begpos(0);string word;
    int sSL(0);
    for(size_t i(0);i<subSentenceVec.size();++i)
    {
        for (size_t j(0);j<resource.keyPhraseVec.size();++j)
        {
            if(regex_search(subSentenceVec[i],mr,regex(resource.keyPhraseVec[j])))
            {
                //cerr<<subSentenceVec[i]<<endl;
                //cerr<<resource.keyPhraseVec[j]<<endl;
                if(item.start2WordId.find(sSL+mr.position(2))==item.start2WordId.end()){cerr<<"wrong in finding start position\n";exit(1);}
                if(item.end2WordId.find(sSL+mr.position(2)+mr.length(2))==item.end2WordId.end()){cerr<<mr.position(2)<<' '<<mr.length(2)<<endl;cerr<<"wrong in finding end position\n";exit(1);}
                SegCharUnit unit(item.start2WordId[sSL+mr.position(2)],item.end2WordId[sSL+mr.position(2)+mr.length(2)],resource.oriKeyPhrase[j],j);

                if(mr.size()==5){
                    if(item.start2WordId.find(sSL+mr.position(3))==item.start2WordId.end()){cerr<<"wrong in finding start position\n";exit(1);}
                    if(item.end2WordId.find(sSL+mr.position(3)+mr.length(3))==item.end2WordId.end()){cerr<<"wrong in finding end position\n";exit(1);}
                    unit.Beg_Without_Conn=item.start2WordId[sSL+mr.position(3)];unit.Back_Without_Conn=item.end2WordId[sSL+mr.position(3)+mr.length(3)];
                    }
                segCharUnitVec.push_back(unit);
            }
        }
        sSL += subSentenceVec[i].length();
        stringstream ss(subSentenceVec[i]);

        //cerr<<subSentenceVec[i]<<endl;
        //copy(istream_iterator<string,char>(ss),istream_iterator<string,char>(),ostream_iterator<string,char>(cout," "));
        begpos=count;
        while(ss>>word)
        {
           // cerr<<word<<endl;

            if(resource.keyWordSet.find(word)!=resource.keyWordSet.end())
            {
                SegCharUnit unit(count,count,word,-1);
                segCharUnitVec.push_back(unit);
            }
            count++;
        }
        sort(segCharUnitVec.begin(),segCharUnitVec.end(),PosLess);

        for(size_t j(0);j<segCharUnitVec.size();++j)
        {
            if(segCharUnitVec[j].Beg>begpos){segCharUnitVec.insert(segCharUnitVec.begin()+j,SegCharUnit(begpos,segCharUnitVec[j].Beg-1,"NULL",-1));++j;}
            begpos=segCharUnitVec[j].Back+1;
            if(begpos > item.sentenceVec.size()-1)continue;
            if((item.sentenceVec[begpos]=="，" ||item.sentenceVec[begpos]=="。"))
            {segCharUnitVec[j].Back=begpos;begpos+=1;}
        }
        if(begpos < count)segCharUnitVec.push_back(SegCharUnit(begpos,count-1,"NULL",-1));

        segCharUnitMatrix[i]=segCharUnitVec;
        vector<SegCharUnit>().swap(segCharUnitVec);
    }
//    for(size_t i(0);i<segCharUnitMatrix.size();++i)
//    {
//        for(size_t j(0);j<segCharUnitMatrix[i].size();++j)
//        {
//            cerr<<segCharUnitMatrix[i][j].Beg<<' '<<segCharUnitMatrix[i][j].Back<<' '<<segCharUnitMatrix[i][j].keyPhIndex<<" "<<segCharUnitMatrix[i][j].keyWordOrPhrase<<endl;
//        }
//    }
}

int ChooseInSentence(vector<vector<SegCharUnit> >&segUnitMatrix)
{
    for(size_t i(0);i<segUnitMatrix.size();++i)
    {

    }
}
void FeatExt::GetTestFeat_1(const string&s, SynTree *mytree,DataItem &item, Resource &resource)
{

}
int SubDecision(const string&s)
{
    if(s.rfind("in")!=string::npos)return 1;
    else if(s.rfind("over")!=string::npos)return 3;
    else return 2;
}
void FeatExt::DecisionLearn(const char*filename)
{
    ifstream in(filename);
    string line,keyword,tmp;
    size_t pos;
    while(getline(in,line))
    {
        stringstream ss(line);
        Choice decisionUnit;
        ss>>decisionUnit.keyValue;
        ss>>tmp;pos= tmp.find("/");
        decisionUnit.pos=tmp.substr(0,pos)=="pre"?1:2;
        decisionUnit.syn_sub_1=(tmp.rfind("sub")!=string::npos)?2:1;
        if(tmp.find("eq")!=string::npos)decisionUnit.equal=true;
        else decisionUnit.equal=false;
        if(decisionUnit.syn_sub_1 == 2)
            decisionUnit.sub_pos_1 = SubDecision(tmp.substr(pos));
        else{
            while(ss>>tmp)
                if (tmp.compare("//"))
                    decisionUnit.syn_1.insert(tmp);
                else break;
        }
        ss>>tmp;
        decisionUnit.syn_sub_0=(tmp.rfind("sub")!=string::npos)?2:1;
        if(decisionUnit.syn_sub_0 == 2)
            decisionUnit.sub_pos_0 = SubDecision(tmp);
        else {
            while(ss>>tmp)
                if (tmp.compare("//"))
                    decisionUnit.syn_0.insert(tmp);
                else break;
        }
        ss>>tmp;if(tmp=="share")decisionUnit.share=true;else decisionUnit.share=false;
        decisionVec.push_back(decisionUnit);
    }
}
void FeatExt::LoadLM(const char*filename)
{
using namespace boost;
    ifstream in(filename);
    string line,keyword,tmp;
    multimap<string,RelAndProc> *tmpMap;
    multimap<string,RelAndProc>::iterator it;
    size_t pos,pos1;
    while(getline(in,line))
    {
        if(starts_with(line,"###unigram")){getline(in,line);tmpMap = &uniLm;}
        else if(starts_with(line,"###bigram")){getline(in,line);tmpMap = &biLm;}
        else{
            stringstream ss(line);
            ss>>keyword;
            while(ss>>tmp)
            {
                RelAndProc value;pos = tmp.find("/");
                value.rel = lexical_cast<int>(tmp.substr(0,pos));
                pos1= tmp.find("/",pos+1);
                value.proc=lexical_cast<float>(tmp.substr(pos+1,pos1-pos-1));
                value.count=lexical_cast<int>(tmp.substr(pos1+1));
                tmpMap->insert(make_pair(keyword,value));
            }
        }
    }
    //cerr<<uniLm.size()<<' '<<biLm.size()<<endl;
}
TreeNode *DirectParent_1(SynTree *mytree, int beg, int back)
{
    while (mytree->GetNodeWithId(beg)->value == "，")beg += 1;
    while(mytree->GetNodeWithId(back)->value=="，" || mytree->GetNodeWithId(back)->value=="。")back -=1;
    if(beg > back){cerr<<"wrong in ShareParent\n";exit(1);}

    TreeNode *leftP = mytree->GetNodeWithId(beg);
    while(leftP->Back < back)leftP = leftP->parent;
   // cerr<<leftP->value<<" "<<leftP->Begin<<leftP->Back<<endl;
    return leftP;
}
bool FeatExt::GetPreSyntacticStr(SynTree *mytree,SegCharUnit &unit,int subSenBeg,Choice &choiceUnit,vector<int>&result)
{
  //  cerr<<"GetPreSyn: "<<unit.Beg<<' '<<unit.Back<<endl;
    result.resize(2,-1);
    if(unit.Beg == 0)return false;
    if(choiceUnit.share)
    {
        TreeNode *paNode = DirectParent_1(mytree,unit.Beg,unit.Back);
  //      cerr<<paNode->Begin<<' '<<paNode->Back<<' '<<paNode->value<<endl;
        //paNode = ZeroParent(paNode);
        while(paNode->parent != &mytree->root && paNode->parent->Begin==unit.Beg )paNode=paNode->parent;
        vector<TreeNode *>::reverse_iterator it = paNode->children.rbegin();
  //      cerr<<paNode->children.size()<<' '<<paNode->value<<endl;
        while(it!=paNode->children.rend())
        {
   //         cerr<<"GetPreSyn: "<<(*it)->Begin<<' '<<(*it)->Back<<endl;
            if((*it)->Begin < subSenBeg)return false;//若是在句中则考虑句子的边界，若是在句首则另subsenbeg为前一句的句首位置
            if((*it)->Begin < unit.Beg && ((*it)->value =="PU"))
            {
                ++it;continue;
            }
            else if((*it)->Begin < unit.Beg && (mytree->GetNodeWithId((*it)->Begin)->value =="，" || (mytree->GetNodeWithId((*it)->Begin)->value =="。")))
            {result[0]=(*it)->Begin+1;result[1]=unit.Beg-1;return true;}
            else if((*it)->Begin < unit.Beg )
            {result[0]=(*it)->Begin;result[1]=unit.Beg-1;return true;}
            ++it;
        }
    }
    else
    {
        TreeNode *preNode=mytree->GetNodeWithId(unit.Beg-1);
        if(preNode != &mytree->root)
            while(choiceUnit.syn_1.find(preNode->value)==choiceUnit.syn_1.end() && preNode->parent->Back < unit.Beg&&preNode->parent != &mytree->root)preNode=preNode->parent;
        if(choiceUnit.syn_1.find(preNode->value)!=choiceUnit.syn_1.end()){result[0]=preNode->Begin;result[1]=preNode->Back;return true;}
        else {return false;}
    }
    return false;
}
bool FeatExt::GetSufSyntacticStr(SynTree *mytree,SegCharUnit &unit,int subSenBack,Choice &choiceUnit,vector<int>&result)
{
    result.resize(2,-1);
    //cerr<<"GetSufSyn: "<<unit.Beg<<' '<<unit.Back<<endl;
    if(unit.Back >= mytree->root.Back-1)return false;
    if(choiceUnit.share)
    {
        TreeNode *paNode = DirectParent_1(mytree,unit.Beg,unit.Back);
        paNode = ZeroParent(paNode);

        if(paNode != &mytree->root)
            while(paNode->parent != &mytree->root && paNode->parent->Back==unit.Back)paNode=paNode->parent;
        else return false;

        vector<TreeNode *>::iterator it = paNode->children.begin();
        while(it!=paNode->children.end())
        {
           // cerr<<"GetSufSyn: "<<(*it)->Begin<<' '<<(*it)->Back<<endl;
            if((*it)->Back > subSenBack)return false;//若是在句中则考虑句子的边界，若是在句首则另subsenbeg为前一句的句首位置
            if((*it)->Back+1 < mytree->GetSize())
                if((*it)->Back > unit.Back &&(mytree->GetNodeWithId((*it)->Back+1)->value =="，" || (mytree->GetNodeWithId((*it)->Back+1)->value =="。") ))
                    {result[0]=unit.Back+1;result[1]=(*it)->Back+1;return true;}
            if((*it)->Back > unit.Back &&( (*it)->value.compare("，")||(*it)->value.compare("。")))
            {result[0]=unit.Back+1;result[1]=(*it)->Back;return true;}
            ++it;
        }
    }
    else
    {
        TreeNode *sufNode=mytree->GetNodeWithId(unit.Back+1);
        if(sufNode != &mytree->root)
            while(choiceUnit.syn_0.find(sufNode->value)==choiceUnit.syn_0.end() && sufNode->parent->Begin > unit.Back &&sufNode->parent != &mytree->root )sufNode=sufNode->parent;
        else return false;
        if(choiceUnit.syn_0.find(sufNode->value)!=choiceUnit.syn_0.end())
        {
            result[0]=sufNode->Begin;
            if(mytree->GetSize() > sufNode->Back+1)
            {
                TreeNode *curNode=mytree->GetNodeWithId(sufNode->Back+1);
                if(curNode->value=="，"||curNode->value=="。")
                    result[1]=sufNode->Back+1;return true;
            }
            result[1]=sufNode->Back;return true;
        }
        else {return false;}
    }
    return false;
}

bool FeatExt::GetCurSyntaticStr(SynTree *mytree, SegCharUnit &unit,Choice &choiceUnit,vector<int> &result)
{

    result.resize(2,-1);result[0]=unit.Beg;result[1]=unit.Back;
//    cerr<<unit.Beg<<" "<<unit.Back<<endl;
    if(unit.Beg_Without_Conn != -1)return true;
    else
    {
        TreeNode *curNode = mytree->GetNodeWithId(unit.Beg);
        while(choiceUnit.syn_0.find(curNode->value)==choiceUnit.syn_0.end() && curNode->parent->Begin == unit.Beg&&curNode->parent != &mytree->root)curNode = curNode->parent;
        if(choiceUnit.syn_0.find(curNode->value)!=choiceUnit.syn_0.end())
        {
       //    cerr<<curNode->value<<' '<<curNode->Begin<<' '<<curNode->Back<<endl;
            if(mytree->GetSize()>curNode->Back+1)
            {
                TreeNode *tmpNode=mytree->GetNodeWithId(curNode->Back+1);
                if(tmpNode->value=="，"||tmpNode->value=="。")
                    result[1] = curNode->Back+1;
                else result[1]=curNode->Back;
                return true;
            }
            result[1] = curNode->Back;return true;}
        else return false;
    }
}
void GetCurSyntacticStr_withConn(SynTree *mytree,SegCharUnit &unit,vector<int> &result)
{
    result.resize(2,-1);result[0]=unit.Beg;result[1]=unit.Back;
    TreeNode *curNode = mytree->GetNodeWithId(unit.Beg);
    if(curNode->parent->Begin == curNode->Begin)curNode = curNode->parent;
    while(curNode->parent != &mytree->root && curNode->parent->children.size()==1)curNode=curNode->parent;
    result[1]=curNode->Back;
}
void FeatExt::Deal_Sub_Over_Condition(vector<TestRes> &resVec,vector<size_t> &bufferSatelliteVec,int wordLen)
{
    for(size_t i(0);i<bufferSatelliteVec.size();++i)
    {
        int j=bufferSatelliteVec[i];int preIn;
        if(resVec[j].firstNucleiValue==-1)
        {
            if(j==0)preIn=0;else preIn=resVec[j-1].firstUnit.Beg;
            if(resVec[j].secondUnit.Beg<=0){resVec[j].secondUnit.Back=-1;resVec[j].firstUnit.Beg=preIn;continue;}
            if(j==0){
                if(resVec[j].secondUnit.Beg>0){resVec[j].firstUnit.Beg=0;resVec[j].firstUnit.Back=resVec[j].secondUnit.Beg-1;}
                else {resVec[j].firstUnit.Back=-1;continue;}
            }
            else{resVec[j].firstUnit.Beg=resVec[j-1].firstUnit.Beg;resVec[j].firstUnit.Back=resVec[j].secondUnit.Beg-1;}
            resVec[j].firstNucleiValue=0;
        }
    }
    for(size_t i(bufferSatelliteVec.size());i>0;--i)
    {
        int j=bufferSatelliteVec[i-1];int sufIn;
        if(resVec[j].secondNucleiValue==-1)
        {
            sufIn=(j==resVec.size()-1?wordLen-1:resVec[j+1].secondUnit.Back);
            if(resVec[j].firstUnit.Back >= wordLen-1){resVec[j].firstUnit.Back=-1;resVec[j].secondUnit.Back=sufIn;continue;}
            //cerr<<resVec[j].firstUnit.Back<<endl;
            if(j==resVec.size()-1){
                //cerr<<resVec[j].firstUnit.Back<<endl;
                {resVec[j].secondUnit.Beg=resVec[j].firstUnit.Back+1;resVec[j].secondUnit.Back=wordLen-1;}
            }
            else{
                resVec[j].secondUnit.Beg=resVec[j].firstUnit.Back+1;resVec[j].secondUnit.Back=resVec[j+1].secondUnit.Back;
                }
            resVec[j].secondNucleiValue=0;
        }
    }
}

vector<TestRes> FeatExt::DecideForEachSegCharUnit(const string&s,DataItem &item,Resource &resource,SynTree *mytree)
{
    vector<vector<SegCharUnit> > segUnitVec;
    set<size_t> sepVec;
    set<size_t>::iterator it=sepVec.begin();
    copy(item.sepVec.begin(),item.sepVec.end(),insert_iterator<set<size_t> >(sepVec,sepVec.begin()));
    UnitSegmentation(s,item,resource,segUnitVec);

    size_t decisionIndex(0);
    vector<TestRes> resVec;
    vector<size_t> bufferVec;
    vector<size_t> bufferSatelliteVec;
    //cerr<<"segmented\n";
    for(size_t i(0);i<segUnitVec.size();++i)
    {
        for(size_t j(0);j<segUnitVec[i].size();++j)
        {
            decisionIndex=0;
            //cerr<<segUnitVec[i][j].keyWordOrPhrase<<endl;
            for(;decisionIndex<decisionVec.size();++decisionIndex)
                if (decisionVec[decisionIndex].keyValue==segUnitVec[i][j].keyWordOrPhrase)
                    break;
               // else cerr<<decisionVec[decisionIndex].keyValue<<endl;
            if(decisionIndex == decisionVec.size()){continue;}
            TestRes resUnit;
            vector<int> result;

            //get the current segunit;
            if(segUnitVec[i][j].Beg_Without_Conn != -1)//在【X】之前
            {
                if(decisionVec[decisionIndex].pos==1)resUnit.secondUnit = segUnitVec[i][j];
                else resUnit.firstUnit = segUnitVec[i][j];

            }
            else if (decisionVec[decisionIndex].syn_sub_0 ==2 && decisionVec[decisionIndex].sub_pos_0 ==1)//cur is of in t a subclause
                if(decisionVec[decisionIndex].pos==1) resUnit.secondUnit = segUnitVec[i][j];
                else resUnit.firstUnit = segUnitVec[i][j];
            else if (decisionVec[decisionIndex].syn_sub_0 ==2 && decisionVec[decisionIndex].sub_pos_0 ==2)//cur is  just a subclause
                if(decisionVec[decisionIndex].pos==1) {resUnit.secondUnit = segUnitVec[i][j];resUnit.secondUnit.Back=segUnitVec[i].back().Back;}
                else {resUnit.firstUnit = segUnitVec[i][j];resUnit.firstUnit.Back=segUnitVec[i].back().Back;}
            else if(decisionVec[decisionIndex].sub_pos_0 ==3){//bufferVec.push_back(i);bufferVec.push_back(j);
                resUnit.secondUnit=segUnitVec[i][j];
                resUnit.secondUnit.Back=segUnitVec.back().back().Back;
            }//cur is over subclauses
            else if(decisionVec[decisionIndex].syn_sub_0==1)//cur is a syntactic phrase
            {
                if(GetCurSyntaticStr(mytree, segUnitVec[i][j],decisionVec[decisionIndex],result))
                    if(decisionVec[decisionIndex].pos==1)//cur is syntactic
                    {resUnit.secondUnit = segUnitVec[i][j];resUnit.secondUnit.Back=result[1];}
                    else
                    {resUnit.firstUnit = segUnitVec[i][j];resUnit.firstUnit.Back=result[1];}
              //  cerr<<result[0]<<" "<<result[1];
                else //no syntactic
                continue;
                //cerr<<"cur beg and back: "<<result[0]<<" "<<result[1]<<endl;
            }
            //cerr<<"nuclei part has been checked\n";
            //get the satellite part
            int subSenBeg; int subSenBack;
            if(i==0)subSenBeg=0;
            else if(resUnit.secondUnit.Beg==segUnitVec[i][0].Beg)subSenBeg=segUnitVec[i-1][0].Beg;
            else subSenBeg=segUnitVec[i][0].Beg;
            if(i==segUnitVec.size()-1)subSenBack=segUnitVec[i].back().Back;
            else if(resUnit.firstUnit.Back==segUnitVec[i].back().Back)subSenBack=segUnitVec[i+1].back().Back;
            else if(resUnit.firstUnit.Back<segUnitVec[i].back().Back)subSenBack=segUnitVec[i].back().Back;
            else if((it=sepVec.find(resUnit.firstUnit.Back))!=sepVec.end())
                {   ++it;
                    if(it!=sepVec.end())subSenBack=(*it);
                    else subSenBack=segUnitVec.back().back().Back;
                }
            else continue;
            //cerr<<subSenBeg<<" "<<subSenBack<<endl;
            //cerr<<"syn_sub_1: "<<decisionVec[decisionIndex].syn_sub_1<<" pos "<<decisionVec[decisionIndex].pos<<endl;
            if(decisionVec[decisionIndex].syn_sub_1 == 2 && decisionVec[decisionIndex].sub_pos_1!=3)//the satellite part is in or just a subclause
            {
                //cerr<<decisionVec[decisionIndex].pos<<endl;
                if(decisionVec[decisionIndex].pos==1)//satellite is prefix
                    {if(resUnit.secondUnit.Beg == segUnitVec[i].front().Beg && i!=0){resUnit.firstUnit.Beg = segUnitVec[i-1][0].Beg;resUnit.firstUnit.Back=segUnitVec[i-1].back().Back;resUnit.firstNucleiValue=0;resUnit.secondNucleiValue=1;resVec.push_back(resUnit);}}
                else if(decisionVec[decisionIndex].pos==2)//satellite is suffix
                    //if(resUnit.firstUnit.Back == segUnitVec[i].back().Back && i!=segUnitVec.size()-1){resUnit.secondUnit.Beg=segUnitVec[i+1][0].Beg;resUnit.secondUnit.Back=segUnitVec[i+1].back().Back;resUnit.firstNucleiValue=1;resUnit.secondNucleiValue=0;resVec.push_back(resUnit);}
                   // else if(resUnit.firstUnit.Back < segUnitVec[i].back().Back){resUnit.secondUnit.Beg=resUnit.firstUnit.Back+1;resUnit.secondUnit.Back=segUnitVec[i].back().Back;resUnit.firstNucleiValue=1;resUnit.secondNucleiValue=0;resVec.push_back(resUnit);}
                    if(subSenBack > resUnit.firstUnit.Back){resUnit.secondUnit.Beg=resUnit.firstUnit.Back+1;resUnit.secondUnit.Back=subSenBack;resUnit.firstNucleiValue=1;resUnit.secondNucleiValue=0;resVec.push_back(resUnit);}
                    else continue; // no suffix subclause
            }
            else if(decisionVec[decisionIndex].sub_pos_1 ==3 && decisionVec[decisionIndex].pos==2){
                resUnit.firstNucleiValue=1;bufferSatelliteVec.push_back(resVec.size());resVec.push_back(resUnit);
            }
             else if(decisionVec[decisionIndex].sub_pos_1 ==3 && decisionVec[decisionIndex].pos==1){
                resUnit.secondNucleiValue=1;bufferSatelliteVec.push_back(resVec.size());resVec.push_back(resUnit);
            }
            else if(decisionVec[decisionIndex].syn_sub_1==1)//satellite is syntactic
            {
               // if(decisionVec[decisionIndex].pos==1 && GetPreSyntacticStr(mytree,segUnitVec[i][j],subSenBeg,decisionVec[decisionIndex],result) )//choose pre string
               if(decisionVec[decisionIndex].pos==1 && GetPreSyntacticStr(mytree,resUnit.secondUnit,subSenBeg,decisionVec[decisionIndex],result) )//choose pre string
                {resUnit.firstUnit.Beg = result[0];resUnit.firstUnit.Back=result[1];resUnit.firstNucleiValue=1;resUnit.secondNucleiValue=0;resVec.push_back(resUnit);}
                //else if(decisionVec[decisionIndex].pos==2 && GetSufSyntacticStr(mytree,segUnitVec[i][j],subSenBack,decisionVec[decisionIndex],result))//choose suff string
                else if(decisionVec[decisionIndex].pos==1 && decisionVec[decisionIndex].equal)
                    {if(resUnit.secondUnit.Beg == segUnitVec[i].front().Beg && i!=0){resUnit.firstUnit.Beg = segUnitVec[i-1][0].Beg;resUnit.firstUnit.Back=segUnitVec[i-1].back().Back;resUnit.firstNucleiValue=0;resUnit.secondNucleiValue=1;resVec.push_back(resUnit);}}
                else if(decisionVec[decisionIndex].pos==2 && GetSufSyntacticStr(mytree,resUnit.firstUnit,subSenBack,decisionVec[decisionIndex],result))//choose suff string
                {resUnit.secondUnit.Beg = result[0];resUnit.secondUnit.Back=result[1];resUnit.firstNucleiValue=0;resUnit.secondNucleiValue=1;resVec.push_back(resUnit);}
                else if(decisionVec[decisionIndex].pos==2 && decisionVec[decisionIndex].equal)
                //if(resUnit.firstUnit.Back == segUnitVec[i].back().Back && i!=segUnitVec.size()-1){resUnit.secondUnit.Beg=segUnitVec[i+1][0].Beg;resUnit.secondUnit.Back=segUnitVec[i+1].back().Back;resUnit.firstNucleiValue=1;resUnit.secondNucleiValue=0;resVec.push_back(resUnit);}
                if(subSenBack > resUnit.firstUnit.Back){resUnit.secondUnit.Beg=resUnit.firstUnit.Back+1;resUnit.secondUnit.Back=subSenBack;resUnit.firstNucleiValue=1;resUnit.secondNucleiValue=0;resVec.push_back(resUnit);}
                else{continue;}//no suffix subclause
                else {continue;}
            }
        }
    }
    //cerr<<"rough decide\n";
    Deal_Sub_Over_Condition(resVec,bufferSatelliteVec,item.sentenceVec.size());
    for(size_t i(0);i<resVec.size();)
    {
        //cerr<<resVec[i].firstUnit.Back<<endl;
        if(resVec[i].firstUnit.Back==-1)resVec.erase(resVec.begin()+i);
        ++i;
    }
    //print the result
//    int resBeg=0;
//    for (size_t i(0);i<resVec.size();++i)
//    {
//        if(resVec[i].firstUnit.Beg>resBeg){cerr<<item.GetStrWithId(resBeg,resVec[i].firstUnit.Beg-1)<<endl;}
//        cerr<<item.GetStrWithId(resVec[i].firstUnit.Beg,resVec[i].firstUnit.Back)<<' '<<resVec[i].firstNucleiValue<<endl;
//        cerr<<item.GetStrWithId(resVec[i].secondUnit.Beg,resVec[i].secondUnit.Back)<<' '<<resVec[i].secondNucleiValue<<endl;
//        resBeg=resVec[i].secondUnit.Back+1;
//    }
//    if(resBeg!=item.sentenceVec.size())cerr<<item.GetStrWithId(resBeg,item.sentenceVec.size()-1)<<endl;
    return resVec;
}
int IsContain(TestRes &left,TestRes &right)
{
    if(left.firstUnit.Beg <= right.firstUnit.Beg &&left.firstUnit.Back>=right.secondUnit.Back)
    return 1;// left.first contains right
    if(left.secondUnit.Beg <= right.firstUnit.Beg &&left.secondUnit.Back>=right.secondUnit.Back)
    return 2;//left.second contains right
    if(left.firstUnit.Beg >= right.firstUnit.Beg &&left.secondUnit.Back <= right.firstUnit.Back)
    return 3;//right.first contains left
    //if(left.firsttUnit.Beg >= right.secondUnit.Beg &&left.secondUnit.Back <= right.secondUnit.Back)
    //return 4;//right.second contains left
    return 0;
}
int IsOverlap(TestRes &left,TestRes &right)
{
    if(left.secondUnit.Beg <= right.firstUnit.Beg &&left.secondUnit.Back >=right.firstUnit.Beg)
    return 1;// left.second overlaps right.first
    //if(right.secondUnit.Beg<=left.firstUnit.Beg && right.secondUnit.Back>=left.firstUnit.Beg)
    //return 2;//right.second overlaps left.first
    return 0;
}
int IsTwisted(TestRes &left,TestRes &right)
{
    if(left.firstUnit.Beg <= right.firstUnit.Beg &&left.firstUnit.Back<=right.secondUnit.Back)
    return 1;// left.first comes first, but end among right
    // left.second comes first, but end among right = overlap 1

    //if(left.firstUnit.Beg>=right.firstUnit.Beg && left.secondUnit.Back >= right.firstUnit.Back)//r.first.beg-l.first.beg-l.first.back-l.second.beg-r.first.back-r.second.beg-r.second.back-l.second.back
    //return 2; //right.first comes first, but end among left
    //right.second comes first, but end among left = overlap 2
    return 0;
}
bool PosLessRes(TestRes a,TestRes b)
{
    if(a.firstUnit.Beg < b.firstUnit.Beg)return true;
    if (a.firstUnit.Beg==b.firstUnit.Beg && a.secondUnit.Beg < b.secondUnit.Beg) return true;
    if (a.firstUnit.Beg==b.firstUnit.Beg && a.secondUnit.Beg == b.secondUnit.Beg && a.secondUnit.Back <=b.secondUnit.Back) return true;
    return false;
}
vector<TestRes> FeatExt::ReorderSegUnit(vector<TestRes> &resVec,vector<vector<int> >&reConfMat)
{

    set<int> blackSet;
    vector<vector<int> > confMat(resVec.size(),vector<int>(resVec.size(),0));
    sort(resVec.begin(),resVec.end(),PosLessRes);
     //print
//    for (size_t i(0);i<resVec.size();++i)
//    {
//        cerr<<resVec[i].firstUnit.Beg<<" "<<resVec[i].firstUnit.Back<<' '<<resVec[i].firstNucleiValue<<" "<<resVec[i].firstUnit.keyWordOrPhrase<<" "<<resVec[i].firstUnit.keyPhIndex<<endl;
//        cerr<<resVec[i].secondUnit.Beg<<" "<<resVec[i].secondUnit.Back<<' '<<resVec[i].secondNucleiValue<<resVec[i].secondUnit.keyWordOrPhrase<<" "<<resVec[i].secondUnit.keyPhIndex<<endl;
//    }

    for(size_t i(0);i<resVec.size()-1;++i)
    {
        if(blackSet.find(i)!=blackSet.end())continue;
        for(size_t j(i+1);j<resVec.size();++j)
        {
            if(blackSet.find(j)!=blackSet.end())continue;
            if(resVec[i].secondUnit.Back < resVec[j].firstUnit.Beg){break;}
            else if(IsContain(resVec[i],resVec[j])==1)
            {confMat[i][j]=1;}//left.first contains
            else if(IsContain(resVec[i],resVec[j])==2)
            {confMat[i][j]=2;}//left.second contains
            else if(IsContain(resVec[i],resVec[j])==3)
            {confMat[i][j]=3;}//right.first contains
            else if(IsOverlap(resVec[i],resVec[j])==1)
            {confMat[i][j]=4;resVec[i].secondUnit.Back=resVec[j].secondUnit.Back;}//left.second overlapped
            else if(IsTwisted(resVec[i],resVec[j])==1)
            {confMat[i][j]=5;int back1=resVec[i].secondUnit.Back,back2=resVec[j].secondUnit.Back;
                if(resVec[j].firstUnit.Back<resVec[i].firstUnit.Back)
                    {resVec[j].secondUnit.Back=resVec[i].firstUnit.Back;confMat[i][j]=6;
                    }
                else if(resVec[j].secondUnit.Beg>resVec[i].secondUnit.Beg)
                    {resVec[j].firstUnit.Beg=resVec[i].secondUnit.Beg;resVec[j].secondUnit.Back=resVec[i].secondUnit.Back;confMat[i][j]=7;}
                else
                    {resVec[i].secondUnit.Back=max(back1,back2);resVec[i].secondUnit.keyWordOrPhrase=resVec[j].secondUnit.keyWordOrPhrase;resVec[i].secondUnit.keyPhIndex=resVec[j].secondUnit.keyPhIndex;blackSet.insert(j);}

            }//left.first comes first
            else
            {blackSet.insert(j);}
        }
    }
    vector<TestRes> reResVec;
    for(size_t i(0);i<resVec.size();++i)
    {
        if(blackSet.find(i)!=blackSet.end()){continue;}
        reResVec.push_back(resVec[i]);vector<int> tmpv;
        for(size_t j(0);j<confMat[i].size();++j)
            if(blackSet.find(j)==blackSet.end())
                tmpv.push_back(confMat[i][j]);
        reConfMat.push_back(tmpv);vector<int>().swap(tmpv);
    }
    return reResVec;
}
int FeatExt::JudgeBasicConn(vector<string>&avec,vector<string>&bvec,RSTNode*p,int mode)
{
    for(size_t i(0);i<avec.size();++i)
        for(size_t j(0);j<bvec.size();++j)
        {
            if(avec[i]=="NULL"&&bvec[j]=="NULL") return -1;
            if(biLm.find(avec[i]+"/"+bvec[j])!=biLm.end())
            {
                p->leftChild->keyWord=avec[i];p->rightChild->keyWord=bvec[j];
                p->relationValue=biLm.lower_bound(avec[i]+"/"+bvec[j])->second.rel;
                return 1;
            }
        }
    return -1;
}
int FeatExt::AddConn_1(vector<TestRes> &resVec,DataItem&item,Resource&resource,vector<RSTNode *>&nodeVec)
{
    using namespace boost;
    boost::match_results<string::const_iterator> mr;
    string firstStr,secondStr,firstKey,secondKey;int firstConn,secondConn;
    multimap<string,RelAndProc>::iterator it1,it2,it3;
    vector<string> firstKeyVec;
    vector<string> secondKeyVec;
    int mode1,mode2;
    for(size_t i(0);i<resVec.size();++i)
    {
        mode1=0;mode2=0;
        vector<string>().swap(firstKeyVec);vector<string>().swap(secondKeyVec);
        if(i!=0 && resVec[i].firstUnit.Beg==resVec[i-1].firstUnit.Beg && resVec[i].secondUnit.Back==resVec[i-1].secondUnit.Back)continue;

        firstStr=item.GetStrWithId(resVec[i].firstUnit.Beg,resVec[i].firstUnit.Back);
      //  cerr<<'\n'<<resVec[i].firstUnit.Beg<<" "<<resVec[i].firstUnit.Back<<endl;
        secondStr=item.GetStrWithId(resVec[i].secondUnit.Beg,resVec[i].secondUnit.Back);
       // cerr<<resVec[i].firstUnit.keyPhIndex<<' '<<resVec[i].firstUnit.keyWordOrPhrase<<' '<<resVec[i].secondUnit.keyPhIndex<<' '<<resVec[i].secondUnit.keyWordOrPhrase<<endl;
        if(resVec[i].firstUnit.keyWordOrPhrase.compare("")&&firstStr.find(resVec[i].firstUnit.keyWordOrPhrase)!=string::npos)
            {mode1=1;firstKeyVec.push_back(resVec[i].firstUnit.keyWordOrPhrase);}
        else if(resVec[i].firstUnit.keyWordOrPhrase.compare("")&&resVec[i].firstUnit.keyPhIndex!=-1)
            {if(regex_search(firstStr,mr,regex(resource.keyPhraseVec[resVec[i].firstUnit.keyPhIndex])))
             {mode1=1;firstKeyVec.push_back(resVec[i].firstUnit.keyWordOrPhrase);}}
        else{
            for(int j(resVec[i].firstUnit.Beg);j<(2*resVec[i].firstUnit.Beg+resVec[i].firstUnit.Back)/3+1&&j<resVec[i].firstUnit.Beg+5;++j)
                if(resource.keyWordSet.find(item.sentenceVec[j])!=resource.keyWordSet.end())
                {mode1=2;firstKeyVec.push_back(item.sentenceVec[j]);}
        }

        if(resVec[i].secondUnit.keyWordOrPhrase.compare("")&&secondStr.find(resVec[i].secondUnit.keyWordOrPhrase)!=string::npos)
            {mode2=1;secondKeyVec.push_back(resVec[i].secondUnit.keyWordOrPhrase);}
        else if(resVec[i].secondUnit.keyWordOrPhrase.compare("")&& resVec[i].secondUnit.keyPhIndex!=-1)
            {  if(regex_search(secondStr,mr,regex(resource.keyPhraseVec[resVec[i].secondUnit.keyPhIndex])))
                {mode2=1;secondKeyVec.push_back(resVec[i].secondUnit.keyWordOrPhrase);}
            }
        else{
            for(int j(resVec[i].secondUnit.Beg);j<(2*resVec[i].secondUnit.Beg+resVec[i].secondUnit.Back)/3+1&&j<resVec[i].secondUnit.Beg+5;++j)
                if(resource.keyWordSet.find(item.sentenceVec[j])!=resource.keyWordSet.end())
                    {mode2=2;secondKeyVec.push_back(item.sentenceVec[j]);}
            }

        if(mode1==0&&mode2==0)continue;
        if(mode1!=1)firstKeyVec.push_back("NULL");
        if(mode2!=1)secondKeyVec.push_back("NULL");
        RSTNode *p=new RSTNode(NULL);
        RSTNode *leftNode=new RSTNode(p);leftNode->Begin=resVec[i].firstUnit.Beg;leftNode->Back=resVec[i].firstUnit.Back;leftNode->nucleiValue=resVec[i].firstNucleiValue;
        RSTNode *rNode=new RSTNode(p);rNode->Begin=resVec[i].secondUnit.Beg;rNode->Back=resVec[i].secondUnit.Back;rNode->nucleiValue=resVec[i].secondNucleiValue;
        p->leftChild=leftNode;p->rightChild=rNode;p->Begin=p->leftChild->Begin;p->Back=p->rightChild->Back;
        if(mode1==1 && mode2==1)
            if(biLm.find(firstKeyVec[0]+"/"+secondKeyVec[0])!=biLm.end())
            {
                leftNode->keyWord=firstKeyVec[0];rNode->keyWord=secondKeyVec[0];
                p->relationValue=biLm.lower_bound(firstKeyVec[0]+"/"+secondKeyVec[0])->second.rel;
            }
            else continue;
        else{
            if(JudgeBasicConn(firstKeyVec,secondKeyVec,p,mode1+10*mode2)==-1)continue;
        }

 //       cerr<<p->Begin<<' '<<p->Back<<' '<<p->relationValue<<" l: "<<p->leftChild->keyWord<<" r: "<<p->rightChild->keyWord<<endl;
        nodeVec.push_back(p);
    }
}
void FeatExt::AddConn(vector<TestRes> &resVec,DataItem&item,Resource&resource,vector<RSTNode *>&nodeVec)
{
    using namespace boost;
    boost::match_results<string::const_iterator> mr;
    string firstStr,secondStr,firstKey,secondKey;int firstConn,secondConn;
    multimap<string,RelAndProc>::iterator it1,it2,it3;
    for(size_t i(0);i<resVec.size();++i)
    {
        if(i!=0 && resVec[i].firstUnit.Beg==resVec[i-1].firstUnit.Beg && resVec[i].secondUnit.Back==resVec[i-1].secondUnit.Back)continue;
        firstKey="NULL";secondKey="NULL";firstConn=-1;secondConn=-1;
        firstStr=item.GetStrWithId(resVec[i].firstUnit.Beg,resVec[i].firstUnit.Back);
      //  cerr<<'\n'<<resVec[i].firstUnit.Beg<<" "<<resVec[i].firstUnit.Back<<endl;
        secondStr=item.GetStrWithId(resVec[i].secondUnit.Beg,resVec[i].secondUnit.Back);
       // cerr<<resVec[i].firstUnit.keyPhIndex<<' '<<resVec[i].firstUnit.keyWordOrPhrase<<' '<<resVec[i].secondUnit.keyPhIndex<<' '<<resVec[i].secondUnit.keyWordOrPhrase<<endl;
        if(resVec[i].firstUnit.keyWordOrPhrase.compare("")&&firstStr.find(resVec[i].firstUnit.keyWordOrPhrase)!=string::npos)firstKey=resVec[i].firstUnit.keyWordOrPhrase;
        else if(resVec[i].firstUnit.keyWordOrPhrase.compare("")&&resVec[i].firstUnit.keyPhIndex!=-1)
            {if(regex_search(firstStr,mr,regex(resource.keyPhraseVec[resVec[i].firstUnit.keyPhIndex])))firstKey=resVec[i].firstUnit.keyWordOrPhrase;}
        else{
            for(int j(resVec[i].firstUnit.Beg);j<(2*resVec[i].firstUnit.Beg+resVec[i].firstUnit.Back)/3+1;++j)
                if(resource.keyWordSet.find(item.sentenceVec[j])!=resource.keyWordSet.end()&&j<resVec[i].firstUnit.Beg+5)
                {firstKey=item.sentenceVec[j];break;}
        }

        if(resVec[i].secondUnit.keyWordOrPhrase.compare("")&&secondStr.find(resVec[i].secondUnit.keyWordOrPhrase)!=string::npos)secondKey=resVec[i].secondUnit.keyWordOrPhrase;
        else if(resVec[i].secondUnit.keyWordOrPhrase.compare("")&& resVec[i].secondUnit.keyPhIndex!=-1)
            {  if(regex_search(secondStr,mr,regex(resource.keyPhraseVec[resVec[i].secondUnit.keyPhIndex]))){firstKey=resVec[i].firstUnit.keyWordOrPhrase;}}
        else{
            for(int j(resVec[i].secondUnit.Beg);j<(2*resVec[i].secondUnit.Beg+resVec[i].secondUnit.Back)/3+1;++j)
                if(resource.keyWordSet.find(item.sentenceVec[j])!=resource.keyWordSet.end()&&j<resVec[i].secondUnit.Beg+5)
                    {secondKey=item.sentenceVec[j];break;}
            }


        if(firstKey=="NULL" && secondKey=="NULL")continue;
        RSTNode *p=new RSTNode(NULL);
        if(firstKey.compare("NULL") && uniLm.find(firstKey)!=uniLm.end())//leftchild
        {it1=uniLm.lower_bound(firstKey);firstConn=(*it1).second.rel;}
        RSTNode *leftNode=new RSTNode(p);leftNode->Begin=resVec[i].firstUnit.Beg;leftNode->Back=resVec[i].firstUnit.Back;leftNode->keyWord=firstKey;leftNode->nucleiValue=resVec[i].firstNucleiValue;
        p->leftChild=leftNode;
        if(secondKey.compare("NULL") && uniLm.find(secondKey)!=uniLm.end())//right child
        {it2=uniLm.lower_bound(secondKey);secondConn=(*it2).second.rel;}
        RSTNode *rNode=new RSTNode(p);rNode->Begin=resVec[i].secondUnit.Beg;rNode->Back=resVec[i].secondUnit.Back;rNode->keyWord=secondKey;rNode->nucleiValue=resVec[i].secondNucleiValue;
        p->rightChild=rNode;
        p->Begin=p->leftChild->Begin;p->Back=p->rightChild->Back;
        if(firstConn==-1&&secondConn==-1)
        {p->relationValue=-1;continue;}
//        else if(firstConn!=-1&&secondConn==-1)p->relationValue=firstConn;
//        else if(firstConn==-1&&secondConn!=-1)p->relationValue=secondConn;
        else if(biLm.find(firstKey+"/"+secondKey)!=biLm.end())p->relationValue=biLm.lower_bound(firstKey+"/"+secondKey)->second.rel;
        else if(firstKey!="NULL" && secondKey!="NULL")p->relationValue=firstConn;
        else {continue;}//p->relationValue=firstConn;}


 //       cerr<<p->Begin<<' '<<p->Back<<' '<<p->relationValue<<" l: "<<p->leftChild->keyWord<<" r: "<<p->rightChild->keyWord<<endl;
        nodeVec.push_back(p);
    }
}
bool PosLessSpanGreat(RSTNode *a,RSTNode *b)
{
    if(a->Begin < b->Begin)return true;
    else if(a->Begin==b->Begin && a->Back>b->Back)return true;
    return false;
}
void FeatExt::ReConstructed(vector<RSTNode*>&nodeVec)
{
    sort(nodeVec.begin(),nodeVec.end(),PosLessSpanGreat);
    int beg(-1),back(-1);
    RSTNode *p=nodeVec[0];
  //  cerr<<nodeVec[0]->Begin<<" "<<nodeVec[0]->Back<<endl;
    for(size_t i(1);i<nodeVec.size();)
    {
     //   cerr<<nodeVec[i]->Begin<<" "<<nodeVec[i]->Back<<endl;
        while(p->parent!=NULL && (nodeVec[i]->Begin > p->Back))
        p=p->parent;
     //  cerr<<p->Begin<<" "<<p->Back<<endl;
        if(nodeVec[i]->Begin>p->Back){p=nodeVec[i];++i;continue;}
        else if(nodeVec[i]->Begin<p->Back && nodeVec[i]->Back <= p->Back)
        {
            //cerr<<p->leftChild->Begin<<" "<<p->leftChild->Back<<" "<<endl;
            if(p->leftChild != NULL)
            {if(p->leftChild->Back >= nodeVec[i]->Back)
            p=p->leftChild;}
            if(p->rightChild != NULL)
            {if(p->leftChild->Back<nodeVec[i]->Begin)p=p->rightChild;}
            //else{cerr<<"wrong in constructing the rst node\n"<<nodeVec[i]->Begin<<" "<<nodeVec[i]->Back<<' '<<nodeVec[i]->keyWord<<endl;exit(1);}
            if(p->leftChild==NULL){p->leftChild=nodeVec[i];nodeVec[i]->parent=p;p=p->leftChild;}
            else if(p->rightChild==NULL){p->rightChild=nodeVec[i];nodeVec[i]->parent=p;p=p->rightChild;}
            else{
                RSTNode *tmpJoint=new RSTNode(p);p->rightChild->parent=tmpJoint;nodeVec[i]->parent=tmpJoint;
                tmpJoint->leftChild=p->rightChild;tmpJoint->rightChild=nodeVec[i];
                tmpJoint->Begin=tmpJoint->leftChild->Begin;tmpJoint->Back=tmpJoint->rightChild->Back;
                tmpJoint->keyWord="NULL";tmpJoint->relationValue=3;//joint relationship;
                tmpJoint->leftChild->nucleiValue=0;tmpJoint->rightChild->nucleiValue=0;
                p->rightChild=tmpJoint;p=p->rightChild->rightChild;
                }
            nodeVec.erase(nodeVec.begin()+i);
        }
        else {cerr<<"wrong in constructing the rst node\n"<<nodeVec[i]->Begin<<" "<<nodeVec[i]->Back<<' '<<nodeVec[i]->keyWord<<endl;exit(1);}
    }

}
void PrintRSTTree(DataItem &item, vector<RSTNode*>& nodeVec)
{
    vector<RSTNode> vec;
    for(size_t i(0);i<nodeVec.size();++i)vec.push_back((*nodeVec[i]));
    PrintRSTTree(item,vec);
}
void FeatExt::DecideConnForNewSegCharUnit(vector<TestRes> &resVec,DataItem &item,Resource&resource)
{
    vector<vector<int> >reConfMat;
    if(resVec.empty()){cout<<"0 ||| ";copy(item.sentenceVec.begin(),item.sentenceVec.end(),ostream_iterator<string,char>(cout," "));cout<<endl;return;}

    vector <TestRes> reResVec=ReorderSegUnit(resVec,reConfMat);


    //print
//    int resBeg=0;
//    for (size_t i(0);i<reResVec.size();++i)
//    {
//        if(reResVec[i].firstUnit.Beg>resBeg){cerr<<item.GetStrWithId(resBeg,reResVec[i].firstUnit.Beg-1)<<endl;}
//        cerr<<item.GetStrWithId(reResVec[i].firstUnit.Beg,reResVec[i].firstUnit.Back)<<' '<<reResVec[i].firstNucleiValue<<' '<<reResVec[i].firstUnit.keyWordOrPhrase<<endl;
//        cerr<<item.GetStrWithId(reResVec[i].secondUnit.Beg,reResVec[i].secondUnit.Back)<<' '<<reResVec[i].secondNucleiValue<<' '<<reResVec[i].secondUnit.keyWordOrPhrase<<endl;
//        resBeg=reResVec[i].secondUnit.Back+1>resBeg?reResVec[i].secondUnit.Back+1:resBeg;
//    }
//    if(resBeg!=item.sentenceVec.size())cerr<<item.GetStrWithId(resBeg,item.sentenceVec.size()-1)<<endl;


    vector<RSTNode*>nodeVec;
    AddConn(reResVec,item,resource,nodeVec);
    if(nodeVec.empty()){cout<<"0 ||| ";copy(item.sentenceVec.begin(),item.sentenceVec.end(),ostream_iterator<string,char>(cout," "));cout<<endl;return;}
    ReConstructed(nodeVec);
    if(nodeVec.empty()){cout<<"0 ||| ";copy(item.sentenceVec.begin(),item.sentenceVec.end(),ostream_iterator<string,char>(cout," "));cout<<endl;return;}
    cout<<"1 ||| ";
    PrintRSTTree(item,nodeVec);
}











