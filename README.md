
# Build Instructions:
The codebase has been structured so that key functions required in unit tests, timing and the point&surface based registration are stored in one place and made available to each. This requires compiling each .cc file individually to create the objects. These are then linked as necessary with the functions object. Once done, the build folder contains:
-  The point & surface-based registration program
-  A unit-test binary program
-  A separate program which was used to perform timing tests on the code
## OSX 
gcc/g++ on OSX are sym-linked to invoke Clang. However the Clang version installed is not able to use OpenMP. Therefore, you must install a newer version from homebrew:

`brew install llvm`

Note the installation location. If using the default location, you will find Clang++ in  **/usr/local/opt/llvm/bin/**.

If you don't already have Boost installed, you can install this from homebrew using:

`brew install boost`

Compile source code using:
- `clang++ -c srs/functions.cc -std=c++11`
- `clang++ -c src/pbreg.cc -std=c++11`
- `clang++ -c tests/unit_tests.cc -std=c++11`

You can then link the binaries by running:

`clang++  -o build/pb pbreg.o functions.o  -lboost_program_options -std=c++11 -fopenmp -L/usr/local/Cellar/llvm/5.0.1/lib `

where you should change the `build/pb` to the desired executable name depending on whether you wish to link e.g. `pbreg.o`, `unit_tests.o`.

Note: 
-  `-std=c++11` is required to compile c++11 syntax
-  `-fopenmp` is required to use OpenMP
- `-L/usr/local/Cellar/llvm/5.0.1/lib` is required to find clang++ 
- `-lboost_program_options` is required to link the program_options binary.


## Linux
Installation instructions have not been tested on linux. However, the key differences are:
- 1) OpenMP will work natively on the gcc/g++ version installed on Ubuntu
- 2) Use the appropriate package manager (e.g. apt-get) to install Boost

It should therefore be as simple as running the command above, only swapping `g++` in place of `clang++` and removing `-L/usr/local/Cellar/llvm/5.0.1/lib`.

## Windows
Installation is a little slower on Windows
- 1) From https://www.apple.com/mac/ order a new machine
- 2) Follow steps for OSX installation above




# Run Instructions:

### Run Point-Based or Surface-Based Registration with:
Sample data is provided in the data folder. You can therefore run the program, once compiled, using this data by running the following commands:

`build/./pb --fixed data/fixed.txt --moving data/moving.txt --regtype pbreg`

`build/./pb --fixed data/fran_cut_transformed.txt --moving data/fran_cut.txt --regtype pbreg`

Note: 
-  Change regtype to pbreg or sbreg to perform point-based or surface-based registration respectively.
-  Note: regtype is optional. With no parameter, it will default to point-based registration.




# Code timings (Question 14)

### Run the executable to calculate compute times using different cores.
`build/./codetimer`
- This will calculate the code executed first without OpenMP, then using 1-4 cores.
- The mean time taken across 5 iterations is recorded in each case.

### You can optionally build the code-timing executable simply using 
`clang5 -o build/codetimer src/test_openmp.cc -lboost_program_options  -std=c++11 -fopenmp -L/usr/local/Cellar/llvm/5.0.1/lib`


Q14: Serial=83,250ms, Parallel=78,307, Cores=1
Q14: Serial=n/a, Parallel=38,463ms, Cores=2
Q14: Serial=n/a, Parallel=39,688ms, Cores=3
Q14: Serial=n/a, Parallel=38,210ms, Cores=4

- We note that the inital decrease from parallelising is significant; doubling the cores from 1 to 2 almost halves execution time. However, this trend is not maintained when increasing the cores to 3 or 4. We see little benefit parallelising beyond two separate threads.
- It is likely that the Nearest-Neighbour function, once parallelised, takes a small portion of the overall execution time. The rest of the code is strictly serial which, by Amdahl's law, may explain why there is limited speed-up below ~40s.


# OpenMP Parallelisation Schedules (Question 15):
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

### Since we cannot pass these as variables to the pre-processor, the comparisons were performed manually by changing the OpenMP Preprocessor directive to include the schedule each time.

Q15: Strategy=static, Serial=n/a, Parallel=37,379ms, Cores=2
Q15: Strategy=static, Serial=n/a, Parallel=36,404ms, Cores=3
Q15: Strategy=static, Serial=n/a, Parallel=35,287ms, Cores=4

Q15: Strategy=guided, Serial=n/a, Parallel=36,403ms, Cores=2
Q15: Strategy=guided, Serial=n/a, Parallel=40,926ms, Cores=3
Q15: Strategy=guided, Serial=n/a, Parallel=37,520ms, Cores=4

Q15: Strategy=dynamic, Serial=n/a, Parallel=39,956ms, Cores=2
Q15: Strategy=dynamic, Serial=n/a, Parallel=36,430ms, Cores=3
Q15: Strategy=dynamic, Serial=n/a, Parallel=37,385ms, Cores=4

Q15: Strategy=runtime, Serial=n/a, Parallel=45,317ms, Cores=2
Q15: Strategy=runtime, Serial=n/a, Parallel=40,464ms, Cores=3
Q15: Strategy=runtime, Serial=n/a, Parallel=37,037ms, Cores=4

- The notable effect across all schemes is a large speedup for 2 cores, and a smaller but still significant speedup using 3 cores. By the time all four cores are used, the marginal gains 
- Interestingly, we see little difference between the different strategies. Static appears to offer the most reliable improvement in times irrespective of the number of cores. Runtime has the largest difference between 2 and 4 cores, although under 4 cores it is in line with the other strategies; so this is the least useful schedule in this case.


# Unit tests (Questions 16 & 17):
- Unit tests using the Catch2 header-only testing library have been included
    - These compare the output of the PBReg and SBReg functions against provided transformation matrices
    - We use fixed.txt, moving.txt and Matrix.4x4, which are all provided in the /data directory

- Run the tests using `build/./unittests`

- If required this can be built from source using:
`/usr/local/opt/llvm/bin/clang++ -o build/unittests tests/unit_tests.cc -std=c++11 -fopenmp -L/usr/local/Cellar/llvm/5.0.1/lib`

- The functions from pbreg.cc have been copied directly into the unit_tests.cc file. This is because including the declarations in a header file meant each file needed to be compiled together, then linked. This required the compilation of all required libraries including boost program_options which was a considerable effort to achieve. Therefore copying the code was unfortunately a far more efficient approach.