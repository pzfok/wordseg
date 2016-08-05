#include "hmm.h"
namespace seg{
	Hmm::Hmm(const char *  dictpath){
		transprob = new float*[4];
		for(int i =0; i < 4; i++){
			transprob[i] = new float[4];
		}
		LoadHmm(dictpath);
	}

	Hmm::~Hmm(){
		//delete
	}
	bool Hmm::TrainHmm(const char* dictfile, const char* hmmfile){
		FILE*	fi = fopen(dictfile, "r");
		FILE*	fo = fopen(hmmfile, "ab+");
		char buffer[SHORT_LENTH];
		std::vector<std::string> vec;
		intvector intvec;
		std::string str = "";
		int wordnumber = 0;
		std::map<std::string, int>dict;
		std::map<std::string, int>dictb;
		std::map<std::string, int>dicte;
		std::map<std::string, int>dictm;
		std::map<std::string, int>dicts;
		float b = 0;
		float e = 0;
		float m = 0;
		float s = 0;
		while (commom::ReadLine(buffer,SHORT_LENTH,fi)!=NULL){	
			str = commom::GetLine(buffer);
			if(str == "")continue;			
			commom::Split(" ", str, vec);
			if(vec.size() != 2){
			}else{
				int n = atoi(vec.at(1).c_str());
				if(commom::StrToVec(vec.at(0), intvec) == false){
					continue;
				}
				if(intvec.size() == 1){
					dicts[vec.at(0)] += n;
					dict[vec.at(0)]++;
					s += n;
				}else if(intvec.size() == 2){
					dictb[commom::VecToStr(intvec,0,1)] += n;
					dict[commom::VecToStr(intvec,0,1)] ++;
					b += n;
					dicte[commom::VecToStr(intvec,1,2)] += n;
					dict[commom::VecToStr(intvec,1,2)] ++;
					e += n;
				}else{
					dictb[commom::VecToStr(intvec,0,1)] += n;
					dict[commom::VecToStr(intvec,0,1)] ++;

					b += n;
					for(int j =1; j< intvec.size()-1; j++){
						dictm[commom::VecToStr(intvec,j,j+1)] += n;
						dict[commom::VecToStr(intvec,j,j+1)] ++;
						m += n;
					}
					dicte[commom::VecToStr(intvec,intvec.size()-1,intvec.size())] += n;
					dict[commom::VecToStr(intvec,intvec.size()-1,intvec.size())] ++;
					e += n;
				}
			}
		}
		commom::DEBUG_INFO(commom::ConvertToStr(dict.size()));
		for(std::map<std::string, int>::iterator it = dict.begin(); it != dict.end(); it++){
			str = it->first;
			if(dictb.find(it->first) == dictb.end()){
				str = str +"\t9999";
			}else{
				str = str + "\t" + commom::ConvertToStr(log(b/(dictb[it->first] + 0.1)) );
			}
			if(dicte.find(it->first) == dicte.end()){
				str = str +" 9999";
			}else{
				str = str + " " + commom::ConvertToStr(log(e/(dicte[it->first] + 0.1)) );
			}
			if(dictm.find(it->first) == dictm.end()){
				str = str +" 9999";
			}else{
				str = str + " " + commom::ConvertToStr(log(m/(dictm[it->first] + 0.1)) );
			}
			if(dicts.find(it->first) == dicts.end()){
				str = str +" 9999";
			}else{
				str = str + " " + commom::ConvertToStr(log(s/(dicts[it->first] + 0.1)) );
			}
			str += "\n";
			commom::WiteLine(str.c_str(), fo);
		}
		return true;
	}
	
