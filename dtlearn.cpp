#include "Common.h"
#include "dtlearn.h"

using namespace std;

void test(const char*filename_rst,const char*filename_align,const char *filename_en)
{
    //string s="0-0 0-1 0-2 1-2 2-2 3-2 4-2 0-3 0-4 2-5 8-6 8-7 0-8 9-9 5-10 0-11 0-12 0-13 6-14 6-15 7-16";
//    AlignedRel arObj;
//    arObj.LoadAlignedSentence(s);
//    arObj.GetESpanGivenCSpan(0,4);
//    arObj.GetESpanGivenCSpan(0,9);

    ifstream in(filename_rst);
    ifstream in1(filename_align);
    ifstream in2(filename_en);
    int count(1),pos;
    if(!in || !in1||!in2){cerr<<"wrong to open file\n";exit(1);}
    string line,mark,line1,line2;
    map<string,int> biIndex;
    vector<vector<int> > biIndexMap;
    vector<vector<int> > orderIndexMap;
    Row transSet;
    vector<string> dataVec;
    while(getline(in,line) && getline(in1,line1) &&getline(in2,line2))
    {
        cerr<<count<<endl;
        line=half2full(line);line1=half2full(line1);
        pos=line.find(" ");
        mark=line.substr(0,pos);
        pos= pos=line.find_first_not_of(" ",pos+1);
        line = line.substr(pos);
        TransRule rule;
        if(rule.LoadItemAndRST(line1,line,line2))
            MakeStOfBiLm_1(count-1,rule,biIndex,biIndexMap,orderIndexMap,transSet);
        rule.Clear();
        ++count;
    }
    map<string,int >::iterator it=biIndex.begin();
    cerr<<biIndex.rbegin()->first<<endl;
    float tmpOrder,tmpTrans;
    int ind;
    while(it!=biIndex.end())
    {// source ||| target ||| trans Prob. ||| reorder Prob.
        ind = (*it).second;
       // tmpOrder=orderIndexMap[ind].at(0)+orderIndexMap[ind].at(1);
        NewRow tmpVC;
        tmpTrans = SumRow(transSet[ind]);
        map<string,CountStruct >::iterator it1 = transSet[ind].begin();
        while(it1!=transSet[ind].end()){tmpVC.push((*it1));it1++;}
        //vector<VCPair>::iterator it1 = tmpVC.begin();
        while(!tmpVC.empty())
        {
            float revRate=float(tmpVC.top().second.revCount)/tmpVC.top().second.count;
            cout<<(*it).first<<" ||| "<<tmpVC.top().first<<" ||| " << tmpVC.top().second.count/tmpTrans<<" ||| "<<1-revRate<<" "<<revRate<<" "\

            <<tmpVC.top().second.count-tmpVC.top().second.revCount << " "<<tmpVC.top().second.revCount<<" "<<tmpVC.top().second.count<<endl;
           tmpVC.pop();
        }

         it++;
    }

}
void test_1(const char*filename_rst,const char* filename_syn,const char* filename_ori)
{
    ifstream in(filename_rst);
    ifstream in1(filename_syn);
    ifstream in2(filename_ori);
    int count(1);
    if(!in || !in1 || !in2){cerr<<"wrong to open file\n";exit(1);}
    string line,line1,line2;
    while(getline(in,line) && getline(in1,line1)&&getline(in2,line2))
    {
        cerr<<count++<<endl;
        cerr<<line2<<endl;
        line=half2full(line); line1=half2full(line1);line2=half2full(line2);
        SynTree *mytree=new SynTree(line2);
        mytree->ParseTree(line1,0);
        //mytree->PrintTree(&mytree->root);
        cerr<<"syn tree has been parsed\n";
        RSTTree rstTree;
        rstTree.MyConstructRSTTree(line);
        cerr<<"rst tree has been parsed\n";

        if(rstTree.wordVec.size()!=mytree->GetSize()){cerr<<"wrong in file\n";cerr<<rstTree.wordVec.size()<<" "<<mytree->GetSize()<<endl;
        copy(rstTree.wordVec.begin(),rstTree.wordVec.end(),ostream_iterator<string,char>(cout," "));
        cout<<endl;

        exit(1);}
        DataItem item(rstTree.wordVec);
        FeatExt extracter;
        extracter.FeatIntial(rstTree);
        extracter.GetTrainFeat_1(line,mytree,item);
        extracter.PrintTrainFeatures();
//==============================
        //mytree->ClearTree();
        //extracter.Clear();
    }
}
void test_2(const char*filename,const char* filename_syn,const char*dec_pos,const char*dec_conn,const char*keywordfile)
{
    ifstream in(filename_syn);
    ifstream in1(filename);
    int count(1);
    if(!in ){cerr<<"wrong to open file: "<<filename_syn<<endl;;exit(1);}
    string line,line1;
    Resource objR;
    objR.LoadKeyWordSet(keywordfile);

    FeatExt extracter;
    extracter.DecisionLearn(dec_pos);
    extracter.LoadLM(dec_conn);
    while(getline(in,line) && getline(in1,line1))
    {
        line=half2full(line); line1=half2full(line1);
//        cerr<<line<<endl;
//        cerr<<line1<<endl;
        cerr<<count++<<endl;
        SynTree *mytree=new SynTree(line1);


        if(mytree->ParseTree(line,0)<0){cout<<"0 ||| "<<line1<<endl;continue;}
        if (mytree->GetSize()==0){cout<<line1<<endl;continue;}
        cerr<<"tree has been parsed\n";
       // DataItem item(mytree->sentenceVec);
        DataItem item(line1);
        cerr<<item.sentenceVec.size()<<endl;

        vector<TestRes> resVec=extracter.DecideForEachSegCharUnit(line1,item,objR,mytree);
        cerr<<resVec.size()<<endl;
        extracter.DecideConnForNewSegCharUnit(resVec,item,objR);
//==============================
        //mytree->ClearTree();
        //extracter.Clear();

    }
}
void test_3(const char*filename_rst)
{
    ifstream in(filename_rst);

    int count(1);
    if(!in ){cerr<<"wrong to open file\n";exit(1);}
    string line,mark;
    map<string,vector<int> > biIndexMap;
    vector<string> dataVec;int pos(0);
    while(getline(in,line))
    {
        cerr<<count<<endl;
        dataVec.push_back(line);
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
            cout<<mark<<' ';
            PrintRSTTree(item,rstTree.RSTNodeVec);}

        ++count;

    }


}
void test_4(const char*filename_rst)
{
    PostDecodeClass obj;
    obj.LoadAndPrintRSTTree(filename_rst);
}

