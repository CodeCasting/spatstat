/*

  distances.c

  Distances between points

  $Revision: 1.22 $     $Date: 2008/04/02 12:26:21 $

  pairdist      Pairwise distances
  pair2dist     Pairwise distances squared
  pairPdist     Pairwise distances with periodic correction
  pairP2dist    Pairwise distances squared, with periodic correction

  crossdist     Pairwise distances for two sets of points
  cross2dist    Pairwise distances squared, for two sets of points
  crossPdist    Pairwise distances for two sets of points, periodic correction

  matchxy       Find matches between two sets of points   

  THE FOLLOWING FUNCTIONS ASSUME THAT y IS SORTED IN ASCENDING ORDER 

  nndistsort    Nearest neighbour distances 
  nnwhichsort   Nearest neighbours and their distances
  nnXwhich      Nearest neighbour from one list to another
  nnXexclude    Nearest neighbour from one list to another, with overlaps

 */

#include <math.h>
/* #include <stdio.h> */

double sqrt();

void pairdist(n, x, y, d)
     /* inputs */
     int *n;
     double *x, *y;
     /* output */
     double *d;
{ 
  int i, j, npoints; 
  double *dp;
  double xi, yi, dx, dy, dist;

  npoints = *n;

  /* set d[0,0] = 0 */
  *d = 0.0;

  for (i=1; i < npoints; i++) 
    {
      xi = x[i];
      yi = y[i];
      /* point at the start of column i */
      dp = d + i * npoints;
      /* set diagonal to zero */
      dp[i] = 0.0;
      for (j=0; j < i; j++)
	{
	  dx = x[j] - xi;
	  dy = y[j] - yi;
	  dist = sqrt( dx * dx + dy * dy ); 
	  /* upper triangle */
	  *dp = dist;
	  ++dp;
	  /* lower triangle */
	  d[ j * npoints + i] = dist;
	}
    }
}

/* squared distances */

void pair2dist(n, x, y, d)
     /* inputs */
     int *n;
     double *x, *y;
     /* output */
     double *d;
{ 
  int i, j, npoints; 
  double *dp;
  double xi, yi, dx, dy, dist;

  npoints = *n;

  /* set d[0,0] = 0 */
  *d = 0.0;

  for (i=1; i < npoints; i++) 
    {
      xi = x[i];
      yi = y[i];
      /* point at the start of column i */
      dp = d + i * npoints;
      /* set diagonal to zero */
      dp[i] = 0.0;
      for (j=0; j < i; j++)
	{
	  dx = x[j] - xi;
	  dy = y[j] - yi;
	  dist = dx * dx + dy * dy; 
	  /* upper triangle */
	  *dp = dist;
	  ++dp;
	  /* lower triangle */
	  d[ j * npoints + i] = dist;
	}
    }
}

void crossdist(nfrom, xfrom, yfrom, nto, xto, yto, d)
     /* inputs */
     int *nto, *nfrom;
     double *xfrom, *yfrom, *xto, *yto;
     /* output */
     double *d;
{ 
  int i, j, nf, nt; 
  double *dptr;
  double xj, yj, dx, dy;

  nf = *nfrom;
  nt = *nto;

  dptr = d;

  for (j=0; j < nt; j++) {
    xj = xto[j];
    yj = yto[j];
    for(i = 0; i < nf; i++, dptr++) {
	dx = xj - xfrom[i];
	dy = yj - yfrom[i];
	*dptr = sqrt( dx * dx + dy * dy ); 
    }
  }
}

/* squared distances */

void cross2dist(nfrom, xfrom, yfrom, nto, xto, yto, d)
     /* inputs */
     int *nto, *nfrom;
     double *xfrom, *yfrom, *xto, *yto;
     /* output */
     double *d;
{ 
  int i, j, nf, nt; 
  double *dptr;
  double xj, yj, dx, dy;

  nf = *nfrom;
  nt = *nto;

  dptr = d;

  for (j=0; j < nt; j++) {
    xj = xto[j];
    yj = yto[j];
    for(i = 0; i < nf; i++, dptr++) {
	dx = xj - xfrom[i];
	dy = yj - yfrom[i];
	*dptr = dx * dx + dy * dy; 
    }
  }
}



/* distances with periodic correction */

