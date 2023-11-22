#ifndef CONVERTERCTOP_H
#define CONVERTERCTOP_H

#include<iostream>
#include<string>
#include<exception>
#include<vector>
#include<map>


using namespace std;

enum class ioType {input, output};

enum class exprType {varExpr, nullExpr, ioExpr, initExpr, mathExpr, logicExpr, /*TO be finished:*/  cicleExpr,
                      ifExpr, methodExpr, /*?*/ methodInvokeExpr};
typedef pair<string, exprType> p;

static vector<string> parse(string in, string separators);

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
class initExpression : public expressionObj{
    string left;
    expressionObj *right;
public:
    initExpression(string in);
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

//HUGE class for identifying expressions
class Converter{
    static map<string, exprType> keyWords;
    static vector<string> types;
public:
    static bool isTrash(string in);
    static bool isIO(string in);

    static expressionObj *ConvertExpr(string in);
    static stringstream transformExprsToStr(vector<expressionObj*> &v);
    static stringstream ConvertOuter(string in);
    static stringstream ConvertInner(string in);
};




#endif // CONVERTERCTOP_H