	bool Hmm::LoadHmm(const char* hmmpath){
		FILE*	fi = fopen(hmmpath, "r");
		char buffer[SHORT_LENTH];
		std::vector<std::string> vec;
		std::vector<std::string> temp;
		intvector intvec;
		std::string str = "";
		int wordnumber = 0;
		if(commom::ReadLine(buffer,SHORT_LENTH,fi)==NULL){
			commom::LOG_INFO("hmm transprob error");
		}
		for(int i =0; i< 4; i++){
			if(commom::ReadLine(buffer,SHORT_LENTH,fi)!=NULL){
				str = commom::GetLine(buffer);
				commom::Split(" ", str, vec);
				if(vec.size() != 4){
					commom::LOG_INFO("hmm transprob error");
				}else{
					for(int j =0; j< vec.size(); j++){
						transprob[i][j] = atof(vec.at(j).c_str());
					}
				}
			}
		}
		while (commom::ReadLine(buffer,SHORT_LENTH,fi)!=NULL){	
			std::string str = commom::GetLine(buffer);
			if(str == "")continue;			
			commom::Split("\t", str, vec);
			if(vec.size() != 2){
			}else{
				commom::Split(" ", vec.at(1), temp);
				if(temp.size() == 4){
					float* emit = new float[4];
					for(int j =0; j<4; j++){
						emit[j] = atof(temp.at(j).c_str());
					}
					emissionprob.insert(std::pair<uint16_t, emission>(commom::StrToInt(vec.at(0)), emit));
					//emissionprob.insert(std::pair<uint16_t, emission>(0, emit));
				}
			}
		}
		fclose(fi);
		commom::DEBUG_INFO(commom::ConvertToStr(emissionprob.size()));
		return true;
	}
	float Hmm::HmmSeg(intvector& v, int bg, int ed){
		if(ed < bg){
			commom::LOG_INFO("hmm index error");
			return 999;
		}
		if(ed - bg == 0){
			return (PS + emissionprob[v.at(bg)][3]) + 5;
		}
		else if(ed - bg == 1){
			return (PB + emissionprob[v.at(bg)][0] +transprob[0][1] + emissionprob[v.at(ed)][1]) + 5;			
		}else{
			float p = PB + emissionprob[v.at(bg)][0] +transprob[0][2] + emissionprob[v.at(bg + 1)][2] ;
			for(int j =2; j< ed-bg; j++){
				p += (transprob[2][2] + emissionprob[v.at(bg + j)][2]);
			}
			p +=((transprob[2][1] + emissionprob[v.at(ed)][1]));
			return p +5*(ed -bg);
		}	
	}
	 
	float Hmm::HmmSeg(std::string& str){
		float p = 0.0;
		intvector line;
		if((!commom::StrToVec(str.c_str(),  line))||(line.size() == 0)){
			return false;
		}
		return p;
	}
	bool Hmm::HmmSeg(std::string& strin, std::string& strout){
		intvector line;
		if((!commom::StrToVec(strin.c_str(),  line))||(line.size() == 0)){
			return false;
		}
		int n = line.size();
		float* score= new float[4*n];
		int* staue = new int[4*n];
		memset(staue, 0, sizeof(int)*4*n);
		memset(score, 0, sizeof(float)*4*n);
		//init 
		score[0] = PB + emissionprob[line.at(0)][0];
		score[1] = PE + emissionprob[line.at(0)][1];
		score[2] = PM + emissionprob[line.at(0)][2];
		score[3] = PS + emissionprob[line.at(0)][3];
		for(int i =1; i< n; i++){
			for(int j =0; j<4; j++ ){
				score[j+i*4] = -99999;
				staue[j+i*4] = -1;
				for(int k =0; k< 4; k++){
					float s = score[k+(i-1)*4] + transprob[k][j] + emissionprob[line.at(i)][j];
					if(s > score[j+i*4] ) {
						score[j+i*4]  = s;
						staue[j+i*4] = k;
					}
				}
			}
		}
		//
		delete score;
		delete staue;
		return true;
	}
	bool Hmm::HmmSeg(float ** D, intvector& part){		
		int lastunicode = 0;
		bool single = false;
		std::string str = "";
		int k = part.size();
		for (int i=0; i<k; i++){	
			//找到第一个非连接点
			int j = k-1;
			for(; j > i; j--){
				if(D[i][j] < 100)break;
			}
			if(j == i){
				D[i][j] = HmmSeg(part, i, j);
			}
			for(int x = j+1; x < k;  x++){
				if(x - i > 2)break;
				if(D[i][x] > 100){
					D[i][x] = HmmSeg(part, i, x);
				}else{
					continue;
				}				
			} 
		}
		return true;
	}
}