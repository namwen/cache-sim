# cache-sim

Simulates a cache by reading a mock memory trace file formatted as such:
```
0x804ae1c: W 0x9cb2874
```
The first address is the instruction pointer, then W or R for read or write, and then the memory address which is being acted on.

The program takes arguments as follows:
```
c-sim <cache size> <associativity> <block size> <write policy> <trace file>
```
cache size: total size of the cache, must be a power of two

associativity: 'direct', 'assoc', or 'assoc:n', where n is the number of lines per set (also a power of 2)

block size: size of the cache block, must be a power of two

write policy: 'wt' for a write-through simulation, 'wb' for a write-back simulation.

tracefile: filename for a txt file formatted as above
