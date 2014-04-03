/*  
Author: Sankalp gulati
email: sankalp.gulati@gmai.com
Affiliation: Universitat Pompeu Fabra

License: to be decided !!!   
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#define char16_t uint16_t
#include "mex.h"

void mexFunction ( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] );

void mexFunction ( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{

    double *inp1, *inp2, *out1;
    int N;
    int ii;

    inp1 = mxGetPr(prhs[0]);
    inp2 = mxGetPr(prhs[1]);
    N = mxGetScalar(prhs[2]);

    plhs[0] = mxCreateDoubleMatrix(N,1,mxREAL);

    out1 = mxGetPr(plhs[0]);

    for(ii=0;ii<N;ii++)
    {
        out1[ii] = inp1[ii]  + inp2[ii] ;      
    }


    return;



}

/*
// euclidean distance
double EucDist(double a, double b)
{
    double diff;
    diff = a-b;
        return (diff*diff);
}


double LocalDist(double *a, double*b, int dim, DistMethods fptr, double *wghts)
{
        switch (dim)
        {
        case 1:
            return ((*fptr)(a[0],b[0]))*wghts[0];
        case 2:
            return ((*fptr)(a[0],b[0]))*wghts[0] + ((*fptr)(a[1],b[1]))*wghts[1];
        case 3:
            return ((*fptr)(a[0],b[0]))*wghts[0] + ((*fptr)(a[1],b[1]))*wghts[1] + ((*fptr)(a[2],b[2]))*wghts[2];
        }
        return 0.0;
}

double min3(double a, double b, double c)
{

if (b < a)
    a = b;
if (c < a)
    return c;
return a;
}



// This is the standard dtw implementation for multidimensional time series. In case the dimension is > 1 the point to point distance is computed as weighted point to point 
//distance between each dimension. The weight function is input to the function.
double dtwNd_std(double *x, double*y, MatrixSize*size_x, MatrixSize*size_y, int NFeatDim, double*cost, Config*  myConfig)
{
        // declarations of variables
        int i,j, NRows, NCols;	
        DistMethods myDistMethods[5]={NULL};
        
        //some initialization
        NRows=size_x->Nrow;
        NCols=size_y->Nrow;
        myDistMethods[Euclidean]=&EucDist;
        
        //printf("%f\t%f\n",myConfig->DistWghts[0],myConfig->DistWghts[1]);
        //Initializing the row and columns of cost matrix
        cost[0]=LocalDist(&x[0],&y[0],NFeatDim, myDistMethods[myConfig->DistMthd],myConfig->DistWghts);
        for (i=1;i<NRows;i++)
        {
        cost[i*NCols]=LocalDist(&x[i*size_x->Ncol],&y[0], NFeatDim, myDistMethods[myConfig->DistMthd], myConfig->DistWghts) + cost[(i-1)*NCols];
        }
        for (j=1;j<NCols;j++)
        {
        cost[j]=LocalDist(&x[0],&y[j*size_y->Ncol], NFeatDim, myDistMethods[myConfig->DistMthd], myConfig->DistWghts) + cost[(j-1)];
        }
        //filling in all the cumulative cost matrix
        for (i=1;i<NRows;i++)
        {
        for (j=1;j<NCols;j++)
        {
            cost[(i*NCols)+ j] = LocalDist(&x[i*size_x->Ncol],&y[j*size_y->Ncol], NFeatDim, myDistMethods[myConfig->DistMthd], myConfig->DistWghts) + 
                                    min3(cost[(i-1)*NCols+j], cost[(i-1)*NCols+(j-1)], cost[i*NCols+(j-1)]);
        }
        
        }
        return cost[(NRows*NCols)-1];
}
// Compute the warp path starting at cost[startx, starty]
// If startx = -1 -> startx = n-1; if starty = -1 -> starty = m-1
int path(double *cost, int n, int m, int startx, int starty, DTW_path *p)
{
int i, j, k, z1, z2;
int *px;
int *py;
double min_cost;

if ((startx >= n) || (starty >= m))
    return 0;

if (startx < 0)
    startx = n - 1;

if (starty < 0)
    starty = m - 1;
    
i = startx;
j = starty;
k = 1;

// allocate path for the worst case
px = (int *) malloc ((startx+1) * (starty+1) * sizeof(int));
py = (int *) malloc ((startx+1) * (starty+1) * sizeof(int));

px[0] = i;
py[0] = j;

while ((i > 0) || (j > 0))
    {
    if (i == 0)
        j--;
    else if (j == 0)
        i--;
    else
        {
        min_cost = min3(cost[(i-1)*m+j],
                        cost[(i-1)*m+(j-1)], 
                        cost[i*m+(j-1)]);
        
        if (cost[(i-1)*m+(j-1)] == min_cost)
            {
            i--;
            j--;
            }
        else if (cost[i*m+(j-1)] == min_cost)
            j--;
        else
            i--;
        }
    
    px[k] = i;
    py[k] = j;
    k++;      
    }

p->px = (int *) malloc (k * sizeof(int));
p->py = (int *) malloc (k * sizeof(int));
for (z1=0, z2=k-1; z1<k; z1++, z2--)
    {
    p->px[z1] = px[z2];
    p->py[z1] = py[z2];
    }
p->plen = k;

free(px);
free(py);

return 1;
}


double dist4Path(double *x, double*y, MatrixSize*size_x, MatrixSize*size_y, int NFeatDim, DTW_path* path_t, int path_len, Config* myConfig)
{
        // declarations of variables
        int i,j, NRows, NCols, ind_x, ind_y;	
        DistMethods myDistMethods[5]={NULL};
        double dist=0;
        float *a;
        a = (float*)x;
        
        //some initialization
        NRows=size_x->Nrow;
        NCols=size_y->Nrow;
        myDistMethods[Euclidean]=&EucDist;
        
        
        for (i=0;i<path_len;i++)
        {
        ind_x = path_t->px[i];
        ind_y = path_t->py[i];
        dist+=LocalDist(&x[ind_x*size_x->Ncol],&y[ind_y*size_y->Ncol], NFeatDim, myDistMethods[myConfig->DistMthd], myConfig->DistWghts);
        }
        return dist;
}

//This is a basic (standard) DTW measure without any constraint or fansy stuff. This is for one dimensional time series.
double dtw1d_std(double *x, double*y, int x_len, int y_len, double*cost, int dist_type)
{
        // declarations of variables
        int i,j;	
        DistMethods myDistMethods[5]={NULL};
        
        //setting up types of methods availale for measuring point to point distance
        myDistMethods[Euclidean]=&EucDist;
        
        //Initializing the row and columns of cost matrix
        cost[0]= (*myDistMethods[dist_type])(x[0],y[0]);
        for (i=1;i<x_len;i++)
        {
        cost[i*y_len]=(*myDistMethods[dist_type])(x[i],y[0]) + cost[(i-1)*y_len];
        }
        for (j=1;j<y_len;j++)
        {
        cost[j]=(*myDistMethods[dist_type])(x[0],y[j]) + cost[(j-1)];
        }
        
        //filling in all the cumulative cost matrix
        for (i=1;i<x_len;i++)
        {
        for (j=1;j<y_len;j++)
        {
            cost[(i*y_len)+ j] = (*myDistMethods[dist_type])(x[i],y[j]) + 
                                    min3(cost[(i-1)*y_len+j], cost[((i-1)*y_len)+(j-1)], cost[(i*y_len)+(j-1)]);
        }
        
        }
        
        return cost[(x_len*y_len)-1];
}

//This is a constrained DTW, constraint is a band of certain width along the 45 degree line. (quite and standard constraint talked about by Sakoe, H. and Chiba)
double dtw1d_BandConstraint45(double *x, double*y, int x_len, int y_len, double*cost, int dist_type, int bandwidth)
{
        // declarations of variables
        int i,j;	
        DistMethods myDistMethods[5]={NULL};
        
        
        //CHANGES DUE TO CONSTRAINTS
        //the bandwidth of the constraint can't go beyong the abs(y_len-x_len), so
        bandwidth = max(bandwidth, abs(y_len-x_len)); // adapt constraint width
        //putting infi in all cost mtx
        for (i=1;i<x_len;i++)
        {
        for (j=1;j<y_len;j++)
        {
            cost[(i*y_len)+ j] = FLT_MAX;
        }
        
        }

        //setting up types of methods availale for measuring point to point distance
        myDistMethods[Euclidean]=&EucDist;
        
        //Initializing the row and columns of cost matrix
        cost[0]= (*myDistMethods[dist_type])(x[0],y[0]);
        for (i=1;i<x_len;i++)
        {
        cost[i*y_len]=(*myDistMethods[dist_type])(x[i],y[0]) + cost[(i-1)*y_len];
        }
        for (j=1;j<y_len;j++)
        {
        cost[j]=(*myDistMethods[dist_type])(x[0],y[j]) + cost[(j-1)];
        }
        
        //filling in all the cumulative cost matrix
        for (i=1;i<x_len;i++)
        {
        for (j=max(1, i-bandwidth);j<min(y_len, i+bandwidth);j++)
        {
            cost[(i*y_len)+ j] = (*myDistMethods[dist_type])(x[i],y[j]) + 
                                    min3(cost[(i-1)*y_len+j], cost[((i-1)*y_len)+(j-1)], cost[(i*y_len)+(j-1)]);
        }
        
        }
        
        return cost[(x_len*y_len)-1];
}*/