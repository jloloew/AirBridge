
EXE = transmitter

OBJS = transmitter.o main.o

CC = clang
CCFLAGS = -c -O2 -msse2 -Wall -Wextra -pedantic
LD = clang
LDFLAGS = 


all: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@

%.o : %.c
	$(CC) $(CCFLAGS) $< -o $*.o

.PHONY: all clean
clean:
	-rm -f *.o $(EXE)

