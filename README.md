# coc
a cmd parser library for c++\
[video](https://www.bilibili.com/video/BV1Ej411B7Wb/)
## log
### v1.0.0
- complete basic function.
### v1.1.0
- fix some bugs.
- separation of interface and implementation.
### v1.2.0
- fix large bugs.
- different classes are divided into different files.
## Overview 
[example.cc](example/example.cc):
```c++
#include "coc/coc.hpp"
#include <iostream>
using namespace coc;
using namespace std;

void action1(Getter g) {

    cout << "option test:\n";
    cout << g.get_opt()->at(0) << '\n';
    cout << g.get_opt()->getOption('o') << '\n';
    cout << g.get_opt()->isOnlyOpt("option") << '\n';
    for (auto iter: g.get_opt()->get_list()) {
        cout << iter->name << '\n';
    }
}
void action2(Getter g) {
    cout << "value test:\n";
    cout << g.get_val()->get<std::string>("value1") << '\n';
    cout << g.get_val()->get<int>("value2") << '\n';
}
void action3(Getter g) {
    cout << "target test:\n";
    cout << g.get_tar()->at(0, "default target");
    cout << g.get_tar()->at("fseparator", 0, "default target");
    cout << g.get_tar()->atAbsoluteIndex(3, "default target");
    cout << g.get_tar()->atOutOfRange("sseparator", 2, "default target");
}

void test(Getter g){
    if(g.get_opt()->getOption('o')){
        std::cout<<"true";
    }else{
        std::cout<<"false";
    }
    std::cout<<"default";
}

int main(int argc, char **argv) {
    auto config = new ParserConfig;
    auto log = new PrefabParserLog;
    Parser parser = Parser(config, log);
    parser.addAction("action1", "this is a action", action1, 'a')
            ->addOption("option", "this is a option", 1, 'o')
            ->addOption("another", "this is another option", 2, 'a');
    parser.addAction("action2", "this is a action", action2)
            ->addValue("value1", "please input", "string", "this is a value")
            ->addValue("value2", "please input", "int", "this is a value", "111");
    parser.addAction("action3", "this is a action", action3)
            ->addOption("fseparator", "this is a separator", 1, 'f')
            ->addOption("sseparator", "this is a separator", 1, 's');
    parser.addAction("test","",test,'t')
            ->addOption("option","",1,'o')
            ->addValue("value","please enter value","string","","value");
    parser.set_global_action([](Getter g) {
              if (g.is_empty) {
                  std::cout << "empty";
                  return;
              }
              cout << "global and argument:\n";
              cout << g.get_arg()->get<std::string>("arg", "default argument") << std::endl;
              cout << g.get_arg()->get<std::string>("arg", "aaa") << endl;
              cout << g.get_val()->get<std::string>("value1");
          })->addValue("value1", "value", "", "", "value");
    parser.addArgument("arg", "string", "this is a arg");

    return parser.run(argc, argv);
}
```
Compile it.Then input some instruction in terminal to know how coc works:\
`./<execute_file> action1 -oa` or `./<execute_file> a -oa` or `./<execute_file> action1 --option` or ......\
`./<execute_file> action2`\
`./<execute_file> action3 -f tar1 tar2 tar3 -s tar4 tar5 tar6` or ......\
`./<execute_file> -Darg=arg`\
`......`\
You can get more interface to use coc by intellisense.
## Details 
### Memory
You'd better create coc::Parser object on stack,because coc can't collect it by itself.If you want to create it on heap,you can write like:
```cpp
......
int main(int argc,char** argv){
    auto config=new coc::ParserConfig;
    auto log=new coc::IParserLog;
    coc::Parser parser=new coc::Parser(config,log);
    int return_value = parser.run(argc,argv);
    delete parser;
    return return_value;
}
```
Another problem is ParserConfig and IParserLog object will be collected by Parser,include this situation:
```cpp
......
parser.loadConfig(new coc::ParserConfig);
......
```
old coc::ParserConfig will be collected at once,the new object will be collected when the Parser object be collected.(loadLog(IParserLog*) and defaultConfig(void) are same)
### HelpAction
coc give a simple way to create HelpAction:
```cpp
......
coc::IHelpAction* hf=new PrefabHelpAction(&parser,config);
parser.addHelpAction("help","",hf);
......
```
then,you can add some Option,Value or Argument to make effect more clear.Finally,compile it and input`./<execute_file> help`.
