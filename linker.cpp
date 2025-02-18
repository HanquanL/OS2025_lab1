#include<iostream>
#include<string>
#include<fstream>

using namespace std;



int main(int argc, char* argv[])
{
    string fileName;
    if (argc != 2){
        cout << "Invalid file name" << endl;
        exit(0);
   }else{
        fileName = argv[1];
   }
    return 0;
}
