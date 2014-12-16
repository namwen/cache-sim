#include "cache.h"
#include "helpers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int DEBUG_CACHE = 0;
/*
 * newCache
 */
Cache_t* newCache(int cachesize, int blocksize, int setsize, char* associativity, char* writepolicy) {
    
    Cache_t* cache;
    int lines, setnum;
    cache = malloc(sizeof(Cache_t)); 

    cache->cache_size = cachesize;
    cache->block_size = blocksize;
    cache->set_size = setsize; 
    cache->associativity = associativity;
    cache->write_policy = writepolicy;
    cache->hits = 0;
    cache->misses = 0;
    cache->mem_reads = 0;
    cache->mem_writes = 0;

    /*
     * Set up sets and lines
     *
     * 
     */

    if(strcmp("direct", associativity) == 0){
        int i;
        
        lines = (int) (cachesize / blocksize);
        /*
         * set number is equivalent to line number
         */
        cache->sets = (Set_t *)malloc(lines * sizeof(Set_t)); 
        
        /*
         * allocate memory for a line for every set
         */
        for(i = 0;i < lines; i++){
            cache->sets[i] = *newSet(1);
            cache->sets[i].lines[0] = *newCacheLine();
        }
    
    }
    
    if(strcmp("n-way", associativity) == 0) {
        int i, j;

        setnum = (int)(cachesize / (blocksize * setsize));
        cache->sets = (Set_t *)malloc(setnum * sizeof(Set_t));
        /*
         * --iterate over number of sets
         *      - new set
         *          –- iterate over number of lines
         *              - new cacheline for each 
         */
        for(i = 0; i < setnum; i++) {
            cache->sets[i] = *newSet(setsize);
            for(j = 0; j < setsize; j++) {
                cache->sets[i].lines[j] = *newCacheLine();       
            }
        }
    }
    /*
     * Full assoc
     *
     * Only one set
     *  - set contains all lines
     */
    if(strcmp("assoc", associativity) == 0) {
        int i;
        lines = (int)(cachesize / blocksize);
        cache->sets = (Set_t *)malloc(sizeof(Set_t));
        cache->sets[0] = *newSet(lines);
        for(i = 0; i< lines; i++) {
            cache->sets[0].lines[i] = *newCacheLine();
        }
    }
    return cache;
}

/*
 * readFromCache
 */
int readFromCache(Cache_t *cache, char* address, char* tag, char* setid, char* offset, int len ) {
    Set_t set;
    Cache_Line_t *cache_line;
    int i,size, found, lowest_index, lowest_count, temp;
    found = 0;
   /*
    * iterate over the lines of the set
    */ 
    if(strcmp("assoc",cache->associativity) == 0){
        set = cache->sets[0];
    }else{
        set = cache->sets[btoi(setid)];
    }
    size = set.line_count;
    for(i = 0; i < size; i++){
        if(DEBUG_CACHE){
            printf("checking lines, readFromCache\n");
        }
        cache_line = &set.lines[i];
        /*
         * Found
         */ 
        if(cache_line->valid && strcmp(cache_line->tag, tag) == 0) {
           cache_line->counter++;
           cache->hits++; 
           return 1; 
        }
    }
    if(DEBUG_CACHE){
        printf("not found, readFromCache\n");
    }
    cache->misses++;
    cache->mem_reads++;
    if(strcmp("direct",cache->associativity) == 0) {
        cache_line = &set.lines[0];
        cache_line->tag = tag;
        cache_line->valid = 1;
        cache_line->offset = offset;
        if(strcmp("wb",cache->write_policy) == 0 && cache_line->dirty == 1){
            cache->mem_writes++;
            cache_line->dirty = 0;
        }
    }else{ 
       /*
        * Select a new line based on LRU
        *  line to be replaced should be the furthest accessed in time
        */
       lowest_index = 0;
       lowest_count = set.lines[0].counter;
       for(i = 0; i < size; i++){
            temp = set.lines[i].counter;
            if( temp < lowest_count ) {
                lowest_count = temp;
                lowest_index = i;
            }
       }
       cache_line = &set.lines[lowest_index];
       cache_line->tag = tag;
       cache_line->valid = 1;
       cache_line->offset = offset;
       if(strcmp("wb",cache->write_policy) == 0 && cache_line->dirty){
            cache->mem_writes++;
            cache_line->dirty = 0;
       }
    }
    return 0;
}

