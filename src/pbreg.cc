#include <iostream>
#include <fstream>
#include <tuple>
#include <string>
#include <boost/program_options.hpp>
#include <eigen3/Eigen/Dense>
#include <omp.h>

#include "../headers/exceptions.h"
#include "../headers/functions.h"

using namespace std;
namespace po = boost::program_options; // Define namespace for using program_options
namespace eig = Eigen; 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    R E A D    O P T I O N S    F R O M    C O N S O L E 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::tuple <string, string, string, string> get_options(int ac, char* av[])
{
    string fixed_filepath;
    string moving_filepath;
    string output_path;
    string reg_type;

    try {
            // Use Boost::program_options to define command-line options
            po::options_description desc("Allowed options");
            desc.add_options()
                ("fixed",  po::value<string>(), "Filepath of the fixed coordinates textfile")
                ("moving", po::value<string>(), "Filepath of the fixed coordinates textfile")
                ("output", po::value<string>(), "Filepath for the output matrix")
                ("regtype", po::value<string>(), "Filepath for the output matrix");

            po::variables_map vm;        
            po::store(po::parse_command_line(ac, av, desc), vm);
            po::notify(vm);  

            // Read Command-Line options to get location of input data      
            if (vm.count("fixed")==1) {
                fixed_filepath = vm["fixed"].as<string>();
            }
            else{throw myexception("No filepath provided for --fixed option");}


            if (vm.count("moving")==1){
                moving_filepath = vm["moving"].as<string>();
            }
            else{throw myexception("No filepath provided for --moving option");}
            

            // Read Command-Line options to get location for output data
            if (vm.count("output")==1) {
                output_path = vm["output"].as<string>();
            }
            else{output_path = "output/Transformation_Matrix.4x4";}

            // Read Command-Line options to choose between PBReg or SBReg
            if (vm.count("regtype")==1) {
                reg_type = vm["regtype"].as<string>();
            }
            
            cout << endl << "The matrix will be saved to: /" << output_path << endl;
    }

    catch(exception& e) {
        cout << "Error: " << e.what() << "\n";
        exit(1);
    }

    return std::make_tuple(fixed_filepath, moving_filepath, output_path, reg_type);
}


int main(int ac, char* av[])
{
    // Parse command-line options
    string fixed_filepath;
    string moving_filepath;
    string output_path;
    string reg_type;

    std::tie(fixed_filepath, moving_filepath, output_path, reg_type)  = get_options(ac, av);

    // Read in data for fixed and moving point sets 
    eig::MatrixXf fixed = read_matrix_new(fixed_filepath);
    eig::MatrixXf moving = read_matrix_new(moving_filepath);
    eig::MatrixXf output;
    try{
        if (moving.rows() < 3 || fixed.rows() < 3){throw(myexception("PBReg requires at least three points to work."));}
        if (moving.rows() != fixed.rows() 
            && reg_type != "sbreg"){throw(myexception("PBReg requires equal numbers of fixed and moving points."));}
        }
    catch(exception& e) {
    cout << "Error: " << e.what() << "\n";
    exit(1);
    }

    if(reg_type=="sbreg"){
        // Employ ICP method to calculate output matrix
        cout << "Performing Surface-Based Registration" << endl;
        output = ICP(moving, fixed);
        cout << output << endl;
    }
    else{
        // Employ Arun's SVD method to calculate output matrix
        cout << "Performing Point-Based Registration" << endl;
        output = SVD(moving, fixed);
        cout << output << endl;
    }

    // Output matrix to file
    std::ofstream output_file(output_path);
    if (output_file.is_open())
    {
        output_file << output;
    }
    else{cout << "why not" << endl;}

    return 0;

} // End of Main() function

