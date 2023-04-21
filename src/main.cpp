#include <iostream>
import coc;

void test(coc::Options* o,coc::Arguments* a,coc::Values* v){

}

int main(int argc,char** argv) {

    coc::Parser parser;
    parser.getConfig()->logo_and_version="1.0.0";
    parser.getLog()->not_found_argument="some string";
    parser.getActions()->addAction(" "," ",test,'0');

    return parser.run(argc,argv);
}