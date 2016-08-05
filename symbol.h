#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#include "../commom/func.h"
#include <math.h>
#include <algorithm>
namespace seg{
	class Symbol{
	public:
		//////commom::Func f;	;	;			
		intvector m_name;
		intvector m_connet;
		std::map<std::string,unsigned int>namefre;
		std::map<std::string,unsigned int>secondnamefir;
	public:
		Symbol();
		~Symbol();
		void Init(const char* path);
		bool CheckName(uint16_t  x);
		bool CheckConnet(uint16_t  x);				
		bool LoadName(const char *  namepath );		
		bool LoadConnet(const char *  connetpath);
		bool LoadNameFre (const char *  namedir);	

	};
}
#endif 
