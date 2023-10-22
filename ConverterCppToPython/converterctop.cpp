#ifndef CONVERTERCTOP_CPP
#define CONVERTERCTOP_CPP

#include "converterctop.h"
#include <algorithm>
#include <sstream>
#include <vector>

obj::obj(){
    value = "";
}
obj::obj(string in){
    value = in;
}
obj::obj(const obj& right){
    value = right.value;
}


//MISSSING INFORMATION


//Check if unnesessary construction for python
bool Converter::isTrash(string in){
    auto ptr = min(in.find("#"), in.find("using")); //string::npos is max ULL
    if(in[0] == '#' || ptr != string::npos){
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

//Method for a code conversion
string Converter::Convert(string in){
    vector<string> tokens;
    stringstream stream(in);
    string temp;
    while(getline(stream, temp, ';')){
        tokens.push_back(temp);
    }
    for(int i = 0; i < tokens.size(); i++){
        temp = tokens[i];
        if(isTrash(temp))
            continue;
        if(isIO(temp)) {
            //Add to the container of some kind
        }
    }
}

#endif // CONVERTERCTOP_CPP
