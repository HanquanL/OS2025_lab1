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
string fileName,line,token;
ifstream inputFile;
istringstream lineStream;
size_t tokenStart = 0;
map<string, int> symbolTable;
map<int, int> moduleBaseTable;
map<string, int> memoryMap;

/*------  function declaration  ------*/
string getToken();
int readInt(string token);
string readSymbol(string token);
string readMARIE(string token);
void passOne(string fileName);
void passTwo(string fileName);
void createSymbol(string symbol, int val);
void insertMemoryMap(string index, int value);

/*------ Error INFO ------*/
void __parseerror(int errcode);
void __eolerror(int errcode);
void __seplerror(int errcode);

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
    cout <<"Symbol Table" << endl;
    for(auto i : symbolTable){
        cout << i.first << "=" << i.second << endl;
    }
    cout <<"Module Base Table" << endl;
    for(auto i : moduleBaseTable){
        cout <<"Module" << i.first << " Base Adress:" << i.second << endl;
    }
    cout << endl;
    cout <<"Memory Map" << endl;
    for(auto i : memoryMap){
        cout << i.first << ":" << i.second << endl;
    }
   
    return 0;
}

/*------  function definition  ------*/
void passTwo(string fileName){
    int modelCount = 0, modelBase_address = 0, totalInstructions = 0;
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
        if(defcount < 0){
                exit(2);
        }else if(defcount > 16){
                __parseerror(4);
        }
        for(int i = 0; i < defcount; i++){
                string symbol = readSymbol(getToken());
                int val = readInt(getToken());
        }
        int usecount = readInt(getToken());
        for(int i = 0; i < usecount; i++){
            string symbol = readSymbol(getToken());
        }
        int instrcount = readInt(getToken());
        for(int i = 0; i < instrcount; i++){
            string currentInstrcountIndex;
            stringstream instrcountIndex;
            instrcountIndex << setw(3) << setfill('0') << totalInstructions;
            instrcountIndex >> currentInstrcountIndex;
            string MARIE = readMARIE(getToken());
            int operand = readInt(getToken());
            if(MARIE == "R"){
                operand = operand + moduleBaseTable[modelCount];
                insertMemoryMap(currentInstrcountIndex, operand);
            }else{
                insertMemoryMap(currentInstrcountIndex, operand);
            }
            //insertMemoryMap(currentInstrcountIndex, operand);
            totalInstructions++;
        }
        modelCount++;
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
        int defcount = readInt(startToken);
        if(defcount < 0){
                exit(2);
        }else if(defcount > 16){
                __parseerror(4);
        }
        for(int i = 0; i < defcount; i++){
                string symbol = readSymbol(getToken());
                int val = readInt(getToken());
                createSymbol(symbol, modelBase_address + val);
        }
        int usecount = readInt(getToken());
        for(int i = 0; i < usecount; i++){
            string symbol = readSymbol(getToken());
        }
        int instrcount = readInt(getToken());
        for(int i = 0; i < instrcount; i++){
            string MARIE = readMARIE(getToken());
            int operand = readInt(getToken());
        }
        modelCount++;
        modelBase_address += instrcount;
    }
   inputFile.close();
}

void insertMemoryMap(string index, int value){
    if(memoryMap.find(index) == memoryMap.end()){
        memoryMap[index] = value;
    }
}

void createSymbol(string symbol, int val){
    // symbolTable.push_back(symbol);
    // symbolTable.push_back(to_string(val));
    if(symbolTable.find(symbol) == symbolTable.end()){
        symbolTable[symbol] = val;
    }
}

string readMARIE(string token){
    string MARIE = token;
    if(token.size() != 1){
        __parseerror(2);
    }
    if(token != "A" && token != "E" && token != "I" && token != "R" && token != "M"){
        __parseerror(2);
    }
    return MARIE;
}

string readSymbol(string token){
    string symbol = token;
    if(token.size() > 16){
        __parseerror(3);
    }else if(token.size() == 0){
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
        "TO_MANY_DEF_IN_MODULE", // > 16
        "TO_MANY_USE_IN_MODULE", // > 16
        "TO_MANY_INSTR", // total num_instr exceeds memory size (512)
    };
    printf("Parse Error: token<%s> at line %d offset %d: %s\n", token.c_str(),lineNumber, currentOffset+1, errstr[errcode].c_str());
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

void __sepleeror(int errcode){
    if(errcode ==1){
        printf("Warning: Module %d: %s=%d valid=[0..%d] assume zero relative", modelCount, token.c_str(), 0,0);
    }else if(errcode == 2){
        printf("Warning: Module %d: %s redefinition ignored\n", modelCount, token.c_str());
    }else if(errcode ==3){
        printf("Warning: Module %d: uselist[%d]=%s was not used\n", modelCount, 0, token.c_str());
    }else if(errcode == 4){
        printf("Warning: Module %d: %s was defined but never used\n", modelCount, token.c_str());
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