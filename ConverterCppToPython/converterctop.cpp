#ifndef CONVERTERCTOP_CPP
#define CONVERTERCTOP_CPP

#include "converterctop.h"
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <map>

vector<string> parse(string in, string seps){
    vector<string> res;
    string cpyForStrTok = in;

    char *delims = new char[seps.size()];
    strcpy_s(delims, (seps.size()*sizeof(char)), seps.c_str());
    char *TRASH = NULL;
    char *token = strtok_s(const_cast<char*>(cpyForStrTok.c_str()), delims, &TRASH);

    while(token != NULL){
        res.push_back(token);
        token = strtok_s(NULL, delims, &TRASH);
    }

    delete [] delims;
    delete TRASH;
    delete token;

    return res;
}

//<emptyExcpetion>
const char* emptyExprException::what() const{
    static const char msg[] = "Empty exception was created!";
    return msg;
}
//<unhandledExprException>
const char* unhandledExprException::what() const{
    static const char msg[] = "Unhandled expression!!";
    return msg;
}

//<varExpression>
varExpression::varExpression(string in){
    if(in.empty())
        throw emptyExprException();
    value = in;
}
varExpression::varExpression(const varExpression& right){
    value = right.value;
}
void varExpression::load(string in){
    value = in;
}
string varExpression::produce(){
    if(value.find(';') != string::npos){
        value.erase(value.find(';'));
    }
    return value;
}
//end<varExpression>



//<ioExpression>
ioExpression::ioExpression(expressionObj *o, string m){
    if(o == nullptr || m.empty())
        throw emptyExprException();
    object = o;
    method = m;
}
ioExpression::ioExpression(string input){
    if(input.empty())
        throw emptyExprException();

    string subExpr;
    if(input.find("cin") != string::npos){
        method = "input";
        subExpr = input.substr(input.find(">>")+3); //TODO: make multiple arguments for out
    }
    else if(input.find("cout") != string::npos){    //TODO: method may be empty!!
        method = "output";
        subExpr = input.substr(input.find("<<")+3);
    }
    Converter converter;
    object = converter.ConvertExpr(subExpr);
}
string ioExpression::produce(){
    stringstream result;
    if(method == "input"){
        result << "input(";
    }
    else
        result << "print(";
    if(object != nullptr)
        result << object->produce();
    result << ")";
    return result.str();
}
//END<ioExpression>



//<initExpression>
initExpression::initExpression(string in){
    size_t begOfName = in.find(' ') + 1;
    left = in.substr(begOfName, in.find('=') - begOfName);
    string r = in.substr(in.find('=')+1);

    Converter converter;
    right = converter.ConvertExpr(r);
}
string initExpression::produce(){
    string res = left + "=" + right->produce();
    return res;
}
//END<initExpression>



//<mathExpression>
mathExpression::mathExpression(string in) : equation(in)
{ }
string mathExpression::produce(){
    return equation;
}
//END<math<Expression>

vector<pair<string, string>> logicExpression::logToLog={
    {"false", "False"},
    {"true", "True"},
    {"&&", "and"},
    {"||", "or"},
    {"!", "not"},
    {"^", "^"}
};
//<logicExpression>
logicExpression::logicExpression(string in){
    for(int i = 0; i < logToLog.size(); i++){
        size_t index = in.find(logToLog[i].first);
        if(index != string::npos){
            in.erase(index, logToLog[i].first.length());
            in.insert(index, logToLog[i].second);
            i = 0;
        }
    }
    equation = in;
}
string logicExpression::produce(){
    return equation;
}
//END<logicExpresssion



//<codeBlock>

//end<codeBlock>



//<Converter>
map<string, exprType> Converter::keyWords = {
    {"=", exprType::initExpr},

    {"+", exprType::mathExpr},
    {"-", exprType::mathExpr},
    {"*", exprType::mathExpr},
    {"/", exprType::mathExpr},
    {"%", exprType::mathExpr},

    {">>", exprType::ioExpr},
    {"<<", exprType::ioExpr},
    {"cin", exprType::ioExpr},
    {"cout", exprType::ioExpr},

    {"false", exprType::logicExpr},
    {"true", exprType::logicExpr},
    {"&&", exprType::logicExpr},
    {"||", exprType::logicExpr},
    {"!", exprType::logicExpr},
    {"^", exprType::logicExpr},

    {"for", exprType::cicleExpr},
    {"while", exprType::cicleExpr},
    {"do", exprType::cicleExpr},

    {"if", exprType::ifExpr},

    /*TRASH*/
    {"using", exprType::nullExpr},
    {"include", exprType::nullExpr},
};

vector<string> Converter::types ={
    "double", "int", "float", "bool", "void", "stirng"
};

//Convert line or a certain string part
expressionObj *Converter::ConvertExpr(string in){
    if(in.empty())
        return new nullExpression();

    string cpyForStrTok = in;
    char delims[] = ";() <>#";
    char *TRASH = NULL;
    char *token = strtok_s(const_cast<char*>(cpyForStrTok.c_str()), delims, &TRASH);

    exprType type = exprType::varExpr;
    if(token != NULL)
        type = exprType::varExpr;
    else
        type = exprType::nullExpr;
    while(token != NULL){
        string temp = token;
        if(keyWords.count(temp)){
            type = keyWords[temp];
            break;
        }
        token = strtok_s(NULL, delims, &TRASH);
    }

    switch(type){
    case exprType::varExpr:
        return new varExpression(in);
    case exprType::nullExpr:
        return new nullExpression();
    case exprType::ioExpr:
        return new ioExpression(in);
    case exprType::initExpr:
        return new initExpression(in);
    case exprType::mathExpr:
        return new mathExpression(in);
    case exprType::logicExpr:
        return new logicExpression(in);
    default:
        throw unhandledExprException();
    }
    //it never reaches this point anyway
    return nullptr;
}

//Transforming expressions to strings!
stringstream Converter::transformExprsToStr(vector<expressionObj*> &exprs){
    stringstream stream;
    for(int i = 0; i < exprs.size(); i++){
        string produced = exprs[i]->produce();
        if(produced.empty())
            continue;
        stream << produced << '\n';
    }
    return stream;
}

//Method for converting methods (and everything global in the code)
stringstream Converter::ConvertOuter(string in ){
    stringstream stream(in);
    return stream;
}


//Method for a code conversion
stringstream Converter::ConvertInner(string in){
    stringstream stream;
    vector<string> tokens = parse(in, ";");

    for(int i = 0; i < tokens.size(); i++){
        expressionObj* expr = ConvertExpr(tokens[i]);
        stream << expr->produce();
                                                    //TODO: delete expr;
    }

    return stream;
}
//END <Converter>

#endif // CONVERTERCTOP_CPP
