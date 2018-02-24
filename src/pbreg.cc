#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include "../headers/exceptions.h"


using std::cout;
using std::cout;
using std::endl;

int main(){

// Create a filestream object (read only)
ifstream myfile;
string filepath = "../test/fixed.txt";
// Use it to open our text file
myfile.open (filepath, ios::out); 


// Check that the file was successfully opened
if (myfile.is_open()) 
    {

     string line;    
     int row = 0;
     int col = 0;  
     // Read all lines in the file
     while ( getline (myfile,line) )
        {
        string strInput;
        //cout << line << '\n';
       
        std::string str = line;
        std::vector<std::string> tokens;

        boost::split(tokens, line, boost::is_any_of("\t"));
         
        for(int i=0; i<tokens.size(); i++)
            {
            std::cout << tokens[i]  << '\n';
            };

        row += 1;
        
        }
    }
else
    cout << "File " << filepath  << " not found" << endl;
    
return 0;


}