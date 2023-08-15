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
