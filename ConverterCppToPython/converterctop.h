#ifndef CONVERTERCTOP_H
#define CONVERTERCTOP_H

#include<iostream>
#include<string>
#include<exception>
#include<vector>
#include<map>


using namespace std;

enum class ioType {input, output};

enum class exprType {varExpr, nullExpr, ioExpr, /*TO be finished:*/ initExpr, mathExpr, logicExpr, cicleExpr,
                      ifExpr, methodExpr, /*?*/ methodInvokeExpr};
typedef pair<string, exprType> p;

class unhandledExprException : exception{
public:
    char const* what() const;
};

class emptyExprException : exception{
public:
    char const* what() const;
};

//Base-interface class for expressions
class expressionObj{
public:
    virtual string produce() {return "\0";}
};

//Class for translating variables and literals (Doing nothing)
class varExpression : public expressionObj {
    string value;
public:
    varExpression(string in);
    varExpression(const varExpression& right);
    void load(string input);
    string produce();
};

//Class for things that do not have representation in python
class nullExpression : public expressionObj{
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
    static map<string, exprType> keyWords;
public:
    static bool isTrash(string in);
    static bool isIO(string in);

    static expressionObj *ConvertExpr(string in);
    static stringstream transformExprsToStr(vector<expressionObj*> &v);
    static stringstream Convert(string in);
};




#endif // CONVERTERCTOP_H
