#include "../Assign1/LaplaceOp1d.h"        // 3-point Laplace operator assuming homogeneous Dirichlet boundary values
#include "../Assign1/GaussJacobiOp1d.h"    // Weighted Gauss-Jacobi
#include "SparseInterpOp1d.h"   // Restriction/Prolongation operator

#include "../DoubleVectorNd/UCLAQ_DoubleVector1d.h"

/*
 * The  applyVcycle member function of this class implements one V-cycle
 * to obtain an approximation solution to alpha*Delta u = f where alpha < 0
 * and Delta is the 3-point discrete Laplace operator assuming homogeneous
 * Dirichlet boundary values.
 *
 * The number of panels in each direction at level k is 2^(k+1), so the 1d arrays
 * associated with the interior grid points at level k are
 * dimensioned (2^(k+1)-1).
 *
 *
 * maxMultiGridLevel   : maximal multigrid level.
 * relaxationCoujt     : number of jacobi sweeps at each level
 * GJomega             : weighted Gauss-Jacobi relaxation factor
 * alpha               : coefficient of the Laplacian
 *
 * xMin,xMax           : specifies the computational domain
 *
 *
 * May 3, 2016
 * C. Anderson
 */

#ifndef _MultiGridVcycle1d_
#define _MultiGridVcycle1d_

class MultiGridVcycle1d
{
public:

     MultiGridVcycle1d()
     {initialize();}


	 MultiGridVcycle1d(int maxMultiGridLevel,int relaxationCount, double GJomega, double alpha, double xMin, double xMax,
			 int minMultiGridLevel = 0)
	 {
     initialize(maxMultiGridLevel, relaxationCount, GJomega,alpha,xMin,xMax,minMultiGridLevel);
	 }

	 void initialize()
	 {
	 this->maxMultiGridLevel = 0;
	 this->minMultiGridLevel = 0;
	 this->xMin              = 0.0;
	 this->xMax              = 0.0;
	 this->alpha             = 0.0;
	 this->GJomega           = 0.0;
	 this->relaxationCount   = 0;
	 this->verboseFlag       = false;
	 }

	 // level k has 2^(k+1) panels

	 void initialize(int maxMultiGridLevel,int relaxationCount, double GJomega,double alpha, double xMin, double xMax,
			 int minMultiGridLevel = 0)
	 {
	 this->maxMultiGridLevel = maxMultiGridLevel;
	 this->xMin              = xMin;
	 this->xMax              = xMax;
	 this->alpha             = alpha;
	 this->GJomega           = GJomega;
	 this->relaxationCount   = relaxationCount;
	 this->verboseFlag       = false;


	 // Set minimal multigrid level, checking to make sure it's within the proper limits

	 setMinMultiGridLevel(minMultiGridLevel);

	 //
	 // Initialize operators for each level. In order that the array index of
	 // an operator be consistent with the multigrid level index, allocate arrays of size
	 // maxMultiGridLevel+1 but only initialize the operators
	 // for those levels where the operator is used.
	 //

	 LopArray.resize(maxMultiGridLevel+1);
	 GJopArray.resize(maxMultiGridLevel+1);
	 InterpOpArray.resize(maxMultiGridLevel+1);

	 long xPanels;  
	 double    hx;  

	 // Only need to initialize for levels 1 to maxMultiGridLevel

	 for(int k = 1; k <= maxMultiGridLevel; k++)
	 {
	 xPanels = pow(2,k+1);
	 hx       = (xMax-xMin)/xPanels;

	 LopArray[k].initialize(alpha,  xPanels, hx);
	 GJopArray[k].initialize(alpha, xPanels, hx,GJomega);
	 InterpOpArray[k].initialize(k);
	 }

	 // Initialize vectors for each level

	 uArray.resize(maxMultiGridLevel+1);
	 fArray.resize(maxMultiGridLevel+1);

	 uCorrection.resize(maxMultiGridLevel+1);
	 residual.resize(maxMultiGridLevel+1);

	 for(int k = 0; k <= maxMultiGridLevel; k++)
	 {
		 xPanels = pow(2,k+1);
		 uArray[k].initialize(xPanels-1);
		 fArray[k].initialize(xPanels-1);

		 uCorrection[k].initialize(xPanels-1);
		 residual[k].initialize(xPanels-1);

		 uArray[k].setToValue(0.0);
		 fArray[k].setToValue(0.0);
		 uCorrection[k].setToValue(0.0);
		 residual[k].setToValue(0.0);
	 }

	 }

	 void setVerboseFlag(bool verboseFlag = true)
	 {
		 this->verboseFlag = verboseFlag;
	 }

	 void clearVerboseFlag()
	 {
		 this->verboseFlag = false;
	 }


	 void setRelaxationParameter(double omega)
	 {
     for(int k = 1; k <= maxMultiGridLevel; k++)
	 {
    	 GJopArray[k].setRelaxationParameter(omega);
	 }
	 }


