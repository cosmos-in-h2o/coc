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
    cout<<"targets test:\n";
    cout<<g.get_tar()->at(g.get_arg()->getInt("pos",3),"default")<<'\n';
    cout<<g.get_tar()->size_f()<<'\t'<<g.get_tar()->size("option");
}
void global_action(Getter g){

}

int main(int argc,char**argv) {
    auto *config = new ParserConfig;
    Log *log = new Log;
    Parser parser(config, log);
    IHelpFunc *hf=new HelpFunc(config,&parser);
    parser.addAction("test","this is a test",test)
            ->addOption("option","",2,'o');
    parser.addArgument("pos","string","this is a arg");
    return parser.run(argc, argv);
}