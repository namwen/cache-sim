#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>
#include "cache.h"
#include "files.h"
#include "helpers.h"

/*
unsigned int htoi(const char str[]);
char *itob(unsigned int num);
char *formatBinary(char *bstring, int tag, int index, int offset);
 */
typedef struct {
    int cachesize;
    int blocksize;
    int setsize;
    char* associativity;
    char* writepolicy;
} cache_params_t;

int DEBUG = 0;

int main(int argc, char *argv[]){
    cache_params_t params;
    /* file handling stuff */
    FILE *tracefile;
    char instruction_ptr[50];
    char instruction;
    char mem_addr[50];
    int count;
    char *traceData;
    Cache_t cache;
    
    int tagSize, setSize, offsetSize;
    if(argc == 2 && strcmp(argv[1], "-h") == 0){
        printf("help menu\n");
        return 1;
    }
    
    if(argc < 2 || argc > 6){
        printf("ERROR: incorrect number of arguments\n");
        return 1;
    }
    /*
     * TODO:
     * Check cachesize = numsets x setsize x blocksize
     */
    /* Check if cachesize here and correct*/
    /* should check that these are numbers as well   */ 
   
    if(isPowerOfTwo(atoi(argv[1]))){
        params.cachesize = atoi(argv[1]);
    }else{
        printf("ERROR: cachesize must be a power of 2\n");
        return 1;
    }
    /* check if blocksize is here and correct */
    if(isPowerOfTwo(atoi(argv[3]))){
         params.blocksize = atoi(argv[3]);
    }else{
        printf("ERROR: blocksize must be a power of 2\n");
        return 1;
    }
    params.setsize = 1;
    /* check for associativit y */
    if(strcmp("direct", argv[2]) == 0){
        params.associativity = "direct";
    }else if( strcmp("assoc", argv[2]) == 0){
        params.associativity = "assoc";
    }else{
        int i = 0, digit = 0;
        char* str = argv[2];
        char* test;
        while(str[i] != '\0'){
            if(isdigit(str[i])){
                digit = i;
                break;
            }
            i++;
        }
        /*
         * setsize = 1 for d-m caches
         */
        params.setsize = 1;
        test = malloc(strlen(argv[2]));
        strncpy(test, argv[2], digit);
        if(strcmp("assoc:", test) == 0){
            params.associativity = "n-way";
            if(isPowerOfTwo( argv[2][digit] - '0')){
                params.setsize = argv[2][digit] - '0';
            }else{
                printf("ERROR: n must be power of 2\n");
                return 1;
            }
        }else{
            printf("ERROR: invalid associativity format\n");
            return 1;
        }
    }

    /* check for writepolicy*/
    if(strcmp("wt", argv[4]) == 0){
        params.writepolicy = "wt";
    }else if(strcmp("wb", argv[4]) == 0){
        params.writepolicy = "wb";
    }else{
        printf("ERROR: invalid writepolicy format\n");
        return 1;
    }
    /* check for tracefile */
    if(!(tracefile = fopen(argv[5], "r"))){
        printf("ERROR: could not find tracefile: %s\n", argv[5]);
        return 1;
    }else{
        traceData = readFile(argv[5]);
    }

    /*
     * create the new cache, do initialization stuff
     */
    cache = *newCache(params.cachesize, params.blocksize, params.setsize, params.associativity, params.writepolicy);
    /*
     * read each line of the tracefile
     *
     * DIRECT MAP ONLY RIGHT NOW
     */ 
    count = 0;
     while(fscanf(tracefile, "%s %c %s", instruction_ptr, &instruction, mem_addr) == 3) {
        int tag_size;
        int lines;
        int offset_size;
        int set_size;
        int set_lines;
        unsigned int addr_int;
        char *addr_bin;
        char *formatted_bin;
       
        char *tag;
        char *setid;
        char *offset;
        formatted_bin = (char*) malloc(sizeof(char) * 35);
        addr_bin = (char*) malloc(sizeof(char) * 33);
        
        lines = (int)(params.cachesize / params.blocksize);
        set_lines = (int)(params.cachesize / (params.blocksize * params.setsize));
       
        if(strcmp(params.associativity,"n-way") == 0){
            set_size = (int) ceil((log10(set_lines)/log10(2.0))); 
        }else{
            set_size = (int) ceil((log10(lines)/log10(2.0))); 
        }

        offset_size = ceil((log10(params.blocksize)/log10(2.0))); 
        tag_size = (32 - (offset_size+set_size));
        
        /*
         * just for output
         */
        tagSize = tag_size;
        setSize = set_size;
        offsetSize = offset_size;
        
        tag = (char*) malloc(sizeof(char)*tag_size+1);
        setid =  (char*) malloc(sizeof(char)*set_size+1);
        offset =  (char*) malloc(sizeof(char)*offset_size+1);
        
        addr_int = htoi(mem_addr);
        addr_bin = itob(addr_int);

        memcpy(tag, &addr_bin[0], tag_size);
        tag[tag_size+1] = '\0';
        memcpy(setid, &addr_bin[tag_size], set_size);
        setid[set_size+1] = '\0';
        memcpy(offset, &addr_bin[(tag_size+set_size)], offset_size);
        offset[offset_size+1] = '\0';
       
        if(DEBUG) {
            printf("------\n%s\n",mem_addr);    
            printf("%s %s %s \n", tag, setid, offset);
            printf("tag: %i, set: %i, offset: %i\n", tagSize, setSize, offsetSize);
        }
        
        /*
         * Write to the cache
         */
        
        if('W' == instruction){
           if(DEBUG){
                 printf("Write instruction\n Cache State:\n");
                 printCache(&cache); 
                 if( isInCache(&cache, addr_bin, tag, setid, offset)){ 
                        printf("\ncache already contains %s\n",mem_addr);
                 }else{
                     printf("\ncache does not contain %s\n",mem_addr);
                 }


                 if( isInCache(&cache, addr_bin, tag, setid, offset)){ 
                     printf("\ncache contains %s\n",mem_addr);
                 }
                 printf("\nCache State:\n");
                 printCache(&cache);
            }
            writeToCache(&cache, addr_bin, tag, setid, offset, lines); 
        /*
         * Read from the cache
         */
        }else if('R' == instruction ){
           if(DEBUG) {
            printf("Read instruction\n Cache State:\n");
            printCache(&cache);
           if( isInCache(&cache, addr_bin, tag, setid, offset)){ 
                printf("\ncache already contains %s\n",mem_addr);
            }else{
                printf("\ncache does not contain %s\n",mem_addr);
            }
            if( isInCache(&cache, addr_bin, tag, setid, offset)){ 
                printf("\ncache contains %s\n",mem_addr);
            }
            
            printf("\nCache State:\n");
            printCache(&cache);
            }
            readFromCache(&cache, addr_bin, tag, setid, offset, lines); 
        } 
        count++;
     }
     printCache(&cache);
     return 1;
}

