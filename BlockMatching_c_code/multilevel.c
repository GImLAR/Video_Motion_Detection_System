// +-----------------------------------------------------------------------+
// |                                                                       |
// | This subroutine construct multilevel images.                          |
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

#include "cvutil.h"


typedef unsigned char un_char;
typedef unsigned int  un_int;

extern int videoHeight, videoWidth, blockSize, maxLevel;
extern int *pow2;

void ConstructMultilevelImages(un_char **im, un_int ***ims){
   int r, c, i, g, size, height, width;
   un_int **im1, **im2;
   un_int *imp11, *imp12, *imp13, *imp14, *imp2;
   un_char *impc;

   /* bottom level is original image */
   size = videoHeight*videoWidth;
   for (i=0, imp2=ims[0][0], impc=im[0]; i<size; i++, imp2++, impc++)
      *imp2 = (un_int) *impc;

   for (i=0; i<maxLevel; i++){
      im1 = ims[i];
      im2 = ims[i+1];
      g = pow2[i];
      height = videoHeight-pow2[i+1]+1;
      width  = videoWidth -pow2[i+1]+1;
      for (r=0; r<height; r++){
         imp2 = im2[r];
         imp11 = im1[r];
         imp12 = imp11 + g;
         imp13 = imp11 + g*videoWidth;
         imp14 = imp13 + g;
         for (c=0; c<width ; c++){
            *imp2=*imp11+*imp12+*imp13+*imp14;
            imp2++;
            imp11++;
            imp12++;
            imp13++;
            imp14++;
            }
         }
      }
   }