	 void setMinMultiGridLevel(int level)
	 {
		 minMultiGridLevel = level;
		 if(minMultiGridLevel > maxMultiGridLevel) minMultiGridLevel = maxMultiGridLevel;
		 if(minMultiGridLevel < 0)                 minMultiGridLevel = 0;
	 }
/*
 * Input :
 *
 *  f : Right hand side associated with finest discretization level
 */
	 void setRightHandSide(const UCLAQ::DoubleVector1d& f)
	 {
		fArray[maxMultiGridLevel] = f;
	 }
/*
 *
 * This member function implements one V-cycle starting
 * with the f associated with the finest level of
 * discretization (e.g. level = maxMultiGridLevel).
 *
 * The right hand side, f, must be specified using the
 * member function setRightHandSide(...) before calling
 * the applyVcycle member function.
 *
 * Input :
 *
 * u : Initial iterate.
 *
 * Output :
 *
 * u : Approximate solution after one V-cycle
 *
 *
 * The number of panels in each direction at level k is 2^(k+1) so the  1d arrays
 * f and u containing the the interior grid points at level K are
 * dimensioned (2^(K+1)-1)  where K = maxMultiGridLevel.
 */

	 void applyVcycle(UCLAQ::DoubleVector1d& u)
	 {
		 // If u is uninitialized, initialize it with input f, and then
		 // set initial iterate to be zero

		 if(u.index1Size == 0){u.initialize(fArray[maxMultiGridLevel]); u.setToValue(0.0);}

		 uArray[maxMultiGridLevel] = u;

		 multiGridRelax(maxMultiGridLevel);

		 // capture the solution

		 u = uArray[maxMultiGridLevel];
	 }

	 //
	 // Two level relaxation operator for the specified multigrid level. This routine
	 // recursively calls itself when the correction equation for level-1 is required.
	 //

	 void multiGridRelax(int level)
	 {

		 double hx; double hy; double coeff;

		 if(level == 0)      // Solve exactly on coarsest level (2 panels in each direction
		 {
	         hx                 =  (xMax-xMin)/2.0;
	         coeff              =  -alpha*(2.0/(hx*hx));
			 uArray[level](0)   =  fArray[level](0)/coeff;
			 return;
		 }

		 // Relax at the current level starting from u == 0 if level < maxMultiGridLevel

		 if(verboseFlag)  cout << "Relax level: " << level << endl;

		 // Use 0 as an initial iterate for all correction levels

		 if(level < maxMultiGridLevel)
		 {
			 uArray[level].setToValue(0.0);
		 }

		 GJopArray[level].setRightHandSide(fArray[level]);
		 for(long i = 0; i < relaxationCount; i++)
		 {
			GJopArray[level].apply(uArray[level]);
		 }

		 // Return after relaxation sweep if we've reached the minimal level

		 if(level == minMultiGridLevel) return;

         if(verboseFlag)  cout << "Residual level: " << level << endl;

		 // Compute the residual to set up the correction equation
		 // residual[level] = fArray[level] - LopArray[level](uArray[level]);
		 //

		 residual[level] = uArray[level];
		 LopArray[level].apply(residual[level]);  // Compute A*u[level]
		 residual[level] *=-1.0;
		 residual[level] += fArray[level];

		 // Create right hand side for coarse grid correction by applying
		 // the restriction operator

		 if(verboseFlag)  cout << "Restriction From : " << level << " to : " << level-1 << endl;

		 InterpOpArray[level].applyRestriction(residual[level],fArray[level-1]);

		 //  Recursively call this apply operator to create coarse grid
		 //  correction

	     multiGridRelax(level -1);

		 if(verboseFlag)  cout << "Correction From : " << level-1 << " to : " << level << endl;

		 InterpOpArray[level].applyProlongation(uArray[level-1],uCorrection[level]);

		 uArray[level] += uCorrection[level];

		 if(verboseFlag) cout << "Relax level: " << level << endl;

		 for(long i = 0; i < relaxationCount; i++)
		 {
			GJopArray[level].apply(uArray[level]);
		 }
	 }


	 bool                       verboseFlag;
	 int                  maxMultiGridLevel;
	 int                  minMultiGridLevel;

	 double                         GJomega;
	 int                    relaxationCount;

	 double xMin; double xMax;
	 double alpha;


	 vector<LaplaceOp1d>           LopArray;
	 vector<GaussJacobiOp1d>      GJopArray;
	 vector<SparseInterpOp1d> InterpOpArray;

	 vector<UCLAQ::DoubleVector1d>   uArray;
	 vector<UCLAQ::DoubleVector1d>   fArray;

	 vector<UCLAQ::DoubleVector1d>      residual;
	 vector<UCLAQ::DoubleVector1d>   uCorrection;
};
#endif
