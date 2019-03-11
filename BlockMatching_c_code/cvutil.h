#define RGB  1
#define MONO 0
#define SUCCESS 1
#define FAILURE 0
#define MAX2(a,b)  (((a)>(b))?(a):(b))   // maximum value of a,b

#define MIN2(a,b)  (((a)<(b))?(a):(b))   // minimum value of a,b

#define MAX3(a,b,c)  (((a)>(b))?(((a)>(c))?(a):(c)):(((b)>(c))?(b):(c)))
                                         // maximum value of a,b,c

#define MIN3(a,b,c)  (((a)<(b))?(((a)<(c))?(a):(c)):(((b)<(c))?(b):(c)))
                                         // minimum value of a,b,c

enum TIMER_OPERATION {START, PAUSE, STOP};

unsigned char **create_image(int width, int height);
unsigned int  **create_int_image(int width, int height);
double          my_timer(int timern, int oper);
