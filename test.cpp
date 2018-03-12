#include<iostream>
int nss(int a, int b){
    std::cout<<"A == " <<a<<" B == "<<b<<std::endl;
    if(a == 0)
        return b+1;
    if(b == 0) {
        std::cout<<"Calling NSS with "<<a<<" "<<b<<std::endl;
        return nss(a-1, 1);
    }
    std::cout<<"Calling Big NSS "<<a<<" "<<b<<std::endl;
    return nss(a-1, nss(a, b-1));
}
int main(){
    std::cout<<nss(1,1)<<std::endl;
}
