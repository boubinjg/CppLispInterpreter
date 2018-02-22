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

//read std::cin until $$ is encountered
//then split by $ into a vector
void print(SExp* e){
	//std::cout<<"in print"<<std::endl;
	//std::cout<<e->val<<std::endl;
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
std::vector<token> tokenize(std::string s){
	std::vector<token> tokenizedStr;
	for(auto it = s.begin(); it!=s.end(); ++it){
		if(*it == '('){
			token t{.tokenType = "lParen", .tokenText="("};
			tokenizedStr.push_back(t);
		}
		else if(*it == ')'){
			token t{.tokenType = "rParen", .tokenText=")"};
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
size_t findMidDot(std::vector<token> exp){
	std::cout<<"inMidDot"<<std::endl;
	int depth = 0;
	for(auto it = exp.begin(); it!=exp.end(); ++it){
		if(it->tokenType == "lParen")
			++depth;
		else if(it->tokenType == "rParen")
			--depth;
		else if(it->tokenType == "dot" && depth==0){
			std::cout<<it-exp.begin()<<std::endl;
			return (it-exp.begin())+1;
		}
	}
	return exp.begin()-exp.end();
}
std::vector<token> findCarandCdr(std::vector<token>& exp){
	exp.erase(exp.begin());
	exp.erase(exp.end()-1);

	std::vector<token> car, cdr;

	if(exp[0].tokenType == "int"){
		car.push_back(exp[0]);
		exp.erase(exp.begin());		
	} else if(exp[0].tokenType == "symbolId"){

	}

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
SExp* convertToInternalRep(std::vector<token> exp){
	SExp *e = new SExp;
	std::cout<<"in"<<std::endl;
	trim(exp);
	if(exp.size() == 1){
		if(exp[0].tokenType == "int"){
			e->type = 1;
			e->val = std::stoi(exp[0].tokenText);
			//std::cout<<"e1"<<std::endl;
			return e;
		}
		else if(exp[0].tokenType == "symbolId"){
			e->type = 2;
			e->name = exp[0].tokenText;
			//std::cout<<"e2"<<std::endl;
			return e;
		}
	}
	else if(exp[0].tokenType == "lParen" && exp[exp.size()-1].tokenType == "rParen"){
		//auto nextDot = std::find_if(exp.begin()+1, exp.end()-1, [](token t){return t.tokenType == "dot";});
		auto nextDot = findMidDot(std::vector<token>(exp.begin()+1, exp.end()-1));

		std::vector<token> car = findCarAndCdr(exp);		

		e->type = 3;
		std::cout<<(exp.begin()+1)->tokenType<<std::endl;
		std::cout<<(exp.begin()+nextDot-1)->tokenType<<std::endl;

		e->left = convertToInternalRep(std::vector<token>(exp.begin()+1, exp.begin()+nextDot));
		std::cout<<"next2"<<std::endl;
		e->right = convertToInternalRep(std::vector<token>(exp.begin()+nextDot+1, exp.end()-1));
		//std::cout<<"e3"<<std::endl;
		return e;
	} else {
		//std::cout<<exp[0].tokenType<<" "<<exp[0].tokenText<<std::endl;
		//std::cout<<exp[exp.size()-1].tokenType<<" "<<exp[exp.size()-1].tokenText<<std::endl;
		std::cout<<"Not an S Expression"<<std::endl;
		print(exp);
		//std::cout<<"e4"<<std::endl;
		return e;
	}
	//std::cout<<"e5"<<std::endl;
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
		print(e);
		//std::cout<<e->left->val<<std::endl;
	}
}
