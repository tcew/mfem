#pragma once
#include "Utilities.hpp"
#include "PML.hpp"
using namespace std;
using namespace mfem;

class DiagST : public Solver//
{
private:
   int nrpatch;
   int dim;
   SesquilinearForm *bf=nullptr;
   MeshPartition * povlp=nullptr;
   double omega = 0.5;
   Coefficient * ws;
   int nrlayers;
   int nxyz[3];
   const Operator * A=nullptr;
   Vector B;
   DofMap * ovlp_prob = nullptr;
   Array<SparseMatrix *> PmlMat;
   Array<KLUSolver *> PmlMatInv;
   Array2D<double> Pmllength;
   Array3D<int> subdomains;
   mutable Array<Vector * > f_orig;
   int ntransf_directions;
   UniqueIndexGenerator gen;
   Array<int> dirx;
   Array<int> diry;
   Array<int> dirz;
   mutable Array<Array<Vector * >> f_transf;

   SparseMatrix * GetPmlSystemMatrix(int ip);
   void PlotSolution(Vector & sol, socketstream & sol_sock, int ip) const;
   void GetCutOffSolution(Vector & sol, int ip, int direction, bool local=false) const;
   void SourceTransfer(const Vector & Psi, Array<int> direction, int ip);
   int  GetDirectionId(const Array<int> & ijk);
   void GetDirectionijk(int id, Array<int> & ijk);
   void ConstructDirectionsMap();
   void Getijk(int ip, int & i, int & j, int & k );
public:
   DiagST(SesquilinearForm * bf_, Array2D<double> & Pmllength_, 
       double omega_, Coefficient * ws_, int nrlayers_);
   void SetLoadVector(Vector load) { B = load;}
   virtual void SetOperator(const Operator &op) {A = &op;}
   virtual void Mult(const Vector &r, Vector &z) const;
   virtual ~DiagST();
};


