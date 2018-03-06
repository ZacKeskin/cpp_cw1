
#include <eigen3/Eigen/Dense>
#include <string>


#ifndef FUNCTIONS
#define FUNCTIONS

namespace eig = Eigen; 
using namespace std;

eig::MatrixXf read_matrix_new(string);


eig::VectorXf get_T_vector(eig::MatrixXf, eig::MatrixXf, eig::MatrixXf);
eig::MatrixXf get_Nearest_Neighbours(eig::MatrixXf, eig::MatrixXf, eig::MatrixXf);
eig::MatrixXf OPEN_MP_get_Nearest_Neighbours(eig::MatrixXf, eig::MatrixXf, eig::MatrixXf);
eig::MatrixXf SVD(eig::MatrixXf, eig::MatrixXf);
eig::MatrixXf ICP(eig::MatrixXf, eig::MatrixXf);
float get_SSD(eig::MatrixXf, eig::MatrixXf, eig::MatrixXf, eig::VectorXf);


#endif