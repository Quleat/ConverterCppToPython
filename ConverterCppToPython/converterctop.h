#ifndef CONVERTERCTOP_H
#define CONVERTERCTOP_H

#include<iostream>
#include<string>
#include<exception>
#include<vector>

using namespace std;

class emptyExpression : exception{
public:
    char const* what() const;
};

//Base-interface class for expressions
class expressionObj{
public:
    virtual string produce() {return "\0";}
};

//Class for translating variables and literals (Doing nothing)
class obj : public expressionObj {
    string value;
public:
    obj(string in);
    obj(const obj& right);
    void load(string input);
    string produce();
};

//Class for things that do not have representation in python
class nullObj : public expressionObj{
public:
    string produce(){return "";}
};

//Class for output/input work
class ioExpression : public expressionObj{
    expressionObj *object;
    string method;
public:
    ioExpression(string in);
    ioExpression(expressionObj *o, string m);
    string produce();
};

//HUGE class for identifying expressions
class Converter{
public:
    static bool isTrash(string in);
    static bool isIO(string in);

    static expressionObj *ConvertObj(string in);
    static stringstream transformExprs(vector<expressionObj*> &v);
    static stringstream Convert(string in);
};




#endif // CONVERTERCTOP_H
