#ifndef CONVERTERCTOP_H
#define CONVERTERCTOP_H

#include<iostream>
#include<string>

using namespace std;


//Base (interface) class for expressions
class expressionObj{
public:
     void load(string input); //I want to make them virtual, but cann't :(
     string produce();
};

//Class for translating variables and literals (Doing nothing)
class obj : public expressionObj {
    string value = "";
public:
    obj();
    obj(string in);
    obj(const obj& right);
    void load(string input);
    string produce();
};

//Class for output/input work
class ioExpression : expressionObj{
    expressionObj object;
    string method;
public:
    ioExpression();
    ioExpression(expressionObj o);
    ioExpression(expressionObj o, string m);
    void load(string input);
    void load(expressionObj o, string m);
    string produce();
};

//HUGE class for identifying expressions
class Converter{
public:
    static bool isTrash(string in);
    static bool isIO(string in);
    static bool isObj(string in);

    static string Convert(string in);
};




#endif // CONVERTERCTOP_H
