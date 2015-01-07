#ifndef _CACHE_H_
#define _CACHE_H_
typedef struct Cache_Line Cache_Line_t;
typedef struct Cache Cache_t;
typedef struct Set Set_t;

struct Cache_Line {
    int valid;
    char* tag;
    char* offset;
    int counter;
    int dirty;
};

struct Cache {
    int hits;
    int misses;
    int mem_reads;
    int mem_writes;
    int set_size;
    int cache_size;
    int block_size;
    int number_sets;
    int lines_per_set;
    char* write_policy;
    char* associativity;
    /* pointer to array of sets */
    Set_t *sets;
};

struct Set {
    Cache_Line_t *lines;
    int line_count;
    int lru_tag;
    int lru_timestamp;
};

/*
 * Create a new cache based off the parameters given by the user
 *
 * returns a Cache struct or NULL
 *
 */
Cache_t* newCache(int cache_size, int blocksize, int setsize, char* associativity, char* writepolicy);


/*
 * Write to cache
*/
int writeToCache(Cache_t *cache, char* address, char* tag, char* setid, char* offset, int len );

/*
 * Read from cache
 */
int readFromCache(Cache_t *cache, char* address, char* tag, char* setid, char* offset, int len );

/*
 * Create a new Cache Line
 */
Cache_Line_t* newCacheLine();


/*
 * Create a new Set
 */

Set_t* newSet(int line_count);

/*
 * checks if the cache contains an address
 */
int isInCache(Cache_t *cache, char* address, char* tag, char* setid, char* offset); 

void printCache(Cache_t *cache);

#endif
