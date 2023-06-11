# coc
>!  !  !Under development, with many bugs and incomplete features, please do not use it temporarily !  !  !

>writer's mother tongue is Chinese,so if you can understand Chinese,pleace read the README_cn.md,you will have a better experience.
## a module cmd parser library for c++
coc is a cmd parser library.\
coc use module .\
coc is easy to use, you just need to write down"import coc;"in your program to use coc.
## example
I will give an exmple to explain the most basic method of using.
```c++
//main.cpp

import coc;
import std.core;

using namespace coc;
using namespace std;

void test(coc::Options* o,coc::Arguments* a,coc::Values* v,vector<string>& s){
    cout<<s[0];
}

int main(int argc,char** argv){
    Parser p;
    p.getActions()->
    addAction("test","this is test's intro.",test);
    return p.run(argc,argv);
}
```
After you compile it and get an executable file called main.If you input `./main test helloworld`,the terminal will print `helloworld`
## Config
## Log
## Options
If you want to add some options,you can append "->addOption(...)" behind action that you add just now;

```c++
//main.cpp

import coc;
import std.core;

using namespace coc;
using namespace std;

void test(coc::Options* o,coc::Arguments* a,coc::Values* v,vector<string>& s){
    cout<<s[0];
    if(o->at(0)==7)
        cout<<'.';
}

int main(int argc,char** argv){
    Parser p;
    p.getActions()->addAction("test","this is test's intro.",test)->
    addOption("dot","put a dot in the end.",7,'d');
    return p.run(argc,argv);
}
```
If you still input `./main helloworld`,the terminal will print `helloworld`\
But if you input `./main -d helloworld` or `./main --dot helloworld`,the terminal will print `helloworld.`
## Value
## Argument

## **Plan**
- default help fun
- intellisense mode
- some config
    + shut down unix style
