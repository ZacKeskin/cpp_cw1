
Compile using:

g++ -o pb pbreg.cc -lboost_program_options


Run using

./pb --fixed ../data/fixed.txt --moving ../data/moving.txt
./pb --fixed ../data/fran_cut_transformed.txt --moving ../data/fran_cut.txt