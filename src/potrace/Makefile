OBJS = trace.o curve.o potracelib.o

%.o: %.c
	gcc -DHAVE_CONFIG_H -I. -I.. -g -Wall -ffloat-store -c -o $@ $<

all: libpotrace.a

libpotrace.a: $(OBJS)
	rm -f libpotrace.a \ 
	ar cru libpotrace.a curve.o trace.o potracelib.o; \
	ranlib libpotrace.a

test: test.c libpotrace.a
	gcc -DDEBUG -DHAVE_CONFIG_H -I. -g -Wall -ffloat-store -o test test.c trace.c -L. -lpotrace -lm
