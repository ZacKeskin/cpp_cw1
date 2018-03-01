#include <iostream>
#include <fstream>
#include <eigen3/Eigen/Dense>
#include <vector>

using namespace std;
namespace eig = Eigen;



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
                // Iterate through the points in the row
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
            
        }
    return point_matrix;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//   F U N C T I O N    T O    P E R F O R M     S U R F A C E   B A S E D   R E G I S T R A T I O N
//////////////////////////////////////////////////////////////////////////////////////////////////////

eig::MatrixXf ICP(eig::MatrixXf ps, eig::MatrixXf p1s) // ps = {Pi}, p1s = {Pi'} 
{
    using namespace eig;

    // Step 1: Initialise the current transformation to the identity transform 
    MatrixXf T(3,3);
    T = MatrixXf::Identity(3,3);

    // Step 2: Initialise the current SSD to some maximum value. 
    float SSD = 0.1;

    // Step 3: Initialise a counter to zero
    int counter = 0;
    
    
    while (counter < 1){
    
        // For each fixed point pi'  
        for (int i=0;i< p1s.rows(); i++){
            VectorXf pi1 = p1s.row(i);
        
        
        // find the closest corresponding point in the transformed point set   
        

        // Reduce ps with p1
            MatrixXf diff_mat;
            VectorXf diff;

            //diff_mat = (ps.transpose().colwise() - pi1).transpose();
            diff_mat = (ps.rowwise() - pi1.transpose());

            //cout << diff_mat.topLeftCorner(3,3) << endl << endl;
            
            diff = diff_mat.rowwise().squaredNorm();
            //cout << diff.head(3) << endl << endl;

            
            // Find pi with minimum Euclidean distance
            int min_index;
            float min = diff.minCoeff(&min_index);  
            cout << i << ":     Min. Euclidean Distance:  " << min << "       Index: " << min_index << endl;

        //p1i.dot()    

        }

        counter +=1;
    }

    return T;
}


int main(int ac, char* av[])
{

string fixed_filepath = "../data/fran_cut.txt";
string moving_filepath = "../data/fran_cut_transformed.txt";
//string fixed_filepath = "../data/fixed.txt";
//string moving_filepath = "../data/moving.txt";

eig::MatrixXf fixed = read_matrix_new(fixed_filepath);
eig::MatrixXf moving = read_matrix_new(moving_filepath);

//cout << fixed << endl << endl;
//cout << moving << endl;
ICP(moving, fixed);
return 0;
}
