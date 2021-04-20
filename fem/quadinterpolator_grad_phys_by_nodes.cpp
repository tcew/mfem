// Copyright (c) 2010-2021, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#include "quadinterpolator.hpp"
#include "quadinterpolator_grad_phys.hpp"

namespace mfem
{

template<>
void QuadratureInterpolator::PhysDerivatives<QVectorLayout::byNODES>(
   const Vector &e_vec, Vector &q_der) const
{
   const int NE = fespace->GetNE();
   if (NE == 0) { return; }
   Mesh *mesh = fespace->GetMesh();
   const int vdim = fespace->GetVDim();
   const int dim = fespace->GetMesh()->Dimension();
   const FiniteElement *fe = fespace->GetFE(0);
   const IntegrationRule *ir =
      IntRule ? IntRule : &qspace->GetElementIntRule(0);
   constexpr DofToQuad::Mode mode = DofToQuad::TENSOR;
   const DofToQuad &maps = fe->GetDofToQuad(*ir, mode);
   const GeometricFactors *geom =
      mesh->GetGeometricFactors(*ir, GeometricFactors::JACOBIANS, mode);

   const int D1D = maps.ndof;
   const int Q1D = maps.nqpt;

   const double *B = maps.B.Read();
   const double *G = maps.G.Read();
   const double *J = geom->J.Read();
   const double *X = e_vec.Read();
   double *Y = q_der.Write();

   constexpr QVectorLayout L = QVectorLayout::byNODES;

   const int id = (vdim<<8) | (D1D<<4) | Q1D;

   if (dim == 2)
   {
      switch (id)
      {
         case 0x133: return PhysGrad2D<L,1,3,3,8>(NE,B,G,J,X,Y);
         case 0x134: return PhysGrad2D<L,1,3,4,8>(NE,B,G,J,X,Y);
         case 0x143: return PhysGrad2D<L,1,4,3,4>(NE,B,G,J,X,Y);
         case 0x144: return PhysGrad2D<L,1,4,4,4>(NE,B,G,J,X,Y);
         case 0x146: return PhysGrad2D<L,1,4,6,4>(NE,B,G,J,X,Y);
         case 0x158: return PhysGrad2D<L,1,5,8,2>(NE,B,G,J,X,Y);

         case 0x233: return PhysGrad2D<L,2,3,3,8>(NE,B,G,J,X,Y);
         case 0x234: return PhysGrad2D<L,2,3,4,8>(NE,B,G,J,X,Y);
         case 0x243: return PhysGrad2D<L,2,4,3,4>(NE,B,G,J,X,Y);
         case 0x244: return PhysGrad2D<L,2,4,4,4>(NE,B,G,J,X,Y);
         case 0x246: return PhysGrad2D<L,2,4,6,4>(NE,B,G,J,X,Y);
         case 0x258: return PhysGrad2D<L,2,5,8,2>(NE,B,G,J,X,Y);
         default:
         {
            constexpr int MD = MAX_D1D;
            constexpr int MQ = MAX_Q1D;
            MFEM_VERIFY(D1D <= MD, "Orders higher than " << MD-1
                        << " are not supported!");
            MFEM_VERIFY(Q1D <= MQ, "Quadrature rules with more than " << MQ
                        << " 1D points are not supported!");
            PhysGrad2D<L,0,0,0,0,MD,MQ>(NE, B, G, J, X, Y, vdim, D1D, Q1D);
            return;
         }
      }
   }
   if (dim == 3)
   {
      switch (id)
      {
         case 0x133: return PhysGrad3D<L,1,3,3>(NE,B,G,J,X,Y);
         case 0x134: return PhysGrad3D<L,1,3,4>(NE,B,G,J,X,Y);
         case 0x144: return PhysGrad3D<L,1,4,4>(NE,B,G,J,X,Y);
         case 0x146: return PhysGrad3D<L,1,4,6>(NE,B,G,J,X,Y);
         case 0x158: return PhysGrad3D<L,1,5,8>(NE,B,G,J,X,Y);

         case 0x333: return PhysGrad3D<L,3,3,3>(NE,B,G,J,X,Y);
         case 0x334: return PhysGrad3D<L,3,3,4>(NE,B,G,J,X,Y);
         case 0x344: return PhysGrad3D<L,3,4,4>(NE,B,G,J,X,Y);
         case 0x346: return PhysGrad3D<L,3,4,6>(NE,B,G,J,X,Y);
         case 0x358: return PhysGrad3D<L,3,5,8>(NE,B,G,J,X,Y);
         default:
         {
            constexpr int MD = 8;
            constexpr int MQ = 8;
            MFEM_VERIFY(D1D <= MD, "Orders higher than " << MD-1
                        << " are not supported!");
            MFEM_VERIFY(Q1D <= MQ, "Quadrature rules with more than " << MQ
                        << " 1D points are not supported!");
            PhysGrad3D<L,0,0,0,MD,MQ>(NE, B, G, J, X, Y, vdim, D1D, Q1D);
            return;
         }
      }
   }
   mfem::out << "Unknown kernel 0x" << std::hex << id << std::endl;
   MFEM_ABORT("Unknown kernel");
}

} // namespace mfem