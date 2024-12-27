#ifndef RELAYER_H__
#define RELAYER_H__
#include <stdint.h> 

enum
{
    STAT_RUNNING = 1,
    STAT_CANCLED,
    STAT_OVER

};


struct rel_stat_st 
{
    int state;
    int fd1;
    int fd2;
    int64_t count12,count21;
    //struct timerval start,end; 

};


int rel_addjob(int fd1,int fd2);
/*   RETRUNS
 *    0       成功，返回任务ID
 *    -EINVAL 失败，参数非法
 *    -ENOSPC 失败，任务数组满
 *    -ENOMEM 失败，内存分配有误
 *
 */


int rel_canclejob(int id);
/*   RETURNS
 *    0       成功，指定任务取消
 *    -EINVAL 失败，参数非法
 *    -EBUSY  失败，任务早已取消
 */


int rel_waitjob(int id);
/*  RETRUNS
 *   0         成功，指定任务已终止
 *   -EINVAL   失败，参数非法
 */

int rel_statjob(int id);
/*  RETURNS
 *  0   成功，指定任务状态已经返回
 *  -EINVAL   失败，参数非法 
 */


#endif


