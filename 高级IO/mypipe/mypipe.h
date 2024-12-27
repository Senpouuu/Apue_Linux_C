#ifndef MYPIPE_H
#define MYPIPE_H
#define PIPE_SIZE 1024 

#define READER 0x00000001UL
#define WRITER 0x00000010UL


typedef void pipe_t;


pipe_t *mypipe_init(void);

int mypipe_read(pipe_t *p, void *buf, int size);

int mypipe_write(pipe_t *p, void *buf, int size);

int mypipe_register(pipe_t *p, int flag);

int mypipe_unregister(pipe_t *p, int flag);

void mypipe_destroy(pipe_t *p);
 





#endif