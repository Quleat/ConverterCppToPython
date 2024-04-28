#ifndef CONVERTERCTOP_CPP
#define CONVERTERCTOP_CPP

#include "converterctop.h"

using namespace std;
//I HATE THAT I HAVE TO USE C HERE!!!
vector<string> special2023EditionSplit(string in){
    return split(in, "}() \n\t[]<>\0");
}
vector<string> split(string in){
    return split(in, "() \n\t[]<>\0");
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
size_t findBlockEnd(size_t &index, string &in, char symbolBeg, char symbolEnd){
    size_t endIndex = index;
    int innerBlocksCount = 0;
    for(; endIndex < in.size(); endIndex++){
       if(in[endIndex] == symbolBeg){
           innerBlocksCount++;
           continue;
       }
       else if(in[endIndex] == symbolEnd){
           if(!innerBlocksCount--)
               break;
       }
    }
    return endIndex;
}
size_t findCodeBlockEnd(size_t index, string &in){
    return findBlockEnd(index, in, '{', '}');
}
size_t findBraceBlockEnd(size_t index, string &in){
    return findBlockEnd(index, in, '(', ')');
}

//<unhandledExprException>
const char* unhandledExprException::what() const{
    static const char msg[] = "Unhandled expression!!";
    return msg;
}
const char* exceedingSymbolsException::what() const{
    static const char msg[] = "Too many symbols!";
    return msg;
}



//Virtual destructor for expressionObj
expressionObj::~expressionObj()
{ }



ioExpression::ioExpression(string input){
    string subExpr;
    if(input.find("cin") != string::npos){
        method = "input";
        subExpr = input.substr(input.find(">>")+3);
    }
    else if(input.find("cout") != string::npos){
        method = "output";
        subExpr = input.substr(input.find("<<")+3);
    }
    Converter converter;
    object = converter.ConvertInner(subExpr)[0];
}
vector<string> ioExpression::produce(){
    stringstream result;
    vector<string> allLines = object->produce();
    string rightPart = allLines[0];
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

    vector<string> res = {result.str()};
    res.insert(res.end(), allLines.begin()+1, allLines.end());

    return res;
}
ioExpression::~ioExpression(){
    delete object;
}





void varExpression::tryTrunc(string &in){
    vector<string> trunc = split(in, " \n\t");
    if(trunc.size() == 1)
        value = trunc[0];
    else
        value = in;
}
varExpression::varExpression(string in){
    tryTrunc(in);
}
varExpression::varExpression(const varExpression& right){
    value = right.value;
}
vector<string> varExpression::produce(){
    if(value.find(';') != string::npos){
        value.erase(value.find(';'));
    }
    return {value};
}





set<string> mathExpression::mathOps = {
    "+", "-", "*", "/", "%", "++", "--", "(", ")"
};
bool mathExpression::isRvalue(string input){
    if(input[0] <= '9' && input[0] >= '0')
        return true;
    if(mathOps.count(input))
        return true;
    return false;
}

mathExpression::mathExpression(string in) : equation(in)
{
    vector<string> items = split(in, " \n\t");
    for(int i = 0; i < items.size(); i++){
        if(items[i] == "++" || items[i] == "--"){
            if(i && isRvalue(items[i - 1])){        //'++num' kind
                additionalLines.push_back(make_pair(items[i+1], items[i]));
                if(items[i] == "++")
                    items[i + 1] = "(" + items[i+1] + " + 1)";
                else
                    items[i + 1] = "(" + items[i+1] + " - 1)";
            }
            else{                                   //'num++' kind
                additionalLines.push_back(make_pair(items[i-1], items[i]));
            }
            items.erase(items.begin() + i);
            i--;
        }
    }

    //Combine all left items into one equation
    stringstream stream;
    for(int i = 0; i < items.size(); i++){
        stream << items[i] << " ";
    }
    equation = stream.str();
}
vector<string> mathExpression::produce(){
    vector<string> allLines;
    allLines.push_back(equation);
    //processing additional lines ('a+=1' or 'a-=1' kind)
    for(int i = 0; i < additionalLines.size(); i++){
        if(additionalLines[i].second == "++")
            allLines.push_back(additionalLines[i].first + " += 1");
        else
            allLines.push_back(additionalLines[i].first + " -= 1");
    }
    return allLines;
}




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
        right = converter.ConvertInner(r)[0];
    }
}
vector<string> initComponent::produce(){
    if(!empty){
        string res = left;
        vector<string> allLines = right->produce();
        res += " = " + allLines[0];

        vector<string> result = {res};
        result.insert(result.end(), allLines.begin()+1, allLines.end());

        return result;
    }else
        return {""};
}
initComponent::~initComponent(){
    delete right;
}



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
        vector<string> allLines = inits[i]->produce();

        resExprs.insert(resExprs.end(), allLines.begin(), allLines.end());
    }
    return resExprs;
}
initExpression::~initExpression(){
    for(auto &item : inits){
        delete item;
    }
}


