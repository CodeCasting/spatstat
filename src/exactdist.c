/*
       exactdist.c

       Exact distance transform of a point pattern
       (used to estimate the empty space function F)
       
       $Revision: 1.3 $ $Date: 2004/03/08 21:19:32 $

       Author: Adrian Baddeley

       Sketch of functionality:
            the 'data' are a finite list of points in R^2 
	    (x,y coordinates) and the 'output' is a real valued 
	    image whose entries are distances, with the value for
	    each pixel equalling the distance from that pixel
	    to the nearest point of the data pattern.
       
       Routines:

            exact_dt_S()       interface to Splus
	    exact_dt()         implementation of distance transform
	    dist_to_bdry()     compute distance to edge of image frame
	    shape_raster()     initialise a Raster structure
                          
       The appropriate calling sequence for exact_dt_S() 
       is exemplified in 'exactdt.S'
     
*/

#include <math.h>
#include "raster.h"

#define R_INT int

void 
shape_raster(ras,data,xmin,ymin,xmax,ymax,nrow,ncol,mrow,mcol)
     Raster          *ras;           /* the raster structure to be initialised */
     void		*data;
     unsigned long 	nrow, ncol;	/* absolute dimensions of storage array */
     int 		mrow, mcol;	/* margins clipped off */
	                                /* e.g. valid width is ncol - 2*mcol columns */
     double		xmin, ymin,	/* image dimensions in R^2 after clipping */
		        xmax, ymax;     
{
	ras->data	= data;
	ras->nrow 	= nrow;
	ras->ncol 	= ncol;
	ras->length 	= nrow * ncol;
	ras->rmin	= mrow;
	ras->rmax	= nrow - mrow - 1;
	ras->cmin	= mcol;
	ras->cmax	= ncol - mcol - 1;
	ras->x0		= 
	ras->xmin	= xmin;
	ras->x1 	=
	ras->xmax	= xmax;
	ras->y0		=
	ras->ymin	= ymin;
	ras->y1		=
	ras->ymax	= ymax;
	ras->xstep	= (xmax-xmin)/(ncol - 2 * mcol - 1);
	ras->ystep	= (ymax-ymin)/(nrow - 2 * mrow - 1);
	/* printf("xstep,ystep = %lf,%lf\n", ras->xstep,ras->ystep);  */
}