void test_5(const char*filename_rst,const char*conn)
{// for make statistics for each relationship
    LittleFuncClass obj;
    obj.LoadRSTFile(filename_rst);
    //obj.PrintRelAndCount();
    obj.HierarchicalSample_forConn(15,conn);
}
void test_6(const char*filename_rst)
{
     ifstream in(filename_rst);

    int count(1);
    if(!in ){cerr<<"wrong to open file\n";exit(1);}
    string line,mark,line1;
    int pos;
    while(getline(in,line1))
    {
        cerr<<count<<endl;
        line=half2full(line1);
        pos=line.find(" ");
        mark=line.substr(0,pos);
        pos=line.find_first_not_of(" ",pos+1);
        line = line.substr(pos);

        RSTTree rstTree;
        rstTree.MyConstructRSTTree(line);
        cerr<<"rst tree has been parsed\n";

        if(rstTree.RSTNodeVec[0].leftChild!=NULL)
        {
            vector<RSTNode *>vec;
            vec = rstTree.GetAllRSTTree();
            if (vec.size() > 4 )
                cout<<line1<<endl;
        }
        ++count;

    }

}
void test_7(const char*autofile,const char*reffile)
{
    LittleFuncClass obj;
    obj.ComputeAccAndRecall(autofile,reffile);
}
void test_8(const char*rstfile)
{
    LittleFuncClass obj;
    obj.LoadRSTFile(rstfile);
    obj.ComputeRSTFileDistribution();
}
void Baseline(const char* orifile, const char* synfile)
{
    BaselineClass obj;
    obj.Divide(orifile,synfile);
}
int main(int argc,char *argv[])
{
    //test();
    string prefix="/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/";
//    argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/parser/iter1.1.rst");
//    argv[2]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/parser/chtbout-1059.par");
//    argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/parser/chtbout-1059.seg");
 //   argv[1]="/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/test.rst";
//    argv[2]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/test.aligned");
//     argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/test.ori");
//    argv[2]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/test.syn");
 //   argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/fbis/dis.ch.new");
 //   argv[2]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/fbis/dis.ch.ber.new");
  //  argv[1]=("/home/mtu/workdir/chtb6.0/chtb6.0/data/utf8/chtb.seg");
 //   argv[2]=("/home/mtu/workdir/chtb6.0/chtb6.0/data/utf8/chtb.par");
//    argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/cwmt/tst08");
//    argv[2]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/cwmt/tst08.10parse");
//    argv[3]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/dec_pos_6");
//    argv[4]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/conn_rel_6");
//    argv[5]="/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/keyword_6";
 //   argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/testout/out.6.recalled.4");
//    argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/testout/recalled.0314");
//    argv[2]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/testout/recalled.0314.aligned");
//    argv[3]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/testout/recalled.0314.en");
   // argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/testout/train1000.rst.num.newfm");
//    argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/parser/chtbout-1059.test");
//    argv[2]="/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/conn_rel_6";
    //  argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/testout/recalled.0120.revised");
    argv[1]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/parser/testset.auto");
    argv[2]=("/home/mtu/workdir/discourse/bigmodel/src/dtlearn/bin/Debug/parser/testset.revised.1");
   // if(argc != 6){cerr<<argv[0]<<" <original file> <syntax file> <dec_pos> <conn_rel> <keywordfile>\n";exit(1);}
    //cerr<<argv[1]<<'\n'<<argv[2]<<'\n'<<argv[3]<<'\n'<<argv[4]<<'\n';
   // test_1(argv[1],argv[2],argv[3]);
 //   test_2(argv[1],argv[2],argv[3],argv[4],argv[5]);
   //test_3(argv[1]);
  // test(argv[1],argv[2],argv[3]);
   // test_4(argv[1]); //generate post3
  // test_5(argv[1],argv[2]);
   test_7(argv[1],argv[2]);
  //test_8(argv[1]);
  //Baseline(argv[1],argv[2]);
}




