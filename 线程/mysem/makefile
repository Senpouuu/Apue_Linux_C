gFLAGS+=-pthread
LDFLAGS+=-pthread

all:mysem

mysem:main.o mysem.o
	gcc $^ -o mysem $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf	*.o mysem
