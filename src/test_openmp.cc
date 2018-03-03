#include <iostream>
#include <fstream>
#include <eigen3/Eigen/Dense>
#include <vector>

using namespace std;
namespace eig = Eigen;

// Calculate Nearest Neighbours between pointsets
eig::MatrixXf get_Nearest_Neighbours(eig::MatrixXf pointset1, eig::MatrixXf pointset2)
{
    using namespace eig;
    // Define new empty matrix to store Nearest Neighbours
    MatrixXf Neighbours(0,3);
    
    for (int i=0;i< pointset1.rows(); i++){
            VectorXf point_i = pointset1.row(i);

             // Find Nearest Neighbour for each fixed point (pi with the minimum Euclidean distance)
                MatrixXf diff_mat = (pointset2.rowwise() - point_i.transpose()); // Vector distance between pi1 and each point in the transformed pointset
                VectorXf diff = diff_mat.rowwise().squaredNorm();    // Euclidean distance between pi1 and each point in the transformed pointset
           
                int min_index;
                float min = diff.minCoeff(&min_index);  
                // Add Nearest Neighbour to new pointset
                Neighbours.conservativeResize(Neighbours.rows()+1, Neighbours.cols());
                Neighbours.row(Neighbours.rows()-1) = pointset2.row(min_index);
            }
    return Neighbours;
}

eig::MatrixXf OPEN_MP_get_Nearest_Neighbours(eig::MatrixXf pointset1, eig::MatrixXf pointset2)
{
    using namespace eig;
    // Define new empty matrix to store Nearest Neighbours
    MatrixXf Neighbours(0,3);
    
    // Parallelise this for loop using OpenMP
    #pragma omp parallel private(x), firstprivate(sum), shared(pi)
    {
    #pragma omp for
    for (int i=0;i< pointset1.rows(); i++){
            VectorXf point_i = pointset1.row(i);

             // Find Nearest Neighbour for each fixed point (pi with the minimum Euclidean distance)
                MatrixXf diff_mat = (pointset2.rowwise() - point_i.transpose()); // Vector distance between pi1 and each point in the transformed pointset
                VectorXf diff = diff_mat.rowwise().squaredNorm();    // Euclidean distance between pi1 and each point in the transformed pointset
           
                int min_index;
                float min = diff.minCoeff(&min_index);  
                // Add Nearest Neighbour to new pointset
                Neighbours.conservativeResize(Neighbours.rows()+1, Neighbours.cols());
                Neighbours.row(Neighbours.rows()-1) = pointset2.row(min_index);
            }
    return Neighbours;
    }
}


// Get Translation Vector
eig::VectorXf get_T_vector(eig::MatrixXf R, eig::MatrixXf ps, eig::MatrixXf p1s)
{
    using namespace eig;
    // Calculate mean point positions (P and P') for moving and fixed pointsets
    VectorXf p = ps.colwise().mean();
    VectorXf p1 = p1s.colwise().mean();

    VectorXf T = p1 - R * p;
    return T;
}

// Calculate SSD
float get_SSD(eig::MatrixXf ps, eig::MatrixXf p1s, Eigen::MatrixXf R, Eigen::VectorXf T)
{
    float sum_SD = 0.0f;
    //for each pointset/row 
    for (int i =0; i< ps.rows(); i++)
        { // sum the squared difference       
          float SD = (p1s.row(i).transpose() - (R * ps.row(i).transpose() + T)).squaredNorm() ;
          sum_SD += SD;
        }    
    
    
    return sum_SD;
}

// Perform PBReg
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

// Read Data from File
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
    MatrixXf Transform(3,3);
    Transform = MatrixXf::Identity(3,3);
    MatrixXf transformed_ps (0,3);

    // Step 2: Initialise the current SSD to some maximum value. 
    float SSD = 10e6;

    // Step 3: Initialise a counter to zero
    int counter = 0;
    
    
    while (counter < 10){
        // Declare empty matrix to store nearest neighbour pointset
        MatrixXf Neighbours(0,3);
        cout << "Loop " << counter << endl;

        // Transform the moving data. (Inital loop uses the identity transform).
        MatrixXf transformed_ps = (Transform * ps.transpose()).transpose();

        // Get set of Nearest Neighbours from this transformed data
        Neighbours = get_Nearest_Neighbours(p1s, transformed_ps); // args(Fixed, Moving)

        // With Nearest Neighbours pointset, we perform PBReg to get new Transformation
        Transform = SVD(Neighbours, p1s).topLeftCorner(3,3);

        // First calculate the Translation Vector T
        VectorXf T = get_T_vector(Transform, Neighbours, p1s);  
        
        // Then use this to compute the SSD of this transformation
        float new_SSD = get_SSD(Neighbours, p1s, Transform, T);
        if (get_SSD(Neighbours, p1s, Transform, T) <= SSD){
            SSD = new_SSD;
            counter +=1;
        }
        else{
            cout << "Sum of Squared Difference Increased; exiting loop" << endl;
            cout << endl << "Transformation matrix: " << endl;
            return Transform;
        }
        
    }
    cout << endl << "Transformation matrix: " << endl;
    return Transform;
}


int main(int ac, char* av[])
{

    string fixed_filepath = "data/fran_cut.txt";
    string moving_filepath = "data/fran_cut_transformed.txt";
    //string fixed_filepath = "data/fixed.txt";
    //string moving_filepath = "data/moving.txt";

    eig::MatrixXf fixed = read_matrix_new(fixed_filepath);
    eig::MatrixXf moving = read_matrix_new(moving_filepath);
    cout << "Data imported." << endl;

    cout << endl << "Fixed:" << endl << fixed.topLeftCorner(3,3) << endl;
    cout << endl << "Moving:" << endl << moving.topLeftCorner(3,3) << endl;
    
    cout << endl << ICP(moving, fixed).topLeftCorner(3,3) << endl;
    return 0;
}
