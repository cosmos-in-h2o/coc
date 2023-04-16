#include <iostream>
import cxc;
int main(int argc,char** argv) {
    cxc::ParserConfig config;
    config.app_version="1.0.0";
    cxc::Log log;
    cxc::Parser parser(argc,argv);
    parser.loadConfig(&config);
    parser.loadLog(&log);
    parser.addAction("install","install some pkg",'i');
    parser.addOption("stable","is stable pkg",'s');
    return parser.run(argc,argv);
}