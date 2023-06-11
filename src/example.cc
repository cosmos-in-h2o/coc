//example.cc
#include <iostream>
#include <functional>
#include <vector>
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
    cout<<"argv test:\n";
}
void global_action(Getter g){

}
int main(int argc,char**argv) {
    auto *config = new ParserConfig;
    Log *log = new Log;
    Parser parser(config, log);
    IHelpFunc *hf=new HelpFunc(config,&parser);

    parser.addAction("test", "this is a test",test,'t')
            ->addOption("foption", "this is a option", 22)
            ->addOption("soption", "this is a option", 33, 's')
            ->addOption("help","help",11,'h')
            ->addValue("value1", "please input value1", "string", "this is a value")
            ->addValue("value2", "please input value2", "string", "this is a value", "value2");
    parser.addArgument("BOOL", "bool", "this is a bool argument")
            ->addArgument("INT", "int", "this is a int argument")
            ->addArgument("CHAR", "char", "this is a char argument")
            ->addArgument("STRING", "string", "this is a string argument")
            ->addArgument("FLOAT", "float", "this is a float argument");
    parser.set_global_actions(global_action)
            ->addOption("foption", "this is a option", 22, 'f')
            ->addOption("soption", "this is a option", 33, 's')
            ->addValue("value1", "please input value1", "string", "this is a value")
            ->addValue("value2", "please input value2", "string", "this is a value", "value2");
    parser.addHelpAction("help","",hf);
    return parser.run(argc, argv);
    //target/coc test -fs -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
    //target/coc test --foption -s -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
    //target/coc test --foption --soption -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
}