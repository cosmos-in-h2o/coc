#include <vector>
#include <iostream>
import coc;
using namespace std;

void test(coc::Options* o,coc::Arguments* a,coc::Values* v,vector<string>& s){

}

int main(int argc,char** argv) {

    coc::Parser parser;
    parser.getConfig()->logo_and_version="1.0.0";
    parser.getLog()->not_found_argument="some string";
    parser.getActions()->addAction(" "," ",test,'0');

    return parser.run(argc,argv);
}