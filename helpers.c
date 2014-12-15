#include "helpers.h"
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
/*
 * int to binary
 */
char *itob(unsigned int num) {
    char* bin_string;
    int i;
    /*
     * 32 bit binary string
     */
    bin_string = (char*) malloc(sizeof(char) * 33);
    bin_string[32] = '\0';
    for( i = 0; i<32;i++) {
        bin_string[32 - 1 -i] = (num == ((1 <<i) | num)) ? '1' : '0';
    }
    return bin_string;
}

/*
 * binary to integer
 */
int btoi(char *bin) {
    int  b, k, m, n;
    int  len, sum;

    sum = 0;
    len = strlen(bin) - 1;

    for(k = 0; k <= len; k++)
    {
        n = (bin[k] - '0'); 
        if ((n > 1) || (n < 0))
        {
            return 0;
        }
        for(b = 1, m = len; m > k; m--)
        {
            b *= 2;
        }
        sum = sum + n * b;
    }
    return(sum);
}

/* 
 * hex to integer
 */
unsigned int htoi(const char str[]) {
    /* Local Variables */
    unsigned int result;
    int i;

    i = 0;
    result = 0;
    
    if(str[i] == '0' && str[i+1] == 'x')
    {
        i = i + 2;
    }

    while(str[i] != '\0')
    {
        result = result * 16;
        if(str[i] >= '0' && str[i] <= '9')
        {
            result = result + (str[i] - '0');
        }
        else if(tolower(str[i]) >= 'a' && tolower(str[i]) <= 'f')
        {
            result = result + (tolower(str[i]) - 'a') + 10;
        }
        i++;
    }

    return result;
}

/*
 * formats the binary string to be easily parsed
 */
char *formatBinary(char *bstring, int tag, int index, int offset)
{
    char *formatted;
    int i;
    
    /* Format for Output */
    
    formatted = (char *) malloc(sizeof(char) * 35);
    
    formatted[34] = '\0';
    
    for(i = 0; i < tag; i++)
    {
        formatted[i] = bstring[i];
    }
    
    formatted[tag] = ' ';
    
    for(i = tag + 1; i < index + tag + 1; i++)
    {
        formatted[i] = bstring[i - 1];
    }
    
    formatted[index +  tag + 1] = ' ';
    
    for(i = index + tag + 2; i < offset + index + tag + 2; i++)
    {
        formatted[i] = bstring[i - 2];
    }
    
    return formatted;
}

/*
 * isPowerOfTwo
 * returns 1 if x is a power of 2, 0 if not
 */
int isPowerOfTwo(int x){
    return ((x != 0) && !(x & (x-1)));
}


/*
 * current time in microseonds
 */

unsigned int currentTime(){
    struct timeval tv;
    unsigned int time;
    gettimeofday(&tv, NULL);
    time = (int) tv.tv_usec + tv.tv_sec;
    return time;
}
