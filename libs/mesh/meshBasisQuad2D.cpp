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

#include "mesh.hpp"

namespace libp {

// ------------------------------------------------------------------------
// QUAD 2D NODES
// ------------------------------------------------------------------------
void mesh_t::NodesQuad2D(int _N, dfloat _r[], dfloat _s[]){
  int _Nq = _N+1;

  memory<dfloat> r1D(_Nq);
  JacobiGLL(_N, r1D.ptr()); //Gauss-Legendre-Lobatto nodes

  //Tensor product
  for (int j=0;j<_Nq;j++) {
    for (int i=0;i<_Nq;i++) {
      _r[i+j*_Nq] = r1D[i];
      _s[i+j*_Nq] = r1D[j];
    }
  }
}

void mesh_t::FaceNodesQuad2D(int _N, dfloat _r[], dfloat _s[], int _faceNodes[]){
  int _Nq = _N+1;
  int _Nfp = _Nq;
  int _Np = _Nq*_Nq;

  int cnt[4];
  for (int i=0;i<4;i++) cnt[i]=0;

  dfloat deps = 1.;
  while((1.+deps)>1.)
    deps *= 0.5;

  const dfloat NODETOL = 1000.*deps;

  for (int n=0;n<_Np;n++) {
    if(fabs(_s[n]+1)<NODETOL)
      _faceNodes[0*_Nfp+(cnt[0]++)] = n;
    if(fabs(_r[n]-1)<NODETOL)
      _faceNodes[1*_Nfp+(cnt[1]++)] = n;
    if(fabs(_s[n]-1)<NODETOL)
      _faceNodes[2*_Nfp+(cnt[2]++)] = n;
    if(fabs(_r[n]+1)<NODETOL)
      _faceNodes[3*_Nfp+(cnt[3]++)] = n;
  }
}

void mesh_t::VertexNodesQuad2D(int _N, dfloat _r[], dfloat _s[], int _vertexNodes[]){
  int _Nq = _N+1;
  int _Np = _Nq*_Nq;

  dfloat deps = 1.;
  while((1.+deps)>1.)
    deps *= 0.5;

  const dfloat NODETOL = 1000.*deps;

  for(int n=0;n<_Np;++n){
    if( (_r[n]+1)*(_r[n]+1)+(_s[n]+1)*(_s[n]+1)<NODETOL)
      _vertexNodes[0] = n;
    if( (_r[n]-1)*(_r[n]-1)+(_s[n]+1)*(_s[n]+1)<NODETOL)
      _vertexNodes[1] = n;
    if( (_r[n]-1)*(_r[n]-1)+(_s[n]-1)*(_s[n]-1)<NODETOL)
      _vertexNodes[2] = n;
    if( (_r[n]+1)*(_r[n]+1)+(_s[n]-1)*(_s[n]-1)<NODETOL)
      _vertexNodes[3] = n;
  }
}

/*Find a matching array between nodes on matching faces */
void mesh_t::FaceNodeMatchingQuad2D(int _N, dfloat _r[], dfloat _s[],
                                   int _faceNodes[], int R[]){

  int _Nfp = (_N+1);

  const dfloat NODETOL = 1.0e-5;

  dfloat V[2] = {-1.0, 1.0};

  dfloat EX0[Nverts];
  dfloat EX1[Nverts];

  memory<dfloat> x0(_Nfp);
  memory<dfloat> x1(_Nfp);

  for (int fM=0;fM<Nfaces;fM++) {

    for (int v=0;v<Nverts;v++) {
      EX0[v] = 0.0;
    }
    //setup top element with face fM on the bottom
    for (int v=0;v<NfaceVertices;v++) {
      int fv = faceVertices[fM*NfaceVertices + v];
      EX0[fv] = V[v];
    }

    for(int n=0;n<_Nfp;++n){ /* for each face node */
      const int fn = _faceNodes[fM*_Nfp+n];

      /* (r,s) coordinates of interpolation nodes*/
      dfloat rn = _r[fn];
      dfloat sn = _s[fn];

      /* physical coordinate of interpolation node */
      x0[n] = 0.25*(1-rn)*(1-sn)*EX0[0]
             +0.25*(1+rn)*(1-sn)*EX0[1]
             +0.25*(1+rn)*(1+sn)*EX0[2]
             +0.25*(1-rn)*(1+sn)*EX0[3];
    }

    for (int fP=0;fP<Nfaces;fP++) { /*For each neighbor face */
      for (int rot=0;rot<NfaceVertices;rot++) { /* For each face rotation */
        // Zero vertices
        for (int v=0;v<Nverts;v++) {
          EX1[v] = 0.0;
        }
        //setup bottom element with face fP on the top
        for (int v=0;v<NfaceVertices;v++) {
          int fv = faceVertices[fP*NfaceVertices + ((v+rot)%NfaceVertices)];
          EX1[fv] = V[v];
        }

        for(int n=0;n<_Nfp;++n){ /* for each node */
          const int fn = _faceNodes[fP*_Nfp+n];

          /* (r,s,t) coordinates of interpolation nodes*/
          dfloat rn = _r[fn];
          dfloat sn = _s[fn];

          /* physical coordinate of interpolation node */
          x1[n] = 0.25*(1-rn)*(1-sn)*EX1[0]
             +0.25*(1+rn)*(1-sn)*EX1[1]
             +0.25*(1+rn)*(1+sn)*EX1[2]
             +0.25*(1-rn)*(1+sn)*EX1[3];
        }

        /* for each node on this face find the neighbor node */
        for(int n=0;n<_Nfp;++n){
          const dfloat xM = x0[n];

          int m=0;
          for(;m<_Nfp;++m){ /* for each neighbor node */
            const dfloat xP = x1[m];

            /* distance between target and neighbor node */
            const dfloat dist = pow(xM-xP,2);

            /* if neighbor node is close to target, match */
            if(dist<NODETOL){
              R[fM*Nfaces*NfaceVertices*_Nfp
                + fP*NfaceVertices*_Nfp
                + rot*_Nfp + n] = m;
              break;
            }
          }

          /*Check*/
          const dfloat xP = x1[m];

          /* distance between target and neighbor node */
          const dfloat dist = pow(xM-xP,2);
          //This shouldn't happen
          LIBP_ABORT("Unable to match face node, face: " << fM
                     << ", matching face: " << fP
                     << ", rotation: " << rot
                     << ", node: " << n
                     << ". Is the reference node set not symmetric?",
                     dist>NODETOL);
        }
      }
    }
  }
}

} //namespace libp
