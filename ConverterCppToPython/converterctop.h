#ifndef CONVERTERCTOP_H
#define CONVERTERCTOP_H

#include<iostream>
#include<string>
#include<exception>
#include<vector>
#include<map>


using namespace std;

enum class ioType {input, output};

enum class exprType {varExpr, nullExpr, ioExpr, initConvExpr, mathExpr, logicExpr, /*TO be finished:*/  cicleExpr,
                      ifExpr, methodExpr, /*?*/ methodInvokeExpr};
typedef pair<string, exprType> p;

vector<string> split(string in, string separators);

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

//Class for initializing variables
class initConvExpression : public expressionObj{
    string left;
    expressionObj *right;
    bool typeConv = false;
public:
    initConvExpression(string in);
    string produce();
};

//Class for math calculations
class mathExpression : public expressionObj{
    string equation;
public:
    mathExpression(string in);
    string produce();
};

//Class for logic calculations
class logicExpression : public expressionObj{
    string equation;
    static vector<pair<string, string>> logToLog;
public:
    logicExpression(string in);
    string produce();
};

//Class for blocks of code
class codeBlockExpression : public expressionObj{ //Inherits expressionObj because can be met by itself
    vector<expressionObj*> exprs;
public:
    codeBlockExpression(); //RETURNS THE RIGHT TYPE OF EXPRESSION WITH A CODE BLOCK
    codeBlockExpression(string in);
    codeBlockExpression(expressionObj*);
    codeBlockExpression(vector<expressionObj*>);
    string produce();
};

//Class for initializing methods
class methodExpression : public expressionObj{
    vector<expressionObj*> exprs;
    string prototype;
    size_t endIndex = -1;
public:
    methodExpression(size_t index, string &in); //Because we cannot know where the method ends, until analyzing what's inside
    size_t getEnd();
    string produce();
};

//HUGE class for identifying expressions
class Converter{
    static map<string, exprType> keyWords;
public:
    static expressionObj *ConvertExpr(string in);
    static stringstream transformExprsToStr(vector<expressionObj*> &v);
    static stringstream TranslateOuter(string in);
    static vector<expressionObj*> ConvertInner(string in);
};




#endif // CONVERTERCTOP_H
