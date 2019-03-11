// +-----------------------------------------------------------------------+
// |                                                                       |
// | This subroutine write image file of ppm/pgm format.                   |
// |                                                                       |
// | Notice: This subroutine is partially implemented of the ppm/pgm       |
// |         format supporting only uncompressed 8-bit (gray scale image)  |
// |         and 24-bit (true color RGB image) format.                     |
// |                                                                       |
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                                                                       |
// | Usage: void write_ppm_image(char *fname, un_char **img, int width,    |
// |                             int height, int iscolor, int wantcolor)   |
// |                                                                       |
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                                                                       |
// | Author: Yong-Sheng Chen (yschen@iis.sinica.edu.tw)    1998. 8.24      |
// |         Institute of Information Science,                             |
// |         Academia Sinica, Taiwan.                                      |
// |                                                                       |
// +-----------------------------------------------------------------------+
//

#include <stdio.h>
#include <string.h>

#include "imageio.h"

typedef unsigned char un_char;


// iscolor:   img is a color (RGB) image or not.
// wantcolor: want to save color (ppm) or grayscale (pgm) image format.

void write_ppm_image(char *fname, un_char **img, int width, int height, 
		     int iscolor, int wantcolor) {
   FILE *out;
   int r, c;
   un_char *pix, pixv;
  
#ifdef VERBOSE
   printf("Writing the image file %s...\n", fname);
#endif

   out = fopen(fname, "wb");

   if (out == NULL) {
      fprintf(stderr, "write_ppm_image(): cannot open file: %s\n", fname);
      return;
   }

   if (wantcolor) 
      fputs("P6\n", out);
   else 
      fputs("P5\n", out);
   fprintf(out, "%d %d\n255\n", width, height);

   for (r=0; r<height; r++) {
      pix = img[r];
      for (c=0; c<width; c++) {
         if (wantcolor) {
   	    if (iscolor) {
	       fputc(*pix++, out);
	       fputc(*pix++, out);
	       fputc(*pix++, out);
	    }
	    else {
	       pixv = *pix++;
	       fputc(pixv, out);
	       fputc(pixv, out);
	       fputc(pixv, out);
	    }
	 } // endif of wantcolor
	 else {
	    if (iscolor){
               pixv = (un_char) (0.299*(*pix++) 
		   	       + 0.587*(*pix++)
		      	       + 0.114*(*pix++));
	       fputc(pixv, out);
	    }
	    else {
	       fputc(*pix++, out);
	    }
	 } // endelse of wantcolor
      }  // endif of for c
   }  // endif of for r
   fclose(out);
}
