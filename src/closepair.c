/*

  closepair.c

  $Revision: 1.27 $     $Date: 2012/03/26 07:41:50 $

  Assumes point pattern is sorted in increasing order of x coordinate

  paircount()    count the number of pairs (i, j) with distance < rmax

  crosscount()   count number of close pairs in two patterns

  duplicatedxy() find duplicated (x,y) pairs

  closepairs()  extract close pairs of coordinates 
                 .C interface - output vectors have Fixed length 

  crosspairs()  extract close pairs in two patterns 
                 .C interface - output vectors have Fixed length 

  Vclosepairs()  extract close pairs of coordinates 
                 .Call interface - output vectors have Variable length 

  Vcrosspairs()  extract close pairs in two patterns 
                 .Call interface - output vectors have Variable length 

*/

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Utils.h>

#define OK 0
#define ERR_OVERFLOW 1
#define ERR_ALLOC 2

#define FAILED(X) ((void *)(X) == (void *)NULL)

#define intRealloc(PTR, OLDLENGTH, NEWLENGTH) \
  (int *) S_realloc((char *) PTR, NEWLENGTH, OLDLENGTH, sizeof(int))

#define dblRealloc(PTR, OLDLENGTH, NEWLENGTH) \
  (double *) S_realloc((char *) PTR, NEWLENGTH, OLDLENGTH, sizeof(double))

double sqrt();

void paircount(nxy, x, y, rmaxi, count) 
     /* inputs */
     int *nxy;         /* number of (x,y) points */
     double *x, *y;    /* (x,y) coordinates */
     double *rmaxi;    /* maximum distance */
     /* output */
     int *count;
{
  int n, maxchunk, i, j, counted;
  double xi, yi, rmax, r2max, dx, dy, a;

  n = *nxy;
  rmax = *rmaxi;
  r2max = rmax * rmax;

  *count = counted = 0;
  if(n == 0) 
    return;

  /* loop in chunks of 2^16 */

  i = 0; maxchunk = 0; 

  while(i < n) {

    R_CheckUserInterrupt();

    maxchunk += 65536; 
    if(maxchunk > n) maxchunk = n;

    for(; i < maxchunk; i++) {

      xi = x[i];
      yi = y[i];

      if(i > 0) { 
	/* scan backwards from i */
	for(j = i - 1; j >= 0; j--) {
	  dx = x[j] - xi;
	  a = r2max - dx * dx;
	  if(a < 0) 
	    break;
	  dy = y[j] - yi;
	  a -= dy * dy;
	  if(a >= 0)
	    ++counted;
	}
      }
      if(i + 1 < n) {
	/* scan forwards from i */
	for(j = i + 1; j < n; j++) {
	  dx = x[j] - xi;
	  a = r2max - dx * dx;
	  if(a < 0) 
	    break;
	  dy = y[j] - yi;
	  a -= dy * dy;
	  if(a >= 0)
	    ++counted;
	}
      } 
      /* end loop over i */
    }
  } 

  *count = counted;
}


/*
  analogue for two different point patterns
*/


void crosscount(nn1, x1, y1, nn2, x2, y2, rmaxi, count) 
     /* inputs */
     int *nn1, *nn2;
     double *x1, *y1, *x2, *y2, *rmaxi;
     /* output */
     int *count;
{
  int n1, n2, maxchunk, i, j, jleft, counted;
  double x1i, y1i, rmax, r2max, xleft, dx, dy, a;

  n1 = *nn1;
  n2 = *nn2;
  rmax = *rmaxi;
  r2max = rmax * rmax;

  *count = counted = 0;

  if(n1 == 0 || n2 == 0) 
    return;

  jleft = 0;

  i = 0; maxchunk = 0; 

  while(i < n1) {

    R_CheckUserInterrupt();

    maxchunk += 65536; 
    if(maxchunk > n1) maxchunk = n1;

    for(; i < maxchunk; i++) {
  
      x1i = x1[i];
      y1i = y1[i];

      /* 
	 adjust starting index
      */
      xleft = x1i - rmax;
      while((x2[jleft] < xleft) && (jleft+1 < n2))
	++jleft;

      /* 
	 process from j=jleft until dx > rmax
      */
      for(j=jleft; j < n2; j++) {
	dx = x2[j] - x1i;
	a  = r2max - dx * dx;
	if(a < 0)
	  break;
	dy = y2[j] - y1i;
	a -= dy * dy;
	if(a > 0) 
	  ++counted;
      }
    }
  }
  *count = counted;
}



