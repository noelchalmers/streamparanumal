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

#include "bs8.hpp"

void bs8_t::Run(){

  //create occa buffers
  dlong N = mesh.Np*mesh.Nelements;
  dlong Ngather = mesh.ogs->Ngather;
  occa::memory o_q = mesh.device.malloc(N*sizeof(dfloat));
  occa::memory o_gq = mesh.device.malloc(Ngather*sizeof(dfloat));

  /* Scatter test */
  for(int n=0;n<5;++n){
    mesh.ogs->Scatter(o_q, o_gq, ogs_dfloat, ogs_add, ogs_notrans); //dry run
  }

  int Ntests = 50;

  mesh.device.finish();
  MPI_Barrier(mesh.comm);
  double startTime = MPI_Wtime();

  for(int n=0;n<Ntests;++n){
    mesh.ogs->Scatter(o_q, o_gq, ogs_dfloat, ogs_add, ogs_notrans);
  }

  mesh.device.finish();
  MPI_Barrier(mesh.comm);
  double endTime = MPI_Wtime();
  double elapsedTime = (endTime - startTime)/Ntests;

#if 0
  hlong Nblocks = mesh.ogs->localScatter.NrowBlocks+mesh.ogs->haloScatter.NrowBlocks;
  hlong NblocksGlobal;
  MPI_Allreduce(&Nblocks, &NblocksGlobal, 1, MPI_HLONG, MPI_SUM, mesh.comm);

  hlong NgatherGlobal = mesh.ogsMasked->NgatherGlobal;

  hlong NunMasked = N - mesh.Nmasked;
  hlong NunMaskedGlobal;
  MPI_Allreduce(&NunMasked, &NunMaskedGlobal, 1, MPI_HLONG, MPI_SUM, mesh.comm);

  size_t bytesIn=0;
  size_t bytesOut=0;

  bytesIn += (NblocksGlobal+1)*sizeof(dlong); //block starts
  bytesIn += (NgatherGlobal+1)*sizeof(dlong); //row starts
  bytesIn += NunMaskedGlobal*sizeof(dlong); //local Ids
  bytesIn += NgatherGlobal*sizeof(dfloat); //values
  bytesOut+= NunMaskedGlobal*sizeof(dfloat);
#else
  hlong Ntotal = mesh.Nelements*mesh.Np;
  hlong NtotalGlobal;
  MPI_Allreduce(&Ntotal, &NtotalGlobal, 1, MPI_HLONG, MPI_SUM, mesh.comm);
  size_t bytesIn  = NtotalGlobal*(sizeof(dlong)+sizeof(dfloat));
  size_t bytesOut = NtotalGlobal*(sizeof(dfloat));
#endif
  size_t bytes = bytesIn + bytesOut;

  hlong Ndofs = mesh.ogsMasked->NgatherGlobal;
  size_t Nflops = 0;

  if ((mesh.rank==0)){
    printf("BS8 (scatter): %d, " hlongFormat "," hlongFormat ", %4.4f, %1.2e, %4.1f, %4.1f, %1.2e; N, NlocalTotal, DOFs, elapsed, time per DOF, avg BW (GB/s), avg GFLOPs, DOFs/ranks*time \n",
           mesh.N,
           Ndofs,
	   NtotalGlobal,
           elapsedTime,
           elapsedTime/(Ndofs),
           bytes/(1.0e9 * elapsedTime),
           Nflops/(1.0e9 * elapsedTime),
           Ndofs/(mesh.size*elapsedTime));
  }

  o_q.free();
  o_gq.free();
}
