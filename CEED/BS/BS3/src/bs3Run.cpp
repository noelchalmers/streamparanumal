/*

The MIT License (MIT)

Copyright (c) 2020 Tim Warburton, Noel Chalmers, Jesse Chan, Ali Karakus

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

*/

#include "bs3.hpp"

void bs3_t::Run(){

#if 0
  //create arrays buffers
  int N = 0;
  settings.getSetting("BYTES", N);
  N /= sizeof(dfloat);
#else
  int N = 0;
  int Nmin = 0, Nmax = 0, Nstep = 0;
  int B = 0, Bmin = 0, Bmax = 0, Bstep = 0;
  settings.getSetting("BYTES", B);
  if(B){
    Bmin = B;
    Bmax = B;
    Bstep = sizeof(dfloat);
  }
  else{
    settings.getSetting("BMIN", Bmin);
    settings.getSetting("BMAX", Bmax);
    settings.getSetting("BSTEP", Bstep);
  }
  // should scale down by #reads + #writes per entry
  N = Bmax/sizeof(dfloat);
  Nmax = Bmax/sizeof(dfloat);
  Nmin = Bmin/sizeof(dfloat);
  Nstep = Bstep/sizeof(dfloat);

#endif
  occa::memory o_a = device.malloc(N*sizeof(dfloat));
  occa::memory o_tmp = device.malloc(blockSize*sizeof(dfloat));
  occa::memory o_norm = device.malloc(1*sizeof(dfloat));

  for(int Nrun=Nmin;Nrun<=Nmax;Nrun+=Nstep){
    int Nblock = (Nrun+blockSize-1)/blockSize;
    Nblock = (Nblock>blockSize) ? blockSize : Nblock; //limit to blockSize entries
    
    int Ntests = 10;
    int Nwarm = 5;
    
    for(int n=0;n<Nwarm;++n){ //warmup
      kernel1(Nblock, Nrun, o_a, o_tmp); //partial reduction
      kernel2(Nblock, o_tmp, o_norm); //finish reduction
    }
    
    // let GPU rest
    device.finish();
    
    /* NORM Test */
    occa::streamTag start = device.tagStream();
    
    for(int n=0;n<Ntests;++n){
      kernel1(Nblock, Nrun, o_a, o_tmp); //partial reduction
      kernel2(Nblock, o_tmp, o_norm); //finish reduction
    }
    
    occa::streamTag end = device.tagStream();
    device.finish();
    
    double elapsedTime = device.timeBetween(start, end)/Ntests;
    
    size_t bytesIn  = Nrun*sizeof(dfloat);
    size_t bytesOut = 0;
    size_t bytes = bytesIn + bytesOut;
    
    printf("BS3: " dlongFormat ", %4.4f, %1.2e, %1.2e, %4.1f ; dofs, elapsed, time per DOF, DOFs/time, BW (GB/s) \n",
	   Nrun, elapsedTime, elapsedTime/Nrun, ((dfloat) Nrun)/elapsedTime, bytes/(1e9*elapsedTime));
  }
  
  o_a.free();
  o_tmp.free();
  o_norm.free();
}