void pairPdist(n, x, y, xwidth, yheight, d)
     /* inputs */
     int *n;
     double *x, *y, *xwidth, *yheight;
     /* output */
     double *d;
{ 
  int i, j, npoints; 
  double *dp;
  double xi, yi, dx, dy, dx2, dy2, dx2p, dy2p, dist, wide, high;

  npoints = *n;
  wide = *xwidth;
  high = *yheight;

  /* set d[0,0] = 0 */
  *d = 0.0;

  for (i=1; i < npoints; i++) 
    {
      xi = x[i];
      yi = y[i];
      /* point at the start of column i */
      dp = d + i * npoints;
      /* set diagonal to zero */
      dp[i] = 0.0;
      for (j=0; j < i; j++)
	{
	  dx = x[j] - xi;
	  dy = y[j] - yi;
	  dx2p = dx * dx;
	  dy2p = dy * dy;
	  dx2 = (dx - wide) * (dx - wide);
	  dy2 = (dy - wide) * (dy - wide);
	  if(dx2 < dx2p) dx2p = dx2;
	  if(dy2 < dy2p) dy2p = dy2;
	  dx2 = (dx + wide) * (dx + wide);
	  dy2 = (dy + wide) * (dy + wide);
	  if(dx2 < dx2p) dx2p = dx2;
	  if(dy2 < dy2p) dy2p = dy2;
	  dist = sqrt( dx2p + dy2p ); 
	  /* upper triangle */
	  *dp = dist;
	  ++dp;
	  /* lower triangle */
	  d[ j * npoints + i] = dist;
	}
    }
}

/* same function without the sqrt */

void pairP2dist(n, x, y, xwidth, yheight, d)
     /* inputs */
     int *n;
     double *x, *y, *xwidth, *yheight;
     /* output */
     double *d;
{ 
  int i, j, npoints; 
  double *dp;
  double xi, yi, dx, dy, dx2, dy2, dx2p, dy2p, dist, wide, high;

  npoints = *n;
  wide = *xwidth;
  high = *yheight;

  /* set d[0,0] = 0 */
  *d = 0.0;

  for (i=1; i < npoints; i++) 
    {
      xi = x[i];
      yi = y[i];
      /* point at the start of column i */
      dp = d + i * npoints;
      /* set diagonal to zero */
      dp[i] = 0.0;
      for (j=0; j < i; j++)
	{
	  dx = x[j] - xi;
	  dy = y[j] - yi;
	  dx2p = dx * dx;
	  dy2p = dy * dy;
	  dx2 = (dx - wide) * (dx - wide);
	  dy2 = (dy - wide) * (dy - wide);
	  if(dx2 < dx2p) dx2p = dx2;
	  if(dy2 < dy2p) dy2p = dy2;
	  dx2 = (dx + wide) * (dx + wide);
	  dy2 = (dy + wide) * (dy + wide);
	  if(dx2 < dx2p) dx2p = dx2;
	  if(dy2 < dy2p) dy2p = dy2;
	  dist = dx2p + dy2p; 
	  /* upper triangle */
	  *dp = dist;
	  ++dp;
	  /* lower triangle */
	  d[ j * npoints + i] = dist;
	}
    }
}

void crossPdist(nfrom, xfrom, yfrom, nto, xto, yto, xwidth, yheight, d)
     /* inputs */
     int *nto, *nfrom;
     double *xfrom, *yfrom, *xto, *yto, *xwidth, *yheight;
     /* output */
     double *d;
{ 
  int i, j, nf, nt; 
  double *dptr;
  double xj, yj, dx, dy, dx2, dy2, dx2p, dy2p, wide, high;

  nf = *nfrom;
  nt = *nto;
  wide = *xwidth;
  high = *yheight;

  dptr = d;

  for (j=0; j < nt; j++) {
    xj = xto[j];
    yj = yto[j];
    for(i = 0; i < nf; i++, dptr++) {
	dx = xj - xfrom[i];
	dy = yj - yfrom[i];
	  dx2p = dx * dx;
	  dy2p = dy * dy;
	  dx2 = (dx - wide) * (dx - wide);
	  dy2 = (dy - wide) * (dy - wide);
	  if(dx2 < dx2p) dx2p = dx2;
	  if(dy2 < dy2p) dy2p = dy2;
	  dx2 = (dx + wide) * (dx + wide);
	  dy2 = (dy + wide) * (dy + wide);
	  if(dx2 < dx2p) dx2p = dx2;
	  if(dy2 < dy2p) dy2p = dy2;
	  *dptr = sqrt( dx2p + dy2p ); 
    }
  }
}

