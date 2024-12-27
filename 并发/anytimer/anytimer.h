#ifndef ANYTIMER_H
#define ANYTIMER_H

#define AT_MAX_JOBS 100

typedef void at_jobfunc_t(void *arg);

int at_addjob(int sec, at_jobfunc_t *func, void *arg,int repeat);

int at_canceljob(int id);


int at_waitjob(int id);

// at_pausejob(int id);

// at_resumejob(int id);



#endif /* ANYTIMER_H */