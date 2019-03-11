// +-----------------------------------------------------------------------+
// |                                                                       |
// | This subroutine estimate the block motion by the combination of the   |
// | three-step search algorithm and the winner-update algorithm with the  |
// | lower bound derived from Minkowski's inequality. The selection of     |
// | the new winner each iteration is implemented by using heap.           |
// |                                                                       |
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                                                                       |
// | Author: Yong-Sheng Chen (yschen@iis.sinica.edu.tw)    1999. 6.15      |
// |         Institute of Information Science,                             |
// |         Academia Sinica, Taiwan.                                      |
// |                                                                       |
// +-----------------------------------------------------------------------+

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cvutil.h"


typedef struct {
   int r, c;                  // pixel position
   int level;                 // level number at multi-level images
   int err;
   } NODE;

typedef unsigned char un_char;
typedef unsigned int  un_int;

extern int videoHeight, videoWidth, blockSize, searchRange;
extern double operationCount;
extern un_char **recons;      // reconstructed image
int  maxLevel;                // multi-level images from level 0 to maxLevel
int *pow2;                    // array of power 2, i.e., 1,2,4,8,16,32,...
un_int ***now, ***ref;        // multi-level images

static NODE heap[9];
static int  hindex[9];


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


struct {
   int r,c;
   } dir[8] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};

void motionEstimation(int or, int oc, int *vr, int *vc){
   int level, r, c, nr, nc, rmin, rmax, cmin, cmax, gap, jump, range;
   int i, j, k, v, tmpv;
   int SAD;
   un_int **nowim, **refim, **reftop, nowtop_uint;
   NODE *node;
   int operation_count=0;
   int tvr, tvc, neighbor;

   rmin = MAX2(0, or-searchRange);      // calculate search region
   rmax = MIN2(videoHeight-blockSize, or+searchRange);
   cmin = MAX2(0, oc-searchRange);
   cmax = MIN2(videoWidth-blockSize, oc+searchRange);

   nowtop_uint = now[maxLevel][or][oc];
   reftop = ref[maxLevel];

   nowim = now[0];
   refim = ref[0];
   SAD=0;
   for (nr=0; nr<blockSize; nr++)   /* sum of absolute error */
   for (nc=0; nc<blockSize; nc++){
      SAD+=abs(nowim[or+nr][oc+nc]-refim[or+nr][oc+nc]);

#ifdef ANALYSIS
      operation_count++;
#endif

      }

   for (i=0; i<9; i++)
      hindex[i]=i;

   node = heap;
   node->r = or;
   node->c = oc;
   node->level = 0;
   node->err = SAD;

   for (jump=(searchRange+1)/2; jump>0; jump=(jump+1)/2){
      node = &heap[hindex[0]];
      tvr=node->r;
      tvc=node->c;
      range=1;
      for (i=0; i<8; i++){
	 r=tvr+jump*dir[i].r;
	 c=tvc+jump*dir[i].c;
	 if (r>=rmin && r<=rmax && c>=cmin && c<=cmax){
	    node = &heap[hindex[range++]];
	    node->r = r;
	    node->c = c;
	    node->err = abs(nowtop_uint-reftop[r][c]);
	    node->level = maxLevel;
#ifdef ANALYSIS
            operation_count++;
#endif
	    }
	 }
      /* downheap */
      for (k=range/2-1; k>=0; k--){
         i = k;
         v = hindex[k];
         tmpv=heap[v].err;
         while (i<range/2){
            j = i+i+1;
            if (j < range-1 && heap[hindex[j]].err > heap[hindex[j+1]].err) j++;
            if (tmpv<=heap[hindex[j]].err) break;
            hindex[i] = hindex[j];
            i = j;
            }
         hindex[i]=v;
         }

      node = &heap[hindex[0]];

      while (node->level > 0){
         level = node->level = node->level-1;
         nowim = now[level];
         refim = ref[level];
         gap = pow2[level];
         SAD=0;
         for (nr=0; nr<blockSize; nr+=gap)   /* sum of absolute error */
         for (nc=0; nc<blockSize; nc+=gap){
            SAD+=abs(nowim[or+nr][oc+nc]-refim[node->r+nr][node->c+nc]);

#ifdef ANALYSIS
            operation_count++;
#endif
            }
         node->err = SAD;

   /* downheap */
         i = 0;
         v = hindex[0];
         while (i<range/2){
            j = i+i+1;
            if (j < range-1 && heap[hindex[j]].err > heap[hindex[j+1]].err) j++;
            if (node->err<=heap[hindex[j]].err) break;
            hindex[i] = hindex[j];
            i = j;
            }
         hindex[i]=v;

         node = &heap[hindex[0]];
         }

      if (jump==1)
	 break;
      }

   node = &heap[hindex[0]];
   *vr = node->r;
   *vc = node->c;

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
