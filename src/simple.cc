//
// Created by dream on 5/19/23.
//
#include<cstdio>
#include<vector>
#include <string>
import coc;

using namespace std;
using namespace coc;

void test(Options*o,Arguments*a,Values*v,vector<string>&argv){
    for(auto i:o->get_list()){
        printf("%s\n",i->name.c_str());
    }
}
void test2(Options*o,Arguments*a,Values*v,vector<string>&argv){
    printf("hello world");
}
int main(int argc,char**argv){
    Parser p;
    p.addAction("test","describe",test,'t')
            ->addOption("opt1","des",2233,'s')
            ->addOption("opt2","des",2233,'f');
    p.addAction("print","describe",test2,'p');
    return p.run(argc,argv);
}