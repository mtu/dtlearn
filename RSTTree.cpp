#include"RSTTree.h"


regex rgx_fortrain=regex("([0-1]):(.*):");
boost::match_results<string::const_iterator> mr_fortrain;
RSTRelation relationMap;

int LeftBranketForRST(const string&s, string &str_flw_lb)
{
    if(s.length() < 2)return -1;
    str_flw_lb = s.substr(1);
    if(relationMap.Find(str_flw_lb))return 1;
    if (regex_match(str_flw_lb,mr_fortrain,rgx_fortrain))return 2;
    return -1;
}
int RightBranketForRST(const string&s)
{
    for (size_t i(0);i<s.length();++i)
        if(s[i]==')')continue;
        else return -1;
    return s.length();
}
int RightBranketForRST_1(const string&s)
{
    if(s.length() < 2 || s.length()%2==1)return -1;
    for (size_t i(0);i<s.length()-1;i+=2)
        if(s[i]=='#' &&s[i+1]==')')continue;
        else return -1;
    return s.length()/2;
}

void ConstructRSTTree(const string&s, vector<string>& sentenceVec, vector<RSTNode>& result)
{
    stringstream ss(s);
    string word,str_flw_lb;
    int lbNum(0),rbNum(0),wordNum(0);
    RSTNode node(NULL);
    RSTNode *currentNode=&node;
    RSTNode *root=&node;
    //RSTNode *currentNode = new RSTNode(&node);
    bool singlebrk=false;
    while(ss>>word)
    {
        //cerr<<word<<' ';
        if(word[0]=='(')
        {
           lbNum = LeftBranketForRST(word,str_flw_lb);
           if(lbNum == -1){sentenceVec.push_back(word);++wordNum;continue;}
           else if(singlebrk==true){currentNode->Back = wordNum-1;currentNode = currentNode->parent;singlebrk=false;}

           if(lbNum == 1)//开始新的篇章关系
           {
               if(currentNode ==NULL)//树已满，另建新树
               {
                   result.push_back(*root);
                    RSTNode node1(NULL); currentNode = &node1; root=&node1;
                   // currentNode = new RSTNode(node.leftChild);
               }
               if(currentNode->relationValue==-1 && currentNode->nucleiValue==-1);
               else if(currentNode->leftChild == NULL){RSTNode *p = new RSTNode(currentNode);currentNode->leftChild=p;currentNode=currentNode->leftChild;}
               else if(currentNode->rightChild == NULL){RSTNode *p = new RSTNode(currentNode);currentNode->rightChild=p;currentNode=currentNode->rightChild;}

               currentNode->Begin = wordNum;
               currentNode->relationValue=relationMap.Find(str_flw_lb)?relationMap.GetRSTMapId(str_flw_lb):relationMap.Add(str_flw_lb);
           }
           else if(lbNum == 2)//核心辅助关系
           {
               RSTNode *p = new RSTNode(currentNode);
               if(currentNode->leftChild==NULL){currentNode->leftChild=p;currentNode=currentNode->leftChild;}
               else if(currentNode->rightChild==NULL){currentNode->rightChild=p;currentNode=currentNode->rightChild;}
               else{cerr<<"wrong nulcei\n"<<s<<'\n'<<word<<'\n';exit(1);}
               currentNode->Begin = wordNum;
               currentNode->keyWord=mr_fortrain[2];currentNode->nucleiValue=atoi(string(mr_fortrain[1]).c_str());
           }

        }
        else
        {
            if(singlebrk){sentenceVec.push_back(")");++wordNum;singlebrk=false;}
            if(word[0]==')')
            {
                rbNum = RightBranketForRST(word);
                if(rbNum==-1){sentenceVec.push_back(word);++wordNum;continue;}
                else if(rbNum>1){
                    for(;rbNum>0;rbNum--)
                    {currentNode->Back = wordNum-1;
                    currentNode = currentNode->parent;}
                }
                else{singlebrk=true;}
            }
            else {sentenceVec.push_back(word);++wordNum;}
        }
    }
    result.push_back(*root);
   // cerr<<endl;
  //  cerr<<"rst tree has been constructed "<<result.size()<<endl;
}
void ConstructRSTTree_1(const string&s, vector<string>& sentenceVec, vector<RSTNode>& result)
{
    stringstream ss(s);
    string word,str_flw_lb;
    int lbNum(0),rbNum(0),wordNum(0);
    RSTNode node(NULL);
    RSTNode *currentNode=&node;
    RSTNode *root=&node;
    //RSTNode *currentNode = new RSTNode(&node);
    bool singlebrk=false;
    while(ss>>word)
    {
        //cerr<<word<<' ';
        if(word[0]=='(')
        {
           lbNum = LeftBranketForRST(word,str_flw_lb);
           if(lbNum == -1){sentenceVec.push_back(word);++wordNum;continue;}

           if(lbNum == 1)//开始新的篇章关系
           {
               if(currentNode ==NULL)//树已满，另建新树
               {
                   result.push_back(*root);
                    RSTNode node1(NULL); currentNode = &node1; root=&node1;
                   // currentNode = new RSTNode(node.leftChild);
               }
               if(currentNode->relationValue==-1 && currentNode->nucleiValue==-1);
               else if(currentNode->leftChild == NULL){RSTNode *p = new RSTNode(currentNode);currentNode->leftChild=p;currentNode=currentNode->leftChild;}
               else if(currentNode->rightChild == NULL){RSTNode *p = new RSTNode(currentNode);currentNode->rightChild=p;currentNode=currentNode->rightChild;}

               currentNode->Begin = wordNum;
               currentNode->relationValue=relationMap.Find(str_flw_lb)?relationMap.GetRSTMapId(str_flw_lb):relationMap.Add(str_flw_lb);
           }
           else if(lbNum == 2)//核心辅助关系
           {
               RSTNode *p = new RSTNode(currentNode);
               if(currentNode->leftChild==NULL){currentNode->leftChild=p;currentNode=currentNode->leftChild;}
               else if(currentNode->rightChild==NULL){currentNode->rightChild=p;currentNode=currentNode->rightChild;}
               else{cerr<<"wrong nulcei\n"<<s<<'\n'<<word<<'\n';exit(1);}
               currentNode->Begin = wordNum;
               currentNode->keyWord=mr_fortrain[2];currentNode->nucleiValue=atoi(string(mr_fortrain[1]).c_str());
           }

        }
        else
        {
            if(word[0]=='#')
            {
                rbNum = RightBranketForRST_1(word);
                if(rbNum==-1){sentenceVec.push_back(word);++wordNum;continue;}
                else {
                    for(;rbNum>0;rbNum--)
                    {currentNode->Back = wordNum-1;
                    currentNode = currentNode->parent;}
                }
            }
            else {sentenceVec.push_back(word);++wordNum;}
        }
    }
    result.push_back(*root);
   // cerr<<endl;
  //  cerr<<"rst tree has been constructed "<<result.size()<<endl;
}
void PrintSingleTree(DataItem &item, RSTNode *node)
{

//     if(node->relationValue != -1){cout<<" ("<<node->relationValue<<" "<<node->Begin<<'-'<<node->Back<<' ';}
//     else if(node->nucleiValue != -1){cout<<" ("<<node->nucleiValue<<":"<<node->keyWord<<": "<<node->Begin<<'-'<<node->Back<<' ';}
     if(node->relationValue != -1){cout<<" ("<<node->relationValue;}
     else if(node->nucleiValue != -1){cout<<" ("<<node->nucleiValue<<":"<<node->keyWord<<": ";}


     if(node->leftChild ==NULL)
     {
        cout<<item.GetStrWithId(node->Begin,node->Back)<<" #)";
        return ;
     }
     else
     {
           if(node->Begin < node->leftChild->Begin)
             cout<<' '<<item.GetStrWithId(node->Begin,node->leftChild->Begin-1);
           PrintSingleTree(item,node->leftChild);
     }

     if(node->rightChild!=NULL)
    {
            PrintSingleTree(item,node->rightChild);
            if(node->Back > node->rightChild->Back)
              cout<<' '<<item.GetStrWithId(node->rightChild->Back+1,node->Back)<<' ';
    }
     else
     {
         if(node->Back>node->leftChild->Back)
            {cout<<' '<<item.GetStrWithId(node->leftChild->Back+1,node->Back)<<" #)";return;}
     }

    cout<<"#)";
}
void PrintRSTTree(DataItem &item, vector<RSTNode>& nodeVec)
{
    int pos(0);
    for(size_t i(0);i<nodeVec.size();++i)
    {
    //    cerr<<nodeVec[i].Begin<<" "<<nodeVec[i].Back<<endl;
        if(nodeVec[i].Begin > pos){cout<<item.GetStrWithId(pos,nodeVec[i].Begin-1);}
        pos = nodeVec[i].Back+1;
        PrintSingleTree(item,&nodeVec[i]);
        cout<<" ";
    }
   // cout<<" ";
    if(pos<item.sentenceVec.size())cout<<item.GetStrWithId(pos,item.sentenceVec.size()-1)<<' ';
    cout<<endl;
}
void RSTTree::UnitRSTTree(RSTNode *node)
{
     if(node->relationValue != -1){relationVec.push_back(node);\
        //cerr<<node->Begin<<" checkrstrelation "<<node->Back<<" value is "<<node->relationValue<<endl;
     }
     //else if(node->nucleiValue != -1){cout<<" ("<<node->nucleiValue<<":"<<node->keyWord<<": ";}
     if(node->leftChild !=NULL)
        UnitRSTTree(node->leftChild);

     if(node->rightChild!=NULL)
        UnitRSTTree(node->rightChild);
}
vector<RSTNode *> RSTTree::GetAllRSTTree()
{
    for(size_t i(0);i<RSTNodeVec.size();++i)
    {
        UnitRSTTree(&RSTNodeVec[i]);
    }
    return this->relationVec;
}
void RSTTree::RSTTreeClear()
{
    vector<RSTNode>().swap(RSTNodeVec);
    vector<string>().swap(wordVec);
}
