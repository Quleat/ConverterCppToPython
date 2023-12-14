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
#include <set>


vector<string> types ={
    "double", "int", "float", "bool", "void", "string",
    "double*", "int*", "float*", "bool*", "void*", "string*",
    "const"
};
map<string, string> varTypes  = {

};

//I HATE THAT I HAVE TO USE C HERE!!!
vector<string> special2023EditionSplit(string in){
    return split(in, "}() \n\t[]<>\0#");
}
vector<string> split(string in){
    return split(in, "() \n\t[]<>\0#");
}
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
size_t findCodeBlockEnd(size_t index, string &in){
    size_t endIndex = 0;
    int innerBlocksCount = 0;
    for(auto i = index; i < in.size(); i++){
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
    return endIndex;
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
//Virtual destructor for expressionObj
expressionObj::~expressionObj()
{ }


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
vector<string> varExpression::produce(){
    if(value.find(';') != string::npos){
        value.erase(value.find(';'));
    }
    return {value};
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
        subExpr = input.substr(input.find(">>")+3);
    }
    else if(input.find("cout") != string::npos){            //TODO: method may be empty!!
        method = "output";
        subExpr = input.substr(input.find("<<")+3);
    }
    Converter converter;
    object = converter.ConvertExpr(subExpr);
}
vector<string> ioExpression::produce(){
    stringstream result;
    string rightPart = object->produce()[0];
    if(method == "input"){
        if(varTypes.count(rightPart)){
           string type = varTypes[rightPart];
           if(type == "int"){
               result << rightPart << " = int(input())";
           }
           else if(type == "float"){
               result << rightPart << " = float(input())";
           }
        }
        else
            result << rightPart << " = input()";
    }
    else{
        result << "print(";
        if(object != nullptr)
            result << rightPart;
        result << ")";
    }


    return {result.str()};
}
ioExpression::~ioExpression(){
    delete object;
}
//END<ioExpression>



//<initExpression>
initComponent::initComponent(string in){
    size_t equalIndex = in.find('=');
    string leftPart = in.substr(0, equalIndex);
    vector<string> words = split(leftPart);
    string type = "NOT FOUND";
    for(int i = 0; i < words.size(); i++){
        if(std::find(types.begin(), types.end(), words[i]) == types.end()){
           left = words[i];
        }
        else{
            if(words[i] != "const"){
                if(type == "NOT FOUND")
                    type = words[i];
                else
                    break;
            }
        }
    }

    varTypes.insert(std::make_pair(left, type));

    if(equalIndex != string::npos){
        empty = false;
        string r = in.substr(equalIndex+1);

        Converter converter;
        right = converter.ConvertExpr(r);
    }
}
vector<string> initComponent::produce(){
    if(!empty){
        string res = left;
        res += " = " + right->produce()[0];
        return {res};
    }else
        return {""};
}
initComponent::~initComponent(){
    delete right;
}
//END<initExpression>


//<initExpression>
initExpression::initExpression(string in){
    vector<string> exprs = split(in, ",");
    vector<string> words = split(exprs[0], " ");
    for(int i = 0; i < exprs.size(); i++){  //TO DEDUCE THE TYPE OF THE VARIABLE
        if(std::find(types.begin(), types.end(), words[i]) != types.end())
            if(words[i] != "const"){
                if(type == "NOT FOUND")
                    type = words[i];
                else
                    break;
        }
    }
    for(int i = 0; i < exprs.size(); i++){
        string temp = exprs[i];
        if(i)
            temp = type + exprs[i];
        initComponent *init = new initComponent(temp);
        inits.push_back(init);
    }
}
vector<string> initExpression::produce(){
    vector<string> resExprs;
    for(int i = 0; i < inits.size();i++){
        string exprStr = inits[i]->produce()[0];
        if(!exprStr.empty())
            resExprs.push_back(exprStr);
    }
    return resExprs;
}
initExpression::~initExpression(){
    for(auto &item : inits){
        delete item;
    }
}
//END<initExpression>


//<mathExpression>
set<string> mathExpression::mathOps = {
    "+", "-", "*", "/", "%", "++", "--"
};
mathExpression::mathExpression(string in) : equation(in)
{
    /*auto ptr = in.find("++");

    while(ptr != string::npos){
        size_t leftVarBegin = INT_MAX;

        for(size_t z = ptr, tempLeft = 0; z >= 0; z--, tempLeft++){
            if(in[z] == ' ' || ((in[z] > '0') && (in[z] < '9')))
                continue;
            else{
                leftVarBegin = tempLeft;
                break;
            }
        }

        size_t rightVarBegin = INT_MAX;

        for(size_t z = ptr, tempRight = 0; z >= 0; z++, tempRight++){
            if(in[z] == ' ' || ((in[z] > '0') && (in[z] < '9')))
                continue;
            else{
                leftVarBegin = tempRight;
                break;
            }
        }

        if(leftVarBegin > rightVarBegin){

        }

    }
    */
}
vector<string> mathExpression::produce(){
    return {equation};
}
//END<math<Expression>



//<logicExpression>
vector<pair<string, string>> logicExpression::logToLog={        //Could make it map
    {"false", "False"},
    {"true", "True"},
    {"&&", "and"},
    {"||", "or"},
    {"!", "not"},
};
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
vector<string> logicExpression::produce(){
    return {equation};
}
//END<logicExpresssion>



//<typeConversionExpression>
typeConversionExpression::typeConversionExpression(string in){
    vector<string> words = split(in);
    for(int i = 0; i < words.size(); i++){
        if(std::find(types.begin(), types.end(), words[i]) == types.end()){
            name = words[i];
            break;
        }
    }
}
vector<string> typeConversionExpression::produce(){
    return {name};                                //CHECK IF NEED TO PUT HERE \n
}
//End<typeConversionExpression>



//<ifExpression>
enum class ifExpression::ifTypes {elseIfType, elseType, none};

string ifExpression::getStrCondition(size_t index, string &in, size_t &endCondition){
    int innerBracketCount = 0;
    size_t begCondition = 0;
    for(auto i = index + 1; i < in.size(); i++){
        if(in[i] == '('){
            if(!begCondition)
                begCondition = i + 1;
            else
                innerBracketCount++;
        }
        else if(in[i] == ')'){
            if(innerBracketCount){
                innerBracketCount--;
            }
            else{
                endCondition = i;
            }
        }
    }
    string res = in.substr(begCondition, endCondition - begCondition);      //NEED TESTING
    return res;
}

ifExpression::ifTypes ifExpression::deduceNextComponent(size_t index, string &in){
    ifTypes type = ifTypes::none;
    for(auto i = index; i < in.size(); i++){
        if(in[i] == ' ' || in[i] == '\n' || in[i] == '\t')
            continue;

        stringstream stream;
        for(auto j = i; j < in.size() && j < i + 10; j++){
            if(in[j] == '(' || in[j] == '{' || in[j] == ';')
                break;
            stream << in[j];
        }

        string rawString = stream.str();
        vector<string> words = split(rawString, " \n");

        if(words.size() >= 1 && words[0] == "else"){
            if(words.size() > 1 && words[1] == "if")
                type = ifTypes::elseIfType;
            else
                type = ifTypes::elseType;
        }
        else
            type = ifTypes::none;
        break;
    }
    return type;
}

string ifExpression::getStrInnerExpression(const size_t &index, string &in, size_t& endInnerExpr){
    size_t begInnerExpr = 0;
    bool codeBlock = false;
    for(auto i = index + 1; i < in.size(); i++){
        if(in[i] == ' ' || in[i] == '\n' || in[i] == '\t')
            continue;
        if(in[i] == '{'){
            begInnerExpr = i + 1;
            codeBlock = true;
            break;
        }
        else{
            begInnerExpr = index + 1;
            break;
        }
    }

    string strInnerExpr;
    if(codeBlock)
        endInnerExpr = findCodeBlockEnd(begInnerExpr, in);
    else
        endInnerExpr = in.find(";", begInnerExpr);

    strInnerExpr = in.substr(begInnerExpr, endInnerExpr-begInnerExpr);

    return strInnerExpr;
}

void ifExpression::convertElse(size_t index, string &in){
    Converter converter;

    size_t endCondition = 0;
    for(auto i = index; i < in.size() && i < index + 20; i++){
        if(in[i] == ' ' || in[i] == '\n' || in[i] == '\t')   //To skip all the spaces and enters to else
            continue;

        endCondition = i + 4;
        break;
    }

    string strCodeBlock;
    size_t endInnerExpr;

    strCodeBlock = getStrInnerExpression(endCondition, in, endInnerExpr);

    elses = converter.ConvertInner(strCodeBlock);

    endPoint = endInnerExpr+1;
}

void ifExpression::convertElseIf(size_t index, string &in){
    Converter converter;
    size_t endCondition = 0;
    string strCondition = getStrCondition(index, in, endCondition);
    logicExpression* cond = new logicExpression(strCondition);

    string strCodeBlock;
    size_t endInnerExpr;

    strCodeBlock = getStrInnerExpression(endCondition, in, endInnerExpr);

    vector<expressionObj*> exprs = converter.ConvertInner(strCodeBlock);
    ifExpression* newElseIf = new ifExpression(cond, exprs);
    elseIfs.push_back(newElseIf);

    convertElseAndIf(endInnerExpr+1, in);
}

void ifExpression::convertElseAndIf(size_t index, string &in){
    if(index > in.size())
        return;

    ifTypes type = deduceNextComponent(index, in);

    switch(type){
        case ifTypes::elseIfType:
            convertElseIf(index, in);
            break;
        case ifTypes::elseType:
            convertElse(index, in);
            break;
        case ifTypes::none:
            endPoint = index;
            return;
    }
}

ifExpression::ifExpression(size_t index, string& in){
    Converter converter;

    size_t endCondition = 0;
    string strCondition = getStrCondition(index, in, endCondition);
    condition = new logicExpression(strCondition);

    string strCodeBlock;
    size_t endInnerExpr;

    strCodeBlock = getStrInnerExpression(endCondition, in, endInnerExpr);

    ifs = converter.ConvertInner(strCodeBlock);

    convertElseAndIf(endInnerExpr+1, in);
}

size_t ifExpression::getEnd(){
    return endPoint;
}

string ifExpression::produceAsComponent(){
    stringstream stream;

    stream << "elif " << condition->produce()[0] << ":\n";
    for(int i = 0; i < ifs.size(); i++){
        string strExpr = ifs[i]->produce()[0];
        if(!strExpr.empty())
            stream << "\t" << ifs[i]->produce()[0] << "\n";
    }

    return stream.str();
}

vector<string> ifExpression::produce(){
    stringstream stream;

    string strCondition = condition->produce()[0];
    stream << "if " << strCondition << " :\n";

    for(int i = 0; i < ifs.size(); i++){
        string strExpr = ifs[i]->produce()[0];
        if(!strExpr.empty())
            stream << "\t" << strExpr << "\n";
    }

    for(int i = 0; i < elseIfs.size(); i++){
        string DEBUG = elseIfs[i]->produceAsComponent();
        stream << elseIfs[i]->produceAsComponent() << "\n";
    }

    if(!elses.empty())
        stream << "else:\n";
    for(int i = 0; i < elses.size(); i++){
        string strExpr = ifs[i]->produce()[0];
        stream << "\t" << strExpr << "\n";
    }

    vector<string> res = split(stream.str(), "\n");
    return res;
}

ifExpression::~ifExpression(){
    for(auto &item : ifs)
        delete item;

    for(auto &item : elses)
        delete item;

    if(!elseIfs.empty())
        for(auto &item : elseIfs)
            delete item;

    delete condition;
}
//END<ifExpression>


//<methodExpression>
methodExpression::methodExpression(size_t index, string &in){
    if(index == string::npos)
        throw emptyExprException();

    auto codeBlockIndex = in.find('{', index) + 1;
    string rawPrototype = in.substr(index, codeBlockIndex - index - 1);
    vector<string> words = split(rawPrototype);
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

    endIndex = findCodeBlockEnd(codeBlockIndex, in);

    Converter converter;

    string codeBlockSource = in.substr(codeBlockIndex, endIndex-codeBlockIndex);
    exprs = converter.ConvertInner(codeBlockSource);
}
vector<string> methodExpression::produce(){
    stringstream out;
    out << prototype << '\n';
    for(int i = 0; i < exprs.size(); i++){
        vector<string> inExprStr = exprs[i]->produce();

        for(int j = 0; j < inExprStr.size(); j++){
            string input = inExprStr[j];
            if(!input.empty())
                out << '\t' << input << '\n';
        }
    }
    return {out.str()};
}
size_t methodExpression::getEnd(){
    return endIndex;
}
methodExpression::~methodExpression(){
    for(auto &item : exprs)
        delete item;
}
//END<methodExpression>



//<Converter>
map<string, exprType> Converter::keyWords = {
    {"=", exprType::initExpr},
    {"int", exprType::initExpr},
    {"double", exprType::initExpr},
    {"float", exprType::initExpr},
    {"string", exprType::initExpr},
    {"char", exprType::initExpr},

    {"++", exprType::mathExpr},
    {"--", exprType::mathExpr},
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
    {">=", exprType::logicExpr},
    {"<=", exprType::logicExpr},
    {"==", exprType::logicExpr},
    {"<", exprType::logicExpr},
    {">", exprType::logicExpr},

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
    vector<string> tokens = split(in);

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
        string produced = exprs[i]->produce()[0];
        if(produced.empty())
            continue;
        stream << produced << '\n';
    }
    return stream;
}

