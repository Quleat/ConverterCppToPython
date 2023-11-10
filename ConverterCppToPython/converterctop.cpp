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


//<obj>
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
//end<obj>



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
    {"&&", exprType::logicExpr},
    {"||", exprType::logicExpr},
    {"!", exprType::logicExpr},
    {"for", exprType::cicleExpr},
    {"while", exprType::cicleExpr},
    {"do", exprType::cicleExpr},
    {"if", exprType::ifExpr},
    /*TRASH*/
    {"#include<iostream>", exprType::nullExpr},
    {"#int main(){", exprType::nullExpr},
    {"}", exprType::nullExpr}

};

//Check if unnesessary construction for python
bool Converter::isTrash(string in){
    auto ptr = min(in.find("#"), in.find("using"));
    if(in[0] == '#' || ptr != string::npos){    //string::npos is max ULL
        return true;
    }
    ptr = min(in.find("int main"), in.find('}'));
    if(ptr != string::npos){
        return true;
    }
    else
        return false;
}

//Check if it is an input/output
bool Converter::isIO(string in){
    auto ptr = min(in.find(">>"), in.find("<<"));
    if(ptr != string::npos)
        return true;
    else
        return false;
}

//Convert line or a certain string part
expressionObj *Converter::ConvertExpr(string in){
    if(in.empty())
        return new nullExpression();
    char delims[] = ";() ";
    char *TRASH = NULL;
    char *token = strtok_s(const_cast<char*>(in.c_str()), delims, &TRASH);
    exprType type = exprType::varExpr;
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
        return new varExpression(token);
    case exprType::nullExpr:
        return new nullExpression();
    case exprType::ioExpr:
        return new ioExpression(token);
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
        stream << exprs[i]->produce();
    }
    return stream;
}

//Method for a code conversion
stringstream Converter::Convert(string in){
    vector<expressionObj*> exprs;

    vector<string> tokens;
    stringstream stream(in);
    string temp;
    while(getline(stream, temp, '\n')){
        tokens.push_back(temp);
    }
    for(int i = 0; i < tokens.size(); i++){
        temp = tokens[i];
        exprs.push_back(ConvertExpr(temp));
    }

    stream = transformExprsToStr(exprs);

    return stream;
}
//END <Converter>

#endif // CONVERTERCTOP_CPP
