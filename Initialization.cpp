
#include"Initialization.h"
/*=================================================================================================*/
std::string DataItem::GetStrWithId(unsigned start_word,unsigned end_word)
{
    stringstream ss;
    if((int) start_word < 0){cerr<<"wrong start-word : "<<int (start_word)<<endl;}
    copy(sentenceVec.begin()+start_word,sentenceVec.begin()+end_word+1, ostream_iterator<string,char>(ss," "));
    string r=ss.str();
    return r.substr(0,r.length()-1);
}

void DataItem::GetSubStrVec(vector<string>&result)
{
    stringstream ss; size_t i(0);
    if(this->sepVec.empty()){copy(sentenceVec.begin(),sentenceVec.end(), ostream_iterator<string,char>(ss," "));result.push_back(ss.str());return;}
    else if(this->sepVec.size()==1 && (this->sepVec[0]==0 || this->sepVec[0]==sentenceVec.size()-1)) {copy(sentenceVec.begin(),sentenceVec.end(), ostream_iterator<string,char>(ss," "));result.push_back(ss.str());return;}
    else{
        if(sepVec.front()!=0){copy(sentenceVec.begin(),sentenceVec.begin()+sepVec.front()+1, ostream_iterator<string,char>(ss," "));result.push_back(ss.str());ss.str("");}
        for(i=1;i<sepVec.size();++i)
        {
            copy(sentenceVec.begin()+sepVec[i-1]+1,sentenceVec.begin()+sepVec[i]+1, ostream_iterator<string,char>(ss," "));
            result.push_back(ss.str());ss.str("");
        }
        if(sepVec.back()!=sentenceVec.size()-1){copy(sentenceVec.begin()+sepVec.back()+1,sentenceVec.end(), ostream_iterator<string,char>(ss," "));result.push_back(ss.str());ss.str("");}
    }
}

/*==============================================================================================*/
void Resource::LoadKeyWordSet(const char*file)
{
    ifstream in(file);
    if(!in.is_open()){cerr<<"wrong to open file: "<<file<<'\n';exit(1);}

    string line;
    string rp1 = "\\[X\\]";
    string rp2="\\s";
    regex rg1(rp1),rg2(rp2);
	string rgx_result("");
    boost::match_results<std::string::const_iterator> mr;
    bool CHECK1,CHECK2;

    while(getline(in,line))
    {
        rgx_result="";CHECK1 = regex_search(line,mr,rg1);CHECK2=regex_search(line,mr,rg2);
        if(!CHECK1 && !CHECK2)keyWordSet.insert(line);
        else
        {
            oriKeyPhrase.push_back(line);
            if(CHECK1)
            {
                boost::regex_replace(back_inserter(rgx_result),line.begin(),line.end(),rg1," (.*) ");
                line=rgx_result;rgx_result="";
            }
            if(CHECK2)
            {
                boost::regex_replace(back_inserter(rgx_result),line.begin(),line.end(),rg2," ");
                line=rgx_result;rgx_result="";
            }
            keyPhraseVec.push_back("(^| )("+line+")( |$)");
        }
    }
    cerr<<"keyWordSet size is: "<<keyWordSet.size()<<endl;
    cerr<<"keyPhraseSet size is: "<<keyPhraseVec.size()<<endl;
}
string Resource::RegexCompile()
{
    return "";
}
void Resource::TestKeyPhraseSet(const string& s)
{
    vector<string>::iterator it = keyPhraseVec.begin();
    boost::match_results<string::const_iterator> mr;
    while(it != keyPhraseVec.end()){
        if(regex_search(s,mr,regex(*it++)))
            cerr<< mr[0] <<endl;
        else cerr<<"no"<<endl;
    }
}
string half2full(const string&s)
{
    using namespace boost;
    return replace_all_copy(s,",","，");
}
