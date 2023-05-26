#include <iostream>
#include <vector>
import coc;

using namespace std;
using namespace coc;

void test(Getter g){
    cout<<"-----------------------------------\n";
    cout<<"options test:\n";
    for(auto iter:g.options.get_list()){
        cout<<iter->name<<"\n";
    }
    cout<<"-----------------------------------\n";
    cout<<"arguments test\n";
    cout<<"arguments bool test:"<<g.arguments.getBool("BOOL",true)<<"\n";
    cout<<"arguments int test:"<<g.arguments.getInt("INT",-1)<<"\n";
    cout<<"arguments char test:"<<g.arguments.getChar("CHAR",'t')<<"\n";
    cout<<"arguments string test:"<<g.arguments.getString("STRING","string")<<"\n";
    cout<<"arguments float test:"<<g.arguments.getFloat("FLOAT",-1.1f)<<"\n";

    cout<<"-----------------------------------\n";
    cout<<"value test:\n";
    cout<<"default value:"<<g.values.getString("value1")<<"\n";
    cout<<"value:"<<g.values.getString("value2")<<"\n";

    cout<<"-----------------------------------\n";
    cout<<"argv test:\n";
    for(auto &iter:g.argv){
        cout<<iter<<'\n';
    }
}

int main(int argc,char**argv) {
    ParserConfig *config = new ParserConfig;
    Log *log = new Log;
    Parser parser(config, log);
    parser.addAction("test", "this is a test", test,'t')
            ->addOption("foption", "this is a option", 22, 'f')
            ->addOption("soption", "this is a option", 33, 's')
            ->addValue("value1", "please input value1", "string", "this is a value")
            ->addValue("value2", "please input value2", "string", "this is a value", "value2");
    parser.addArgument("BOOL", "bool", "this is a bool argument")
            ->addArgument("INT", "int", "this is a int argument")
            ->addArgument("CHAR", "char", "this is a char argument")
            ->addArgument("STRING", "string", "this is a string argument")
            ->addArgument("FLOAT", "float", "this is a float argument");

    return parser.run(argc, argv);
    //target/coc test -fs -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
    //target/coc test --foption -s -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
    //target/coc test --foption --soption -DBOOL=false -DINT=2233 -DCHAR=c -DSTRING=s -DFLOAT=22.33 argv argv
}