#include<iostream>
#include<string>
#include<fstream>

using namespace std;



int main(int argc, char* argv[])
{
    string fileName, token;
    int lineNumber, lineOffset;
    
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
   string word;
   while(inputFile >> word){
        cout << word << endl;
   }

    return 0;
}
