#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "../headers/exceptions.h"

using std::cout;
using std::cout;
using std::endl;
namespace po = boost::program_options; // Define namespace for using program_options


int main(int ac, char* av[])
{

//////////////////////////////////////////////////////////////////
//   R E A D    C O M M A N D - L I N E    O P T I O N S     
//////////////////////////////////////////////////////////////////
string filepath;
string output_path;

 try {
        // Use Boost::program_options to define command-line options
        po::options_description desc("Allowed options");
        desc.add_options()
            ("fixed","Filepath of the fixed coordinates textfile")
            ("moving","Filepath of the fixed coordinates textfile")
            ("output", po::value<string>(),"Filepath for the output matrix");

        po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);  

        // Read Command-Line options to define which data will be read         
        if (vm.count("fixed")==1) {
            filepath = "../Data/fixed.txt";
        }
        else if (vm.count("moving")==1){
            filepath = "../Data/moving.txt";
        }
        //else{throw "user must provide --fixed or --moving options";}
        else{throw myex;}

        if (vm.count("output")==1) {
            output_path = vm["output"].as<string>();
        }
        else{output_path = "../Data/matrix.4x4";}
        cout << "The matrix will be saved to:  {current directory}/" << output_path << endl;
 }

catch(exception& e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
}


//////////////////////////////////////////////////////////////////
//   R E A D     P O I N T     D A T A    F R O M    F I L E 
//////////////////////////////////////////////////////////////////



std::vector<std::vector<string> > two_D_array;

// Create a filestream object (read only)
ifstream myfile;
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
        // Add this vector as a new line in the 2D vector
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