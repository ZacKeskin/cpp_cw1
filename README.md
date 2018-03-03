
### Compile using:

` g++ -o build/pb src/pbreg.cc -lboost_program_options -std=c++11`



### Run Point-Based or Surface-Based Registration with:

`build/./pb --fixed data/fixed.txt --moving data/moving.txt --regtype pbreg`

`build/./pb --fixed data/fran_cut_transformed.txt --moving data/fran_cut.txt --regtype pbreg`

* Change regtype to pbreg or sbreg
* Note: regtype is optional. With no parameter, it will default to point-based registration.



### Using OpemMP

`/usr/local/opt/llvm/bin/clang++ -o build/pb src/pbreg.cc -lboost_program_options  -std=c++11 -fopenmp`
clang5 -o build/pb src/pbreg.cc -lboost_program_options  -std=c++11 -fopenmp -L/usr/local/Cellar/llvm/5.0.1/lib

clang5 -o build/test src/test_openmp.cc -lboost_program_options  -std=c++11 -fopenmp -L/usr/local/Cellar/llvm/5.0.1/lib

* Note included homebrew's llvm 5.0
* Will alias this in terminal as `clang5`


## Code timings (Question 14)

Q14: Serial=12,356ms Parallel=12,595ms Cores=1
Q14: Serial=12,356ms Parallel=6136ms Cores=2
Q14: Serial=12,356ms Parallel=5675ms Cores=3
Q14: Serial=12,356ms Parallel=5721ms Cores=4

We see that doubling the cores from 1 to 2 roughly halves the overall time taken to perform the algorithm. However, this relationship does not seem to scale when extending to more than two cores (in fact 4 cores was measured as being slightly slower than 3.)

- **Note:** Serial uses 1 core (no OpenMP) so the figure was calculated once and shared on all lines below
- **Note also:** The SBReg algorithm (ICP() function) is not correctly implemented, giving incorrect results. 