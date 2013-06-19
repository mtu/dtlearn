#include "PostDecode.h"
#include "ExtractRule.h"
void PrintSingleTree_PD(DataItem &item, RSTNode *node)
{

//     if(node->relationValue != -1){cout<<" ("<<node->relationValue<<" "<<node->Begin<<'-'<<node->Back<<' ';}
//     else if(node->nucleiValue != -1){cout<<" ("<<node->nucleiValue<<":"<<node->keyWord<<": "<<node->Begin<<'-'<<node->Back<<' ';}
     if(node->relationValue != -1){cout<<"RELVALUE: "<<node->relationValue<<endl;}
     else if(node->nucleiValue != -1){cout<<"NUCLEIVALUE: "<<node->nucleiValue<<'\n'<<"KEYWORD: "<<node->keyWord<<"\n";}


     if(node->leftChild ==NULL)
     {
        cout<<node->Begin<<" "<<node->Back<<" ||| "<<item.GetStrWithId(node->Begin,node->Back)<<endl;
        return ;
     }
     else
     {
           if(node->Begin < node->leftChild->Begin)
             cout<<node->Begin<<" "<<node->Back-1<<" ||| "<<item.GetStrWithId(node->Begin,node->leftChild->Begin-1)<<endl;
           PrintSingleTree_PD(item,node->leftChild);
     }

     if(node->rightChild!=NULL)
    {
            PrintSingleTree_PD(item,node->rightChild);
            if(node->Back > node->rightChild->Back)
              cout<<node->rightChild->Back+1<<" "<<node->Back<<" ||| "<<item.GetStrWithId(node->rightChild->Back+1,node->Back)<<'\n';
    }
     else
     {
         if(node->Back>node->leftChild->Back)
            {cout<<node->leftChild->Back+1<<" "<<node->Back<<" ||| "<<item.GetStrWithId(node->leftChild->Back+1,node->Back)<<'\n';return;}
     }

    //cout<<"#)";
}
void PrintSingleTree_PD3(DataItem &item, RSTNode *node,string &posId)
{
    string tmpId;
//     if(node->relationValue != -1){cout<<" ("<<node->relationValue<<" "<<node->Begin<<'-'<<node->Back<<' ';}
//     else if(node->nucleiValue != -1){cout<<" ("<<node->nucleiValue<<":"<<node->keyWord<<": "<<node->Begin<<'-'<<node->Back<<' ';}
//     if(node->relationValue != -1){cout<<"RELVALUE: "<<node->relationValue<<endl;}
//     else if(node->nucleiValue != -1){cout<<"NUCLEIVALUE: "<<node->nucleiValue<<'\n'<<"KEYWORD: "<<node->keyWord<<"\n";}

     if(node->relationValue != -1)
     {
         string lS("NULL"),rS("NULL");
         if(node->leftChild != NULL)lS=node->leftChild->keyWord;
         if(node->rightChild !=NULL)rS=node->rightChild->keyWord;
         cout <<posId<<" %%% "<<lS+'/'+rS<<endl;
     }
     if(node->leftChild ==NULL)
     {
        cout<<posId<<" T ||| <FLAT> ||| "<<node->Begin<<" "<<node->Back<<" ||| "<<item.GetStrWithId(node->Begin,node->Back)<<endl;
        return ;
     }
     else
     {
         tmpId = posId+" 0";
           if(node->Begin < node->leftChild->Begin)
             cout<<posId<<" S ||| <FLAT> ||| "<<node->Begin<<" "<<node->Back-1<<" ||| "<<item.GetStrWithId(node->Begin,node->leftChild->Begin-1)<<endl;
           PrintSingleTree_PD3(item,node->leftChild,tmpId);
     }

     if(node->rightChild!=NULL)
    {
        tmpId = posId+" 1";
            PrintSingleTree_PD3(item,node->rightChild,tmpId);
            if(node->Back > node->rightChild->Back)
              cout<<posId+" S ||| <FLAT> ||| "<<node->rightChild->Back+1<<" "<<node->Back<<" ||| "<<item.GetStrWithId(node->rightChild->Back+1,node->Back)<<'\n';
    }
     else
     {
         if(node->Back>node->leftChild->Back)
            {cout<<posId<<" T ||| <FLAT> ||| "<<node->leftChild->Back+1<<" "<<node->Back<<" ||| "<<item.GetStrWithId(node->leftChild->Back+1,node->Back)<<'\n';return;}
     }

    //cout<<"#)";
}
void PostDecodeClass::PrintFlatRSTTree_1(DataItem &item, vector<RSTNode>& nodeVec)
{
    using namespace boost;
     int pos(0);
     string posId="0";
    for(size_t i(0);i<nodeVec.size();++i)
    {
    //    cerr<<nodeVec[i].Begin<<" "<<nodeVec[i].Back<<endl;
        if(nodeVec[i].Begin > pos){cout<<posId+" S ||| <FLAT> ||| ";cout<<pos<<" "<<nodeVec[i].Begin-1<<" ||| "<<item.GetStrWithId(pos,nodeVec[i].Begin-1)<<endl;}
        pos = nodeVec[i].Back+1;
        string tmpId=posId+" "+ lexical_cast<string>(i);
        PrintSingleTree_PD3(item,&nodeVec[i],tmpId);
        //cout<<" ";
    }
   // cout<<" ";
    if(pos<item.sentenceVec.size())cout<<posId+" S ||| <FLAT> ||| "<<pos<<" "<<item.sentenceVec.size()-1<<" ||| "<<item.GetStrWithId(pos,item.sentenceVec.size()-1)<<'\n';
    cout<<endl;
}

void PostDecodeClass::LoadAndPrintRSTTree(const char*filename)
{
    ifstream in(filename);

    int count(1);
    if(!in ){cerr<<"wrong to open file\n";exit(1);}
    string line,mark;

    int pos(0);
    while(getline(in,line))
    {
        cerr<<count<<endl;

        line=half2full(line);
        pos=line.find(" ");
        mark=line.substr(0,pos);
        pos=line.find_first_not_of(" ",pos+1);
        line = line.substr(pos);

        RSTTree rstTree;
        rstTree.MyConstructRSTTree(line);
        cerr<<"rst tree has been parsed\n";
        DataItem item(rstTree.wordVec);
        cerr<<item.sentenceVec.size()<<endl;
        if(rstTree.RSTNodeVec[0].leftChild!=NULL)
        {
            PrintFlatRSTTree_1(item,rstTree.RSTNodeVec);}
        else cout<<line<<endl<<endl;
        ++count;

    }
}