/*
  Find duplicated locations

   xx, yy are not sorted
*/


void duplicatedxy(n, x, y, out) 
     /* inputs */
     int *n;
     double *x, *y;
     /* output */
     int *out;  /* logical vector */
{
  int m, i, j;
  double xi, yi;
  m = *n;
  for(i = 1; i < m; i++) {
    R_CheckUserInterrupt();
    xi = x[i];
    yi = y[i];
    for(j = 0; j < i; j++) 
      if((x[j] == xi) && (y[j] == yi)) 
	break;
    if(j == i) out[i] = 0; else out[i] = 1;
  }
}

/* ............... fixed output length .............. */

void closepairs(nxy, x, y, r, noutmax, 
	      nout, iout, jout, 
	      xiout, yiout, xjout, yjout, dxout, dyout, dout,
	      status)
     /* inputs */
     int *nxy, *noutmax;
     double *x, *y, *r;
     /* outputs */
     int *nout, *iout, *jout;
     double *xiout, *yiout, *xjout, *yjout, *dxout, *dyout, *dout;
     int *status;
{
  int n, k, kmax, maxchunk, i, j;
  double xi, yi, rmax, r2max, dx, dy, dx2, d2;

  n = *nxy;
  rmax = *r;
  r2max = rmax * rmax;

  *status = OK;
  *nout = 0;
  k = 0;   /* k is the next available storage location 
              and also the current length of the list */ 
  kmax = *noutmax;

  if(n == 0) 
    return;

  /* loop in chunks of 2^16 */

  i = 0; maxchunk = 0; 
  while(i < n) {

    R_CheckUserInterrupt();

    maxchunk += 65536; 
    if(maxchunk > n) maxchunk = n;

    for(; i < maxchunk; i++) {

      xi = x[i];
      yi = y[i];

      if(i > 0) {
	/* scan backwards */
	for(j = i - 1; j >= 0; j++) {
	  dx = x[j] - xi;
	  dx2 = dx * dx;
	  if(dx2 > r2max)
	    break;
	  dy = y[j] - yi;
	  d2 = dx2 + dy * dy;
	  if(d2 <= r2max) {
	    /* add this (i, j) pair to output */
	    if(k >= kmax) {
	      *nout = k;
	      *status = ERR_OVERFLOW;
	      return;
	    }
	    jout[k] = j + 1;  /* R indexing */
	    iout[k] = i + 1;
	    xiout[k] = xi;
	    yiout[k] = yi;
	    xjout[k] = x[j];
	    yjout[k] = y[j];
	    dxout[k] = dx;
	    dyout[k] = dy;
	    dout[k] = sqrt(d2);
	    ++k;
	  }
	}
      }
    
      if(i + 1 < n) {
	/* scan forwards */
	for(j = i + 1; j < n; j++) {
	  dx = x[j] - xi;
	  dx2 = dx * dx;
	  if(dx2 > r2max)
	    break;
	  dy = y[j] - yi;
	  d2 = dx2 + dy * dy;
	  if(d2 <= r2max) {
	    /* add this (i, j) pair to output */
	    if(k >= kmax) {
	      *nout = k;
	      *status = ERR_OVERFLOW;
	      return;
	    }
	    jout[k] = j + 1;  /* R indexing */
	    iout[k] = i + 1; 
	    xiout[k] = xi;
	    yiout[k] = yi;
	    xjout[k] = x[j];
	    yjout[k] = y[j];
	    dxout[k] = dx;
	    dyout[k] = dy;
	    dout[k] = sqrt(d2);
	    ++k;
	  }
	}
      }
    }
  }
  *nout = k;
}

