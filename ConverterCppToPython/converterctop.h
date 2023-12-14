#ifndef CONVERTERCTOP_H
#define CONVERTERCTOP_H

#include<iostream>
#include<string>
#include<exception>
#include<vector>
#include<map>
#include<set>


/*THINGS TO DO:
 *
 * SECOND LEVEL:
 *  PREFIX/POSTFIX INCREMENT/DECREMENT
 *  SWITCH??
 *  CICLE-FOR (MAYBE JUST COUNTING)
 *  CICLE-WHILE
 *  CICLE OPERATORS
 *
 * THIRD LEVEL:
 *  BLOCK OPERATORS
 *  MEHTOD INVOKE
 *
*/


using namespace std;

enum class ioType {input, output};

enum class exprType {varExpr, nullExpr, ioExpr, initExpr, mathExpr, logicExpr, ifExpr, methodExpr,
                      /*TO be finished:*/ cicleExpr, /*?*/ methodInvokeExpr};

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
    virtual vector<string> produce() {return {"\0"};}//vector because one line in c++, may be converted to many in python
    virtual ~expressionObj();
};

//Class for translating variables and literals (Doing nothing)
class varExpression : public expressionObj {
    string value;
public:
    varExpression(string in);
    varExpression(const varExpression& right);
    void load(string input);
    vector<string> produce();
};

//Class for things that do not have representation in python
class nullExpression : public expressionObj{
public:
    vector<string> produce(){return {""};}
};

//Class for output/input work
class ioExpression : public expressionObj{
    expressionObj *object;
    string method;
public:
    ioExpression(string in);
    ioExpression(expressionObj *o, string m);
    vector<string> produce();

    ~ioExpression();
};

//Class for initializing variables
class initComponent : public expressionObj{
    string left;
    expressionObj *right = NULL;
    bool empty = true;
public:
    initComponent(string in);
    vector<string> produce();

    ~initComponent();
};

class initExpression : public expressionObj{
    vector<initComponent*> inits;
    string type = "NOT FOUND";
public:
    initExpression(string in);
    vector<string> produce();

    ~initExpression();
};

//Class for math calculations
class mathExpression : public expressionObj{
    string equation;
    static set<string> mathOps;
public:
    mathExpression(string in);
    vector<string> produce();
};

//Class for logic calculations
class logicExpression : public expressionObj{
    string equation;
    static vector<pair<string, string>> logToLog;
public:
    logicExpression(string in);
    vector<string> produce();
};

//Class for explicit type conversions
class typeConversionExpression : public expressionObj{
    string name;
public:
    typeConversionExpression(string in);
    vector<string> produce();
};

class ifExpression : public expressionObj{
    enum class ifTypes;

    logicExpression* condition;
    vector<expressionObj*> ifs;
    vector<ifExpression*> elseIfs;
    vector<expressionObj*> elses;

    size_t endPoint = 0;

    string getStrCondition(size_t index, string &in, size_t &endCondition);
    ifTypes deduceNextComponent(size_t index, string &in);
    string getStrInnerExpression(const size_t &index, string &in, size_t& endInnerExpr);

    void convertElseAndIf(size_t index, string &in);
    void convertElseIf(size_t index, string &in);
    void convertElse(size_t index, string &in);
public:
    ifExpression(logicExpression* cond, vector<expressionObj*> exprs) : condition(cond), ifs(exprs)
    { }
    ifExpression(size_t index, string &in);
    size_t getEnd();
    string produceAsComponent();
    vector<string> produce();

    ~ifExpression();
};

//Class for initializing methods
class methodExpression : public expressionObj{
    vector<expressionObj*> exprs;
    string prototype;
    size_t endIndex = -1;
public:
    methodExpression(size_t index, string &in); //Because we cannot know where the method ends, until analyzing what's inside
    size_t getEnd();
    vector<string> produce();

    ~methodExpression();
};

//HUGE class for identifying expressions
class Converter{
    static map<string, exprType> keyWords;
public:
    static expressionObj* ConvertExpr(string in);
    static stringstream transformExprsToStr(vector<expressionObj*> &v);
    static stringstream TranslateOuter(string in);
    static vector<expressionObj*> ConvertInner(string in);
};




#endif // CONVERTERCTOP_H
