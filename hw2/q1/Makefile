# Makefile for Cilk++ example.  See source code for details.

CILKPP	= cilk++
LIBARG	= -O1 -g -lcilkutil
TARGET	= q1
SRC	= $(addsuffix .cilk,$(TARGET))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CILKPP) $(SRC) $(LIBARG) -o $@

clean:
	rm -f $(TARGET)
