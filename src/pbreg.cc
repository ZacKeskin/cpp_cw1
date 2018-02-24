#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

#include "../headers/exceptions.h"

using std::cout;
using std::cout;
using std::endl;
namespace po = boost::program_options; // Define namespace for using program_options

// Declare functions to be defined below
vector< vector<string> > read_matrix(string filepath);



int main(int ac, char* av[])
{

//////////////////////////////////////////////////////////////////
//   R E A D    C O M M A N D - L I N E    O P T I O N S     
//////////////////////////////////////////////////////////////////
string fixed_filepath;
string moving_filepath;
string output_path;

 try {
        // Use Boost::program_options to define command-line options
        po::options_description desc("Allowed options");
        desc.add_options()
            ("fixed",  po::value<string>(), "Filepath of the fixed coordinates textfile")
            ("moving", po::value<string>(), "Filepath of the fixed coordinates textfile")
            ("output", po::value<string>(), "Filepath for the output matrix");

        po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);  

        // Read Command-Line options to get location of input data      
        if (vm.count("fixed")==1) {
            fixed_filepath = vm["fixed"].as<string>();
        }//else{throw myex;}
        //else{throw "user must provide --fixed option";}

        if (vm.count("moving")==1){
            moving_filepath = vm["moving"].as<string>();
        }//else{throw myex;}
        //else{throw "user must provide --moving option";}
        

        // Read Command-Line options to get location for output data
        if (vm.count("output")==1) {
            output_path = vm["output"].as<string>();
        }
        //else{output_path = "/output/matrix.4x4";}
        //else{throw "user must provide --output option";}
        
        cout << "The matrix will be saved to:  {current directory}/" << output_path << endl;
 }

catch(exception& e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
}


vector< vector<string> > fixed = read_matrix(fixed_filepath);
    //   P R I N T    O U T    T H E    2 D    V E C T O R
for(int row=0; row < fixed.size(); row++)
{
for (int col=0; col < fixed[row].size(); col++)
    {
        std::cout << fixed[row][col] ;
    }
    std::cout << endl;
};

return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    P E R F O R M     A R U N'S    S V D    O N   T W O   M A T R I C E S
//////////////////////////////////////////////////////////////////////////////////////////////////////

vector<float> SVD(vector<float> p, vector<float> pi)
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    R E A D    M A T R I C E S   F R O M    T E X T F I L E
//////////////////////////////////////////////////////////////////////////////////////////////////////

vector< vector<string> > read_matrix(string filepath)
{
    
    vector< vector<string> > two_D_array;

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

            // Convert all the strings in tokens to floats
            std::vector<float> flVect(tokens.size());  
            for (int c=0; c<tokens.size(); c++){flVect[c] = std::stof(tokens[c]);}
    
            // Add this vector as a new line in the 2D vector
            two_D_array.push_back(tokens); 
            }

        }
    else
        {
        cout << "File " << filepath  << " not found" << endl;
        }


return two_D_array;
}