#include "seg.h"
namespace seg{
	Wordseg::Wordseg(){}
	Wordseg::~Wordseg(){}	
	void Wordseg::InitDict(const char* dictpath){
		path = dictpath;
		std::string segdictpath = path + "dict";
		mydict.LoadDict(segdictpath.c_str());
		mysymbol.Init(path.c_str());
		//commom::DEBUG_INFO(commom::ConvertToStr(mysymbol.m_connet.size()));
	}
	bool Wordseg::CheckNoeWord(treenode* r,uint16_t x){
		if(r->childlist.end() == r->childlist.find(x)){	
			return true;
		}
		trienodemap::iterator it = r->childlist.find(x);	r = it->second;	
		if((r->wordtag== true)&&(mydict.nodelist[r->index].freq>10000)){
			return true;
		}
		return false;
	}

	//初始化距离
	void Wordseg::InitDistence(float ** D, intvector &part){
		int wordlen = part.size();
		long long twd = 400000000000LL;
		for(int i= 0 ,h=0; i<wordlen ; ++i, ++h ){   
			treenode* p =mydict.root;	int j = i;
			while(p->childlist.end()!=p->childlist.find(part[j])){
				trienodemap::iterator it = p->childlist.find(part[j]);
				p = it->second;
				if(p->wordtag==true){										
					float temp = mydict.nodelist[p->index].freq;	temp=temp/twd;
					D[h][j]=0 - log(temp)/sqrt(float(mydict.nodelist[p->index].wlen));
				}
				if(j+1<wordlen)	j++;
			}
		}
	}

	//检查中文姓名
	void Wordseg::CheakChineseName(float ** D, intvector &part){
		int ii = 0;
		for (intvector::iterator it = part.begin(); it!= part.end(); it++,ii++){
			if( mysymbol.CheckName(*it)){
				unsigned x = 0;	float snf = 0;
				float a = 0.4;
				if(ii + 1 <  part.size()){
					snf= mysymbol.namefre[commom::VecToStr( part, ii+1, ii+2)];
					D[ii][ii+1] = std::min(D[ii][ii+1],float(350/(pow(snf,a))));
				}
				if(ii + 2 <  part.size()){					
					float lnf= mysymbol.namefre[commom::VecToStr( part, ii+2, ii+3)];
					if(lnf <= 0)continue;
					treenode *r = mydict.root;
					if(!CheckNoeWord(r,part.at(ii+2))){
						if(0!=snf){
							D[ii][ii+2]= std::min(D[ii][ii+2], float((100/pow(snf,a))*110/pow(lnf,a))) ;
						}
					}
				}			
			}
		}
	}

	//Hmm分词
	std::string Wordseg::Unit(std::map<unsigned int , unsigned int>& position, intvector& part){		
		int lastunicode = 0;
		bool single = false;
		std::string str = "";
		for(std::map<unsigned int , unsigned int> :: iterator it = position.begin(); it != position.end(); it++){
			str =(str == ""? str : str+" ") + commom::VecToStr( part, it->first, it->second);
		}
		return str;
	}

	//解图模型
	std::string Wordseg::Solve(float ** D,intvector& part, int wordlen){
		std::map<unsigned int , unsigned int> position;
		float score=0;
		std::map<float,std::string>dmap;
		float * worddistance = new float[wordlen];//
		std::string*strdistance = new std::string[wordlen];
		float distance=0;
		std::string pathstr="";
		for(int i=0; i<wordlen; ++i){
			distance = D[0][i];
			pathstr =  commom::ConvertToStr(i);	
			for(int k = 0; k<i; ++k){   
				if (distance > (worddistance[k] + D[k+1][i])){
					distance = worddistance[k] + D[k+1][i];//加入路径
					pathstr = strdistance[k] + commom::ConvertToStr(i);	
				}
			}
			worddistance[i] = distance;
			strdistance[i] = pathstr + "/";
		}
		std::string wa = strdistance[wordlen-1];
		int temp_n=0;
		while(std::string::npos != wa.find("/")){
			std::string temp_sttr= std::string(wa,0,wa.find("/"));
			wa=std::string(wa,wa.find("/")+1);
			position.insert(std::pair<unsigned int , unsigned int>(temp_n,(atoi(temp_sttr.c_str())+1)));
			temp_n=atoi(temp_sttr.c_str())+1;			
		}
		delete []worddistance;
		delete []strdistance;		
		return Unit(position,part);
	}
	//检查句子,去标点，并以连接符切分
	void Wordseg::Check(intvector& line, std::vector<intvector>& part){
		treenode* p =mydict.root;
		part.clear();
		intvector temp;
		for(intvector::iterator it = line.begin(); it != line.end(); ){
			if(p->childlist.end()==p->childlist.find(*it)){
				it = line.erase(it);
				if(temp.size() != 0){
					part.push_back(temp);
					temp.clear();
				}
			}
			else{
				
					temp.push_back(*it);
					it++;
			}
		}
		if(temp.size() != 0){
			part.push_back(temp); temp.clear();
		}
	}

