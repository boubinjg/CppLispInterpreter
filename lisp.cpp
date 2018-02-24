#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
struct token{
	//rParen, lParen, dollar, symbolID, integer, whitespace
	std::string tokenType;
	std::string tokenText;
};

struct SExp{
	int type;                //1: integer atom; 2: symbolic atom; 3: non-atom
	int val;                 //if type 1
	std::string name;        //if type 2
	SExp* left; SExp* right; //if type 3;
};

SExp* convertList(std::vector<token> exp);
SExp* convertToInternalRep(std::vector<token> exp);

//read std::cin until $$ is encountered
//then split by $ into a vector
void print(SExp* e){
	if(e->type == 3){
		std::cout<<"(";
		print(e->left);
		std::cout<<".";
		print(e->right);
		std::cout<<")";
	}
	else if(e->type == 2){
		std::cout<<e->name;
	}
	else if(e->type == 1){
		std::cout<<e->val;
	}
}
void print(std::vector<token> exp){
	for(auto i : exp){
		std::cout<<i.tokenType<<" "<<i.tokenText<<std::endl;
	}
}
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
bool isAtom(token t){
	return (t.tokenType == "int" || t.tokenType == "symbolId");
}
void trim(std::vector<token>& exp){
	auto wsfront = exp.begin()-1, wsend = exp.end();
	for(auto it = exp.begin(); it!=exp.end(); it++){
		if(it->tokenType == "whitespace")
			wsfront = it;
		else
			break;
	}
	for(auto it = exp.end()-1; it!=exp.begin(); it--){
		if(it->tokenType == "whitespace")
			wsend = it;
		else
			break;
	}
	//erase back first so iterator aren't invalidated
	if(wsend != exp.end()){
		exp.erase(wsend, exp.end());
	}
	if(wsfront != exp.begin()-1){
		exp.erase(exp.begin(), wsfront+1);
	}
	
}
std::vector<token> tokenize(std::string s){
	std::vector<token> tokenizedStr;
	for(auto it = s.begin(); it!=s.end(); ++it){
		if(*it == '('){
			token t{.tokenType = "lParen", .tokenText="("};
			tokenizedStr.push_back(t);
		} else if(*it == ')'){
			token t{.tokenType = "rParen", .tokenText=")"};
			tokenizedStr.push_back(t);
		} else if(isspace(*it)){
			token t{.tokenType = "whitespace", .tokenText=" "};
			tokenizedStr.push_back(t);
			auto beg = it;
			for(auto itt = it; it != s.end() && isspace(*itt); itt++)
				it = itt;
		} else if(*it == '.'){
			token t{.tokenType = "dot", .tokenText="."};
			tokenizedStr.push_back(t);	
		} else if(isdigit(*it)){
			auto beg = it;
			for(auto itt = it; it != s.end() && isdigit(*itt); itt++)
				it = itt;
			std::string s(beg, it+1);
			token t{.tokenType = "int", .tokenText=s};
			tokenizedStr.push_back(t);
		} else if(isalpha(*it)){
			auto beg = it;
			//allows for alphanumeric symbolic IDs as long as they start with a letter
			for(auto itt = it; it != s.end() && isalnum(*itt); itt++)
				it = itt;
			std::string s(beg, it+1);
			token t{.tokenType = "symbolId", .tokenText=s};
			tokenizedStr.push_back(t);
		} else{
			std::cout<<"Unrecognized Token"<<std::endl;
		}
	}
	return tokenizedStr; 
}
std::vector<token> findCarAndCdr(std::vector<token>& exp){
	exp.erase(exp.begin());
	exp.erase(exp.end()-1);

	std::vector<token> car, cdr;
	trim(exp);
	
	if(exp[0].tokenType == "rParen" || exp[0].tokenType == "dot") {
		std::cout<<"Invalid Token"<<std::endl;
	} else if(exp[0].tokenType == "int" || exp[0].tokenType == "symbolId"){
		car.push_back(exp[0]);
		exp.erase(exp.begin());		
	} else if(exp[0].tokenType == "lParen"){
		auto end = exp.begin() + 1;
		int level = 0;
		for(auto it = exp.begin(); it != exp.end(); it++){
			if(it->tokenType == "rParen") {
				++level;
			} else if(it->tokenType == "lParen") {
				--level;
			}
			if(level == 0){
				end = it;
				break;
			}
		}
		car = std::vector<token>(exp.begin(), end+1);
		exp.erase(exp.begin(), end+1);
	}
	//std::cout<<"CAR"<<std::endl;
	//print(car);
	
	if(exp.size() == 0){
		token t{.tokenType = "symbolId", .tokenText="NIL"};
		cdr.push_back(t);
	} else if(exp[0].tokenType == "whitespace"){
		if(exp[1].tokenType == "dot") {
			cdr = std::vector<token>(exp.begin()+2, exp.end());
			trim(cdr);
		} else
			cdr = std::vector<token>(exp.begin(), exp.end());
	} else if(exp[0].tokenType == "dot"){
		cdr = std::vector<token>(exp.begin()+1, exp.end());
		trim(cdr);
	} else
		std::cout<<"error1"<<std::endl;
	//std::cout<<"CDR"<<std::endl;
	//print(cdr);

	exp = cdr; //great practice, thumbs up.
	return car;
}
SExp* convertList(std::vector<token> exp){
	trim(exp);
	SExp *e = new SExp;
	e->type = 3;
	if(exp.size() > 1){
		if(isAtom(exp[0])) {		
			e->left = convertToInternalRep(std::vector<token>(exp.begin(), exp.begin()+1));
			e->right = convertList(std::vector<token>(exp.begin()+1, exp.end()));
		} else if(exp[0].tokenType == "lParen"){
			auto rp = exp.begin();
			int level = 0;
			for(auto it = exp.begin(); it != exp.end(); it++){
				if(it->tokenType == "rParen"){
					++level;
				} else if(it->tokenType == "lParen"){
					--level;
				}
				if(level == 0){
					rp = it;
					break;
				}	
			}
			if(rp == exp.begin()){
				std::cout<<"error2"<<std::endl;
			} else if(rp+1 == exp.end()){
				SExp *nil = new SExp;
				nil->type = 2;
				nil->name = "NIL";
				e->left = convertToInternalRep(std::vector<token>(exp.begin(), rp+1));
				e->right = nil;
			} else {
				e->left = convertToInternalRep(std::vector<token>(exp.begin(), rp+1));
				e->right = convertList(std::vector<token>(rp+1, exp.end()));
			}	
		} else {	
			std::cout<<"error3"<<std::endl;
			print(exp);
		}
	} else if(isAtom(exp[0])) {
		SExp *nil = new SExp;
		nil->type = 2;
		nil->name = "NIL";
		
		e->left = convertToInternalRep(std::vector<token>(exp.begin(), exp.begin()+1));
		e->right = nil;
	}
	return e;
}
SExp* convertToInternalRep(std::vector<token> exp){
	SExp *e = new SExp;
	trim(exp);
	if(exp.size() == 1){
		if(exp[0].tokenType == "int"){
			e->type = 1;
			e->val = std::stoi(exp[0].tokenText);
			return e;
		} else if(exp[0].tokenType == "symbolId"){
			e->type = 2;
			e->name = exp[0].tokenText;
			return e;
		}
	} else if(exp[0].tokenType == "lParen" && exp[exp.size()-1].tokenType == "rParen"){
		//find null list
		if(exp.size() == 2 || (exp.size() == 3 && exp[1].tokenType == "whitespace")){ 
			e->type = 2;
			e->name = "NIL";
		} else {

			std::vector<token> car = findCarAndCdr(exp);		
			std::vector<token> cdr = exp;

			e->type = 3;
			e->left = convertToInternalRep(car);
			if(cdr[0].tokenType == "whitespace"){
				e->right = convertList(cdr);
			} else {
				e->right = convertToInternalRep(cdr);
			}
		}
		return e;
	} else {
		std::cout<<"error 4"<<std::endl;
		print(exp);
		return e;
	}
	return e;
}
int main(){
	std::vector<std::string> s = readInput();
	std::vector<std::vector<token>> tokenizedExprs;
	for(auto exp : s){
		tokenizedExprs.push_back(tokenize(exp));
	}
	
	for(auto exp : tokenizedExprs){
		SExp* e = convertToInternalRep(exp);
		findCarAndCdr(exp);
		print(e);
	}
}