string codeBlockExpression::getStrInnerExpression(const size_t &index, string &in, size_t& endInnerExpr){
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
    if(codeBlock){
        globalEndBraceCOUNT++;
        endInnerExpr = findCodeBlockEnd(begInnerExpr, in);
        for(auto i = endInnerExpr; i >= 0; i--){
            if(in[i] == ';'){
                endInnerExpr = i + 1;
                break;
            }
        }
    }
    else
        endInnerExpr = in.find(";", begInnerExpr);

    strInnerExpr = in.substr(begInnerExpr, endInnerExpr-begInnerExpr);

    return strInnerExpr;
}
codeBlockExpression::codeBlockExpression(){

}
codeBlockExpression::codeBlockExpression(size_t index, string &in){
    Converter converter;

    string strCodeBlock;
    size_t endInnerExpr;

    strCodeBlock = getStrInnerExpression(index, in, endInnerExpr);
    endPoint = endInnerExpr + 1;
    innerExprs = converter.ConvertInner(strCodeBlock);
}
size_t codeBlockExpression::getEnd(){
    return endPoint;
}
vector<string> codeBlockExpression::produce(){
    vector<string> res;
    for(int i = 0; i < innerExprs.size(); i++){
        vector<string> innerStrs = innerExprs[i]->produce();
        res.reserve(res.size() + innerStrs.size());
        res.insert(res.end(), innerStrs.begin(), innerStrs.end());
    }
    return res;
}
codeBlockExpression::~codeBlockExpression(){
    for(auto &item : innerExprs){
        delete item;
    }
}




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




keyWordExpression::keyWordExpression(string in){
    if(in.find("continue") != string::npos){
        output = "continue";
    }
    else                                        //Because there are only two cases!
        output = "break";
}
vector<string> keyWordExpression::produce(){
    return {output};
}




enum class ifExprsn::ifTypes {elseIfType, elseType, none};

string ifExprsn::getStrCondition(size_t index, string &in, size_t &endCondition){
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
    string res = in.substr(begCondition, endCondition - begCondition);
    return res;
}

