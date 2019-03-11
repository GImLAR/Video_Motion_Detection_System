// +-----------------------------------------------------------------------+
// |                                                                       |
// | This subroutine estimate the block motion by using Block Sum Pyramid  |
// | method (Lee and Chen, IEEE T. Image Processing, 6(11), pp. 1587-1591. |
// |                                                                       |
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                                                                       |
// | Author: Yong-Sheng Chen (yschen@iis.sinica.edu.tw)    1999. 5.17      |
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
typedef unsigned int  un_int;

extern int videoHeight, videoWidth, blockSize, searchRange;
extern double operationCount;
extern un_char **recons;    // reconstructed image
int  maxLevel;              // multi-level images from level 0 to maxLevel
int *pow2;                  // array of power 2, i.e., 1,2,4,8,16,32,...
un_int ***now, ***ref;      // multi-level images


void initialize(){
   int i, levelNumber, r, c;

   for (maxLevel=0, i=1; i<blockSize; maxLevel++)  /* check blockSize */
      i *= 2;
   if (i != blockSize){
      fprintf(stderr, "This program can only deal with 2^n block size.\n");
      exit(1);
      }
   levelNumber = maxLevel+1;

   pow2 = (int *)malloc(sizeof(int)*levelNumber);
   now  = (un_int ***)malloc(sizeof(un_int **)*levelNumber);
   ref  = (un_int ***)malloc(sizeof(un_int **)*levelNumber);
   if (pow2==NULL || now==NULL || ref==NULL){
      fprintf(stderr, "initialize(): malloc error.\n");
      exit(1);
      }

   for (i=1, pow2[0]=1; i<levelNumber; i++)
      pow2[i] = pow2[i-1]*2;
   for (i=0; i<levelNumber; i++){
      now[i] = create_int_image(videoWidth, videoHeight);
      ref[i] = create_int_image(videoWidth, videoHeight);
      }
   }


void preprocessing(un_char **im){
   ConstructMultilevelImages(im, now);
   }


void swap_image(void){
   un_int ***tmp;

   tmp = ref;
   ref = now;
   now = tmp;
   }


void motionEstimation(int or, int oc, int *vr, int *vc){
   int rmin, rmax, cmin, cmax, r, c, nr, nc, orb, ocb;
   un_int **nowim, **refim;
   int level, gap, SAD, minerr;
   int operation_count=0;

   rmin = MAX2(0, or-searchRange);
   rmax = MIN2(videoHeight-blockSize, or+searchRange);
   cmin = MAX2(0, oc-searchRange);
   cmax = MIN2(videoWidth-blockSize, oc+searchRange);

   nowim=now[0];refim=ref[0];
   *vr=or;
   *vc=oc;
   minerr=0;
   for (nr=0; nr<blockSize; nr++)
   for (nc=0; nc<blockSize; nc++){
      minerr+=abs(nowim[or+nr][oc+nc]-refim[*vr+nr][*vc+nc]);

#ifdef ANALYSIS
      operation_count++;
#endif

      }

   for (r=rmin; r<=rmax; r++)
   for (c=cmin; c<=cmax; c++){
      if (r!=*vr || c!=*vc){
         level=maxLevel;
         do {
            nowim = now[level];
            refim = ref[level];
            gap = pow2[level];
            SAD=0;
            for (nr=0; nr<blockSize; nr+=gap)
            for (nc=0; nc<blockSize; nc+=gap){
               SAD+=abs(nowim[or+nr][oc+nc]-refim[r+nr][c+nc]);

#ifdef ANALYSIS
               operation_count++;
#endif

               }
	    if (SAD>=minerr)
	       break;
	    level--;
	    } while (level>=0);
         if (SAD<minerr){
   	    *vr=r;
   	    *vc=c;
	    minerr=SAD;
	    }
	 }
      }

#ifdef ANALYSIS
   operationCount += (double)operation_count;
#endif
   }


double calculateMSE(int or, int oc, int vr, int vc){
   int r, c;
   un_int tmp;
   double diff, error=0.0;

   for(r=0; r<blockSize; r++)
   for(c=0; c<blockSize; c++){
      tmp = ref[0][vr+r][vc+c];
      recons[or+r][oc+c] = (un_char)tmp;
      diff = (double)now[0][or+r][oc+c] - (double)tmp;
      error += diff*diff;
      }
   return (error);
   }
