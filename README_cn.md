# coc
>！！！正在开发中，bug较多，功能不完善，请暂时不要使用！！！
## 一个基于c++20模块的命令行解析库
coc是一个命令行解析库。\
coc使用c++20模块构建。\
coc使用起来很容易，你只需要在项目中输入"import coc;"就能够使用它了。
## 第一个例子
接下来我会展示一个最基础的使用方法。
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
    addAction("test","this is test's describe.",test);
    return p.run(argc,argv);
}
```
编译上面的程序，得到一个叫main的可执行文件，然后输入`./main test helloworld`，终端就会输出`helloworld`
## Action
## Config
## Log
## Options
如果你想添加一些选项，你只需要在刚刚你添加的Action后面加上 "->addOption(...)"。

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
    p.getActions()->addAction("test","this is test's describe.",test)->
    addOption("dot","put a dot in the end.",7,'d');
    return p.run(argc,argv);
}
```
像刚才一样输入 `./main helloworld`,输出信息也会获得相同的`helloworld`\
但当你输入 `./main -d helloworld` or `./main --dot helloworld`，终端就会打印`helloworld.`
## Value
## Argument