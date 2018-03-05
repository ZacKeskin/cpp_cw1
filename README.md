
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

Mean runtime over 5 iterations; Serial execution took: 85744ms
Mean runtime over 5 iterations; Parallel execution with 1 threads took: 83085ms
Mean runtime over 5 iterations; Parallel execution with 2 threads took: 66167ms
Mean runtime over 5 iterations; Parallel execution with 3 threads took: 42170ms
Mean runtime over 5 iterations; Parallel execution with 4 threads took: 38891ms

## OpenMP Parallelisation Schemes:
### OpenMP can distribute operations across threads in different ways. The inbuilt schemes are described below (from https://computing.llnl.gov/tutorials/openMP/)

* STATIC    
    - Loop iterations are divided into pieces of size chunk and then statically assigned to threads. If chunk is not specified, the iterations are evenly (if possible) divided contiguously among the threads.
* DYNAMIC
    - Loop iterations are divided into pieces of size chunk, and dynamically scheduled among the threads; when a thread finishes one chunk, it is dynamically assigned another. The default chunk size is 1.
* GUIDED
    - Iterations are dynamically assigned to threads in blocks as threads request them until no blocks remain to be assigned. Similar to DYNAMIC except that the block size decreases each time a parcel of work is given to a thread. The size of the initial block is proportional to: number_of_iterations / number_of_threads. Subsequent blocks are proportional to number_of_iterations_remaining / number_of_threads. The chunk parameter defines the minimum block size. The default chunk size is 1.
* RUNTIME
    - The scheduling decision is deferred until runtime by the environment variable OMP_SCHEDULE. It is illegal to specify a chunk size for this clause.
* AUTO
    - The scheduling decision is delegated to the compiler and/or runtime system.

### Since we cannot pass these as variables to the pre-processor, the comparisons were performed manually by changing the 