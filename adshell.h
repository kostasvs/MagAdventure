/*
 * Adventure parser shell header file.
 * By Martin Houston for CUG(UK)
 * 25/5/86
 */
 
 /* default sizes for various things */
 #define WORDLEN 25 /* no words longer than 25 chars */
 #define LINELEN 255 /* < 255 characters in one input from
 user */
 
 /* declaration for function look up table structure */
 struct look_up_func
 {
     char token[WORDLEN];
     int (*action)(); /* a POINTER to a function */
 };
 
 