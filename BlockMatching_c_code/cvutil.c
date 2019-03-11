// +-----------------------------------------------------------------------+
// |                                                                       |
// | This file contain several utility routines                            |
// |                                                                       |
// | 1. create_image(): create image with given width and height           |
// | 2. create_int_image(): create int. image with given width and height  |
// | 3. my_timer(): start, pause, and stop the timers numbered 0 to 4.     |
// |                                                                       |
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                                                                       |
// |                                                                       |
// | Author: Yong-Sheng Chen (yschen@iis.sinica.edu.tw)    1998. 8.24      |
// |         Institute of Information Science,                             |
// |         Academia Sinica, Taiwan.                                      |
// |                                                                       |
// +-----------------------------------------------------------------------+

#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "cvutil.h"

typedef unsigned char un_char;
typedef unsigned int  un_int;
typedef long          time_t;  // time of day in seconds

struct timeb {
   time_t  time;               // time, seconds since the epoch
   unsigned short millitm;     // 1000 msec of additional accuracy
   short   timezone;           // timezone, minutes west of GMT
   short   dstflag;            // daylight savings when appropriate?
   };

// create and malloc for the image with given width and height

un_char **create_image(int width, int height) {
   int r;
   un_char **img;

   img = (un_char **) malloc(sizeof(un_char *)*height);
   if (img==NULL){
      fprintf(stderr, "create_image(): memory allocation error!\n");
      return(NULL);
   }

   img[0] = (un_char *) malloc(sizeof(un_char)*height*width);
   if (img[0]==NULL){
      fprintf(stderr, "create_image(): memory allocation error!\n");
      free(img);
      return(NULL);
   }

   for (r=1; r<height; r++)
      img[r] = img[0] + r*width;

   return (img);
}


// create and malloc for the integer image with given width and height

un_int **create_int_image(int width, int height) {
   int r;
   un_int **img;

   img = (un_int **) malloc(sizeof(un_int *)*height);
   if (img==NULL){
      fprintf(stderr, "create_int_image(): memory allocation error!\n");
      return(NULL);
   }

   img[0] = (un_int *) malloc(sizeof(un_int)*height*width);
   if (img[0]==NULL){
      fprintf(stderr, "create_int_image(): memory allocation error!\n");
      free(img);
      return(NULL);
   }

   for (r=1; r<height; r++)
      img[r] = img[0] + r*width;

   return (img);
}


double my_timer(int timern, int oper){

// this routine will start, pause, and stop the timers numbered from 0 to 4.

   struct timeb tp;
   static double st[5], et[5];
   static int running[5]={0,0,0,0,0};

   if (timern<0 || timern>=5){
      fprintf(stderr, "my_timer(): invalid timer number.\n");
      return(0.0);
   }

   switch(oper) {
      case START:
	 running[timern] = 1;
	 et[timern] = 0.0;
         ftime(&tp);
	 st[timern] = (double)tp.time+(double)tp.millitm/1000.0;
	 return(0.0);
      case PAUSE:
	 if (running[timern]){
            ftime(&tp);
            et[timern] += (double)tp.time+(double)tp.millitm/1000.0-st[timern];
	    running[timern] = 0;
	    return(et[timern]);
         }
	 else {
	    running[timern] = 1;
	    ftime(&tp);
	    st[timern] = (double)tp.time+(double)tp.millitm/1000.0;
	    return(0.0);
         }
      case STOP:
	 ftime(&tp);
	 et[timern] += (double)tp.time+(double)tp.millitm/1000.0-st[timern];
	 running[timern] = 0;
	 return(et[timern]);
      default:
	 fprintf(stderr, "my_timer(): invalid timer operation.\n");
	 return(0.0);
   }
}
