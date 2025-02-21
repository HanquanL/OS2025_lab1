#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include <iomanip>

using namespace std;

/*------  global variable  ------*/
int lineNumber = 0, currentOffset = 0;
int modelCount = 0, modelBase_address = 0;
int totalInstructions = 0;
string fileName,line,token;
ifstream inputFile;
istringstream lineStream;
size_t tokenStart = 0;
map<string, string> symbolTable;
map<int, int> moduleBaseTable;
vector<string> memoryMap;
vector<string> sepLineError;
vector<string> topError;

/*------  function declaration  ------*/
string getToken();
int readInt(string token);
string readSymbol(string token);
string readMARIE(string token);
void passOne(string fileName);
void passTwo(string fileName);
void createSymbol(string symbol, string val);
void insertMemoryMap(string index, string value);
int findValueFromSymbolTable(string symbol);

/*------ Error INFO ------*/
void __parseerror(int errcode);
void __eolerror(int errcode);

/*------  main function  ------*/
int main(int argc, char* argv[])
{
    if (argc != 2){
        cout << "Invalid file name!" << endl;
        exit(0);
    }else{
        fileName = argv[1];
    }
    passOne(fileName); 
    passTwo(fileName);
    // print top errors
    for(auto i : topError){
        cout << i;
    }
    cout <<"Symbol Table" << endl;
    for(auto i : symbolTable){
        cout << i.first << "=" << i.second << endl;
    }
    cout << endl;
    // cout <<"Module Base Table" << endl;
    // for(auto i : moduleBaseTable){
    //     cout <<"Module" << i.first << " Base Adress:" << i.second << endl;
    // }
    // cout << endl;
    cout <<"Memory Map" << endl;
    for(auto i : memoryMap){
        cout << i << endl;
    }
    cout << endl;
    // print errors
    for(auto i : sepLineError){
        cout << i;
    }
    cout << endl;
    return 0;
}

