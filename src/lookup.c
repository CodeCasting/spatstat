#include <R.h>
#include <math.h>
#include <Rmath.h>
#include "methas.h"
#include "dist2.h"

/*
 Conditional intensity function for a general pairwise
 interaction process with the pairwise interaction function
 given by a ``lookup table'', passed through the par argument. 

*/

/* For debugging code, insert the line: #define DEBUG 1 */

/* Storage of parameters and precomputed/auxiliary data */

struct {
  double beta;
  int nlook;
  int equisp;   
  double delta;
  double rmax;
  double r2max;
  double *h;   /* values of pair interaction */
  double *r;   /* r values if not equally spaced */
  double *r2;   /* r^2 values if not equally spaced */
  double *period;
  int per;
} Lookup;


/* initialiser function */

void lookupinit(state, model, algo)
     State state;
     Model model;
     Algor algo;
{
  int i, nlook;
  double ri;
  /* Interpret model parameters*/
  Lookup.beta   = model.par[0];
  Lookup.nlook  = nlook = model.par[1];
  Lookup.equisp = (model.par[2] > 0); 
  Lookup.delta  = model.par[3];
  Lookup.rmax   = model.par[4];
  Lookup.r2max  = pow(Lookup.rmax, 2);
  /* periodic boundary conditions? */
  Lookup.period = model.period;
  Lookup.per    = (model.period[0] > 0.0);
/*
 If the r-values are equispaced only the h vector is included in
 ``par'' after ``rmax''; the entries of h then consist of
 h[0] = par[5], h[1] = par[6], ..., h[k-1] = par[4+k], ...,
 h[nlook-1] = par[4+nlook].  If the r-values are NOT equispaced then
 the individual r values are needed and these are included as
 r[0] = par[5+nlook], r[1] = par[6+nlook], ..., r[k-1] = par[4+nlook+k],
 ..., r[nlook-1] = par[4+2*nlook].
*/
  Lookup.h = (double *) R_alloc((size_t) nlook, sizeof(double));
  for(i = 0; i < nlook; i++)
    Lookup.h[i] = model.par[5+i];
  if(!(Lookup.equisp)) {
    Lookup.r = (double *) R_alloc((size_t) nlook, sizeof(double));
    Lookup.r2 = (double *) R_alloc((size_t) nlook, sizeof(double));
    for(i = 0; i < nlook; i++) {
      ri = Lookup.r[i] = model.par[5+nlook+i];
      Lookup.r2[i] = ri * ri;
    }
  }
#ifdef DEBUG
  printf("Exiting lookupinit: nlook=%d, equisp=%d\n", nlook, Lookup.equisp);
#endif
}

/* conditional intensity evaluator */

double lookupcif(prop, state)
     Propo prop;
     State state;
{
  int npts, nlook, k, kk, ix, ixp1, j;
  double *x, *y;
  double u, v;
  double r2max, d2, d, delta, cifval, ux, vy;

  r2max = Lookup.r2max;
  delta = Lookup.delta;
  nlook = Lookup.nlook;

  u  = prop.u;
  v  = prop.v;
  ix = prop.ix;
  x  = state.x;
  y  = state.y;

  npts = state.npts;

  cifval = Lookup.beta;
  if(npts == 0) 
    return(cifval);

  ixp1 = ix+1;
  /* If ix = NONE = -1, then ixp1 = 0 is correct */

  if(Lookup.equisp) {
    /* equispaced r values */
    if(Lookup.per) { /* periodic distance */
      if(ix > 0) {
	for(j=0; j < ix; j++) {
	  d = sqrt(dist2(u,v,x[j],y[j],Lookup.period));
	  k = floor(d/delta);
	  if(k < nlook) {
	    if(k < 0) k = 0;
	    cifval *= Lookup.h[k];
	  }
	}
      }
      if(ixp1 < npts) {
	for(j=ixp1; j<npts; j++) {
	  d = sqrt(dist2(u,v,x[j],y[j],Lookup.period));
	  k = floor(d/delta);
	  if(k < nlook) {
	    if(k < 0) k = 0;
	    cifval *= Lookup.h[k];
	  }
	}
      }
    } else { /* Euclidean distance */
      if(ix > 0) {
	for(j=0; j < ix; j++) {
	  d = pythag(u - x[j], v-y[j]);
	  k = floor(d/delta);
	  if(k < nlook) {
	    if(k < 0) k = 0;
	    cifval *= Lookup.h[k];
	  }
	}
      }
      if(ixp1 < npts) {
	for(j=ixp1; j<npts; j++) {
	  d = pythag(u - x[j], v-y[j]);
	  k = floor(d/delta);
	  if(k < nlook) {
	    if(k < 0) k = 0;
	    cifval *= Lookup.h[k];
	  }
	}
      }
    }
  } else {
    /* non-equispaced r values */
    if(Lookup.per) { /* periodic distance */
      if(ix > 0) {
	for(j=0; j < ix; j++) {
	  d2 = dist2(u,v,x[j],y[j],Lookup.period);
	  if(d2 < r2max) {
	    for(kk = 0; kk < nlook && Lookup.r2[kk] <= d2; kk++)
	      ;
	    k = (kk == 0) ? 0 : kk-1;
	    cifval *= Lookup.h[k];
	  }
	}
      }
      if(ixp1 < npts) {
	for(j=ixp1; j<npts; j++) {
	  d2 = dist2(u,v,x[j],y[j],Lookup.period);
	  if(d2 < r2max) {
	    for(kk = 0; kk < nlook && Lookup.r2[kk] <= d2; kk++)
	      ;
	    k = (kk == 0) ? 0 : kk-1;
	    cifval *= Lookup.h[k];
	  }
	}
      }
    } else { /* Euclidean distance */
      if(ix > 0) {
	for(j=0; j < ix; j++) {
	  ux = u - x[j];
	  vy = v - y[j];
	  d2 = ux * ux + vy * vy;
	  if(d2 < r2max) {
	    for(kk = 0; kk < nlook && Lookup.r2[kk] <= d2; kk++)
	      ;
	    k = (kk == 0) ? 0 : kk-1;
	    cifval *= Lookup.h[k];
	  }
	}
      }
      if(ixp1 < npts) {
	for(j=ixp1; j<npts; j++) {
	  ux = u - x[j];
	  vy = v - y[j];
	  d2 = ux * ux + vy * vy;
	  if(d2 < r2max) {
	    for(kk = 0; kk < nlook && Lookup.r2[kk] <= d2; kk++)
	      ;
	    k = (kk == 0) ? 0 : kk-1;
	    cifval *= Lookup.h[k];
	  }
	}
      }
    }
  }

  return cifval;
}

Cifns LookupCifns = { &lookupinit, &lookupcif, (updafunptr) NULL, FALSE};
