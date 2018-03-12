//Author: Jayson Boubin, Spring 2018
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<stdexcept>
#include<unordered_map>
#include"lisp.h"
//read std::cin until $$ is encountered
//then split by $ into a vector
void output(SExp* e){
    if(e == NULL){ /*c++0x..."*/
        return;
    } else if(e->type == 3){
		std::cout<<"(";
		output(e->left);
		std::cout<<".";
		output(e->right);
		std::cout<<")";
	} else if(e->type == 2){
		std::cout<<e->name;
	} else if(e->type == 1){
		std::cout<<e->val;
	}
}
void print(std::vector<token> exp){
	for(auto it = exp.begin(); it != exp.end(); it++){
		std::cout<<it->tokenType<<" "<<it->tokenText<<std::endl;
	}
}
bool read(std::string& s){
    bool finished = false;
    std::string nl;
    while(true){
        std::getline(std::cin, nl);
        if(nl == "$"){
            //std::cout<<"dollar"<<std::endl;
            break;
        } else if(nl == "$$"){
            //std::cout<<"2dollar"<<std::endl;
            finished = true;
            break;
        } else {
            //std::cout<<"Else: "+nl<<std::endl;
            s += nl+"\n";
        }
    }
    return finished;
}
bool isAtom(token t){
	return (t.tokenType == "int" || t.tokenType == "symbolId");
}
void trim(std::vector<token>& exp){
	if(exp.size() < 1){
		return;
	}
	
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
		if(*it == '(') {
			token t("lParen", "(");
			tokenizedStr.push_back(t);
		} else if(*it == ')') {
			token t("rParen",")");
			tokenizedStr.push_back(t);
		} else if(isspace(*it)) {
			token t("whitespace"," ");
			tokenizedStr.push_back(t);
			for(auto itt = it; it != s.end() && isspace(*itt); itt++)
				it = itt;
		} else if(*it == '.') {
			token t("dot", ".");
			tokenizedStr.push_back(t);	
		} else if(isdigit(*it)) {
			auto beg = it;
			for(auto itt = it; it != s.end() && isdigit(*itt); itt++)
				it = itt;
			std::string s(beg, it+1);
			token t("int",s);
			tokenizedStr.push_back(t);
		} else if(*it == '+') { 
	        auto beg = it+1;
		    for(auto itt = beg; it != s.end() && isdigit(*itt); itt++)
		        it = itt;
		    if(it == beg-1)
		        throw std::runtime_error("invalid '+' token placement");
            std::string s(beg, it+1);
            token t("int", s);
            tokenizedStr.push_back(t);
        } else if(*it == '-') {
            auto beg = it;
		    for(auto itt = beg+1; it != s.end() && isdigit(*itt); itt++)
		        it = itt;
		    if(beg == it)  
		        throw std::runtime_error("Invalid '-' token placement");
            std::string s(beg, it+1);
            token t("int", s);
            tokenizedStr.push_back(t);
        } else if(isalpha(*it)) {
			auto beg = it;
			//allows for alphanumeric symbolic IDs as long as they start with a letter
			for(auto itt = it; it != s.end() && isalnum(*itt); itt++){
				it = itt;
				if (isalpha(*it)) {*itt = std::toupper(*itt);}
			}
			std::string s(beg, it+1);
			token t("symbolId", s);
			tokenizedStr.push_back(t);
		} else {
			throw std::runtime_error("Invalid Token: "+(*it));
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
		throw std::runtime_error("Invalid Token");
	} else if(exp[0].tokenType == "int" || exp[0].tokenType == "symbolId"){
		car.push_back(exp[0]);
		exp.erase(exp.begin());		
	} else if(exp[0].tokenType == "lParen" && exp.size() > 1){
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
	} else {
		throw std::runtime_error("Invalid Token");
	}
	//std::cout<<"CAR"<<std::endl;
	//print(car);
	
	if(exp.size() == 0){
		token t("symbolId", "NIL");
		cdr.push_back(t);
	} else if(exp[0].tokenType == "whitespace"){
		if(exp[1].tokenType == "dot") {
			cdr = std::vector<token>(exp.begin()+2, exp.end());
			trim(cdr);
		} else
			cdr = std::vector<token>(exp.begin(), exp.end());
	} else if(exp[0].tokenType == "dot" && exp.size() > 1){
		cdr = std::vector<token>(exp.begin()+1, exp.end());
		trim(cdr);
	} else
		throw std::runtime_error("Invalid end to S-Expression");
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
		if(isAtom(exp[0]) && exp[1].tokenType == "whitespace") {
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
				throw std::runtime_error("");
			} else if(rp+1 == exp.end()){
				if(usedIds.find("NIL") != usedIds.end()){
					e->right = usedIds["NIL"];
				} else {
					SExp *nil = new SExp;
					nil->type = 2;
					nil->name = "NIL";
					e->right = nil;
					usedIds["NIL"] = nil;
				}
				e->left = convertToInternalRep(std::vector<token>(exp.begin(), rp+1));
			} else {
				e->left = convertToInternalRep(std::vector<token>(exp.begin(), rp+1));
				e->right = convertList(std::vector<token>(rp+1, exp.end()));
			}	
		} else {	
			delete e;
			throw std::runtime_error("Invalid start to S-expression");
		}
	} else if(isAtom(exp[0])) {
		if(usedIds.find("NIL") != usedIds.end()){
			e->right = usedIds["NIL"];
		} else {
			SExp *nil = new SExp;
			nil->type = 2;
			nil->name = "NIL";
			e->right = nil;
			usedIds["NIL"] = nil;
		}
		e->left = convertToInternalRep(std::vector<token>(exp.begin(), exp.begin()+1));
	} else {
		throw std::runtime_error("Invalid Token");
		delete e;
	}
	return e;
}
SExp* convertToInternalRep(std::vector<token> exp){
	SExp *e = new SExp;
	trim(exp);
	if(exp.size() == 0){
		//throw std::runtime_error("empty expression");
		return NULL;
	} else if(exp.size() == 1){
		if(exp[0].tokenType == "int"){
			e->type = 1;
			e->val = std::stoi(exp[0].tokenText);
			return e;
		} else if(exp[0].tokenType == "symbolId"){
			if(usedIds.find(exp[0].tokenText) != usedIds.end()){
				SExp *ret = usedIds[exp[0].tokenText];
				delete e;
				return ret;
			} else {
				e->type = 2;
				e->name = exp[0].tokenText;
				usedIds[exp[0].tokenText] = e;
			}
			return e;
		} else {
			throw std::runtime_error("Invalid Token");
		}
	} else if(exp[0].tokenType == "lParen" && exp[exp.size()-1].tokenType == "rParen"){
		//find null list
		if(exp.size() == 2 || (exp.size() == 3 && exp[1].tokenType == "whitespace")){ 
			if(usedIds.find("NIL") != usedIds.end()){
				e = usedIds["NIL"];
			} else {
				e->type = 2;
				e->name = "NIL";
				usedIds["NIL"] = e;
			}
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
		throw std::runtime_error("Invalid Token");
		return e;
	}

	return e;
}
void bindDlist(SExp* arg){
    SExp* dl = new SExp(arg, dlist);
    dlist = dl;
}
//places variable at top of Alist
void bind(SExp*& alist, std::string name, SExp* arg){
    SExp* argName = new SExp(name);
    SExp* argVal = arg->left;
    SExp* ret = new SExp(argName, argVal);
    SExp* newAlist = new SExp(ret, alist);
    alist = newAlist;
}
void bindFP(SExp*& alist, std::string name, SExp* arg){
    SExp* argName = new SExp(name);
    SExp* argVal = arg;
    SExp* ret = new SExp(argName, argVal);
    SExp* newAlist = new SExp(ret, alist);
    alist = newAlist;
}
SExp* find(std::string name, SExp* alist){
    while(alist!=usedIds["NIL"]){
        if(alist->left->left->name == name)
            return alist->left->right;
        else
            alist = alist->right;
    }
    throw std::runtime_error("Invalid Function");
}
size_t argCt(SExp* e){
    if(e->type != 3){
        throw std::runtime_error("Not a function call");
    }
    size_t ret = 0;
    while(e->right!=usedIds["NIL"]){
        e = e->right;
        ++ret;
    }
    return ret;
}
bool isList(SExp* e){
    while(e != usedIds["NIL"]){
        if(e->type != 3){
            return false;
        } 
        e = e->right;
    }
    return true;
}
SExp* eval(SExp* e, SExp* alist){
    if(e->type == 1)
        return e;
    else if(e->type == 2){
        //correct behavior?
        SExp* als = alist;
        if(e->name == "T" || e->name == "NIL"){
            return usedIds[e->name];
        }
        while(als != usedIds["NIL"]) {
            if(als->left->left->name == e->name) {
                return als->left->right;
            }
            als = als->right;
        }

        throw std::runtime_error("UNBOUND ATOM");
    }
    else if(e->type == 3) {
        //function application
        if(!isList(e)){
            throw std::runtime_error("Not a valid lisp expression");
        }
        if(e->left->name == "CAR") {
            if(argCt(e) != 1)
                throw std::runtime_error("Wrong number of argumants: CAR");
            bind(alist, "CARA", e->right);
            return car(e->right, alist);
        } else if(e->left->name == "CDR") {
            if(argCt(e) != 1)
                throw std::runtime_error("Wrong number of arguments: CDR");
            bind(alist, "CDRA", e->right);
            return cdr(e->right, alist);
        } else if(e->left->name == "CONS") {
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: CONS");
            bind(alist, "CONSA", e->right);
            bind(alist, "CONSB", e->right->right);
            return cons(e, alist);
        } else if(e->left->name == "ATOM"){
            if(argCt(e) != 1)
                throw std::runtime_error("Wrong Number of arguments: ATOM");
            bind(alist, "ATOMA", e->right);
            return atom(e, alist);
        } else if(e->left->name == "EQ"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: EQ");
            bind(alist, "EQA", e->right);
            bind(alist, "EQB", e->right->right);
            return eq(e, alist);
        } else if(e->left->name == "NULL"){
            if(argCt(e) != 1)
                throw std::runtime_error("Wrong Number of arguments: NULL");
            bind(alist, "NULL", e->right);
            return null(e, alist);
        } else if(e->left->name == "INT"){
            if(argCt(e) != 1)
                throw std::runtime_error("Wrong Number of arguments: INT");
            bind(alist, "INTA", e->right);
            return intF(e, alist);
        } else if(e->left->name == "PLUS"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: PLUS");
            bind(alist, "PLUSA", e->right);
            bind(alist, "PLUSB", e->right->right);
            return plus(e, alist);
        } else if(e->left->name == "MINUS"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: MINUS");
            bind(alist, "MINA", e->right);
            bind(alist, "MINB", e->right->right);
            return minus(e, alist);
        } else if(e->left->name == "TIMES"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: TIMES");
            bind(alist, "TIMA", e->right);
            bind(alist, "TIMB", e->right->right);
            return times(e, alist);
        } else if(e->left->name == "QUOTIENT"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: QUOTIENT");
            bind(alist, "QUOA", e->right);
            bind(alist, "QUOB", e->right->right);
            return quotient(e, alist);
        } else if(e->left->name == "REMAINDER"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: REMAINDER");
            bind(alist, "REMA", e->right);
            bind(alist, "REMB", e->right->right);
            return remainder(e, alist);
        } else if(e->left->name == "LESS"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: LESS");
            bind(alist, "LESA", e->right);
            bind(alist, "LESB", e->right->right);
            return less(e, alist);
        } else if(e->left->name == "GREATER"){
            if(argCt(e) != 2)
                throw std::runtime_error("Wrong Number of arguments: GREATER");
            bind(alist, "GREA", e->right);
            bind(alist, "GREB", e->right->right);
            return greater(e, alist);
        } else if(e->left->name == "QUOTE"){
            if(argCt(e) != 1)
                throw std::runtime_error("Wrong Number of arguments: QUOTE");
            bind(alist, "QUOTE", e->right);
            return quote(e, alist);
        } else if(e->left->name == "COND"){
            size_t argcount = argCt(e);
            if(argcount == 0)
                throw std::runtime_error("Wrong Number of arguments: COND");
            SExp* sto = e;
            for(size_t i = 0; i<argcount; i++) {
                if(argCt(sto->right->left) != 1)
                    throw std::runtime_error("Wrong Number of arguments: COND");
                bind(alist, "CONDB" + std::to_string(i),  
                     sto->right->left);
                bind(alist, "CONDE" + std::to_string(i),
                     sto->right->left->right);
                sto = sto->right;
            }
            return cond(e, alist, argcount);
        } else if(e->left->name == "DEFUN") {
            if(argCt(e) != 3)
                throw std::runtime_error("Wrong Number of arguments: DEFUN");
            
            SExp* params = new SExp(e->right->right->left,
                            e->right->right->right->left); 
            if(!isList(params->left))
                throw std::runtime_error("Function Params must be a list");
            SExp* def = new SExp(e->right->left, params);
            if(e->right->left->type != 2)
                throw std::runtime_error("DEFUN: First arg is not a function name");
            bindDlist(def);   
            return e->right->left;
        } else if(e->left->type == 2) {
            std::string name = e->left->name;
            SExp* func = find(name, dlist);
            SExp* params = func->left;
            SExp* body = func->right;

            //get names of each arg from params
            std::vector<std::string> paramNames;
            SExp* sto = params;
            while(sto != usedIds["NIL"]){
                paramNames.push_back(sto->left->name);
                sto = sto->right;
            }   
            if(argCt(e) != paramNames.size())
                throw std::runtime_error("Incorrect Number of Args in Call to User Defined Func");
            sto = e->right;
            std::vector<SExp*> argList;
            for(size_t i = 0; i<paramNames.size(); i++){
                SExp* arg = eval(sto->left, alist);
                argList.push_back(arg);
                sto = sto->right;
            }
            sto = e->right;
            for(size_t i = 0; i<paramNames.size(); i++){
                bindFP(alist, paramNames[i], argList[i]);
            }
            SExp* ret = eval(body, alist);
            return ret;
        }
    }
    throw std::runtime_error("Invalid Lisp Expression");
}
int main(){
	bool end = false;
	SExp* NIL = new SExp;
	NIL->type = 2;
	NIL->name = "NIL";
	usedIds["NIL"] = NIL;
	SExp* T = new SExp;
	T->type = 2;
	T->name = "T";
	usedIds["T"] = T;
    dlist = NIL;

    while(!end){
	    std::string s;
	    end = read(s);
        try{
            //read S expression
            SExp* e = convertToInternalRep(tokenize(s));
            //print S expression
            output(e);
            std::cout<<std::endl;
            
            //evaluate S expression
            if(e != NULL){
                SExp* o = eval(e, NIL);
                output(o);
                std::cout<<std::endl;
            }

        } catch(std::runtime_error e){
            std::cerr<<"Exception: "<<e.what()<<std::endl;
        }
	}
}
