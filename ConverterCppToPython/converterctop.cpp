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
#include <QDebug>

vector<string> types ={
    "double", "int", "float", "bool", "void", "string",
    "double*", "int*", "float*", "bool*", "void*", "string*",
    "const"
};

//I HATE THAT I HAVE TO USE C HERE!!!
vector<string> split(string in, string seps){
    in+='\0';
    seps+='\0';
    vector<string> res;
    string cpyForStrTok = in;

    char *delims = new char[seps.length()];
    char *source = new char[in.length()];
    strcpy_s(delims, seps.length(), seps.c_str());
    strcpy_s(source, in.length(), in.c_str());
    char *TRASH;
    char *token = strtok_s(source, const_cast<char*>(delims), &TRASH);
    string newStr;
    if(token)
        newStr = token;

    while(token != NULL){
       res.push_back(newStr);
       token = strtok_s(NULL, delims, &TRASH);
       if(token)
           newStr = token;
    }

    delete []source;
    delete []delims;

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
initConvExpression::initConvExpression(string in){
    if(in.find('=') != string::npos)
        typeConv = false;
    else
        typeConv = true;

    size_t begOfName = in.find(' ') + 1;
    left = in.substr(begOfName, in.find('=') - begOfName);
    string r = in.substr(in.find('=')+1);

    Converter converter;
    right = converter.ConvertExpr(r);
}
string initConvExpression::produce(){
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



//<methodExpression>
methodExpression::methodExpression(size_t index, string &in){
    if(index == string::npos)
        throw emptyExprException();

    auto codeBlockIndex = in.find('{', index) + 1;
    string rawPrototype = in.substr(index, codeBlockIndex - index - 1);
    vector<string> words = split(rawPrototype, " ()\n\0");
    for(int i = 0; i < words.size(); i++){
        if(std::find(types.begin(), types.end(), words[i]) != types.end()){
            words.erase(words.begin() + i);
            i--;
        }
    }
    prototype = "def " + words[0] + "(";
    for(int i = 1; i < words.size(); i++){
        prototype += words[i];
        if(i + 1 < words.size())
            prototype += ", ";
    }
    prototype += "):";

    int innerBlocksCount = 0;
    for(auto i = codeBlockIndex; i < in.size(); i++){
        if(in[i] == '{'){
            innerBlocksCount++;
            continue;
        }
        else if(in[i] == '}'){
            if(!innerBlocksCount--){
                endIndex = i;
                break;
            }
        }
    }

    Converter converter;

    string codeBlockSource = in.substr(codeBlockIndex, endIndex-codeBlockIndex);
    exprs = converter.ConvertInner(codeBlockSource);
}
string methodExpression::produce(){
    stringstream out;
    out << prototype << '\n';
    for(int i = 0; i < exprs.size(); i++){
        string test = exprs[i]->produce();

        out << '\t' << exprs[i]->produce() << '\n';
    }
    return out.str();
}
size_t methodExpression::getEnd(){
    return endIndex;
}
//END<methodExpression>



//<Converter>
map<string, exprType> Converter::keyWords = {
    {"=", exprType::initConvExpr},      //initExpr also catches explicit type conversions!
    {"int", exprType::initConvExpr},
    {"double", exprType::initConvExpr},
    {"float", exprType::initConvExpr},
    {"string", exprType::initConvExpr},
    {"char", exprType::initConvExpr},

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
    {"}", exprType::nullExpr}
};

//Convert line or a certain string part
expressionObj *Converter::ConvertExpr(string in){
    if(in.empty())
        return new nullExpression();

    exprType type;
    vector<string> tokens = split(in, "\n() <>#");

    if(!tokens.empty())
        type = exprType::varExpr;
    else
        type = exprType::nullExpr;

    for(int i = 0; i < tokens.size(); i++){
        if(keyWords.count(tokens[i])){
            type = keyWords[tokens[i]];
            break;
        }
    }


    switch(type){
    case exprType::varExpr:
        return new varExpression(in);
    case exprType::nullExpr:
        return new nullExpression();
    case exprType::ioExpr:
        return new ioExpression(in);
    case exprType::initConvExpr:
        return new initConvExpression(in);
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

//Method for converting methods (and everything global in the code ((In the future))
stringstream Converter::TranslateOuter(string in){
    stringstream stream;
    auto endIndex = in.rfind('}');
    size_t curEnd = 0;
    while(endIndex > curEnd){
        size_t beginIndex = INT_MAX;
        for(int i = 1; i < types.size(); i++){
            auto tempPtr = in.find(types[i], curEnd);
            beginIndex = min(beginIndex, tempPtr);
        }
        //using method init expression
        methodExpression method(beginIndex, in);
        stream << method.produce();
        curEnd = method.getEnd();
    }

    stream << "if __name__ == \"__main__\":\n"
                    "\tmain()";

    return stream;
}


//Method for a code conversion
vector<expressionObj*> Converter::ConvertInner(string in){
    vector<expressionObj*> exprs;
    vector<string> tokens = split(in, ";");

    for(int i = 0; i < tokens.size(); i++){
        expressionObj* expr = ConvertExpr(tokens[i]);
        exprs.push_back(expr);
    }

    return exprs;
}
//END <Converter>

#endif // CONVERTERCTOP_CPP
