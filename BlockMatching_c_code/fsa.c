// +-----------------------------------------------------------------------+
// |                                                                       |
// | This program estimate the block motion by using full search algorithm.|
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


void motionEstimation(int or, int oc, int *vr, int *vc){
   int r, c, m, n, rmin, rmax, cmin, cmax;
   int SAD, minerror;
   int operation_count=0;

   minerror = 255*blockSize*blockSize;
   *vr=0; *vc=0;
   rmin = MAX2(0, or-searchRange);
   rmax = MIN2(videoHeight-blockSize, or+searchRange);
   cmin = MAX2(0, oc-searchRange);
   cmax = MIN2(videoWidth-blockSize, oc+searchRange);
   for (r=rmin; r<=rmax; r++)
   for (c=cmin; c<=cmax; c++){
      SAD = 0;
      for (m=0; m<blockSize; m++)
      for (n=0; n<blockSize; n++){
         SAD += abs(now[or+m][oc+n]-ref[r+m][c+n]);
#ifdef ANALYSIS
         operation_count++;
#endif
         }
      if(SAD < minerror){
         minerror = SAD;
         *vr = r;
         *vc = c;
         }
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
