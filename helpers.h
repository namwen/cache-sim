#ifndef _HELPERS_H_
#define _HELPERS_H_

/*
 * integer to binary
 */
char *itob(unsigned int num);

/*
 * binary to integer
 */
int btoi(char *bin);

/*
 * hex to integer
 */
unsigned int htoi(const char str[]); 

/*
 * formatted binary
 */
char *formatBinary(char *bstring, int tag, int index, int offset);

/*
 * check if num is a power of 2
 */
int isPowerOfTwo(int x);


#endif
