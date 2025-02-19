#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

using namespace std;

/*------  global variable  ------*/
int lineNumber = 0, currentOffset = 0;
string fileName,line;

/*------  function declaration  ------*/
void __parseerror(int errcode);
string getToken(string fileName);
int readInt(string token);
string readSymbol(string token);
string readMARIE(string token);
void pass1(string fileName);

/*------  main function  ------*/
int main(int argc, char* argv[])
{
    if (argc != 2){
        cout << "Invalid file name!" << endl;
        exit(0);
    }else{
        fileName = argv[1];
    }
   //getToken(fileName);    // only uncomment this line for testing purpose
    cout <<"Symbol Table" << endl;
    cout <<"Memory Map" << endl;
    //pass1(fileName);
   
    return 0;
}

/*------  function definition  ------*/
void pass1(string fileName){
    while(true){
        string currentToken = getToken(fileName);
        int defcount;
        if(currentToken == ""){
            break;
        }else if(defcount >16){
            __parseerror(3);
        }else{
            defcount = readInt(currentToken);
        }
        while(defcount > 0){
            string symbol = readSymbol(currentToken);
            int relAdd = readInt(currentToken);
            
            defcount--;
        }
    }
}

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
string getToken(string fileName){
    string token;
    ifstream inputFile(fileName);
    if(!inputFile){
            cout << "Failed to open file: " << fileName << endl;
            return "";
    }
    
    while(getline(inputFile , line)){
            lineNumber++;
            istringstream lineToken(line);
            size_t tokenStart = 0;
            while(lineToken >> token){
                size_t found = line.find(token, tokenStart);
                currentOffset = found;
                if(found != string::npos){
                    //printf("token:<%s> position=%d:%zd\n", token.c_str(), lineNumber, found +1);
                    cout << token << endl;
                    tokenStart = found + token.length();
                    return token;
                }
            }
    }
    printf("EOF position = %d:%d\n", lineNumber, currentOffset +1); 
    inputFile.close();
    return "";
}