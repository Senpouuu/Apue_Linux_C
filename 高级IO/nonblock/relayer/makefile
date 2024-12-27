CFLAGS+=-pthread -g
LDFLAGS+=-pthread -g 
 
all:relayer

relayer:relayer.o main.o
	gcc $^ -g -o $@ $(LDFLAGS) $(CFLAGS)

clean:
	rm -rf *.o relayer

