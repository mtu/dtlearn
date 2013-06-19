#ifndef INITIALIZATION_H_INCLUDED
#define INITIALIZATION_H_INCLUDED


#include"SynTree.h"

using namespace boost::regex_constants;
using boost::regex;
using boost::regex_match;
using boost::smatch;

class DataItem
{
public:
  vector<string> sentenceVec;
  map<int,int> start2WordId;
  map<int,int> end2WordId;

  DataItem(const string&s)
  {
    stringstream ss(s);
    copy(istream_iterator<string,char>(ss),istream_iterator<string,char>(),back_insert_iterator<vector<string> >(sentenceVec));
    size_t pos(0);
    for(size_t i(0);i<sentenceVec.size();++i)
    {
        if(sentenceVec[i]=="，"){sepVec.push_back(i);}
        pos = s.find(sentenceVec[i],pos);
        start2WordId[pos]=i;pos = pos+sentenceVec[i].length();
        end2WordId[pos]=i;
    }
    ss.str("");
  }
  DataItem(vector<string> &vec)
  {
      sentenceVec  = *(&vec);
      size_t pos(0);
      for(size_t i(0);i<sentenceVec.size();++i)
      {
        if(sentenceVec[i]=="，"){sepVec.push_back(i);}
        start2WordId[pos]=i;pos = pos+sentenceVec[i].length()+1;
        end2WordId[pos-1]=i;
      }
  }
  string GetStrWithId(unsigned start_word, unsigned end_word);
  void GetSubStrVec(vector<string> &result);
  vector<size_t> sepVec;
private:

};

class Resource
{
private:


public:
    void LoadKeyWordSet(const char*file);
    string RegexCompile();
    void TestKeyPhraseSet(const string & s);
    vector<string> keyPhraseVec;
    vector<string> oriKeyPhrase;
    set<string> keyWordSet;

};

class RSTRelation
{
public:
    RSTRelation(){
        rstMap["ELABRATION"]=0;//TOPIC-COMMENTARY
        rstMap["CONTRAST"]=1;
        rstMap["REASON-RESULT"]=2;
        rstMap["JOINT"]=3;
        rstMap["BACKGROUND"]=4;
        rstMap["MEANS"]=5;
        rstMap["PURPOSE"]=6;
        rstMap["CONCESSION"]=7;
        rstMap["EVIDENCE"]=8;
        rstMap["CONDITION"]=9;
        rstMap["CHOICE"]=10;
        rstMap["ENABLE"]=11;
        rstMap["0"]=0;//TOPIC-COMMENTARY
        rstMap["1"]=1;
        rstMap["2"]=2;
        rstMap["3"]=3;
        rstMap["4"]=4;
        rstMap["5"]=5;
        rstMap["6"]=6;
        rstMap["7"]=7;
        rstMap["8"]=8;
        rstMap["9"]=9;
        rstMap["10"]=10;
        rstMap["11"]=11;
    }
    int GetRSTMapId(const string&s){return rstMap[s];}
    bool Find(const string&s){return rstMap.find(s)!=rstMap.end();}
    int GetSize(){return rstMap.size();}
    int Add(const string&s){rstMap[s]=rstMap.size();return (rstMap.size()-1);}

private:
    map<string, int> rstMap;
};
string half2full(const string&s);
#endif // INITIALIZATION_H_INCLUDED
