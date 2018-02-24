#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "../headers/exceptions.h"

using std::cout;
using std::cout;
using std::endl;

int main(){

std::vector<std::vector<string> > two_D_array;

// Create a filestream object (read only)
ifstream myfile;
string filepath = "../Data/fixed.txt";
// Use it to open our text file
myfile.open (filepath, ios::out); 


// Check that the file was successfully opened
if (myfile.is_open()) 
    {

     string line;    

     // Read all lines in the file
     while ( getline (myfile,line) )
        {

        string strInput;
        // Split each line into an array using tab delimiter
        std::string str = line;
        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of("\t"));
        two_D_array.push_back(tokens); 
        }

    }
else
    {
    cout << "File " << filepath  << " not found" << endl;
    }


for(int row=0; row < two_D_array.size(); row++)
    {
    for (int col=0; col < two_D_array[row].size(); col++)
        {
            std::cout << two_D_array[row][col] ;
        }
        std::cout << endl;
    };


return 0;


}