//example.cc
#include <iostream>
#include <functional>
#include <vector>
#include <string>

#include "coc.hpp"
//import coc;
#include <chrono>
using namespace std;
using namespace coc;
void test(Getter g){
    g.get_opt()->getOption("foption");
    g.get_opt()->getOption("soption");
    g.get_opt()->getOption("toption");
    g.get_arg()->getString("farg","farg");
    g.get_arg()->getString("sarg","sarg");
    g.get_arg()->getString("targ","targ");
    g.get_tar()->at("foption",0,"default");
    g.get_tar()->at("foption",1,"default");
    g.get_tar()->at("foption",2,"default");
    g.get_tar()->at("soption",0,"default");
    g.get_tar()->at("soption",1,"default");
    g.get_tar()->at("soption",2,"default");
    g.get_tar()->at("toption",0,"default");
    g.get_tar()->at("toption",1,"default");
    g.get_tar()->at("toption",2,"default");
}

int main(int argc,char**argv) {
    auto *config = new ParserConfig;
    Log *log = new Log;
    Parser parser(config, log);
    IHelpFunc *hf=new HelpFunc(config,&parser);
    parser.addAction("test1","this is a test",test)
            ->addOption("foption","",1,'f')
            ->addOption("soption","",2,'s')
            ->addOption("toption","",3,'t');
    parser.addAction("test2","this is a test",test)
            ->addOption("foption","",1,'f')
            ->addOption("soption","",2,'s')
            ->addOption("toption","",3,'t');
    parser.addAction("test3","this is a test",test)
            ->addOption("foption","",1,'f')
            ->addOption("soption","",2,'s')
            ->addOption("toption","",3,'t');
    parser.addAction("test4","this is a test",test)
            ->addOption("foption","",1,'f')
            ->addOption("soption","",2,'s')
            ->addOption("toption","",3,'t');
    parser.addArgument("farg","string","")
            ->addArgument("sarg","string","")
            ->addArgument("targ","string","");

    auto start = std::chrono::high_resolution_clock::now();

    int result=parser.run(argc, argv);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

    // 输出执行时间
    std::cout << duration.count()*1000 << " ms\n";
    return result;
}