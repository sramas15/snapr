#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

## Main application file
BENCHMARKS = \
	benchmark-tsv \
        benchmark-totable \
        benchmark-converttype \
        benchmark-json
	\

all: $(BENCHMARKS)

# COMPILE
benchmark-%: benchmark-%.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(BENCHMARKS) *.exe
	rm -rf Debug Release
	rm -rf *.Err benchmark*.dat

