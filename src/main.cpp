#include <vector>
#include <iostream>
import coc;
using namespace coc;
using namespace std;
void test(Options * o,Arguments* a, Values* v, vector<string>&vec){
    o->at(1);

}
int main(int argc,char** argv) {
    coc::Parser parser;
    return parser.run(argc,argv);
}
