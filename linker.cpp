#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

using namespace std;



int main(int argc, char* argv[])
{
    string fileName, token;
    int lineNumber = 0, currentOffset = 0;
    
    if (argc != 2){
        cout << "Invalid file name!" << endl;
        exit(0);
   }else{
        fileName = argv[1];
   }

   ifstream inputFile(fileName);
   if(!inputFile){
        cout << "Failed to open file: " << fileName << endl;
        return 1;
   }
   string line;
   while(getline(inputFile , line)){
        lineNumber++;
        istringstream lineToken(line);
        size_t tokenStart = 0;
        while(lineToken >> token){
            size_t found = line.find(token, tokenStart);
            currentOffset = found;
            if(found != string::npos){
                printf("token:<%s> position=%d:%zd\n", token.c_str(), lineNumber, found);
                tokenStart = found + token.length();
            }
        }  
   }
   printf("EOF position = %d:%d\n", lineNumber, currentOffset); 

    return 0;
}