/*------  function definition  ------*/
void passTwo(string fileName){
    int modelCount = 0, modelBase_address = 0, totalInstructions = 0;
    vector<string> memoryMapUseList;
    map<string, int> tempDefList;
    map<string, int> tempUseList;
    inputFile.clear();
    inputFile.open(fileName);
    if(!inputFile.is_open()){
        cout << "File not found!" << fileName << endl;
        exit(0);
    }
    while(true){
        string startToken = getToken();
        if(startToken == ""){
            break;
        }
        int defcount = readInt(startToken);
        for(int i = 0; i < defcount; i++){
                string symbol = readSymbol(getToken());
                int val = readInt(getToken());
                if(tempDefList.find(symbol) == tempDefList.end()){
                    tempDefList[symbol] = modelCount;
                }
        }
        int usecount = readInt(getToken());
        for(int i = 0; i < usecount; i++){
            string symbol = readSymbol(getToken());
            memoryMapUseList.push_back(symbol);
            if(tempUseList.find(symbol) == tempUseList.end()){
                tempUseList[symbol] = 0;
            }else{
                tempUseList[symbol]++;
            }
        }
        vector<bool> ifSymbolUsed(memoryMapUseList.size(), false);
        int instrcount = readInt(getToken());
        for(int i = 0; i < instrcount; i++){
            string currentInstrcountIndex;
            stringstream instrcountIndex;
            instrcountIndex << setw(3) << setfill('0') << totalInstructions;
            instrcountIndex >> currentInstrcountIndex;
            string MARIE = readMARIE(getToken());
            int operand = readInt(getToken());
            if(MARIE == "R"){
                if(operand % 1000 > instrcount){
                    operand = operand/1000*1000 + moduleBaseTable[modelCount];
                    string err =to_string(operand)+ " Error: Relative address exceeds module size; relative zero used";
                    //insertMemoryMap(currentInstrcountIndex, err);
                    memoryMap.push_back(currentInstrcountIndex + ": " + err);
                }else{
                    operand = operand + moduleBaseTable[modelCount];
                    // insertMemoryMap(currentInstrcountIndex, to_string(operand));
                    memoryMap.push_back(currentInstrcountIndex + ": " + to_string(operand));
                }
            }else if(MARIE == "E"){
                // int useIndex = operand % 1000;
                // int useBaseAddress = operand - useIndex;
                // string referenceSymbol = memoryMapUseList[useIndex];
                // int referenceSymbolValue = findValueFromSymbolTable(referenceSymbol);
                // operand = useBaseAddress + referenceSymbolValue;
                // ifSymbolUsed[useIndex] = true; // mark as used
                // if(symbolTable.find(referenceSymbol) == symbolTable.end()){
                //     string err =to_string(operand)+ " Error: "+ referenceSymbol +" is not defined; zero used";
                //     memoryMap.push_back(currentInstrcountIndex + ": " + err);
                // }else if(useIndex >= memoryMapUseList.size()){
                //     string err =to_string(operand/1000*1000)+ " Error: External operand exceeds length of uselist; treated as relative=0";
                //     memoryMap.push_back(currentInstrcountIndex + ": " + err);

                // }else{
                //     memoryMap.push_back(currentInstrcountIndex + ": " + to_string(operand));
                // }
              
                int useIndex = operand % 1000;
                int useBaseAddress = operand - useIndex;
                if(useIndex >= memoryMapUseList.size()){
                    string err = to_string(operand/1000*1000)+ " Error: External operand exceeds length of uselist; treated as relative=0";
                    //insertMemoryMap(currentInstrcountIndex, err);
                    memoryMap.push_back(currentInstrcountIndex + ": " + err);
                } else {
                    ifSymbolUsed[useIndex] = true; // mark as used
                    string referenceSymbol = memoryMapUseList[useIndex];
                    int referenceSymbolValue = findValueFromSymbolTable(referenceSymbol);
                    operand = useBaseAddress + referenceSymbolValue;
                    if(symbolTable.find(referenceSymbol) == symbolTable.end()){
                        string err = to_string(operand)+ " Error: " + referenceSymbol + " is not defined; zero used";
                        //insertMemoryMap(currentInstrcountIndex, err);
                        memoryMap.push_back(currentInstrcountIndex + ": " + err);
                    } else {
                        //insertMemoryMap(currentInstrcountIndex, to_string(operand));
                        memoryMap.push_back(currentInstrcountIndex + ": " + to_string(operand));
                    }
                }
            }else if(MARIE == "A"){
                int realOperand = operand%1000;
                if(realOperand >= 512){
                    realOperand = (operand/1000)*1000;
                    string err =to_string(realOperand)+ " Error: Absolute address exceeds machine size; zero used";
                    //insertMemoryMap(currentInstrcountIndex, err);
                    memoryMap.push_back(currentInstrcountIndex + ": " + err);
                }else{
                    //insertMemoryMap(currentInstrcountIndex, to_string(operand));
                    memoryMap.push_back(currentInstrcountIndex + ": " + to_string(operand));
                }
            }else if(MARIE =="M"){
                stringstream formatedOperand;
                formatedOperand << setw(4) << setfill('0') << to_string(moduleBaseTable[operand]);
                memoryMap.push_back(currentInstrcountIndex + ": " + formatedOperand.str());
            }else{
                //insertMemoryMap(currentInstrcountIndex, to_string(operand));
                memoryMap.push_back(currentInstrcountIndex + ": " + to_string(operand));
            }
            //insertMemoryMap(currentInstrcountIndex, operand);
            totalInstructions++;
        }

        /*------ error: Unused uselist ------*/
        for(int i = 0; i < ifSymbolUsed.size(); i++){
            if(!ifSymbolUsed[i]){
                string key = "notUsedWarning" + to_string(modelCount) + to_string(i) + memoryMapUseList[i];
                string value = "Warning: Module " + to_string(modelCount) + ": uselist[" + to_string(i) + "]=" + memoryMapUseList[i]+ " was not used";
                //insertMemoryMap(key, value);
                memoryMap.push_back(value);
            }
        }
        memoryMapUseList = {};  // every module has defferent useList, so we need to clear it.
        modelCount++;
    }

    /*------ error: defined but never use------*/
    for (const auto &entry : tempUseList) {
        tempDefList.erase(entry.first);
    }
    for(auto i : tempDefList){
        string warning = "Warning: Module " + to_string(i.second)+": "+ i.first + " was defined but never used\n";
        sepLineError.push_back(warning);
    }
    inputFile.close();
}

void passOne(string fileName){
    inputFile.open(fileName);
    if(!inputFile.is_open()){
        cout << "File not found!" << fileName << endl;
        exit(0);
    }
    while(true){
        string startToken = getToken();
        if(startToken == ""){
            break;
        }
        moduleBaseTable[modelCount] = modelBase_address;
        int moduleSise = 0;
        map<string, int> tempDefList;
        int defcount = readInt(startToken);
        if(defcount < 0){
                exit(2);
        }else if(defcount > 16){
                __parseerror(4);
        }
        for(int i = 0; i < defcount; i++){
                string symbol = readSymbol(getToken());
                string symbolValue;
                int val = readInt(getToken());
                if(tempDefList.find(symbol) == tempDefList.end()){
                    tempDefList[symbol] = val;
                }
                if(symbolTable.find(symbol) != symbolTable.end()){
                    string warning = "Warning: Module " + to_string(modelCount) + ": " + symbol + " redefinition ignored\n";
                    topError.push_back(warning);
                    symbolValue = to_string(findValueFromSymbolTable(symbol)) + " Error: This variable is multiple times defined; first value used";
                }else{
                    symbolValue = to_string(modelBase_address + val);
                }
                createSymbol(symbol, symbolValue);    // insert to symbol table
        }
        int usecount = readInt(getToken());
        if(usecount > 16){
            __parseerror(5);
        }
        for(int i = 0; i < usecount; i++){
            string symbol = readSymbol(getToken());
        }
        int instrcount = readInt(getToken());
        moduleSise += instrcount;
        for(int i = 0; i < instrcount; i++){
            totalInstructions += instrcount;
            if(totalInstructions > 512){
                __parseerror(6);
            }
            string MARIE = readMARIE(getToken());
            int operand = readInt(getToken());
        }
        for(auto i : tempDefList){
            int value = i.second;
            if(value > moduleSise){
                string warning = "Warning: Module " + to_string(modelCount) + ": "+ i.first + "="+ to_string(i.second) + " valid=[0.." + to_string(instrcount -1) + "] assume zero relative\n";
                topError.push_back(warning);
                symbolTable[i.first] = to_string(modelBase_address);
            }
        }
        modelCount++;
        modelBase_address += instrcount;
    }
   inputFile.close();
}

