#ifndef PROTO_H__
#define PROTO_H__
#include <stdint.h>

#define RCV_PORT "1025"


#define NAMEMAX 512-8-8-32-32-32 // 512-udpbaseheader-math-chinese-english-nameheader

struct msg_st
{
    uint32_t math;
    uint32_t chinese;
    uint32_t english;
    uint8_t name[1];

}__attribute__((__packed__));       // 编译器数据对齐


#endif


