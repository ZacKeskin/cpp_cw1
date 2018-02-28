#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <eigen3/Eigen/Dense>

#include "../headers/exceptions.h"

using std::cout;
using std::cout;
using std::endl;
namespace po = boost::program_options; // Define namespace for using program_options
namespace eig = Eigen; 

// Declare functions to be defined below
vector< vector<string> > read_matrix(string filepath);
eig::MatrixXf read_matrix_new(string filepath);
eig::MatrixXf SVD(eig::MatrixXf ps, eig::MatrixXf p1s);

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



 // Read in list of Vectors for moving point sets 
eig::MatrixXf fixed = read_matrix_new(fixed_filepath);
eig::MatrixXf moving = read_matrix_new(moving_filepath);


// Employ Arun's SVD method to calculate output matrix
eig::MatrixXf test = SVD(fixed, moving);
cout << test << endl;

return 0;
} // End of Main() function



//////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    P E R F O R M     A R U N'S    S V D    O N   T W O   M A T R I C E S
//////////////////////////////////////////////////////////////////////////////////////////////////////

eig::MatrixXf SVD(eig::MatrixXf ps, eig::MatrixXf p1s) // ps = {Pi}, p1s = {Pi'} 
{
    using namespace eig;

    //  Step 1: From {Pi},{Pi'} calculate means Pi and Pi';
    VectorXf pi = ps.colwise().mean();
    VectorXf pi1 = p1s.colwise().mean();

    // and then calculate error qi and qi';
    MatrixXf qs = -(ps.transpose().colwise() -= pi).transpose();
    MatrixXf q1s = -(p1s.transpose().colwise() -= pi1).transpose();


    // Step 2: Calculate the 3 x 3 matrix H
    MatrixXf H = MatrixXf::Zero(qs.cols(),qs.cols());
    for (int i = 0; i < qs.rows(); i++ ){
        H += (q1s.row(i).transpose() * qs.row(i));
    }
    
    
    // Step 3: Calculate the SVD of matrix H
    JacobiSVD<MatrixXf> svd(H, ComputeThinU | ComputeThinV);
    MatrixXf U = svd.matrixU();
    MatrixXf V = svd.matrixV();
    

    // Step 4: Calculate matrix X = VU'
    MatrixXf X = V * U.transpose();


    // Step 5: Calculate determinant of X
    cout << "Det(X) = " << X.determinant() << endl;
    if (X.determinant() == -1){ cout << "Error! Determinant = -1";}
    
    // Step 6: Include Translation vector to return 4x4 matri
    X.conservativeResize(X.rows()+1,X.cols()+1);

    return X;
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

eig::MatrixXf read_matrix_new(string filepath)
{
// Create a filestream object (read only)
    ifstream myfile;
    // Use it to open our text file
    myfile.open (filepath, ios::out); 
    eig::MatrixXf point_matrix(0,3);

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
            std::vector<float> std_row_vector(3);
            //boost::split(tokens, line, boost::is_any_of("\t"));

            // Declare variables used to receive data from file
            eig::Vector3f row_vector;
            std::istringstream lineStream(line);
            std::string cell;
            int i = 0;
            
            // Iterate over the line (skipping whitespace with std::ws())
            while (std::getline(std::ws(lineStream), cell, ' ')) {
                // Iterate through the row to add each element to the std::row_vector
                std_row_vector[i] = std::stof(cell);
                i++;
                }
            
            // Map this filled std::vector into an Eigen::vector
            eig::Map<eig::Vector3f> eig_vector(std_row_vector.data());

            // Append the Eigen::vector to the Eigen::matrix
            point_matrix.conservativeResize(point_matrix.rows()+1, point_matrix.cols());
            point_matrix.row(point_matrix.rows()-1) = eig_vector;
            
            }
            
        }
cout << endl;
return point_matrix;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    R E T U R N     S S D
//////////////////////////////////////////////////////////////////////////////////////////////////////


float SSD(eig::Vector3f pi_prime, eig::Vector3f pi, Eigen::Matrix3f R, Eigen::Vector3f T)
{
    float sum_SD = 0.0f;
    //for each pointset/row 
        // sum the squared difference
    float SD = (pi_prime - (R * pi + T)).squaredNorm() ;
    
    return sum_SD;
}