/*

  matchxy

  Find matches between two lists of points

 */

void matchxy(na, xa, ya, nb, xb, yb, match)
     /* inputs */
     int *na, *nb;
     double *xa, *ya, *xb, *yb;
     /* output */
     int *match; 
{ 
  int i, j, Na, Nb; 
  double xai, yai;

  Na = *na;
  Nb = *nb;

  for (i=1; i < Na; i++) 
    {
      xai = xa[i];
      yai = ya[i];
      match[i] = 0;
      for (j=0; j < Nb; j++) 
	if(xai == xb[j] && yai == yb[j]) {
	  match[i] = j;
	  break;
	}
    }
}

/* THE FOLLOWING CODE ASSUMES THAT y IS SORTED IN ASCENDING ORDER */

void nndistsort(n, x, y, nnd, huge)
     /* inputs */
     int *n;
     double *x, *y, *huge;
     /* output */
     double *nnd;
{ 
  int npoints, i, left, right;
  double dmin, d2, d2min, xi, yi, dx, dy, hu, hu2;

#ifdef SPATSTAT_DEBUG
  FILE *out; 
#endif

  hu = *huge;
  hu2 = hu * hu;

#ifdef SPATSTAT_DEBUG
  out = fopen("out", "w");
#endif

  npoints = *n;

  for(i = 0; i < npoints; i++) {

#ifdef SPATSTAT_DEBUG
    fprintf(out, "\ni=%d\n", i); 
#endif

    dmin = hu;
    d2min = hu2;
    xi = x[i];
    yi = y[i];
    /* search backward */
    for(left = i - 1;
        left >= 0 && (dy = (yi - y[left])) < dmin ;
	--left)
      {

#ifdef SPATSTAT_DEBUG
	fprintf(out, "L");
#endif

	dx = x[left] - xi;
	d2 =  dx * dx + dy * dy;
	if (d2 < d2min) {
	  d2min = d2;
	  dmin = sqrt(d2);
	}
      }

    /* search forward */
    for(right = i + 1;
	right < npoints && (dy = (y[right] - yi)) < dmin ;
	++right)
      {

#ifdef SPATSTAT_DEBUG
	fprintf(out, "R");
#endif
	dx = x[right] - xi;
	d2 =  dx * dx + dy * dy;
	if (d2 < d2min) {
	  d2min = d2;
	  dmin = sqrt(d2);
	}
      }
#ifdef SPATSTAT_DEBUG
    fprintf(out, "\n");
#endif

    nnd[i] = dmin;
  }

#ifdef SPATSTAT_DEBUG
  fclose(out);
#endif

}

/* nnwhichsort: same as nndistsort, 
   but also returns id of nearest neighbour 
*/

void nnwhichsort(n, x, y, nnd, nnwhich, huge)
     /* inputs */
     int *n;
     double *x, *y, *huge;
     /* outputs */
     double *nnd;
     int *nnwhich;
{ 
  int npoints, i, left, right, which;
  double dmin, d2, d2min, xi, yi, dx, dy, hu, hu2;

  hu = *huge;
  hu2 = hu * hu;

  npoints = *n;

  for(i = 0; i < npoints; i++) {
    dmin = hu;
    d2min = hu2;
    which = -1;
    xi = x[i];
    yi = y[i];
    /* search backward */
    if(i > 0){
      for(left = i - 1;
	  left >= 0 && (dy = (yi - y[left])) < dmin ;
	  --left)
	{
	  dx = x[left] - xi;
	  d2 =  dx * dx + dy * dy;
	  if (d2 < d2min) {
	    d2min = d2;
	    dmin = sqrt(d2);
	    which = left;
	  }
	}
    }

    /* search forward */
    if(i < npoints - 1) {
      for(right = i + 1;
	  right < npoints && (dy = (y[right] - yi)) < dmin ;
	  ++right)
	{
	  dx = x[right] - xi;
	  d2 =  dx * dx + dy * dy;
	  if (d2 < d2min) {
	    d2min = d2;
	    dmin = sqrt(d2);
	    which = right;
	  }
	}
    }
    nnd[i] = dmin;
    nnwhich[i] = which;
  }
}


/* 
   nnXwhich:  for TWO point patterns X and Y,
              find the nearest neighbour 
	      (from each point of X to the nearest point of Y)
	      returning both the distance and the identifier

   Requires both patterns to be sorted in order of increasing y coord
*/

