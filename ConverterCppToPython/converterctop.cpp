#ifndef CONVERTERCTOP_CPP
#define CONVERTERCTOP_CPP

#include "converterctop.h"
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>

//<obj>
obj::obj(string in){
    value = in;
}
obj::obj(const obj& right){
    value = right.value;
}
void obj::load(string in){
    value = in;
}
string obj::produce(){
    if(value.find(';') != string::npos){
        value.erase(value.find(';'));
    }
    return value;
}
//end<obj>



//<ioExpression>
ioExpression::ioExpression(expressionObj *o, string m){
    object = o;
    method = m;
}
ioExpression::ioExpression(string input){
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
    object = converter.ConvertObj(subExpr);
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
expressionObj *Converter::ConvertObj(string in){
    if(isTrash(in))
        return new nullObj;
    else if(isIO(in))
        return new ioExpression(in);
    else //We assume that it is a variable or const then
        return new obj(in);
}

stringstream Converter::transformExprs(vector<expressionObj*> &exprs){
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
        exprs.push_back(ConvertObj(temp));
    }

    stream = transformExprs(exprs);

    return stream;
}
//END <Converter>


#endif // CONVERTERCTOP_CPP
