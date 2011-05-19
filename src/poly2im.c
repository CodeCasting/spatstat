/*
  poly2im.c

  Conversion from (x,y) polygon to pixel image

  poly2imI     pixel value =  1{pixel centre is inside polygon}

  poly2imA     pixel value = area of intersection between pixel and polygon

  $Revision: 1.2 $ $Date: 2011/05/17 12:41:47 $

*/
#undef DEBUG

#include <math.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define OUT(I,J) out[I + (J) * Ny]

void 
poly2imI(xp, yp, np, nx, ny, out) 
     double *xp, *yp; /* polygon vertices, anticlockwise, CLOSED  */
     int *np; 
     int *nx, *ny; /* INTEGER raster points from (0,0) to (nx-1, ny-1) */
     int *out;  /* output matrix [ny, nx], initialised to zero */
{
  int Np, Nx, Ny;
  int i, j, k;
  double x0, y0, x1, y1, xleft, xright, yleft, yright;
  double dx, dy, y, slope, intercept;
  int jleft, jright, imax;
  int sign;

  Np = *np;
  Nx = *nx;
  Ny = *ny;
  /*  Nxy = Nx * Ny; */

  /* run through polygon edges */
  for(k = 0; k < Np-1; k++) {
    x0 = xp[k];
    y0 = yp[k];
    x1 = xp[k+1];
    y1 = yp[k+1];
    if(x0 < x1) {
      xleft = x0;
      xright = x1;
      yleft = y0;
      yright = y1;
      sign = -1;
    } else {
      xleft = x1;
      xright = x0;
      yleft = y1;
      yright = y0;
      sign = +1;
    }
    /* determine relevant columns of pixels */
    jleft = (int) ceil(xleft);
    jright = (int) floor(xright);
    if(jleft < Nx && jright >= 0 && jleft <= jright) {
      if(jleft < 0) { jleft = 0; } 
      if(jright >= Nx) {jright = Nx - 1; }
      /* equation of edge */
      dx = xright - xleft;
      dy = yright - yleft;
      slope = dy/dx;
      intercept = yleft - slope * xleft;
      /* visit relevant columns */
      for(j = jleft; j <= jright; j++) {
	y = slope * ((double) j) + intercept;
	imax = (int) floor(y);
	if(imax >= Ny) imax = Ny-1;
	if(imax >= 0) {
	  /* increment entries below edge in this column */
	  for(i = 0; i <= imax; i++) {
	    OUT(i,j) += sign;
	  }
	}
      }
    }
  }
}

#define BELOW -1
#define INSIDE 0
#define ABOVE 1

