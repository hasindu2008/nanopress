SIGTK=../press/sigtk
SLOW5=~/work/garvan/slow5/slow5tools/slow5lib
SLOW5_INCLUDE=$(SLOW5)/include
LIBSLOW5=$(SLOW5)/lib/libslow5.a

print_stats_stall:
	cc $@.c stats.c $(SIGTK)/build/jnn.o $(SIGTK)/build/sigtk.o -lm -I $(SLOW5_INCLUDE) $(LIBSLOW5) -lz -o $@
