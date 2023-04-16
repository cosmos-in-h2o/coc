#include <iostream>
import cxc;
int main(int argc,char** argv) {
    cxc::ParserConfig config;
    config.app_version="1.0.0";
    cxc::Log log;
    cxc::Parser parser;
    parser.loadConfig(&config);
    parser.loadLog(&log);
    return parser.run(argc,argv);
}