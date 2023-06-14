//example.cc
#include <iostream>
#include <functional>
#include <vector>
#include <string>

//#include "coc.hpp"
import coc;
using namespace std;
using namespace coc;
void test(Getter g){
    if(g.is_empty){
        cout<<"empty\n";
        return;
    }
    cout<<"-----------------------------------\n";
    cout<<"options test:\n";
    for(auto iter:g.get_opt()->get_list()){
        cout<<iter->name<<"\n";
    }
    cout<<"-----------------------------------\n";
    cout<<"arguments test\n";
    cout<<"arguments bool test:"<<g.get_arg()->getBool("BOOL",true)<<"\n";
    cout<<"arguments int test:"<<g.get_arg()->getInt("INT",-1)<<"\n";
    cout<<"arguments string test:"<<g.get_arg()->getString("STRING","string")<<"\n";
    cout<<"arguments char test:"<<g.get_arg()->getChar("CHAR",'t')<<"\n";
    cout<<"arguments float test:"<<g.get_arg()->getFloat("FLOAT",-1.1f)<<"\n";

    cout<<"-----------------------------------\n";
    cout<<"value test:\n";

    cout<<"-----------------------------------\n";
    cout<<"targets test:\n";
    cout<<g.get_tar()->at(0,"default");
}
void global_action(Getter g){

}

void rstringsss(string&& rv){

}

void printString(string str){
    rstringsss(std::move(str));
    cout<<str<<endl;
}
int main(int argc,char**argv) {
    printString("hello world");
    auto *config = new ParserConfig;
    Log *log = new Log;
    Parser parser(config, log);
    IHelpFunc *hf=new HelpFunc(config,&parser);
    return parser.run(argc, argv);
    //target/coc test -fs -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
    //target/coc test --foption -s -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
    //target/coc test --foption --soption -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
}