/*
 * writeToCache
 */
int writeToCache(Cache_t *cache, char* address, char* tag, char* setid, char* offset, int len ) {
    
    Set_t set;
    Cache_Line_t *cache_line;
    int i,size, found, lowest_index, lowest_count, temp;
    found = 0;
    if(strcmp("assoc",cache->associativity) == 0){
        set = cache->sets[0];
    }else{
        set = cache->sets[btoi(setid)];
    }
    size = set.line_count; 
    for(i = 0; i < size; i++){
        if(DEBUG_CACHE){
            printf("checking lines, writeToCache\n");
        }
        cache_line = &set.lines[i];
        /*
         * Found
         */
        if(cache_line->valid && strcmp(cache_line->tag, tag) == 0) {
            if(strcmp("wt",cache->write_policy) == 0) {
                cache->mem_writes++;
            }
            cache->hits++; 
            cache_line->offset = offset;
            cache_line->counter++; 
            cache_line->dirty = 1;
            return 1;          
        }
    }
    
    if(DEBUG_CACHE){
          printf("not found, writeToCache\n");
    }
    cache->misses++;
    cache->mem_reads++;
      /*
       * add the cacheline to the set
       */
    if(strcmp("direct",cache->associativity) == 0) {
       cache_line = &set.lines[0];
       cache_line->tag = tag;
       cache_line->valid = 1;
       cache_line->offset = offset;
       cache_line->dirty = 1;
        if(strcmp("wt",cache->write_policy) == 0){
            cache->mem_writes++;
        }else if(strcmp("wb",cache->write_policy) == 0 && cache_line->dirty){
            cache->mem_writes++;
        }
    }else{ 
        /*
        * Select a new line based on LRU
        *  line to be replaced should be the furthest accessed in time
        */
       lowest_index = 0;
       lowest_count = set.lines[0].counter;
       for(i = 0; i < size; i++){
            temp = set.lines[i].counter;
            if( temp < lowest_count ) {
                lowest_count = temp;
                lowest_index = i;
            }
       }
       cache_line = &set.lines[lowest_index];
       cache_line->tag = tag;
       cache_line->valid = 1;
       cache_line->offset = offset;
       cache_line->dirty = 1;
        if(strcmp("wt",cache->write_policy) == 0){
            cache->mem_writes++;
        }else if(strcmp("wb",cache->write_policy) == 0 && cache_line->dirty == 1){
            cache->mem_writes++;
        }
    }
    return 0;
}

/*
 * Cache Line constructor
 */

Cache_Line_t* newCacheLine(){
    Cache_Line_t* cache_line;
    cache_line = malloc(sizeof(Cache_Line_t));
    cache_line->valid = 0;
    cache_line->tag = NULL;
    cache_line->counter = 0;
    cache_line->dirty = 0;
    return cache_line;
}

/*
 * Cache Set constructor
 */
Set_t* newSet(int line_count){
    Set_t* set;    
    set = malloc(sizeof(Set_t));
    set->line_count = line_count;
    set->lines = (Cache_Line_t *)malloc(sizeof(Cache_Line_t)*line_count);
    
    return set; 
}

/*
 * Checks if an address was placed in cache
 */
int isInCache(Cache_t *cache, char* address, char* tag, char* setid, char* offset) {
    Set_t set;
    Cache_Line_t cache_line;
    int i,size;
    set = cache->sets[btoi(setid)];
    size = set.line_count;
    for(i = 0; i < size; i++){
        if(DEBUG_CACHE){
            printf("checking lines\n");
        }
        cache_line = set.lines[i];
        if(cache_line.valid){    
            if(strcmp(cache_line.tag, tag)==0) {
                return 1;
            }
        }
    }
    return 0;
}

/*
 * Print cache
 */
void printCache(Cache_t *cache){
    printf("Memory reads: %i\n", cache->mem_reads);
    printf("Memory writes: %i\n", cache->mem_writes);
    printf("Cache hits: %i\n", cache->hits);
    printf("Cache misses: %i\n", cache->misses);
}
