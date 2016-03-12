/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/*             CCMATH mathematics library source code.
 *
 *  Copyright (C)  2000   Daniel A. Atkinson    All rights reserved.
 *  This code may be redistributed under the terms of the GNU library
 *  public license (LGPL). ( See the lgpl.license file for details.)
 * ------------------------------------------------------------------------
 */
 
#include <stdlib.h>
#include <math.h>

#include "CIR_ccmath.h"

static int solvru(double *a,double *b,int n);
static double qrlsq(double *a,double *b,int m,int n,int *f);

double
cir_ccmath_qrlsq( double *a, double *b, int m, int n, int *f)
{
	return qrlsq( a, b, m, n, f);
}

double qrlsq(double *a,double *b,int m,int n,int *f)
{ double *p,*q,*w,*v;
  double s,h,r;
  int i,j,k,mm,ms;
  if(m<n) return -1;
  w=(double *)calloc(m,sizeof(double));
  for(i=0,mm=m,p=a; i<n ;++i,--mm,p+=n+1){
    if(mm>1){
      for(j=0,q=p,s=0.; j<mm ;++j,q+=n){
	w[j]= *q; s+= *q* *q;
       }
      if(s>0.){
	h=sqrt(s); if(*p<0.) h= -h;
	s+= *p*h; s=1./s; w[0]+=h;
	for(k=1,ms=n-i; k<ms ;++k){
	  for(j=0,q=p+k,r=0.; j<mm ;q+=n) r+=w[j++]* *q;
	  r=r*s;
	  for(j=0,q=p+k; j<mm ;q+=n) *q-=r*w[j++];
	 }
        *p= -h;
        for(j=0,q=b+i,r=0.; j<mm ;) r+=w[j++]* *q++;
        for(j=0,q=b+i,r*=s; j<mm ;) *q++ -=r*w[j++];
       }
     }
   }
  *f=solvru(a,b,n);
  for(j=n,q=b+j,s=0.; j<m ;++j,++q) s+= *q* *q;
  free(w);
  return s;
} 

int solvru(double *a,double *b,int n)
{ int j,k; double fabs();
  double s,t,*p,*q;
  for(j=0,s=0.,p=a; j<n ;++j,p+=n+1) if((t=fabs(*p))>s) s=t;
  s*=1.e-16;
  for(j=n-1,p=a+n*n-1; j>=0 ;--j,p-=n+1){
    for(k=j+1,q=p+1; k<n ;) b[j]-=b[k++]* *q++;
    if(fabs(*p)<s) return -1;
    b[j]/= *p;
   }
  return 0;
}
