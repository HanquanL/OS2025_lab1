#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

using namespace std;

/*------  global variable  ------*/
int lineNumber = 0, currentOffset = 0;
string fileName,token,line;

/*------  function declaration  ------*/
void __parseerror(int errcode);
int readInt(string token);
string readSymbol(string token);
string readMARIE(string token);
int getToken(string fileName);

/*------  main function  ------*/
int main(int argc, char* argv[])
{
    if (argc != 2){
        cout << "Invalid file name!" << endl;
        exit(0);
   }else{
        fileName = argv[1];
   }
    getToken(fileName);
   
    return 0;
}

/*------  function definition  ------*/
string readMARIE(string token){
    string MARIE = token;
    if(token.size() != 1){
        __parseerror(2);
    }
    if(token[0] != 'A' || token[0] != 'E' || token[0] != 'I' || token[0] != 'R' || token[0] != 'M'){
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
    printf("Parse Error line %d offset %d: %s\n", lineNumber, currentOffset, errstr[errcode].c_str());
    exit(0);
}
int getToken(string fileName){
    ifstream inputFile(fileName);
    if(!inputFile){
            cout << "Failed to open file: " << fileName << endl;
            return 1;
    }
    
    while(getline(inputFile , line)){
            lineNumber++;
            istringstream lineToken(line);
            size_t tokenStart = 0;
            while(lineToken >> token){
                size_t found = line.find(token, tokenStart);
                currentOffset = found;
                if(found != string::npos){
                    printf("token:<%s> position=%d:%zd\n", token.c_str(), lineNumber, found +1);
                    tokenStart = found + token.length();
                }
            }  
    }
    printf("EOF position = %d:%d\n", lineNumber, currentOffset +1); 
    return 0;
}