void nnXwhich(n1, x1, y1, n2, x2, y2, nnd, nnwhich, huge)
     /* inputs */
     int *n1, *n2;
     double *x1, *y1, *x2, *y2, *huge;
     /* outputs */
     double *nnd;
     int *nnwhich;
{ 
  int npoints1, npoints2, i, jleft, jright, jwhich, lastjwhich;
  double dmin, d2, d2min, x1i, y1i, dx, dy, hu, hu2;

  hu = *huge;
  hu2 = hu * hu;

  npoints1 = *n1;
  npoints2 = *n2;

  if(npoints1 == 0 || npoints2 == 0)
    return;

  lastjwhich = 0;

  for(i = 0; i < npoints1; i++) {
    dmin = hu;
    d2min = hu2;
    jwhich = -1;
    x1i = x1[i];
    y1i = y1[i];

    /* search backward from previous nearest neighbour */
    if(lastjwhich > 0) {
      for(jleft = lastjwhich - 1;
	  jleft >= 0 && (dy = (y1i - y2[jleft])) < dmin ;
	  --jleft)
	{
	  dx = x2[jleft] - x1i;
	  d2 =  dx * dx + dy * dy;
	  if (d2 < d2min) {
	    d2min = d2;
	    dmin = sqrt(d2);
	    jwhich = jleft;
	  }
	}
    }

    /* search forward from previous nearest neighbour  */
    if(lastjwhich < npoints2) {
      for(jright = lastjwhich;
	  jright < npoints2 && (dy = (y2[jright] - y1i)) < dmin ;
	  ++jright)
	{
	  dx = x2[jright] - x1i;
	  d2 =  dx * dx + dy * dy;
	  if (d2 < d2min) {
	    d2min = d2;
	    dmin = sqrt(d2);
	    jwhich = jright;
	  }
	}
    }
    nnd[i] = dmin;
    nnwhich[i] = jwhich;
    lastjwhich = jwhich;
  }
}

/* 
   nnXexclude:  similar to nnXwhich
              but allows X and Y to include common points
	      (which are not to be counted as neighbours)

   Code numbers id1, id2 are attached to the patterns X and Y respectively, 
   such that
   x1[i], y1[i] and x2[j], y2[j] are the same point iff id1[i] = id2[j].

   Requires both patterns to be sorted in order of increasing y coord
*/

void nnXexclude(n1, x1, y1, id1, n2, x2, y2, id2, nnd, nnwhich, huge)
     /* inputs */
     int *n1, *n2, *id1, *id2;
     double *x1, *y1, *x2, *y2, *huge;
     /* outputs */
     double *nnd;
     int *nnwhich;
{ 
  int npoints1, npoints2, i, jleft, jright, jwhich, lastjwhich, id1i;
  double dmin, d2, d2min, x1i, y1i, dx, dy, hu, hu2;

  hu = *huge;
  hu2 = hu * hu;

  npoints1 = *n1;
  npoints2 = *n2;

  if(npoints1 == 0 || npoints2 == 0)
    return;

  lastjwhich = 0;

  for(i = 0; i < npoints1; i++) {
    dmin = hu;
    d2min = hu2;
    jwhich = -1;
    x1i = x1[i];
    y1i = y1[i];
    id1i = id1[i];

    /* search backward from previous nearest neighbour */
    if(lastjwhich > 0) {
      for(jleft = lastjwhich - 1;
	  jleft >= 0 && (dy = (y1i - y2[jleft])) < dmin ;
	  --jleft)
	{
	  /* do not compare identical points */
	  if(id2[jleft] != id1i) {
	    dx = x2[jleft] - x1i;
	    d2 =  dx * dx + dy * dy;
	    if (d2 < d2min) {
	      d2min = d2;
	      dmin = sqrt(d2);
	      jwhich = jleft;
	    }
	  }
	}
    }

    /* search forward from previous nearest neighbour  */
    if(lastjwhich < npoints2) {
      for(jright = lastjwhich;
	  jright < npoints2 && (dy = (y2[jright] - y1i)) < dmin ;
	  ++jright)
	{
	  if(id2[jright] != id1i) {
	    dx = x2[jright] - x1i;
	    d2 =  dx * dx + dy * dy;
	    if (d2 < d2min) {
	      d2min = d2;
	      dmin = sqrt(d2);
	      jwhich = jright;
	    }
	  }
	}
    }
    nnd[i] = dmin;
    nnwhich[i] = jwhich;
    lastjwhich = jwhich;
  }
}