void crosspairs(nn1, x1, y1, nn2, x2, y2, rmaxi, noutmax, 
	      nout, iout, jout, 
	      xiout, yiout, xjout, yjout, dxout, dyout, dout,
	      status)
     /* inputs */
     int *nn1, *nn2, *noutmax;
     double *x1, *y1, *x2, *y2, *rmaxi;
     /* outputs */
     int *nout, *iout, *jout;
     double *xiout, *yiout, *xjout, *yjout, *dxout, *dyout, *dout;
     int *status;
{
  int n1, n2, maxchunk, k, kmax, i, j, jleft;
  double x1i, y1i, rmax, r2max, xleft, dx, dy, dx2, d2;

  n1 = *nn1;
  n2 = *nn2;
  rmax = *rmaxi;
  r2max = rmax * rmax;

  *status = OK;
  *nout = 0;
  k = 0;   /* k is the next available storage location 
              and also the current length of the list */ 
  kmax = *noutmax;

  if(n1 == 0 || n2 == 0) 
    return;

  jleft = 0;

  i = 0; maxchunk = 0; 

  while(i < n1) {

    R_CheckUserInterrupt();

    maxchunk += 65536; 
    if(maxchunk > n1) maxchunk = n1;

    for(; i < maxchunk; i++) {

      x1i = x1[i];
      y1i = y1[i];

      /* 
	 adjust starting position jleft

      */
      xleft = x1i - rmax;
      while((x2[jleft] < xleft) && (jleft+1 < n2))
	++jleft;


      /* 
	 process from j=jleft until dx > rmax
      */
      for(j=jleft; j < n2; j++) {
	dx = x2[j] - x1i;
	dx2 = dx * dx;
	if(dx2 > r2max)
	  break;
	dy = y2[j] - y1i;
	d2 = dx2 + dy * dy;
	if(d2 <= r2max) {
	  /* add this (i, j) pair to output */
	  if(k >= kmax) {
	    *nout = k;
	    *status = ERR_OVERFLOW;
	    return;
	  }
	  jout[k] = j + 1;  /* R indexing */
	  iout[k] = i + 1;
	  xiout[k] = x1i;
	  yiout[k] = y1i;
	  xjout[k] = x2[j];
	  yjout[k] = y2[j];
	  dxout[k] = dx;
	  dyout[k] = dy;
	  dout[k] = sqrt(d2);
	  ++k;
	}
      }
    }
  }
  *nout = k;
}