// void insertMemoryMap(string index, string value){
//     if(memoryMap.find(index) == memoryMap.end()){
//         memoryMap[index] = value;
//     }
// }

void createSymbol(string symbol, string val){
    //if(symbolTable.find(symbol) == symbolTable.end()){
        symbolTable[symbol] = val;
    //}
}

string readMARIE(string token){
    string MARIE = token;
    if(token.size() != 1){
        __parseerror(7);
    }
    if(token != "A" && token != "E" && token != "I" && token != "R" && token != "M"){
        __parseerror(7);
    }
    if(token == " " || token =="\n"){
        __parseerror(7);
    }
    return MARIE;
}

string readSymbol(string token){
    string symbol = token;
    if(token.size() > 16){
        __parseerror(3);
    }else if(token == " "){
        __parseerror(1);
    }
    for(int i = 0; i < token.size(); i++){
        if(!isalnum(token[i])){
            __parseerror(1);
        }
    }

    return symbol;
}

int readInt(string token){
    int count;
    try{
        count = stoi(token);
    }catch(exception e){
        __parseerror(0);
    }
    return count;
}

void __parseerror(int errcode){
    string errstr[] = {
        "NUM_EXPECTED", // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED", // Symbol Expected
        "ADDR_EXPECTED", // Addressing Expected which is M/A/E/I/R
        "SYM_TOLONG", // Symbol Name is to long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
        "MARIE_EXPECTED", // MARIE Expected
    };
    printf("Parse Error line %d offset %d: %s\n", lineNumber, currentOffset+1, errstr[errcode].c_str());
    exit(0);
}

void __eolerror(int errcode){
    string errstr[] = {
        "Error: This variable is multiple times defined; first value used",
        "Error: External operand exceeds length of uselist; treated as relative=0",
        "Error: Absolute address exceeds machine size; zero used",
        "Error: Relative address exceeds module size; relative zero used",
        "Error: Illegal immediate operand; treated as 999",
        "Error: Illegal opcode; treated as 9999",
        "Error: Illegal module operand ; treated as module=0",
    };
    if(errcode == 7){
        printf("Error: %s is not defined; zero used", token.c_str());
    }else{
        printf("%s\n" ,errstr[errcode].c_str());
    }
}

string getToken(){
    if(lineStream >> token) {
        size_t pos = line.find(token, tokenStart);
        if(pos != string::npos){
            currentOffset = pos;
            tokenStart = pos + token.length();
        }
        //printf("token=<%s> position=%d:%d\n", token.c_str(), lineNumber, currentOffset+1);
        return token;
    } 
    // If current line is exhausted, proceed to the next line.
    while(getline(inputFile, line)){
        if(line.find_first_not_of(" \t\r\n") == string::npos){
            token = " ";
            lineNumber++;
            currentOffset = 0;
            continue;
        }
        lineNumber++;
        tokenStart = 0;  // Reset offset tracker for the new line.
        lineStream.clear();
        lineStream.str(line);
        if(lineStream >> token){
            size_t pos = line.find(token, tokenStart);
            if(pos != string::npos){
                currentOffset = pos;
                tokenStart = pos + token.length();
            }
            //printf("token=<%s> position=%d:%d\n", token.c_str(), lineNumber, currentOffset+1);
            return token;
        }
    }
    //printf("EOF position=%d:%d\n", lineNumber, currentOffset+1);
    return "";
}

int findValueFromSymbolTable(string symbol){
    auto it = symbolTable.find(symbol);
    if(it != symbolTable.end()){
        string valueString = it->second;
        // Extract the contiguous digits from the start of valueString.
        size_t pos = valueString.find_first_not_of("0123456789");
        string numberStr = valueString.substr(0, pos);
        int value = stoi(numberStr);
     
        return value;
    }
    return 0;
}