#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
struct token{
	//rPeren, lPeren, dollar, symbolID, integer, whitespace
	std::string tokenType;
	std::string tokenText;
};

class SExp{
	int type;                //1: integer atom; 2: symbolic atom; 3: non-atom
	int val;                 //if type 1
	string name;             //if type 2
	SExp* left; SExp* right; //if type 3;
}

//read std::cin until $$ is encountered
//then split by $ into a vector
std::vector<std::string> readInput(){
	std::string exps;
	std::vector<std::string> expAsStr;
	
	char cur;
	bool finished = false;
	std::string s;
	while(!finished && std::getline(std::cin, s)){
		for(char cur : s){
			if(exps.length() > 0 && 
		   	   cur == '$' && exps[exps.length()-1] == '$'){
				finished = true;
				break;
			   }
			exps += cur;
		}
		exps += "\n";
	}
	
	size_t pos = 0;
	std::string token;
	while ((pos = exps.find("$")) != std::string::npos) {
    		token = exps.substr(0, pos);
    		expAsStr.push_back(token);
    		exps.erase(0, pos + 1);
	}
	
	return expAsStr;
}
std::vector<token> tokenize(std::string s){
	std::vector<token> tokenizedStr;
	std::cout<<"Tokenizing String:"<<std::endl;
	std::cout<<s<<std::endl;
	std::cout<<"End String"<<std::endl;
	for(auto it = s.begin(); it!=s.end(); ++it){
		if(*it == '('){
			token t{.tokenType = "lPeren", .tokenText="("};
			tokenizedStr.push_back(t);
		}
		else if(*it == ')'){
			token t{.tokenType = "rPeren", .tokenText=")"};
			tokenizedStr.push_back(t);
		}
		else if(isspace(*it)){
			token t{.tokenType = "whitespace", .tokenText=" "};
			tokenizedStr.push_back(t);
		}
		else if(*it == '.'){
			token t{.tokenType = "dot", .tokenText="."};
			tokenizedStr.push_back(t);	
		}
		else if(isdigit(*it)){
			auto beg = it;
			for(auto itt = it; it != s.end() && isdigit(*itt); itt++)
				it = itt;
			std::string s(beg, it+1);
			token t{.tokenType = "int", .tokenText=s};
			tokenizedStr.push_back(t);
		}
		else if(isalpha(*it)){
			auto beg = it;
			//allows for alphanumeric symbolic IDs as long as they start with a letter
			for(auto itt = it; it != s.end() && isalnum(*itt); itt++)
				it = itt;
			std::string s(beg, it+1);
			token t{.tokenType = "symbolId", .tokenText=s};
			tokenizedStr.push_back(t);
		}
		else{
			std::cout<<"Unrecognized Token"<<std::endl;
		}
		
	}
	return tokenizedStr; 
}
SExp sExpresconvertToInternalRep(){
	
}
int main(){
	std::vector<std::string> s = readInput();
	std::vector<std::vector<token>> tokenizedExprs;
	for(auto exp : s){
		tokenizedExprs.push_back(tokenize(exp));
	}
	
	for(auto exp : tokenizedExprs){
		convertToInternalRep();
	}
}
