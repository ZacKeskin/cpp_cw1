#include <iostream>
#include <fstream>
#include <tuple>
#include <string>
#include <boost/program_options.hpp>
#include <eigen3/Eigen/Dense>
#include <omp.h>

#include "../headers/exceptions.h"
using namespace std;
namespace po = boost::program_options; // Define namespace for using program_options
namespace eig = Eigen; 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    R E A D    M A T R I C E S   F R O M    T E X T F I L E
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eig::MatrixXf read_matrix_new(string filepath)
{
    try
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
                // Split each line into an array using tab delimiter
                std::string str = line;

                // Declare variables used to receive data from file
                eig::Vector3f row_vector;
                std::istringstream lineStream(line);
                std::string cell;
                
                int i = 0;
                std::vector<float> point_vector(3);
                // Iterate over the line (skipping whitespace with std::ws())
                while (std::getline(std::ws(lineStream), cell, ' ')) {
                    // Iterate through the points in the row. Put each triple into a coordinate vector:
                    point_vector[i%3] = std::stof(cell);
                    i++;
                    if (i%3 ==0) {
                        // Map this filled std::vector into an Eigen::vector
                        eig::Map<eig::Vector3f> eig_vector(point_vector.data());
                        // Append the Eigen::vector to the Eigen::matrix
                        point_matrix.conservativeResize(point_matrix.rows()+1, point_matrix.cols());
                        point_matrix.row(point_matrix.rows()-1) = eig_vector;
                        // Create a new point-vector
                        std::vector<float> point_vector(3);}
                    }           
                }   
            
            cout << endl;
            return point_matrix;  
            }
        else{
            throw(myexception("Required input file was not found; check you have entered the correct filepath"));
        }
    }
    catch(exception& e) {
    cout << "Error: " << e.what() << "\n";
    exit(1);
    }

      
  
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    R E T U R N     T R A N S L A T I O N     V E C T O R
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eig::VectorXf get_T_vector(eig::MatrixXf R, eig::MatrixXf ps, eig::MatrixXf p1s)
{
    using namespace eig;
    // Calculate mean point positions (P and P') for moving and fixed pointsets
    VectorXf p = ps.colwise().mean();
    VectorXf p1 = p1s.colwise().mean();

    VectorXf T = p1 - R * p;
    return T;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    R E T U R N     S S D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_SSD(eig::MatrixXf ps, eig::MatrixXf fixed_ps, Eigen::MatrixXf R, Eigen::VectorXf T)
{
    float sum_SD = 0.0f;
    //for each pointset/row 
    for (int i =0; i< ps.rows(); i++)
        { // sum the squared difference       
          float SD = (fixed_ps.row(i).transpose() - (R * ps.row(i).transpose() + T)).squaredNorm() ;
          sum_SD += SD;
        }    
    
    
    return sum_SD;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N S   T O    G E T    N E A R E S T    N E I G H B O U R S    B E T W E E N    P O I N T S E T S
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eig::MatrixXf get_Nearest_Neighbours(eig::MatrixXf original_fixed, eig::MatrixXf original_moving, eig::MatrixXf transformed_moving)
{
    using namespace eig;
    // Define new empty matrix to store Nearest Neighbours
    MatrixXf Neighbours(0,3);
    
    for (int i=0;i< original_fixed.rows(); i++){
            VectorXf point_i = original_fixed.row(i);

             // Find Nearest Neighbour for each fixed point (pi with the minimum Euclidean distance)
                MatrixXf diff_mat = (transformed_moving.rowwise() - point_i.transpose()); // Vector distance between pi1 and each point in the transformed pointset
                VectorXf diff = diff_mat.rowwise().squaredNorm();    // Euclidean distance between pi' and each point in the transformed pointset
           
                int min_index;
                float min = diff.minCoeff(&min_index);  
                // Add Nearest Neighbour to new pointset
                Neighbours.conservativeResize(Neighbours.rows()+1, Neighbours.cols());
                Neighbours.row(Neighbours.rows()-1) = original_moving.row(min_index);
            }
    return Neighbours;
}
eig::MatrixXf OPEN_MP_get_Nearest_Neighbours(eig::MatrixXf original_fixed, eig::MatrixXf original_moving, eig::MatrixXf transformed_moving)
{
    using namespace eig;
    // Define new empty matrix to store Nearest Neighbours
    MatrixXf Neighbours(original_fixed.rows(),3);
    VectorXf point_i;
    int nthreads, tid;
    // Parallelise this for loop using OpenMP
    #pragma omp parallel private(point_i)
    {
        /* Obtain thread number */
        tid = omp_get_thread_num();
        nthreads = omp_get_num_threads();
        int min_index;

    #pragma omp for
    for (int i=0;i< original_fixed.rows(); i++){
            point_i = original_fixed.row(i);

             // Get Vector distance between pi1 and each point in the transformed pointset
            MatrixXf diff_mat = (transformed_moving.rowwise() - point_i.transpose()); 
            // Take Norm to find Euclidean distance between pi1 and each point in the transformed pointset
            VectorXf diff = diff_mat.rowwise().squaredNorm();    
            // Find index of minimum distance point
            float min = diff.minCoeff(&min_index);  
            // Add nearest neigbour to new pointset
            Neighbours.row(i) = original_moving.row(min_index);
            }
    } // End OpenMP Parallel region
    return Neighbours;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    P E R F O R M     P B   R E G 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eig::MatrixXf SVD(eig::MatrixXf ps, eig::MatrixXf p1s) // ps = {Pi}, p1s = {Pi'} 
{
    using namespace eig;

    //  Step 1: From {Pi},{Pi'} calculate means Pi and Pi';
    VectorXf p = ps.colwise().mean();
    VectorXf p1 = p1s.colwise().mean();


    // and then calculate error qi and qi';
    MatrixXf qs = (ps.transpose().colwise() -= p).transpose();
    MatrixXf q1s = (p1s.transpose().colwise() -= p1).transpose();


    // Step 2: Calculate the 3 x 3 matrix H
    MatrixXf H = MatrixXf::Zero(qs.cols(),qs.cols());
    for (int i = 0; i < qs.rows(); i++ ){
        H += (qs.row(i).transpose() * q1s.row(i));
    }
    
    
    // Step 3: Calculate the SVD of matrix H
    JacobiSVD<MatrixXf> svd(H, ComputeThinU | ComputeThinV);
    MatrixXf U = svd.matrixU();
    MatrixXf V = svd.matrixV();
    

    // Step 4: Calculate matrix X = VU'
    MatrixXf X = V * U.transpose();


    // Step 5: Calculate determinant of X
    if (X.determinant() == -1){ cout << "Error! Determinant = -1";}
    

    // Final Steps (i): Calculate Translation vector 
    VectorXf T = get_T_vector(X, ps, p1s);

    // Final Steps (ii): Print Sum of Squared Difference
    cout << "Sum of Squared Difference: " << get_SSD(ps, p1s, X, T) << endl;

    // Final Steps (iii): Append Translation Vector as final column and expand to return 4x4 matrix
    X.conservativeResize(X.rows(),X.cols()+1);
    X.col(X.cols()-1) = T;

    RowVectorXf bottom_row(1,4);
    bottom_row << 0, 0, 0, 1;
    X.conservativeResize(X.rows()+1,X.cols());
    X.row(X.rows()-1) = bottom_row;
    
    return X;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    P E R F O R M     S B   R E G 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eig::MatrixXf ICP(eig::MatrixXf moving_ps, eig::MatrixXf fixed_ps) // ps = {Pi}, p1s = {Pi'} 
{
    using namespace eig;
    
    // Step 1: Initialise the current transformation to the identity transform 
    MatrixXf Transform(3,3);
    VectorXf Translate(3);
    Transform = MatrixXf::Identity(3,3);
    Translate = VectorXf::Zero(3);

    // Step 2: Initialise the current SSD to some maximum value. 
    float SSD = 10e6;

    // Step 3: Initialise a counter to zero
    int counter = 0;
    
    
    while (counter < 30){
        // Declare empty matrix to store nearest neighbour pointset
        MatrixXf Neighbours(0,3);
        cout << "Loop " << counter << endl;
        
        // Transform the moving data. (Inital loop uses the identity transform).
        MatrixXf transformed_ps = (Transform * moving_ps.transpose()).transpose();
        transformed_ps = (transformed_ps.transpose().colwise() + Translate).transpose();

        // Get set of Nearest Neighbours from this transformed data
        Neighbours = OPEN_MP_get_Nearest_Neighbours(fixed_ps, moving_ps, transformed_ps); 
        
        // With Nearest Neighbours pointset, we perform PBReg to get new Transformation
        MatrixXf M = SVD(Neighbours, fixed_ps);
        Transform = M.topLeftCorner(3,3);
        Translate = M.topRightCorner(3,1);

        // First calculate the Translation Vector T
        VectorXf T = get_T_vector(Transform, Neighbours, fixed_ps);  

        // Then use this to compute the SSD of this transformation
        float new_SSD = get_SSD(Neighbours, fixed_ps, Transform, T);

        /* Note: we currently run to the max loopcount, unless SSD < 10^-6.
           This is because we find the algorithm ends too soon, which prevents 
           convergence to the transformation matrix  */
        if (new_SSD >10e-6){ 
            SSD = new_SSD;
            counter +=1;
        }
        else{
            //cout << "Sum of Squared Difference Increased; exiting loop" << endl;
            //cout << endl << "Transformation matrix: " << endl;
            break;
            //SSD = new_SSD;
            //counter +=1;
        }
        
    }
    // Final Steps (i): Calculate Translation vector 
    VectorXf T = get_T_vector(Transform, moving_ps, fixed_ps);

    // Final Steps (ii): Append Translation Vector as final column and expand to return 4x4 matrix
    Transform.conservativeResize(Transform.rows(),Transform.cols()+1);
    Transform.col(Transform.cols()-1) = T;

    RowVectorXf bottom_row(1,4);
    bottom_row << 0, 0, 0, 1;
    Transform.conservativeResize(Transform.rows()+1,Transform.cols());
    Transform.row(Transform.rows()-1) = bottom_row;
    cout << endl << "Transformation matrix: " << endl;
    return Transform;
}

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

