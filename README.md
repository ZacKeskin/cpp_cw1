
### Compile using:

` g++ -o build/pb src/pbreg.cc -lboost_program_options -std=c++11`



### Run Point-Based or Surface-Based Registration with:

`build/./pb --fixed data/fixed.txt --moving data/moving.txt --regtype pbreg`

`build/./pb --fixed data/fran_cut_transformed.txt --moving data/fran_cut.txt --regtype pbreg`

* Change regtype to pbreg or sbreg
* Note: regtype is optional. With no parameter, it will default to point-based registration.





### Using OpemMP

`/usr/local/opt/llvm/bin/clang++ -o build/pb src/pbreg.cc -lboost_program_options  -std=c++11 -fopenmp`
clang5 -o build/pb src/pbreg.cc -lboost_program_options  -std=c++11

* Note included homebrew's llvm 5.0
* Will alias this in terminal as `clang5`


### TEMP
clang5 -o build/test src/test_openmp.cc -lboost_program_options  -std=c++11


clang5 -o build/test src/test_openmp.cc -lboost_program_options  -std=c++11 -fopenmp -L/usr/local/Cellar/llvm/5.0.1/lib