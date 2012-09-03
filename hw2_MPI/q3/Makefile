# Makefile for Cilk++ example.  See source code for details.

CILKPP	= mpicc
TARGET	= q3mpi
SRC	= $(addsuffix .cpp,$(TARGET))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CILKPP) $(SRC) -o $@

clean:
	rm -f $(TARGET)
