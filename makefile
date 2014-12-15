COMPILER = gcc
CCFLAGS = -Wall -ansi -pedantic -m32

c-sim: c-sim.c
	$(COMPILER) $(CCFLAGS) -o c-sim c-sim.c cache.c helpers.c files.c -lm

clean:
	 rm -f c-sim