SEXP Vclosepairs(SEXP xx,
		 SEXP yy,
		 SEXP rr,
		 SEXP nguess) 
{
  double *x, *y;
  double xi, yi, rmax, r2max, dx, dy, dx2, d2;
  int n, k, kmax, kmaxold, maxchunk, i, j, m;
  /* local storage */
  int *iout, *jout;
  double *xiout, *yiout, *xjout, *yjout, *dxout, *dyout, *dout;
  /* R objects in return value */
  SEXP Out, iOut, jOut, xiOut, yiOut, xjOut, yjOut, dxOut, dyOut, dOut;
  /* external storage pointers */
  int *iOutP, *jOutP;
  double *xiOutP, *yiOutP, *xjOutP, *yjOutP, *dxOutP, *dyOutP, *dOutP;
  
  /* protect R objects from garbage collector */
  PROTECT(xx     = AS_NUMERIC(xx));
  PROTECT(yy     = AS_NUMERIC(yy));
  PROTECT(rr     = AS_NUMERIC(rr));
  PROTECT(nguess = AS_INTEGER(nguess));
  /* That's 4 objects */

  /* Translate arguments from R to C */

  x = NUMERIC_POINTER(xx);
  y = NUMERIC_POINTER(yy);
  n = LENGTH(xx);
  rmax = *(NUMERIC_POINTER(rr));
  kmax = *(INTEGER_POINTER(nguess));
  
  r2max = rmax * rmax;

  k = 0;   /* k is the next available storage location 
              and also the current length of the list */ 

  if(n > 0 && kmax > 0) {
    /* allocate space */
    iout = (int *) R_alloc(kmax, sizeof(int));
    jout = (int *) R_alloc(kmax, sizeof(int));
    xiout =  (double *) R_alloc(kmax, sizeof(double));
    yiout =  (double *) R_alloc(kmax, sizeof(double));
    xjout =  (double *) R_alloc(kmax, sizeof(double));
    yjout =  (double *) R_alloc(kmax, sizeof(double));
    dxout =  (double *) R_alloc(kmax, sizeof(double));
    dyout =  (double *) R_alloc(kmax, sizeof(double));
    dout  =  (double *) R_alloc(kmax, sizeof(double));
    
    /* loop in chunks of 2^16 */

    i = 0; maxchunk = 0; 
    while(i < n) {

      R_CheckUserInterrupt();

      maxchunk += 65536; 
      if(maxchunk > n) maxchunk = n;

      for(; i < maxchunk; i++) {

	xi = x[i];
	yi = y[i];

	if(i > 0) {
	  /* scan backward */
	  for(j = i - 1; j >= 0; j--) {
	    dx = x[j] - xi;
	    dx2 = dx * dx;
	    if(dx2 > r2max)
	      break;
	    dy = y[j] - yi;
	    d2 = dx2 + dy * dy;
	    if(d2 <= r2max) {
	      /* add this (i, j) pair to output */
	      if(k >= kmax) {
		/* overflow; allocate more space */
		kmaxold = kmax;
		kmax    = 2 * kmax;
		iout  = intRealloc(iout,  kmaxold, kmax);
		jout  = intRealloc(jout,  kmaxold, kmax);
		xiout = dblRealloc(xiout, kmaxold, kmax); 
		yiout = dblRealloc(yiout, kmaxold, kmax); 
		xjout = dblRealloc(xjout, kmaxold, kmax); 
		yjout = dblRealloc(yjout, kmaxold, kmax); 
		dxout = dblRealloc(dxout, kmaxold, kmax); 
		dyout = dblRealloc(dyout, kmaxold, kmax); 
		dout  = dblRealloc(dout,  kmaxold, kmax); 
	      }
	      jout[k] = j + 1; /* R indexing */
	      iout[k] = i + 1;
	      xiout[k] = xi;
	      yiout[k] = yi;
	      xjout[k] = x[j];
	      yjout[k] = y[j];
	      dxout[k] = dx;
	      dyout[k] = dy;
	      dout[k] = sqrt(d2);
	      ++k;
	    }
	  }
	}
      
	if(i + 1 < n) {
	  /* scan forward */
	  for(j = i + 1; j < n; j++) {
	    dx = x[j] - xi;
	    dx2 = dx * dx;
	    if(dx2 > r2max)
	      break;
	    dy = y[j] - yi;
	    d2 = dx2 + dy * dy;
	    if(d2 <= r2max) {
	      /* add this (i, j) pair to output */
	      if(k >= kmax) {
		/* overflow; allocate more space */
		kmaxold = kmax;
		kmax    = 2 * kmax;
		iout  = intRealloc(iout,  kmaxold, kmax);
		jout  = intRealloc(jout,  kmaxold, kmax);
		xiout = dblRealloc(xiout, kmaxold, kmax); 
		yiout = dblRealloc(yiout, kmaxold, kmax); 
		xjout = dblRealloc(xjout, kmaxold, kmax); 
		yjout = dblRealloc(yjout, kmaxold, kmax); 
		dxout = dblRealloc(dxout, kmaxold, kmax); 
		dyout = dblRealloc(dyout, kmaxold, kmax); 
		dout  = dblRealloc(dout,  kmaxold, kmax); 
	      }
	      jout[k] = j + 1; /* R indexing */
	      iout[k] = i + 1;
	      xiout[k] = xi;
	      yiout[k] = yi;
	      xjout[k] = x[j];
	      yjout[k] = y[j];
	      dxout[k] = dx;
	      dyout[k] = dy;
	      dout[k] = sqrt(d2);
	      ++k;
	    }
	  }
	}
	/* end of i loop */
      }
    }
  }

  /* return a list of vectors */
  PROTECT(iOut  = NEW_INTEGER(k));
  PROTECT(jOut  = NEW_INTEGER(k));
  PROTECT(xiOut = NEW_NUMERIC(k));
  PROTECT(yiOut = NEW_NUMERIC(k));
  PROTECT(xjOut = NEW_NUMERIC(k));
  PROTECT(yjOut = NEW_NUMERIC(k));
  PROTECT(dxOut = NEW_NUMERIC(k));
  PROTECT(dyOut = NEW_NUMERIC(k));
  PROTECT(dOut  = NEW_NUMERIC(k));
  if(k > 0) {
    iOutP  = INTEGER_POINTER(iOut);
    jOutP  = INTEGER_POINTER(jOut);
    xiOutP = NUMERIC_POINTER(xiOut);
    yiOutP = NUMERIC_POINTER(yiOut);
    xjOutP = NUMERIC_POINTER(xjOut);
    yjOutP = NUMERIC_POINTER(yjOut);
    dxOutP = NUMERIC_POINTER(dxOut);
    dyOutP = NUMERIC_POINTER(dyOut);
    dOutP  = NUMERIC_POINTER(dOut);
    for(m = 0; m < k; m++) {
      iOutP[m] = iout[m];
      jOutP[m] = jout[m];
      xiOutP[m] = xiout[m];
      yiOutP[m] = yiout[m];
      xjOutP[m] = xjout[m];
      yjOutP[m] = yjout[m];
      dxOutP[m] = dxout[m];
      dyOutP[m] = dyout[m];
      dOutP[m]  = dout[m];
    }
  }
  PROTECT(Out   = NEW_LIST(9));
  SET_VECTOR_ELT(Out, 0,  iOut);
  SET_VECTOR_ELT(Out, 1,  jOut);
  SET_VECTOR_ELT(Out, 2, xiOut);
  SET_VECTOR_ELT(Out, 3, yiOut);
  SET_VECTOR_ELT(Out, 4, xjOut);
  SET_VECTOR_ELT(Out, 5, yjOut);
  SET_VECTOR_ELT(Out, 6, dxOut);
  SET_VECTOR_ELT(Out, 7, dyOut);
  SET_VECTOR_ELT(Out, 8, dOut);
  UNPROTECT(14); /* 4 inputs and 10 outputs (Out and its 9 components) */
  return(Out);
}