	//单元分词	
	std::string Wordseg::SegStr(intvector& part){		
		//\65279
		const unsigned int wordlen = part.size();
		float ** D = new float *[wordlen];
		for (int i=0; i<wordlen; i++){	
			D[i] = new float[wordlen];	
			for(int x = i; x < wordlen;  x++){
				D[i][x] = 100000*(0.9+x-i);
			} 
		}
		InitDistence(D, part);
		CheakChineseName(D, part);
		//mhmm.HmmSeg(D,part);
		std::string restr = Solve(D,part,wordlen);
		for(int v = 0; v<wordlen; ++v){
			delete[]D[v];
		}
		delete[] D;
		return restr;
	}

	//多路径分词
	std::string Wordseg::SegAll(intvector& part, int x){	
		string str = "";
		string restr = "";
		for(int i = 0; i< part.size(); i++){
			for(int j = i+1; j <part.size(); j++){
				str = commom::VecToStr(part,i,j);
				if(mydict.find(str) != NULL){
					if(j-i > x){
						if(restr != ""){
							restr += (" " + str);
						}else{
							restr = (str);
						}						
					}
				}
			}
		}
		return restr;
	}

	std::string Wordseg::_SegStr(intvector& part){		
		//\65279
		const unsigned int wordlen = part.size();
		float ** D = new float *[wordlen];
		for (int i=0; i<wordlen; i++){	
			D[i] = new float[wordlen];	
			for(int x = i; x < wordlen;  x++){
				D[i][x] = 100000*(0.9+x-i);
			} 
		}
		InitDistence(D, part);
		std::string restr = Solve(D,part,wordlen);
		for(int v = 0; v<wordlen; ++v){
			delete[]D[v];
		}
		delete[] D;
		return restr;
	}

	std::string Wordseg::JionCon(string& str){
		string restr = "";
		std::vector<string> v;
		std::string conectstr ="";
		commom::Split(" ", str, v);
		for(int i =0; i< v.size(); i++){
			if((v.at(i).size() == 1)&&(mysymbol.CheckConnet(commom::StrToInt(v.at(i))))){
				conectstr += v.at(i);
			}else{		
				if(restr == ""){
					if(conectstr == ""){
						restr = v.at(i);
					}else{
						restr = conectstr + " " + v.at(i);
						conectstr = "";	
					}
				}else{
					if(conectstr == ""){
						restr += (" " + v.at(i));
					}else{
						restr += (" " + conectstr + " " + v.at(i));
						conectstr = "";	
					}
				}
			}

		}
		if(conectstr != ""){
			if(restr == ""){
				restr += conectstr;
			}else{
				restr += (" " + conectstr);
			}
		}
		return restr;		
	}

	std::string Wordseg::Segement(const char* str){
		std::string strret = "";
		intvector line;
		std::vector<intvector> part;
		if(!commom::StrToVec(str,  line)){	
			return ""; 
		}
		Check(line,  part);
		if(part.size() == 0) {
			return "";
		}	
		std::vector<std::string> arg;
		for(int j =0; j< part.size(); j++){				
			if(part.at(j).size() == 1){
				arg.push_back(commom::VecToStr(part.at(j), 0, 1));
			}else{
				arg.push_back(SegStr(part.at(j)));
			}
		}		
		for(int index = 0; index < arg.size(); index++){
			if(strret == ""){
				strret = JionCon(arg.at(index));
			}else{
				strret += (" " +JionCon(arg.at(index)));
			}
		}
		return strret;
	}
	std::string Wordseg::Segement(const char* str,int x){
		string strret = "";
		intvector line;
		std::vector<intvector> part;
		if(!commom::StrToVec(str,  line)){	
			return ""; 
		}
		Check(line,  part);
		if(part.size() == 0) {
			return "";
		}	
		std::map<std::string,int> arg;
		strret = Segement(str);
		//commom::Split(" ", strret,arg);
		for(int j =0; j< part.size(); j++){				
			if(part.at(j).size() == 1){
				strret += (" " + commom::VecToStr(part.at(j), 0, 1));
			}else{
				strret += (" " + SegAll(part.at(j),x));
			}
		}		
		commom::Split(" ", strret,arg);
		strret = "";
		for(std::map<std::string,int>::iterator it = arg.begin(); it != arg.end(); it++){
			if(strret == ""){
				strret = it->first;
			}else{
				strret += (" " + it->first);
			}
		}
		return strret;
	}

	std::string Wordseg::QuickSegement(const char* str){
		std::string strret = "";
		intvector line;
		std::vector<intvector> part;
		if(!commom::StrToVec(str,  line)){	
			return ""; 
		}
		Check(line,  part);
		if(part.size() == 0) {
			return "";
		}	
		std::vector<std::string> arg;
		for(int j =0; j< part.size(); j++){
			if(part.at(j).size() == 1){
				arg.push_back(commom::VecToStr(part.at(j), 0, 1));

			}else{
				arg.push_back(_SegStr(part.at(j)));
				
			}
		}
		for(int index = 0; index < arg.size(); index++){
			if(strret == ""){
				strret = JionCon(arg.at(index));
			}else{
				strret += (" " +JionCon(arg.at(index)));
			}
		}
		return strret;
	}
}

