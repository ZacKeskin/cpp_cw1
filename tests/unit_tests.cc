#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <iostream>
#include <fstream>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <omp.h>
#include "../headers/catch.h"
#include "../headers/functions.h"


using namespace std;
namespace eig = Eigen;


eig::MatrixXf read_matrix3(string filepath)
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
        }
    cout << endl;
    return point_matrix;
}

eig::MatrixXf read_matrix4(string filepath)
{
    // Create a filestream object (read only)
    ifstream myfile;
    // Use it to open our text file
    myfile.open (filepath, ios::out); 
    eig::MatrixXf point_matrix(0,4);

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
            std::istringstream lineStream(line);
            std::string cell;
            
            int i = 0;
            std::vector<float> point_vector(4);
            // Iterate over the line (skipping whitespace with std::ws())
            while (std::getline(std::ws(lineStream), cell, ' ')) {
                // Iterate through the points in the row. Put each triple into a coordinate vector:
                point_vector[i%4] = std::stof(cell);
                i++;
                if (i%4 ==0) {
                    // Map this filled std::vector into an Eigen::vector
                    eig::Map<eig::Vector4f> eig_vector(point_vector.data());
                    // Append the Eigen::vector to the Eigen::matrix
                    point_matrix.conservativeResize(point_matrix.rows()+1, point_matrix.cols());
                    point_matrix.row(point_matrix.rows()-1) = eig_vector;
                    // Create a new point-vector
                    std::vector<float> point_vector(4);}
                }           
            }   
        }
    cout << endl;
    return point_matrix;
}






TEST_CASE( "Test that Point-Based Registration retuns correct matrix", "[PBReg]" ) {
    string fixed_filepath = "data/fixed.txt";
    string moving_filepath = "data/moving.txt";
    string transform_filepath = "data/matrix.4x4";

    eig::MatrixXf fixed = read_matrix3(fixed_filepath);
    eig::MatrixXf moving = read_matrix3(moving_filepath);
    eig::MatrixXf transformation = read_matrix4(transform_filepath);
    REQUIRE( SVD(moving, fixed).isApprox(transformation,10e-2) == true );
    }

TEST_CASE( "Test that Surface-Based Registration retuns correct matrix", "[SBReg]" ) {
    string fixed_filepath = "data/fixed.txt";
    string moving_filepath = "data/moving.txt";
    string transform_filepath = "data/matrix.4x4";

    eig::MatrixXf fixed = read_matrix3(fixed_filepath);
    eig::MatrixXf moving = read_matrix3(moving_filepath);
    eig::MatrixXf transformation = read_matrix4(transform_filepath);
    REQUIRE( ICP(moving, fixed).isApprox(transformation,10e-2) == true );
    }