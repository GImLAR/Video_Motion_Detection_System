// +-----------------------------------------------------------------------+
// |                                                                       |
// | This subroutine estimate the block motion by using winner-update      |
// | algorithm with the lower bound derived from Minkowski's inequality.   |
// | The selection of the new winner each iteration is implemented by      |
// | using a hash table with size 256.                                     |
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

#define DIM   256

typedef struct {
   int r, c;                  // pixel position
   int level;                 // level number at multi-level images
   int err;
   int next;
   } NODE;

typedef unsigned char un_char;
typedef unsigned int  un_int;

extern int videoHeight, videoWidth, blockSize, searchRange;
extern double operationCount;
extern un_char **recons;      // reconstructed image
int  maxLevel;                // multi-level images from level 0 to maxLevel
int *pow2;                    // array of power 2, i.e., 1,2,4,8,16,32,...
un_int ***now, ***ref;        // multi-level images

int table[DIM];               // MAD value from 0 to DIM
NODE *list;


void initialize(){
   int i, levelNumber, size, r, c;

   for (maxLevel=0, i=1; i<blockSize; maxLevel++)    // check blockSize
      i *= 2;
   if (i != blockSize){
      fprintf(stderr, "This program can only deal with 2^n block size.\n");
      exit(1);
      }
   levelNumber = maxLevel+1;

   pow2 = (int *)malloc(sizeof(int)*levelNumber);
   now  = (un_int ***)malloc(sizeof(un_int **)*levelNumber);
   ref  = (un_int ***)malloc(sizeof(un_int **)*levelNumber);
   size = (2*searchRange+1)*(2*searchRange+1)+1;
   list = (NODE *)malloc(sizeof(NODE)*size);
   if (pow2==NULL || now==NULL || ref==NULL || list==NULL){
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
   int i, level, r, c, nr, nc, rmin, rmax, cmin, cmax, gap, BS, orb, ocb;
   int SAD, MAD, minerr;
   un_int **nowim, **refim, *imp, tmp_uint;
   NODE *node;
   int listp, *tp;
   int operation_count=0, max_count;

   BS = maxLevel*2;

#ifdef PREDICTION
   nowim = now[0];
   refim = ref[0];
   *vr=or;
   *vc=oc;
   minerr = 0;
   for (nr=0; nr<blockSize; nr++)
   for (nc=0; nc<blockSize; nc++){
      minerr += abs(nowim[or+nr][oc+nc]-refim[*vr+nr][*vc+nc]);

   #ifdef ANALYSIS
      operation_count++;
   #endif
      }
#else
   minerr = (int)255*255 << BS;
#endif

   for (i=0, tp=table; i<DIM; i++, tp++)  // clear the table
      *tp = 0;

   rmin = MAX2(0, or-searchRange);      // calculate search region
   rmax = MIN2(videoHeight-blockSize, or+searchRange);
   cmin = MAX2(0, oc-searchRange);
   cmax = MIN2(videoWidth-blockSize, oc+searchRange);

   tmp_uint = now[maxLevel][or][oc];
   refim = ref[maxLevel];
   i=1;
   node = list+1;
   for (r=rmin; r<=rmax; r++){    /* initialize table from top image */
      imp = refim[r]+cmin;
      for (c=cmin; c<=cmax; c++){

#ifdef PREDICTION
	 if (r != *vr || c != *vc){
#endif

	    SAD = abs(tmp_uint-*imp++);

#ifdef ANALYSIS
            operation_count++;
#endif

#ifdef PREDICTION
	    if (SAD < minerr) {
#endif

	       MAD = SAD >> BS;
   	       node->r = r;
	       node->c = c;
	       node->level = maxLevel;
	       node->err = SAD;
               node->next = table[MAD];
               table[MAD] = i++;
	       node++;

#ifdef PREDICTION
	       }
	    }
	 else {
	    imp++;
	    }
#endif

         } // for c
      } // for r

   i=0;
   while (i<DIM){
      tp = table + i;
      if (*tp!=0){
         listp = *tp;
         node = list+listp;
         *tp = node->next;

	 level = node->level;
	 if (level==0){
	    *vr = node->r;
	    *vc = node->c;
	    listp=node->next;
	    while (listp!=0){
	       node = list+listp;
	       SAD = node->err;
	       level = node->level;
	       while (SAD<minerr && level>0){
	          level--;
	          nowim = now[level];
	          refim = ref[level];
	          gap = pow2[level];
	          SAD=0;
	          for (nr=0; nr<blockSize; nr+=gap)
	          for (nc=0; nc<blockSize; nc+=gap){
		   SAD+=abs(nowim[or+nr][oc+nc]-refim[node->r+nr][node->c+nc]);

#ifdef ANALYSIS
                   operation_count++;
#endif
		   }
		  }
	       if (SAD < minerr){
		  minerr = SAD;
		  *vr=node->r;
		  *vc=node->c;
		  }
	       listp=node->next;
	       }
	    break;
	    }
	 else {
	    r = node->r;
	    c = node->c;
	    do {
	       level--;                  /* go on to the lower level */
	       nowim = now[level];
	       refim = ref[level];
	       gap = pow2[level];
	       SAD=0;
	       for (nr=0; nr<blockSize; nr+=gap) /* sum of absolute error */
	       for (nc=0; nc<blockSize; nc+=gap){
		  SAD += abs(nowim[or+nr][oc+nc]-refim[r+nr][c+nc]);

#ifdef ANALYSIS
                  operation_count++;
#endif
		  }
	       MAD = SAD >> BS;
	       } while ( MAD == i && level > 0 && SAD < minerr );
	    if (SAD < minerr){
	       if (level == 0)
	          minerr = SAD;
	       node->level = level;
	       node->err = SAD;
	       node->next = table[MAD];
	       table[MAD]=listp;
	       }
	    } // else (level==0)
         }
      else
	 i++;
      } // while i

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
