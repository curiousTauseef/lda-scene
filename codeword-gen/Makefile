CC=g++
#CFLAGS+=-g
CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv` 
LDFLAGS+= -lboost_system -lboost_filesystem

PROG=codeword-gen
OBJS=$(PROG).o

FILE_OUT="$(PROG).out"

.PHONY: all clean
	$(PROG): $(OBJS)
	    $(CC) -o $(FILE_OUT) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	    $(CC) -c $(CFLAGS) $<

all: $(PROG)

clean:
	    rm -f $(OBJS) $(FILE_OUT)
