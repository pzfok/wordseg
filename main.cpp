#include "seg.h"
#include "../commom/func.h"
int main(int argc, char *argv[]) {
	if(argc != 4){
		std::cout<<"argv[1] : dict_path"<<std::endl;
		std::cout<<"argv[2] : input_file"<<std::endl;
		std::cout<<"argv[3] : output_file"<<std::endl;
	}else{
		seg::Wordseg myseg;
		myseg.InitDict(argv[1]);
		FILE* fi = fopen(argv[2],"r");
		FILE* fo = fopen(argv[3], "ab+");
		if((fi == NULL)|(fo == NULL)){
			commom::LOG_INFO("open file error");
		}
		std::string str;
		char buffer[MAX_LENTH];
		while (commom::ReadLine(buffer,MAX_LENTH,fi)!=NULL){
			str = commom::GetLine(buffer);
			commom::WiteLine((myseg.Segement(str.c_str()) + "\n").c_str(), fo);
		}
		fclose(fi);
		fclose(fo);
	}
	return 0;
}