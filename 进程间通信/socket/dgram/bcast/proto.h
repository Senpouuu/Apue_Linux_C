#ifndef PROTO_H__
#define PROTO_H__
#include <stdint.h>

#define RCV_PORT "1025"


#define NAMESIZE 11

struct msg_st
{
    uint8_t name[NAMESIZE];
    uint32_t math;
    uint32_t chinese;
    uint32_t english;
}__attribute__((__packed__));       // 编译器数据对齐












#endif


