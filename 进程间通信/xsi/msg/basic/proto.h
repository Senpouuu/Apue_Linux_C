#ifndef _PROTO_H_
#define _PROTO_H_

#define KEY_PATH "/etc/services"
#define KEY_PORJ 'g'            // int 
#define NAMESIZE 20


// 数学，语文成绩，数学成绩
struct msg_st 
{
    long mtype;
    
    char name[NAMESIZE];
    int math;
    int chinese;
}; 




#endif /* _PROTO_H_ */
