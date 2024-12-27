#ifndef _MYFTP_PROTO_H__
#define _MYFTP_PROTO_H__

#define KEYPATH "/etc/services"
#define KEYPROJ "g"
#define PATHSIZE 1024 
#define DATASIZE 1024 

enum
{
    PATH_PACKAGE = 1,
    DATA_PACKAGE,
    EOT_PACKAGE
};


// server 端接包
typedef struct msg_path_st
{
    long msg_type;
    char path[PATHSIZE];
}msg_path_t;


// client 端接包，用msg_tpye 区分不同的包
typedef struct msg_data_st
{
    long msg_type;
    char data[DATASIZE];   
    int data_len;
}msg_data_t;


typedef struct msg_eot_st
{
    long msg_type;
    char dummy[1];
}msg_eot_t;







#endif