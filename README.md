## streamParanumal
A collection of streaming microbenchmarks relevent to finite element flow solvers for heterogeneous (GPU/CPU) systems. The initial development of streamParanumal was performed by the [Parallel Numerical Algorithms Group at Virginia Tech](http://paranumal.com).

streamParanumal is funded in part by the US Department of Energy as part of the activities of the [Center for Efficient Exscale Discretizations](http://ceed.exascaleproject.org).

### 1. How to cite the streamParanumal project:
If you use any part of streamParanumal in your research project please use the following bibliographical reference:

<pre>
@MISC{streamParanumal2020,
      author = "Chalmers, N. and Warburton, T.",
      title = "{streamParanumal}: Streaming Microbenchmarks for High-order Finite Element Methods",
      year = "2020",
      url = "https://github.com/paranumal/streamparanumal",
      note = "Release 1.0"
      }
</pre>

see the [references](#10-references) section below for additional papers to reference.

---
### 2. How you can help out:
StreamParanumal is a community project. Please help improve the benchmark suite by submitted an [Issue](https://github.com/paranumal/streamparanumal/issues)  if you notice any unexpected behavior, discover a bug, have problems installing/running/trouble shooting your installation. It benefits us as a community when issues and feature requests are shared so that we can understand how to improve the library.

Please submit feature requests as an [Issue](https://github.com/paranumal/streamparanumal/issues) for the consideration of all contributors. Likewise if you wish to submit a code change please make a GitHub [Pull Request](https://github.com/paranumal/streamparanumal/pulls).

---
### 3. Overview

Overview of microbenchmarks:

0. Kernel Launch Latency:
    - Launches a set of minimal kernels, timing their execution.

1. Copy:
    - Copy one array to another.

2. AXPY:
    - Vector addition, `y = alpha*x + beta*y`.

3. Norm:
    - Vector L2 norm.

4. Dot:
    - Vector dot product.

5. CG Update:
    - Fused Congugate gradient vector update
      * `x += alpha*p`
      * `r -= alpha*Ap`
      * `rdotr = r^T*r`

6. Gather:
   - Global finite-element assembly via summation at repeated local nodes.

7. Scatter:
   - Transfer from assembled global finite-element degrees of freedom to element-local node storage.

8. Gather+Scatter:
   - Fused gather and scatter operation.

---
### 4. OCCA dependency
```
git clone https://github.com/libocca/occa
```

#### 4-1. Build OCCA
```
cd occa
export OCCA_DIR=${PWD}
make -j `nproc` 
cd ../
```

---
### 5. Required Libraries
streamParanumal requires installed BLAS and LAPACK libraries. By default, the build system will look for `libblas` and `liblapack` in your default library search paths. The library paths can also be manually specified in `make.top` with the `CEED_BLAS_DIR` and `CEED_LAPACK_DIR` variables.

Some Linux distributions will package BLAS and LAPACK libraries. For example, on Ubuntu systems these libraries can be installed via
```
sudo apt install libblas-dev liblapack-dev
```

---
### 6. Clone: streamParanumal
```
git clone https://github.com/paranumal/streamparanumal
```

#### 6-1. Build all streamParanumal benchmarks
```
cd streamparanumal`
make -j `nproc` 
```

---
### 7. Running the benchmarks:

Each benchmark is contained in is respective sobfolder in `BS/` .Each benchmark sub-directory includes makefile, src directory, and run script.

Once build, each benchmark can be run with/without MPI directly, e.g.:
```
mpirun -np 1 ./BS1 -m <mode>
```
where `<mode>` is the desrired parallel programming model, i.e. Serial, OpenMP, CUDA, OpenCL, or HIP.

Each benchmark folder contains a run script which runs a more stressful version of each test over many vector/problem sizes. This can be run similarly to the executablies themselves:
```
./runBS1 -m <mode>
```

The run scripts in the root and `BS/` folder can also be used to quickly run one instance of each test.

### 8. License

The MIT License (MIT)

Copyright (c) 2019-2020 Parallel Numerical Algorithms Group @VT

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### 10. References

Portable high-order finite element kernels I: Streaming Operation: [arXiv version](): `Chalmers, N., and Warburton, T., 2020. Portable high-order finite element kernels I: Streaming Operation.`

