
Compile using:

g++ -o pb pbreg.cc -lboost_program_options -std=c++11



Run Point-Based Registration with:

./pb --fixed ../data/fixed.txt --moving ../data/moving.txt --regtype pbreg
./pb --fixed ../data/fran_cut_transformed.txt --moving ../data/fran_cut.txt --regtype pbreg

* Note: regtype is optional. With no parameter, it will default to point-based registration.

Run Surface-Based Registration with:

./pb --fixed ../data/fran_cut_transformed.txt --moving ../data/fran_cut.txt --regtype sbreg
./pb --fixed ../data/fixed.txt --moving ../data/moving.txt --regtype sbreg