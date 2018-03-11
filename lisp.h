#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<stdexcept>
#include<unordered_map>
struct token{
	//rParen, lParen, dollar, symbolID, integer, whitespace
	std::string tokenType;
	std::string tokenText;
	token(std::string toktype, std::string toktext) {
		tokenType = toktype;
		tokenText = toktext;
	}
};

struct SExp{
	int type;                //1: integer atom; 2: symbolic atom; 3: non-atom
	int val;                 //if type 1
	std::string name;        //if type 2
	SExp* left; SExp* right; //if type 3;
    SExp(SExp* l, SExp* r){
        type = 3;
        left = l;
        right = r;
    }
    SExp(std::string n){
        type = 2;
        name = n;
    }
    SExp(int v){
        type = 1;
        val = v;
    }
    SExp(){}
};

SExp* convertList(std::vector<token> exp);
SExp* convertToInternalRep(std::vector<token> exp);
std::unordered_map<std::string, SExp*> usedIds;

void output(SExp* e);
void print(std::vector<token> exp);
bool read(std::string& s);
bool isAtom(token t);
void trim(std::vector<token>& exp);
std::vector<token> tokenize(std::string s);
std::vector<token> findCarAndCdr(std::vector<token>& exp);
SExp* convertList(std::vector<token> exp);
SExp* convertToInternalRep(std::vector<token> exp);
SExp* eval(SExp* e, SExp* alist);
void bind(SExp*& alist, std::string name, SExp* arg);
SExp* find(std::string name, SExp* alist);

//primitive functions
SExp* car(SExp* e, SExp* alist){

    e = eval(find("CARA", alist), alist);
    if(e == NULL || e->type != 3)
        throw std::runtime_error("Can't take CAR of atom");
    return e->left;
}
SExp* cdr(SExp* e, SExp* alist){
    e = eval(find("CDRA", alist), alist);
    if(e == NULL || e->type != 3)
        throw std::runtime_error("Can't take CAR of atom");
    return  e->right;
}
SExp* cons(SExp* e, SExp* alist){
    SExp* ret = new SExp;
    ret->type = 3;
    ret->left = eval(find("CONSA",alist), alist);
    ret->right = eval(find("CONSB",alist), alist);
    return ret;
}
SExp* atom(SExp* e, SExp* alist){
    SExp* at = eval(find("ATOMA", alist), alist);
    if(at->type == 2 || at->type == 1){
        return usedIds["T"];
    } 
    return usedIds["NIL"];
}
SExp* eq(SExp* e, SExp* alist){
    SExp* at1 = eval(find("EQA", alist), alist);
    SExp* at2 = eval(find("EQB", alist), alist);
    if(at1->type == 1 && at2->type == 1){
        if(at1->val == at2->val)
            return usedIds["T"];
        else
            return usedIds["NIL"];
    } else if(at2->type == 2 && at2->type == 2){
        if(at1 == at2)
            return usedIds["T"];
        else
            return usedIds["NIL"];
    } 
    throw std::runtime_error("NON-Atom in EQ");
}
SExp* null(SExp* e, SExp* alist){
    SExp* n = eval(find("NULL", alist), alist);
    if(n == usedIds["NIL"])
        return usedIds["T"];
    else
        return usedIds["NIL"];
}
SExp* intF(SExp* e, SExp* alist){
    SExp* n = eval(find("INTA", alist), alist);
    if(n->type == 1)
        return usedIds["T"];
    return usedIds["NIL"];
}
SExp* plus(SExp* e, SExp* alist){
    SExp* na = eval(find("PLUSA", alist), alist);
    SExp* nb = eval(find("PLUSB", alist), alist);
    if(na->type == 1 && nb->type == 1){
        return new SExp(na->val + nb->val);
    } 
    throw std::runtime_error("Non-Ints passed to math op");
}
SExp* minus(SExp* e, SExp* alist){
    SExp* na = eval(find("MINA", alist), alist);
    SExp* nb = eval(find("MINB", alist), alist);
    if(na->type == 1 && nb->type == 1){
        return new SExp(na->val - nb->val);
    } 
    throw std::runtime_error("Non-Ints passed to math op");
}
SExp* times(SExp* e, SExp* alist){
    SExp* na = eval(find("TIMA", alist), alist);
    SExp* nb = eval(find("TIMB", alist), alist);
    if(na->type == 1 && nb->type == 1){
        return new SExp(na->val * nb->val);
    } 
    throw std::runtime_error("Non-Ints passed to math op");
}
SExp* quotient(SExp* e, SExp* alist){
    SExp* na = eval(find("QUOA", alist), alist);
    SExp* nb = eval(find("QUOB", alist), alist);
    if(na->type == 1 && nb->type == 1){
        return new SExp(na->val / nb->val);
    } 
    throw std::runtime_error("Non-Ints passed to math op");
}
SExp* remainder(SExp* e, SExp* alist){
    SExp* na = eval(find("REMA", alist), alist);
    SExp* nb = eval(find("REMB", alist), alist);
    if(na->type == 1 && nb->type == 1){
        return new SExp(na->val % nb->val);
    } 
    throw std::runtime_error("Non-Ints passed to math op");
}
SExp* less(SExp* e, SExp* alist){
    SExp* na = eval(find("LESA", alist), alist);
    SExp* nb = eval(find("LESB", alist), alist);
    if(na->type == 1 && nb->type == 1){
        if(na->val < nb->val)
            return usedIds["T"];
        else
            return usedIds["NIL"];
    } 
    throw std::runtime_error("Non-Ints passed to math op");
}
SExp* greater(SExp* e, SExp* alist){
    SExp* na = eval(find("GREA", alist), alist);
    SExp* nb = eval(find("GREB", alist), alist);
    if(na->type == 1 && nb->type == 1){
        if(na->val > nb->val)
            return usedIds["T"];
        else
            return usedIds["NIL"];
    } 
    throw std::runtime_error("Non-Ints passed to math op");
}
SExp* quote(SExp* e, SExp* alist){
    SExp* q = find("QUOTE", alist);
    return q;
}