void 
poly2imA(ncol, nrow, xpoly, ypoly, npoly, out, status)
     int *ncol, *nrow; /* pixels are unit squares from (0,0) to (ncol,nrow) */
     double *xpoly, *ypoly; /* vectors of coordinates of polygon vertices */
     int *npoly;
     double *out;  /* double array [nrow, ncol] of pixel areas */
     int *status;
{
  double *xp, *yp;
  int nx, ny, np; 
  int i, j, k;
  double xcur, ycur, xnext, ynext, xleft, yleft, xright, yright;
  int sgn, jmin, jmax, imin, imax;
  double x0, y0, x1, y1, slope, yhi, ylo, area, xcut, xcutA, xcutB;
  int klo, khi;

  nx = *ncol;
  ny = *nrow;
  
  xp = xpoly;
  yp = ypoly;
  np = *npoly;

  *status = 0;

  /* initialise output array */
  for(i = 0; i < ny; i++)
    for(j = 0; j < nx; j++)
      out[j + ny * i] = 0;

  /* ............ loop over polygon edges ...................*/
  for(k = 0; k < np - 1; k++) {
    xcur = xp[k];
    ycur = yp[k];
    xnext = xp[k+1];
    ynext = yp[k+1];
#ifdef DEBUG
    fprintf(stderr, 
	    "\nEdge %d from (%lf, %lf) to (%lf, %lf) .........\n",
	    k, xcur, ycur, xnext, ynext);
#endif
    if(xcur != xnext) {
      /* vertical edges are ignored */
      if(xcur < xnext) {
#ifdef DEBUG
	fprintf(stderr, "negative sign\n");
#endif
	sgn = -1;
	xleft = xcur;
	yleft = ycur;
	xright = xnext;
	yright = ynext;
      } else {
#ifdef DEBUG
	fprintf(stderr, "positive sign\n");
#endif
	sgn = 1;
	xleft = xnext;
	yleft = ynext;
	xright = xcur;
	yright = ycur;
      }
      /* we have now ensured xleft < xright */
      slope = (yright - yleft)/(xright - xleft);
      /* Find relevant columns of pixels */
      jmin = floor(xleft);
      jmin = (jmin < 0) ? 0 : jmin;
      jmax = ceil(xright);
      jmax = (jmax > nx - 1) ? nx - 1 : jmax;
      /* Find relevant rows of pixels */
      imin = floor((yleft < yright) ? yleft : yright);
      imin = (imin < 0) ? 0 : imin;
      imax = ceil((yleft < yright) ? yright : yleft);
      imax = (imax > ny - 1) ? ny - 1 : imax;
#ifdef DEBUG
      fprintf(stderr, "imin=%d, imax=%d, jmin=%d, jmax=%d\n", 
	      imin, imax, jmin, jmax);
#endif
      /* ........... loop over columns of pixels ..............*/
      for(j = jmin; j <= jmax; j++) {
#ifdef DEBUG
      fprintf(stderr, "\t j=%d:\n", j);
#endif
	/* 
	   Intersect trapezium with column of pixels
        */
	if(xleft <= j+1 && xright >= j) {
	  if(xleft >= j) {
	    /* retain left corner */
#ifdef DEBUG
	    fprintf(stderr, "\tretain left corner\n");
#endif
	    x0 = xleft;
	    y0 = yleft;
	  } else {
	    /* trim left corner */
#ifdef DEBUG
	    fprintf(stderr, "\ttrim left corner\n");
#endif
	    x0 = (double) j;
	    y0 = yleft + slope * (x0 - xleft);
	  }
	  if(xright <= j+1) {
	    /* retain right corner */
#ifdef DEBUG
	    fprintf(stderr, "\tretain right corner\n");
#endif
	    x1 = xright;
	    y1 = yright;
	  } else {
	    /* trim right corner */
#ifdef DEBUG
	    fprintf(stderr, "\ttrim right corner\n");
#endif
	    x1 = (double) (j+1);
	    y1 = yright + slope * (x1 - xright);
	  }
	  /* save min and max y */
	  if(y0 < y1) {
#ifdef DEBUG
	    fprintf(stderr, "slope %lf > 0\n", slope);
#endif
	    ylo = y0;
	    yhi = y1;
	  } else {
#ifdef DEBUG
	    fprintf(stderr, "slope %lf <= 0\n", slope);
#endif
	    ylo = y1;
	    yhi = y0;
	  }
	  /* ............ loop over pixels within column ......... */
	  /* first part */
	  if(imin > 0) {
	    for(i = 0; i < imin; i++) {
#ifdef DEBUG
	      fprintf(stderr, "\ti=%d:\n", i);
#endif
	      /*
		The trimmed pixel [x0, x1] * [i, i+1] 
		lies below the polygon edge.
	      */
	      area = (x1 - x0);
#ifdef DEBUG
	      fprintf(stderr, "\tIncrementing area by %lf\n", sgn * area);
#endif
	      out[j + ny * i] += sgn * area;
	    }
	  }
	  /* second part */
	  for(i = imin; i <= imax; i++) {
#ifdef DEBUG
	    fprintf(stderr, "\ti=%d:\n", i);
#endif
	    /* 
	       Compute area of intersection between trapezium
	       and trimmed pixel [x0, x1] x [i, i+1] 
	    */
	    klo = (ylo <= i) ? BELOW : (ylo >= (i+1))? ABOVE: INSIDE;
	    khi = (yhi <= i) ? BELOW : (yhi >= (i+1))? ABOVE: INSIDE;
	    if(klo == ABOVE) {
	      /* trapezium covers pixel */
#ifdef DEBUG
	      fprintf(stderr, "\t\ttrapezium covers pixel\n");
#endif
	      area = (x1-x0);
	    } else if(khi == BELOW) {
#ifdef DEBUG
	      fprintf(stderr, "\t\tpixel avoids trapezium\n");
#endif
	      /* pixel avoids trapezium */
	      area = 0.0;
	    } else if(klo == INSIDE && khi == INSIDE) {
	      /* polygon edge is inside pixel */
#ifdef DEBUG
	      fprintf(stderr, "\t\t polygon edge is inside pixel\n");
#endif
	      area = (x1-x0) * ((ylo + yhi)/2.0 - i);
	    } else if(klo == INSIDE && khi == ABOVE) {
	      /* polygon edge crosses upper edge of pixel */
#ifdef DEBUG
	      fprintf(stderr, 
		      "\t\t polygon edge crosses upper edge of pixel\n");
#endif
	      xcut = x0 + ((i+1) - y0)/slope;
	      if(slope > 0) 
		area = (xcut - x0) * ((y0 + (i+1))/2 - i) + (x1 - xcut);
	      else
		area = (x1 - xcut) * ((y1 + (i+1))/2 - i) + (xcut - x0);
	    } else if(klo == BELOW && khi == INSIDE) {
	      /* polygon edge crosses lower edge of pixel */
#ifdef DEBUG
	    fprintf(stderr, "\t\t polygon edge crosses lower edge of pixel\n");
#endif
	      xcut = x0 + (i - y0)/slope;
	      if(slope > 0) 
		area = (x1 - xcut) * ((y1 + i)/2 - i);
	      else
		area = (xcut - x0) * ((y0 + i)/2 - i);
	    } else if(klo == BELOW && khi == ABOVE) {
	      /* polygon edge crosses upper and lower edges of pixel */
#ifdef DEBUG
	    fprintf(stderr, 
	     "\t\t polygon edge crosses upper and lower edges of pixel\n");
#endif
	      xcutA = x0 + (i - y0)/slope;
	      xcutB = x0 + ((i+1) - y0)/slope;
	      if(slope > 0) 
		area = (xcutB - xcutA)/2 + (x1 - xcutB);
	      else
		area = (xcutB - x0) + (xcutA - xcutB)/2;
	    } else {
	      /* control should not pass to here */
	      *status = 1;
	      return;
	    }
	    /* add contribution to area of pixel */
#ifdef DEBUG
	    fprintf(stderr, "\tIncrementing area by %lf\n", sgn * area);
#endif
	    out[j + ny * i] += sgn * area;
	  }
	  /* ............ end of loop over pixels within column ......... */
	}
      }
      /* ........ end of loop over columns of pixels ...............*/
    }
  } 
  /* ......... end of loop over polygon edges ...................*/
}