void
exact_dt(x, y, npt, dist, index)
	double	*x, *y;		/* data points */
	long	npt;
	Raster	*dist;		/* exact distance to nearest point */
	Raster	*index;		/* which point x[i],y[i] is closest */
{
	long	i,j,k,l,m;
	double	d;
	long	ii;
	double	dd;
	double  bdiag;
	
	    /* initialise rasters */
#define UNDEFINED -1
#define Is_Defined(I) (I >= 0)
#define Is_Undefined(I) (I < 0)
	
	Clear(*index,R_INT,UNDEFINED)
		
	d = 2.0 * DistanceSquared(dist->xmin,dist->ymin,dist->xmax,dist->ymax); 
	Clear(*dist,double,d)

	  /* If the list of data points is empty, ... exit now */
	if(npt == 0) 
	  return;

	for(i = 0; i < npt; i++) {
		/* printf("%ld -> (%lf,%lf)\n", i, x[i], y[i]); */
		j = RowIndex(*dist,y[i]);
		k = ColIndex(*dist,x[i]);
		/* if(!Inside(*dist,j,k))
			printf("(%ld,%ld) out of bounds\n",j,k);
		else if (!Inside(*dist,j+1,k+1))
			printf("(%ld+1,%ld+1) out of bounds\n",j,k);
		*/
		for(l = j; l <= j+1; l++) 
		for(m = k; m <= k+1; m++) {
			d = DistanceToSquared(x[i],y[i],*index,l,m);
			if(   Is_Undefined(Entry(*index,l,m,R_INT))
			   || Entry(*dist,l,m,double) > d)
			{
				/* printf("writing (%ld,%ld) -> %ld\t%lf\n", l,m,i,d); */
				Entry(*index,l,m,R_INT) = i;
				Entry(*dist,l,m,double) = d;
				/* printf("checking: %ld, %lf\n",
				       Entry(*index,l,m,R_INT),
				       Entry(*dist,l,m,double));
				 */
			}
		}
	}
/*
	for(j = 0; j <= index->nrow; j++)
		for(k = 0; k <= index->ncol; k++)
			printf("[%ld,%ld] %ld\t%lf\n",
			       j,k,Entry(*index,j,k,R_INT),Entry(*dist,j,k,double));
*/			
	/* how to update the distance values */
	
#define COMPARE(ROW,COL,RR,CC,BOUND) \
	d = Entry(*dist,ROW,COL,double); \
	ii = Entry(*index,RR,CC,R_INT); \
	/* printf(" %lf\t (%ld,%ld) |-> %ld\n", d, RR, CC, ii); */ \
	if(Is_Defined(ii) /* && ii < npt */ \
	   && Entry(*dist,RR,CC,double) < d) { \
	     dd = DistanceSquared(x[ii],y[ii],Xpos(*index,COL),Ypos(*index,ROW)); \
	     if(dd < d) { \
		/* printf("(%ld,%ld) <- %ld\n", ROW, COL, ii); */ \
		Entry(*index,ROW,COL,R_INT) = ii; \
		Entry(*dist,ROW,COL,double) = dd; \
		/* printf("checking: %ld, %lf\n", Entry(*index,ROW,COL,R_INT), Entry(*dist,ROW,COL,double)); */\
	     } \
	}


	/* bound on diagonal step distance */
	bdiag = sqrt(index->xstep * index->xstep + index->ystep * index->ystep);
	
	/* forward pass */

	for(j = index->rmin; j <= index->rmax; j++)
	for(k = index->cmin; k <= index->cmax; k++) {
		/* printf("Neighbourhood of (%ld,%ld):\n", j,k); */
		COMPARE(j,k, j-1,k-1, bdiag)
		COMPARE(j,k, j-1,  k, index->ystep)
		COMPARE(j,k, j-1,k+1, bdiag)
		COMPARE(j,k, j,  k-1, index->xstep)
	}

	/* backward pass */

	for(j = index->rmax; j >= index->rmin; j--)
	for(k = index->cmax; k >= index->cmin; k--) {
		COMPARE(j,k, j+1,k+1, bdiag)
		COMPARE(j,k, j+1,  k, index->ystep)
		COMPARE(j,k, j+1,k-1, bdiag)
		COMPARE(j,k, j,  k+1, index->xstep)
	}

	/* take square roots of the distances^2 */

	for(j = index->rmin; j <= index->rmax; j++)
	for(k = index->cmin; k <= index->cmax; k++) 
	        Entry(*dist,j,k,double) = sqrt(Entry(*dist,j,k,double));
	
}	

#define MIN(A,B) (((A) < (B)) ? (A) : (B))

void
dist_to_bdry(d)		/* compute distance to boundary from each raster point */
	Raster *d;
	                /* of course this is easy for a rectangular grid
			   but we implement it in C
			   for ease of future modification */
{
	long j, k;
	double x, y, xd, yd;
	for(j = d->rmin; j <= d->rmax;j++) {
		y = Ypos(*d,j);
		yd = MIN(y - d->ymin, d->ymax - y);
		for(k = d->cmin; k <= d->cmax;k++) {
			x = Xpos(*d,k);
			xd = MIN(x - d->xmin, d->xmax - x);
			Entry(*d,j,k,double) = MIN(xd,yd);
		}
	}
}

/* S interface */

exact_dt_S(x, y, npt,
	   xmin, ymin, xmax, ymax,
	   nr, nc,
	   distances, indices, boundary)
	double *x, *y;		/* input data points */
	R_INT	*npt;
	double *xmin, *ymin,
		*xmax, *ymax;  	/* guaranteed bounding box */
	R_INT *nr, *nc;		/* desired raster dimensions
				   EXCLUDING margin of 1 on each side */
	     /* output arrays */
	double *distances;	/* distance to nearest point */
	R_INT   *indices;	/* index to nearest point */
	double	*boundary;	/* distance to boundary */
{
	Raster dist, index, bdist;

	shape_raster( &dist, (char *) distances,*xmin,*ymin,*xmax,*ymax,
			   *nr+2,*nc+2,1,1);
	shape_raster( &index, (char *) indices, *xmin,*ymin,*xmax,*ymax,
			   *nr+2,*nc+2,1,1);
	shape_raster( &bdist, (char *) boundary, *xmin,*ymin,*xmax,*ymax,
			   *nr+2,*nc+2,1,1);
	
	exact_dt(x, y, (long) *npt, &dist, &index);
	dist_to_bdry(&bdist);
}	