SEXP Vcrosspairs(SEXP xx1,
		 SEXP yy1,
		 SEXP xx2,
		 SEXP yy2,
		 SEXP rr,
		 SEXP nguess) 
{
  /* input vectors */
  double *x1, *y1, *x2, *y2;
  /* lengths */
  int n1, n2, nout, noutmax, noutmaxold, maxchunk;
  /* distance parameter */
  double rmax, r2max;
  /* indices */
  int i, j, jleft, m;
  /* temporary values */
  double x1i, y1i, xleft, dx, dy, dx2, d2;
  /* local storage */
  int *iout, *jout;
  double *xiout, *yiout, *xjout, *yjout, *dxout, *dyout, *dout;
  /* R objects in return value */
  SEXP Out, iOut, jOut, xiOut, yiOut, xjOut, yjOut, dxOut, dyOut, dOut;
  /* external storage pointers */
  int *iOutP, *jOutP;
  double *xiOutP, *yiOutP, *xjOutP, *yjOutP, *dxOutP, *dyOutP, *dOutP;
  
  /* protect R objects from garbage collector */
  PROTECT(xx1     = AS_NUMERIC(xx1));
  PROTECT(yy1     = AS_NUMERIC(yy1));
  PROTECT(xx2     = AS_NUMERIC(xx2));
  PROTECT(yy2     = AS_NUMERIC(yy2));
  PROTECT(rr     = AS_NUMERIC(rr));
  PROTECT(nguess = AS_INTEGER(nguess));
  /* That's 6 objects */

  /* Translate arguments from R to C */

  x1 = NUMERIC_POINTER(xx1);
  y1 = NUMERIC_POINTER(yy1);
  x2 = NUMERIC_POINTER(xx2);
  y2 = NUMERIC_POINTER(yy2);
  n1 = LENGTH(xx1);
  n2 = LENGTH(xx2);
  rmax = *(NUMERIC_POINTER(rr));
  noutmax = *(INTEGER_POINTER(nguess));
  
  r2max = rmax * rmax;

  nout = 0;   /* nout is the next available storage location 
		 and also the current length of the list */ 

  if(n1 > 0 && n2 > 0 && noutmax > 0) {
    /* allocate space */
    iout = (int *) R_alloc(noutmax, sizeof(int));
    jout = (int *) R_alloc(noutmax, sizeof(int));
    xiout =  (double *) R_alloc(noutmax, sizeof(double));
    yiout =  (double *) R_alloc(noutmax, sizeof(double));
    xjout =  (double *) R_alloc(noutmax, sizeof(double));
    yjout =  (double *) R_alloc(noutmax, sizeof(double));
    dxout =  (double *) R_alloc(noutmax, sizeof(double));
    dyout =  (double *) R_alloc(noutmax, sizeof(double));
    dout  =  (double *) R_alloc(noutmax, sizeof(double));
    
    jleft = 0;

    i = 0; maxchunk = 0;

    while(i < n1) {

      R_CheckUserInterrupt();

      maxchunk += 65536;
      if(maxchunk > n1) maxchunk = n1;

      for( ; i < maxchunk; i++) {
	x1i = x1[i];
	y1i = y1[i];

	/* 
	   adjust starting point jleft
	*/
	xleft = x1i - rmax;
	while((x2[jleft] < xleft) && (jleft+1 < n2))
	  ++jleft;

	/* 
	   process from j = jleft until dx > rmax
	*/
	for(j=jleft; j < n2; j++) {
	  /* squared interpoint distance */
	  dx = x2[j] - x1i;
	  dx2 = dx * dx;
	  if(dx2 > r2max)
	    break;
	  dy = y2[j] - y1i;
	  d2 = dx2 + dy * dy;
	  if(d2 <= r2max) {
	    /* add this (i, j) pair to output */
	    if(nout >= noutmax) {
	      /* overflow; allocate more space */
	      noutmaxold = noutmax;
	      noutmax    = 2 * noutmax;
	      iout  = intRealloc(iout,  noutmaxold, noutmax);
	      jout  = intRealloc(jout,  noutmaxold, noutmax);
	      xiout = dblRealloc(xiout, noutmaxold, noutmax); 
	      yiout = dblRealloc(yiout, noutmaxold, noutmax); 
	      xjout = dblRealloc(xjout, noutmaxold, noutmax); 
	      yjout = dblRealloc(yjout, noutmaxold, noutmax); 
	      dxout = dblRealloc(dxout, noutmaxold, noutmax); 
	      dyout = dblRealloc(dyout, noutmaxold, noutmax); 
	      dout  = dblRealloc(dout,  noutmaxold, noutmax); 
	    }
	    iout[nout] = i + 1; /* R indexing */
	    jout[nout] = j + 1;
	    xiout[nout] = x1i;
	    yiout[nout] = y1i;
	    xjout[nout] = x2[j];
	    yjout[nout] = y2[j];
	    dxout[nout] = dx;
	    dyout[nout] = dy;
	    dout[nout] = sqrt(d2);
	    ++nout;
	  }
	}
      }
    }
  }

  /* return a list of vectors */
  PROTECT(iOut  = NEW_INTEGER(nout));
  PROTECT(jOut  = NEW_INTEGER(nout));
  PROTECT(xiOut = NEW_NUMERIC(nout));
  PROTECT(yiOut = NEW_NUMERIC(nout));
  PROTECT(xjOut = NEW_NUMERIC(nout));
  PROTECT(yjOut = NEW_NUMERIC(nout));
  PROTECT(dxOut = NEW_NUMERIC(nout));
  PROTECT(dyOut = NEW_NUMERIC(nout));
  PROTECT(dOut  = NEW_NUMERIC(nout));
  if(nout > 0) {
    iOutP  = INTEGER_POINTER(iOut);
    jOutP  = INTEGER_POINTER(jOut);
    xiOutP = NUMERIC_POINTER(xiOut);
    yiOutP = NUMERIC_POINTER(yiOut);
    xjOutP = NUMERIC_POINTER(xjOut);
    yjOutP = NUMERIC_POINTER(yjOut);
    dxOutP = NUMERIC_POINTER(dxOut);
    dyOutP = NUMERIC_POINTER(dyOut);
    dOutP  = NUMERIC_POINTER(dOut);
    for(m = 0; m < nout; m++) {
      iOutP[m] = iout[m];
      jOutP[m] = jout[m];
      xiOutP[m] = xiout[m];
      yiOutP[m] = yiout[m];
      xjOutP[m] = xjout[m];
      yjOutP[m] = yjout[m];
      dxOutP[m] = dxout[m];
      dyOutP[m] = dyout[m];
      dOutP[m]  = dout[m];
    }
  }
  PROTECT(Out   = NEW_LIST(9));
  SET_VECTOR_ELT(Out, 0,  iOut);
  SET_VECTOR_ELT(Out, 1,  jOut);
  SET_VECTOR_ELT(Out, 2, xiOut);
  SET_VECTOR_ELT(Out, 3, yiOut);
  SET_VECTOR_ELT(Out, 4, xjOut);
  SET_VECTOR_ELT(Out, 5, yjOut);
  SET_VECTOR_ELT(Out, 6, dxOut);
  SET_VECTOR_ELT(Out, 7, dyOut);
  SET_VECTOR_ELT(Out, 8, dOut);
  UNPROTECT(16); /* 6 inputs and 10 outputs (Out and its 9 components) */
  return(Out);
}