//Method for converting methods
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
        stream << method.produce()[0];
        curEnd = method.getEnd();
    }

    stream << "if __name__ == \"__main__\":\n"
                    "\tmain()";

    return stream;
}


//Method for a code conversion
vector<expressionObj*> Converter::ConvertInner(string in){
    if(in.empty())
        return {new nullExpression()};

    exprType type;
    vector<expressionObj*> exprs;
    vector<string> rawTokens = split(in, ";");
    size_t globalIndex = 0;
    for(int i = 0; i < rawTokens.size(); i++){
        vector<string> tokens = special2023EditionSplit(rawTokens[i]);

        if(!tokens.empty())
            type = exprType::varExpr;
        else
            type = exprType::nullExpr;

        for(int j = 0; j < tokens.size(); j++){
            if(keyWords.count(tokens[j])){
                type = keyWords[tokens[j]];
                break;
            }
        }

        switch(type){
        case exprType::varExpr:
            exprs.push_back( new varExpression(rawTokens[i])); break;
        case exprType::nullExpr:
            /*exprs.push_back( new nullExpression());*/ break;
        case exprType::ioExpr:
            exprs.push_back(new ioExpression(rawTokens[i])); break;
        case exprType::initExpr:
            exprs.push_back(new initExpression(rawTokens[i])); break;
        case exprType::mathExpr:
            exprs.push_back(new mathExpression(rawTokens[i])); break;
        case exprType::logicExpr:
            exprs.push_back(new logicExpression(rawTokens[i])); break;
        case exprType::ifExpr:{
            ifExpression* ifExp = new ifExpression(in.find("if", globalIndex), in);
            exprs.push_back(ifExp);
            size_t endPoint = ifExp->getEnd();
            size_t tempIndex = globalIndex;
            for(int j = i; j < rawTokens.size(); j++){
                if(endPoint < tempIndex){
                    i = j - 2;
                    globalIndex = endPoint - rawTokens[i].size() - 1;
                    break;
                }
                tempIndex += rawTokens[j].size()+1;
            }
            break;
        }
        default:
            throw unhandledExprException();
        }

        globalIndex += rawTokens[i].size() + 1;
        //it never reaches this point anyway
    }

    return exprs;
}
//END <Converter>

#endif // CONVERTERCTOP_CPP
