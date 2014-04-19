#ifndef DEBUG_H
#define DEBUG_H
#include"moduleswitch.h"
#if 0 != _DEBUG_TRACE_CMH_
    #include <qdebug.h>
#endif

#if 1==_DEBUG_TRACE_CMH_    //普通打印
    #define DEBUG_OUT qDebug
#elif 2==_DEBUG_TRACE_CMH_  //打印文件名、行号
    #define DEBUG_OUT(fmt,...) \
        qDebug("%s(%d): "fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#elif 3==_DEBUG_TRACE_CMH_  //打印文件名、行号、函数名
    #define DEBUG_OUT(fmt,...) \
        qDebug("%s(%d)-<%s>: "fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define DEBUG_OUT(fmt,...) (void *)0
#endif //_TRACE_CMH_DEBUG_

#if 1==_WAR_TRACE_CMH_    //普通打印
    #define WAR_OUT qWarning
#elif 2==_WAR_TRACE_CMH_  //打印文件名、行号
    #define WAR_OUT(fmt,...) \
        qWarning("%s(%d): "fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#elif 3==_WAR_TRACE_CMH_  //打印文件名、行号、函数名
    #define WAR_OUT(fmt,...) \
        qWarning("%s(%d)-<%s>: "fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define WAR_OUT(fmt,...) (void *)0
#endif //_TRACE_CMH_DEBUG_

#endif // DEBUG_H