ifExprsn::ifTypes ifExprsn::deduceNextComponent(size_t index, string &in){
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

void ifExprsn::convertElse(size_t index, string &in){
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

void ifExprsn::convertElseIf(size_t index, string &in){
    Converter converter;
    size_t endCondition = 0;
    string strCondition = getStrCondition(index, in, endCondition);
    logicExpression* cond = new logicExpression(strCondition);

    string strCodeBlock;
    size_t endInnerExpr;

    strCodeBlock = getStrInnerExpression(endCondition, in, endInnerExpr);

    vector<expressionObj*> exprs = converter.ConvertInner(strCodeBlock);
    ifExprsn* newElseIf = new ifExprsn(cond, exprs);
    elseIfs.push_back(newElseIf);

    convertElseAndIf(endInnerExpr+1, in);
}

void ifExprsn::convertElseAndIf(size_t index, string &in){
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

ifExprsn::ifExprsn(size_t index, string& in){
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

string ifExprsn::produceAsComponent(){
    stringstream stream;

    stream << "elif " << condition->produce()[0] << ":\n";
    for(int i = 0; i < ifs.size(); i++){
        vector<string> allLines = ifs[i]->produce();
        if(!allLines.empty())
            for(int j = 0; j < allLines.size(); j++)
                stream << "\t" << allLines[j] << "\n";
    }

    return stream.str();
}

vector<string> ifExprsn::produce(){
    stringstream stream;

    string strCondition = condition->produce()[0];
    stream << "if " << strCondition << " :\n";

    for(int i = 0; i < ifs.size(); i++){
        vector<string> allLines = ifs[i]->produce();
        if(!allLines.empty()){
            for(int j = 0; j < allLines.size(); j++)
                stream << "\t" << allLines[j] << "\n";
        }
    }

    for(int i = 0; i < elseIfs.size(); i++){
        stream << elseIfs[i]->produceAsComponent() << "\n";
    }

    if(!elses.empty())
        stream << "else:\n";
    for(int i = 0; i < elses.size(); i++){
        vector<string> allLines = elses[i]->produce();
        for(int j = 0; j < allLines.size(); j++)
            stream << "\t" << allLines[j] << "\n";
    }

    vector<string> res = split(stream.str(), "\n");
    return res;
}

ifExprsn::~ifExprsn(){
    for(auto &item : ifs)
        delete item;

    for(auto &item : elses)
        delete item;

    if(!elseIfs.empty())
        for(auto &item : elseIfs)
            delete item;

    delete condition;
}




forExprsn::forExprsn(size_t index, string &in){
    Converter converter;

    //Handling 'A' Part
    size_t forConditionBeg = in.find("(", index);
    size_t endAPart = in.find(";", index);
    string APart = in.substr(forConditionBeg + 1, endAPart - forConditionBeg - 1);

    init = new initExpression(APart);

    //Handling 'B' Part
    size_t endBPart = in.find(";", endAPart+1);
    string BPart = in.substr(endAPart+1, endBPart - endAPart - 1);

    check = new logicExpression(BPart);

    //Handling 'C' Part
    size_t endCPart = findBraceBlockEnd(forConditionBeg+1, in);
    string CPart = in.substr(endBPart+1, endCPart - endBPart - 1);

    iteration = converter.ConvertInner(CPart)[0];


    //Handling inner expressions
    string strCodeBlock;
    size_t begStrCodeBlock = endCPart;
    strCodeBlock = getStrInnerExpression(begStrCodeBlock, in, endPoint);

    //Handling continue's
    auto ptr = strCodeBlock.find("continue");
    while(ptr != string::npos){
        string insertString = CPart +";\n";
        strCodeBlock.insert(ptr, insertString);
        ptr = strCodeBlock.find("continue", ptr + insertString.length() + 8);
    }

    innerExprs = converter.ConvertInner(strCodeBlock);
}
vector<string> forExprsn::produce(){
    vector<string> res;

    //Producing 'A' Part
    vector<string> initAllLines = init->produce();
    for(int i = 0; i < initAllLines.size(); i++)
        res.push_back(initAllLines[i]);

    //Handling 'B' Part, making the head of the cicle
    string checkLine = "while " + check->produce()[0] + " :";
    res.push_back(checkLine);

    //Handling the body Part of the cicle
    for(int i = 0; i < innerExprs.size(); i++){
        vector<string> allLines = innerExprs[i]->produce();
        for(int j = 0; j < allLines.size(); j++){
            res.push_back("\t" + allLines[j]);
        }
    }

    //Handling the 'C' Part - iteration
    vector<string> iterAllLines = iteration->produce();
    for(int i = 0; i < iterAllLines.size(); i++){
        res.push_back("\t" + iterAllLines[i]);
    }

    return res;
}
forExprsn::~forExprsn(){
    delete init;
    delete check;
    delete iteration;
}




void switchExprsn::removeKeyWords(string &in){
    size_t index = in.find("break");
    while(index != string::npos){
        size_t beg = index;
        for(; beg > 0; beg--){
            if(in[beg] == ';'){
                break;
            }
        }
        size_t end = index + 5;
        for(; end < in.size(); end++){
            if(in[end] == ' ')
                continue;
            else
                break;
        }

        in.erase(beg, end-beg);
        index = in.find("break");
    }
}
switchExprsn::switchExprsn(size_t index, string &in){
    size_t begVarName = in.find("(", index) + 1;
    size_t endVarName = in.find(")", index);
    string varName = in.substr(begVarName, endVarName - begVarName);

    string innerBlocks = getStrInnerExpression(endVarName, in, endPoint);
    size_t begOfBlock = innerBlocks.find("case");
    size_t endPointOfBlocks = innerBlocks.size() - 1;
    while(begOfBlock != endPointOfBlocks){
        size_t endOfBlock = innerBlocks.find("case", begOfBlock + 8);
        bool isDefault = false;
        if(endOfBlock == string::npos){
            endOfBlock = innerBlocks.find("default", begOfBlock + 8);
            if(endOfBlock == string::npos){
                endOfBlock = endPointOfBlocks;
                isDefault = true;
            }
        }

        string innerBlock = innerBlocks.substr(begOfBlock, endOfBlock - begOfBlock);

        //handling the condition
        size_t endOfCondition = innerBlock.find(":");
        logicExpression* blockCondition{0};
        if(!isDefault){
            string strCondition = innerBlock.substr(5, endOfCondition - 5);
            blockCondition = new logicExpression(varName + " == " + strCondition);
        }

        //handling the body of the block
        string blockBody = innerBlock.substr(endOfCondition, endOfBlock - endOfCondition);
        removeKeyWords(blockBody);
        Converter converter;
        vector<expressionObj*> body = converter.ConvertInner(blockBody);

        if(ifs.empty()){
            condition = blockCondition;
            ifs = body;
        }
        else if(!isDefault){
            ifExprsn* newElif = new ifExprsn(blockCondition, body);
            elseIfs.push_back(newElif);
        }
        else
            elses = body;

        begOfBlock = endOfBlock;
    }
}



whileExprsn::whileExprsn(size_t index, string &in){
    size_t begOfCondition = in.find("(", index)+1;
    size_t endOfCondition = findBraceBlockEnd(begOfCondition, in);
    string strCondition = in.substr(begOfCondition, endOfCondition - begOfCondition);
    check = new logicExpression(strCondition);

    string strBody = getStrInnerExpression(endOfCondition, in, endPoint);
    Converter converter;
    innerExprs = converter.ConvertInner(strBody);
}
vector<string> whileExprsn::produce(){
    vector<string> res;

    res.push_back("while " + check->produce()[0] + " :");
    for(int i = 0; i < innerExprs.size(); i++){
        vector<string> allLines = innerExprs[i]->produce();
        for(int j = 0; j < allLines.size(); j++){
            res.push_back("\t" + allLines[j]);
        }
    }
    return res;
}
whileExprsn::~whileExprsn(){
    delete check;
}

methodUseExprsn::methodUseExprsn(string in){
    //Just for beauty
    size_t ptr = in.find(' ');
    while(ptr != string::npos){
        in.erase(ptr, 1);
        ptr = in.find(' ');
    }
    size_t argBegin = in.find("(") + 1;
    size_t nameBegin = 0;
    for(;nameBegin < in.size(); nameBegin++){
        if(in[nameBegin] == ' ' || in[nameBegin] == '\n' || in[nameBegin] == '\t')
            continue;
        else
            break;
    }
    value = in.substr(nameBegin, argBegin-1 - nameBegin);
    size_t argEnd = in.find(",");
    if(argEnd == string::npos){
        argEnd = findBraceBlockEnd(argBegin, in);
    }
    Converter converter;
    size_t argsEnd = findBraceBlockEnd(argBegin, in);
    do{
        string strArgs = in.substr(argBegin, argEnd - argBegin);
        expressionObj* newArg = converter.ConvertInner(strArgs)[0];
        args.push_back(newArg);

        argBegin = argEnd + 1;

        argEnd = in.find(",", argEnd+1);
        if(argEnd == string::npos){
            argEnd = argsEnd;
        }

    } while(argBegin-1 != argsEnd);

}
vector<string> methodUseExprsn::produce(){
    vector<string> res;
    string mainStr = value + "(";
    res.push_back(mainStr);
    for(int i = 0; i < args.size(); i++){
        if(i)
            res[0] += ", ";

        vector<string> allLines = args[i]->produce();
        res[0] += allLines[0];
        if(allLines.size() > 1)
            res.insert(res.end(), allLines.begin() + 1, allLines.end());
    }
    res[0] += ")";
    return res;
}



methodExpression::methodExpression(size_t index, string &in){
    if(index == string::npos)
        throw unhandledExprException();

    auto codeBlockIndex = in.find('{', index) + 1;
    string rawPrototype = in.substr(index, codeBlockIndex - index - 1);
    vector<string> words = split(rawPrototype, "() \n\t,\0,");
    for(int i = 0; i < words.size(); i++){
        if(std::find(types.begin(), types.end(), words[i]) != types.end()){
            words.erase(words.begin() + i);
            i--;
        }
    }

    Converter converter;
    converter.addKeyWords({words[0], exprType::methodInvokeExpr});

    prototype = "def " + words[0] + "(";
    for(int i = 1; i < words.size(); i++){
        prototype += words[i];
        if(i + 1 < words.size())
            prototype += ", ";
    }
    prototype += "):";

    endIndex = findCodeBlockEnd(codeBlockIndex, in);

    codeBlockSource = in.substr(codeBlockIndex, endIndex-codeBlockIndex);
}
vector<string> methodExpression::produce(){
    Converter converter;
    vector<expressionObj *> exprs = converter.ConvertInner(codeBlockSource);

    stringstream out;
    out << prototype << '\n';
    for(int i = 0; i < exprs.size(); i++){
        vector<string> allLines = exprs[i]->produce();

        for(int j = 0; j < allLines.size(); j++){
            string input = allLines[j];
            if(!input.empty())
                out << '\t' << input << '\n';
        }
    }
    return {out.str()};
}
size_t methodExpression::getEnd(){
    return endIndex;
}



//<Converter>
map<string, exprType> Converter::keyWords = {
    {"=", exprType::initExpr},
    {"int", exprType::initExpr},
    {"double", exprType::initExpr},
    {"float", exprType::initExpr},
    {"string", exprType::initExpr},
    {"char", exprType::initExpr},
    {"auto", exprType::initExpr},

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

    {"for", exprType::forExpr},
    {"while", exprType::whileExpr},

    {"if", exprType::ifExpr},

    {"continue", exprType::keyWordExpr},
    {"break", exprType::keyWordExpr},

    {"switch", exprType::switchExpr},

    /*TRASH*/
    {"using", exprType::nullExpr},
    {"include", exprType::nullExpr},
    {"}", exprType::nullExpr}
};

vector<pair<string, exprType>> Converter::keyOps = {
    {"\"", exprType::varExpr},
    {"\'", exprType::varExpr},

    {"+", exprType::mathExpr},
    {"-", exprType::mathExpr},
    {"*", exprType::mathExpr},
    {"/", exprType::mathExpr},
    {"%", exprType::mathExpr},
    {"++", exprType::mathExpr},
    {"--", exprType::mathExpr},

    {"(", exprType::mathExpr},
    {")", exprType::mathExpr},

    {"&&", exprType::logicExpr},
    {"||", exprType::logicExpr},
    {"^", exprType::logicExpr},

    {">>", exprType::ioExpr},
    {"<<", exprType::ioExpr}
};

//Separates the first occurrence with spaces
size_t Converter::separateFirst(string &in, size_t off, string what){
    size_t ptr = in.find(what, off);

    if(ptr == string::npos)
    { }
    else if(ptr && in[ptr-1] == what[0] && (what == "+" || what == "-"))                             //Just so we don't serparate '+' in "++"
    { }
    else if((ptr + 1) < in.size() && in[ptr + 1] == what[0] && (what == "+" || what == "-"))
    { }
    else{
        in.insert(ptr, " ");
        in.insert(ptr + what.length() + 1, " ");
    }
    return ptr;
}

//Separates all occurrences with spaces
void Converter::separateAll(string &in, string what){
    size_t offset = 0;
    while(offset != string::npos){
        size_t ptr = separateFirst(in, offset, what);
        if(ptr == string::npos)
            break;
        offset = what.size() + ptr+1;
    }
}

//Skips code block expression in "ConvertInner" method
void Converter::skipCodeBlock(size_t &globalIndex, const vector<string> &rawTokens, int &i, const size_t &endPoint){
    size_t tempIndex = globalIndex + globalEndBraceCOUNT;
    int j = i;
    for(; j < rawTokens.size(); j++){
        if(endPoint <= tempIndex){
            i = j-1;
            globalIndex = endPoint;
            return;
        }
        tempIndex += rawTokens[j].size()+1;
    }

    i = j;                  //Need this point because sometimes cicle at the end of the string and string doesn't include the end bracket
    globalIndex = endPoint;
}

//Adds new keyWords to the 'keyWords'
void Converter::addKeyWords(pair<string, exprType> p){
    keyWords.insert(p);
}

//Method for converting methods
stringstream Converter::TranslateOuter(string in){
    stringstream stream;
    auto endIndex = in.rfind('}');
    size_t curEnd = 0;
    vector<methodExpression> methods;
    while(endIndex > curEnd){
        size_t beginIndex = INT_MAX;
        for(int i = 1; i < types.size(); i++){
            auto tempPtr = in.find(types[i], curEnd);
            beginIndex = min(beginIndex, tempPtr);
        }
        //using method init expression
        methodExpression method(beginIndex, in);
        methods.push_back(method);
        curEnd = method.getEnd();
    }

    for(int i = 0; i < methods.size(); i++){
        stream << methods[i].produce()[0];          //Because there should be output in one string lol
    }

    stream << "if __name__ == \"__main__\":\n"
                    "\tmain()";

    globalEndBraceCOUNT = 0;
    return stream;
}

//Method for a code conversion
vector<expressionObj*> Converter::ConvertInner(string in){
    if(in.empty())
        return {0};

    exprType type;
    vector<expressionObj*> exprs;
    vector<string> rawTokens = split(in, ";}");
    size_t globalIndex = 0;
    SUPERTEST = in;
    for(int i = 0; i < rawTokens.size(); i++){
        string line = rawTokens[i];
        vector<string> tokens = special2023EditionSplit(line);

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

        //Looking for the operators ;)
        for(int j = 0; j < keyOps.size(); j++){
            if(line.find(keyOps[j].first) != string::npos){
                if(type == exprType::varExpr || type == exprType::nullExpr)
                    type = keyOps[j].second;
                if(keyOps[j].second == exprType::varExpr)
                    break;
                separateAll(line, keyOps[j].first);
            }
        }

        switch(type){
        case exprType::varExpr:
            exprs.push_back( new varExpression(line)); break;
        case exprType::nullExpr:
            break;
        case exprType::ioExpr:
            exprs.push_back(new ioExpression(line)); break;
        case exprType::initExpr:
            exprs.push_back(new initExpression(line)); break;
        case exprType::mathExpr:
            exprs.push_back(new mathExpression(line)); break;
        case exprType::logicExpr:
            exprs.push_back(new logicExpression(line)); break;
        case exprType::keyWordExpr:
            exprs.push_back(new keyWordExpression(line)); break;
        case exprType::methodInvokeExpr:
            exprs.push_back(new methodUseExprsn(line)); break;
        case exprType::ifExpr:{
            ifExprsn* ifExp = new ifExprsn(in.find("if", globalIndex), in);
            exprs.push_back(ifExp);
            skipCodeBlock(globalIndex, rawTokens, i, ifExp->getEnd());
            continue;
            break;
        }
        case exprType::forExpr:{
            forExprsn* forExpr = new forExprsn(in.find("for", globalIndex), in);
            exprs.push_back(forExpr);
            skipCodeBlock(globalIndex, rawTokens, i, forExpr->getEnd());
            continue;
            break;
        }
        case exprType::switchExpr:{
            switchExprsn* switchExpr = new switchExprsn(in.find("switch", globalIndex), in);
            exprs.push_back(switchExpr);
            skipCodeBlock(globalIndex, rawTokens, i, switchExpr->getEnd());
            continue;
            break;
        }
        case exprType::whileExpr:{
            whileExprsn* whileExpr = new whileExprsn(in.find("while", globalIndex), in);
            exprs.push_back(whileExpr);
            skipCodeBlock(globalIndex, rawTokens, i, whileExpr->getEnd());
            continue;
            break;
        }
        default:
            throw unhandledExprException();
        }
        globalIndex += rawTokens[i].size() + 1;
    }

    return exprs;
}
//END <Converter>

#endif // CONVERTERCTOP_CPP
