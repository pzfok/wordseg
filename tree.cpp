#include "tree.h"
namespace seg{
	Dict::Dict(){
		root = NULL;
	}
	Dict::~Dict(){
		if( root != NULL){
			FreeTree();	
		}
	}
	/*º”‘ÿ◊÷µ‰*/
	bool Dict::LoadDict(const char *  filepath){	
		if (filepath == NULL) return false;	 
		root = new treenode; 
		FILE *fi = fopen(filepath,"r");	 
		if (fi == NULL){
			commom::LOG_INFO("open dict error");
		}

		char buffer[SHORT_LENTH];  
		std::vector<std::string> temp;
		std::string line = ""; 
		int linenum = 0; 
		if(commom::ReadLine(buffer,SHORT_LENTH,fi)==NULL){
			commom::DEBUG_INFO("");
		}
		while (commom::ReadLine(buffer,SHORT_LENTH,fi)!=NULL)	{	
			node nodeInfo;	
			line = commom::GetLine(buffer); 					
			commom::Split(" ",line, temp);	 
			if((3 < temp.size())||(2 > temp.size()))	 continue;	
			nodeInfo.word = temp[0]; 
			nodeInfo.freq = atoi(temp[1].c_str());
			nodeInfo.wlen=(nodeInfo.word.size() +1)/2;	
			if(!Insert(nodeInfo)) continue; 
			linenum++; 
		} 
		fclose(fi);
		return true;
	}

	void Dict::FreeTree() { 		
		if (!DeleteNode(root)){ 
			commom::LOG_INFO("DeleteNode failed!");	 
		}
		root = NULL; 
	}	

	/*≤Â»Î*/
	bool Dict::Insert(node& nodeInfo){ 
		const std::string& word = nodeInfo.word; 
		intvector intvec;
		if(!commom::StrToVec(word.c_str(), intvec)){
			return false;
		}
		treenode* p =root; 
		for (unsigned int i = 0; i < intvec.size(); i++)	{ 
			uint16_t cu = intvec[i];
			if(NULL == p){ 
				return false; 
			}
			if(p->childlist.end() == p->childlist.find( intvec[i])){
				treenode * next = NULL;	
				next = new treenode;	
				if(NULL == next)	{
					return false; 
				} 
				p->leaftag = false;	
				p->childlist[ intvec[i]] = next;	 
				p = next;	 p->leaftag=true;
			}	else{ 
				p = p->childlist[cu]; 
			}
		}
		if(NULL == p)	{
			return false;
		} 
		nodelist.push_back(nodeInfo); 
		p->index = nodelist.size() - 1;
		p->wordtag=true; 
		return true;
	}

	/*…æ≥˝*/
	bool Dict::DeleteNode(treenode* node)	{ 
		trienodemap::iterator it; 
		if (node->childlist.size()>0){	 
			for ( it  = node->childlist.begin(); 
				it  != node->childlist.end();  it++)	{ 
					DeleteNode(it->second); 
			}
			if(root!=node){
				node->childlist.clear(); 
				delete node; 
			} 
		}
		return true;
	}

	/*≤È’“*/
	node* Dict::find(std::string str){ 
		intvector intvec; 
		bool retflag = commom::StrToVec(str.c_str(), intvec);
		if(!retflag){
			return NULL; 
		}
		if(intvec.empty()){
			return NULL;
		} 
		intvector::const_iterator begin = intvec.begin(); 
		intvector::const_iterator end = intvec.end();
		if(begin >= end){ 
			return NULL; 
		}
		treenode* p = root; 
		for (intvector::const_iterator it = begin; it != end; it++){ 
			uint16_t chuni = *it;
			if (p->childlist.find(chuni) == p->childlist.end()){ 
				intvector().swap(intvec); 
				return NULL; 
			} else	{ 
				p = p->childlist[chuni]; 
			}
		}
		if(p->wordtag==true)	{ 
			int pos = p->index; 
			if(pos < nodelist.size()){
				intvector().swap(intvec);	 
				return &(nodelist[pos]);
			}else	{ 
				intvector().swap(intvec); return NULL; 
			} 
		}
		intvector().swap(intvec);
		return NULL;
	}

	node* Dict::find(intvector& intvec,intvector::const_iterator begin ,intvector::const_iterator end  ){
		if(intvec.empty()){ return NULL; } 
		if(begin >= end)	{ return NULL; }
		treenode* p = root;
		for (intvector::const_iterator it = begin; it != end; it++){
			uint16_t chuni = *it; 
			if (p->childlist.find(chuni) == p->childlist.end()){
				intvector().swap(intvec); return NULL; 
			}else	{
				p = p->childlist[chuni]; 
			} 
		}	
		if(p->wordtag==true){ 
			int pos = p->index;
			if(pos < nodelist.size())	{ 
				intvector().swap(intvec); 
				return &(nodelist[pos]); 
			}else	{
				intvector().swap(intvec);
				return NULL; 
			} 
		} 
		intvector().swap(intvec);	
		return NULL;
	}

}