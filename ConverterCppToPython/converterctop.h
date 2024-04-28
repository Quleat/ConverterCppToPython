#ifndef CONVERTERCTOP_H
#define CONVERTERCTOP_H

#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <map>
#include <set>

using namespace std;

enum class ioType {input, output};

enum class exprType {varExpr, nullExpr, ioExpr, initExpr, mathExpr, logicExpr, ifExpr, methodExpr,
                    keyWordExpr, switchExpr, forExpr, whileExpr, methodInvokeExpr};

typedef pair<string, exprType> p;

static string SUPERTEST;
static int globalEndBraceCOUNT = 0;

static map<string, string> varTypes  = {

};

static vector<string> types ={
    "double", "int", "float", "bool", "void", "string",
    "double*", "int*", "float*", "bool*", "void*", "string*",
    "const"
};

vector<string> split(string in);
vector<string> split(string in, string separators);

size_t findBlockEnd(size_t &index, string &in, char symbolBeg, char symbolEnd);
size_t findCodeBlockEnd(size_t index, string &in);
size_t findBraceBlockEnd(size_t index, string &in);

void separateAll(string &in, string what);

class unhandledExprException : exception{
public:
    char const* what() const;
};

class exceedingSymbolsException : exception{
public:
    char const* what() const;
};

class expressionObj{
public:
    virtual std::vector<std::string> produce() {return {"\0"};}
    virtual ~expressionObj();
};

class codeBlockExpression : public expressionObj{
protected:
    vector<expressionObj*> innerExprs;
    string getStrInnerExpression(const size_t &index, string &in, size_t& endInnerExpr);
    size_t endPoint = 0;
public:
    codeBlockExpression();
    codeBlockExpression(size_t index, string &in);
    size_t getEnd();
    vector<string> produce();

    ~codeBlockExpression();
};

class logicExpression : public expressionObj{
    string equation;
    static vector<pair<string, string>> logToLog;
public:
    logicExpression(string in);
    vector<string> produce();
};

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
class forExprsn : public codeBlockExpression{
    initExpression *init;
    logicExpression *check;
    expressionObj *iteration;
public:
    forExprsn(size_t index, string &in);
    vector<string> produce();

    ~forExprsn();
};


class ifExprsn : public codeBlockExpression{
    enum class ifTypes;

protected:
    ifExprsn()
    { }
    logicExpression* condition;
    vector<expressionObj*> ifs;
    vector<ifExprsn*> elseIfs;
    vector<expressionObj*> elses;

    string getStrCondition(size_t index, string &in, size_t &endCondition);
    ifTypes deduceNextComponent(size_t index, string &in);

    void convertElseAndIf(size_t index, string &in);
    void convertElseIf(size_t index, string &in);
    void convertElse(size_t index, string &in);
public:
    ifExprsn(logicExpression* cond, vector<expressionObj*> exprs) : condition(cond), ifs(exprs)
    { }
    ifExprsn(size_t index, string &in);
    string produceAsComponent();
    vector<string> produce();

    ~ifExprsn();
};

//Class for initializing variables




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

class keyWordExpression : public expressionObj{
    string output;
public:
    keyWordExpression(string in );
    vector<string> produce();
};

class mathExpression : public expressionObj{
    string equation;
    vector<pair<string, string>> additionalLines;
    static set<string> mathOps;
    bool isRvalue(string input);
public:

    mathExpression(string in);
    vector<string> produce();
};

class methodExpression : public expressionObj{
    string codeBlockSource;
    string prototype;
    size_t endIndex = -1;
public:
    methodExpression(size_t index, string &in); //Because we cannot know where the method ends, until analyzing what's inside
    size_t getEnd();
    vector<string> produce();
};

class methodUseExprsn : public expressionObj{
    string value;
    vector<expressionObj*> args;
public:
    methodUseExprsn(string in);
    vector<string> produce();
};

class switchExprsn : public ifExprsn{
    void removeKeyWords(string &in);
public:
    switchExprsn(size_t index, string &in);
};

//HUGE class for identifying expressions
class Converter{
    static map<string, exprType> keyWords;
    static vector<pair<string, exprType>> keyOps;
    static void skipCodeBlock(size_t &globalIndex, const vector<string> &rawTokens, int &i, const size_t &endPoint);
    static size_t separateFirst(string &in, size_t off, string what);
    static void separateAll(string &in, string what);
public:
    static void addKeyWords(pair<string, exprType> a);
    static stringstream TranslateOuter(string in);
    static vector<expressionObj*> ConvertInner(string in);
};

class varExpression : public expressionObj {
    string value;
    void tryTrunc(string &in);
public:
    varExpression(string in);
    varExpression(const varExpression& right);
    vector<string> produce();
};

class whileExprsn : public codeBlockExpression{
    logicExpression *check;
public:
    whileExprsn(size_t index, string &in);
    vector<string> produce();

    ~whileExprsn();
};

#endif // CONVERTERCTOP_H
