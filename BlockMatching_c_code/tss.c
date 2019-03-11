// +-----------------------------------------------------------------------+
// |                                                                       |
// | This program estimate the block motion by using three-step search.    |
// |                                                                       |
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                                                                       |
// | Author: Yong-Sheng Chen (yschen@iis.sinica.edu.tw)    1999. 2. 1      |
// |         Institute of Information Science,                             |
// |         Academia Sinica, Taiwan.                                      |
// |                                                                       |
// +-----------------------------------------------------------------------+

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cvutil.h"


typedef unsigned char un_char;

un_char **now;
un_char **ref;

extern int videoHeight, videoWidth, blockSize, searchRange;
extern double operationCount;
extern un_char **recons;

void initialize(void){
   ref = create_image(videoWidth, videoHeight);
   }


void preprocessing(un_char **im){
   now = im;
   }


void swap_image(void){
   memcpy(ref[0], now[0], videoHeight*videoWidth);
   }


struct {
   int r,c;
   } dir[8] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};


void motionEstimation(int or, int oc, int *vr, int *vc){
   int i, r, c, m, n, rmin, rmax, cmin, cmax, gap, tvr, tvc;
   int SAD, minerror;
   int operation_count=0;

   rmin = MAX2(0, or-searchRange);
   rmax = MIN2(videoHeight-blockSize, or+searchRange);
   cmin = MAX2(0, oc-searchRange);
   cmax = MIN2(videoWidth-blockSize, oc+searchRange);

   *vr=or; *vc=oc;
   minerror = 0;
   for (m=0; m<blockSize; m++)
   for (n=0; n<blockSize; n++){
      minerror += abs(now[or+m][oc+n]-ref[*vr+m][*vc+n]);
#ifdef ANALYSIS
      operation_count++;
#endif
      }

   for (gap=(searchRange+1)/2; gap>0; gap=(gap+1)/2){
      tvr=*vr;
      tvc=*vc;
      for (i=0; i<8; i++){
	 r=tvr+gap*dir[i].r;
	 c=tvc+gap*dir[i].c;
	 if (r>=rmin && r<=rmax && c>=cmin && c<=cmax){
            SAD = 0;
            for (m=0; m<blockSize; m++)
            for (n=0; n<blockSize; n++){
               SAD += abs(now[or+m][oc+n]-ref[r+m][c+n]);
#ifdef ANALYSIS
               operation_count++;
#endif
               }
	    if (SAD<minerror){
	       minerror=SAD;
	       *vr=r;
	       *vc=c;
	       }
	    }
	 }
      if (gap==1)
	 break;
      }

#ifdef ANALYSIS
   operationCount += (double)operation_count;
#endif
   }


double calculateMSE(int or, int oc, int vr, int vc){
   int r, c;
   un_char tmp;
   double diff, error=0.0;

   for(r=0; r<blockSize; r++)
   for(c=0; c<blockSize; c++){
      tmp = ref[vr+r][vc+c];
      recons[or+r][oc+c] = tmp;
      diff = (double)now[or+r][oc+c] - (double)tmp;
      error += diff*diff;
      }
   return (error);
   }
