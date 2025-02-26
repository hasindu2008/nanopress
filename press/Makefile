CC = cc
CFLAGS = -g -Wall -Werror
LDFLAGS = -lm -lz -lzstd -lpthread
OBJ = test.o press.o trans.o flat.o bitmap.o stats.o util.o
JNNLIB = $(JNN)/jnn.o
SVB = streamvbyte
SVBLIB = $(SVB)/libstreamvbyte.so.0.0.1
BZIP2 = bzip2
BZIP2LIB = $(BZIP2)/libbz2.a
FLZMA2 = fast-lzma2
FLZMA2LIB = $(FLZMA2)/libfast-lzma2.a
FLAC = flac-1.3.4
FLACLIB = $(FLAC)/src/libFLAC/.libs/libFLAC.so
TURBOPFOR = TurboPFor-Integer-Compression
TURBOPFORLIB = $(TURBOPFOR)/libic.a
SLOW5 = slow5lib
SLOW5INCLUDE = $(SLOW5)/include
SLOW5LIB = $(SLOW5)/lib/libslow5.a
HUFFMAN = huffman
HUFFMANLIB = $(HUFFMAN)/libhuffman.a
TURBORC = Turbo-Range-Coder
TURBORCLIB = $(TURBORC)/librc.a
SIGTK = sigtk
SIGTKLIB = $(SIGTK)/build/jnn.o $(SIGTK)/build/sigtk.o
LIB = $(SVBLIB) $(BZIP2LIB) $(FLZMA2LIB) $(FLACLIB) $(TURBOPFORLIB) $(SLOW5LIB) $(HUFFMANLIB) $(TURBORCLIB) $(SIGTKLIB)

.PHONY = clean $(LIB)

test: $(OBJ) $(LIB)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

press.a: $(OBJ) $(LIB)
	$(AR) rcsT $@ $^

$(SVBLIB):
	make -C $(SVB)

$(BZIP2LIB):
	make -C $(BZIP2)

$(FLZMA2LIB):
	make -C $(FLZMA2)

$(FLACLIB):
	make -C $(FLAC)

$(HUFFMANLIB):
	make -C $(HUFFMAN)

$(TURBOPFORLIB):
	make -C $(TURBOPFOR)

$(TURBORCLIB):
	make -C $(TURBORC) librc.a

$(SIGTKLIB):
	make -C $(SIGTK)

test.o: test.c test.h press.h bitmap.h
	$(CC) $< $(CFLAGS) -I $(SLOW5INCLUDE) -c -o $@

press.o: press.c press.h bitmap.h stats.h trans.h util.h flat.h
	$(CC) $< $(CFLAGS) -I $(SLOW5INCLUDE) -c -o $@

trans.o: trans.c trans.h
	$(CC) $< $(CFLAGS) -c -o $@

flat.o: flat.c flat.h stats.h util.h press.h bitmap.h
	$(CC) $< $(CFLAGS) -c -o $@

bitmap.o: bitmap.c bitmap.h
	$(CC) $< $(CFLAGS) -c -o $@

stats.o: stats.c stats.h
	$(CC) $< $(CFLAGS) -c -o $@

util.o: util.c util.h
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm $(OBJ) test
	make -C $(SVB) clean
	make -C $(BZIP2) clean
	make -C $(FLZMA2) clean
