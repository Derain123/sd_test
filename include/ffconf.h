/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.11a (C)ChaN, 2015
/---------------------------------------------------------------------------*/

#define _FFCONF 64180	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define _FS_READONLY 1  /* 只读模式 */
#define _FS_MINIMIZE 3  /* 最大程度精简 */
#define _USE_STRFUNC 0  /* 禁用字符串函数 */
#define _USE_FIND 0     /* 禁用查找功能 */
#define _USE_MKFS 0     /* 禁用格式化功能 */
#define _USE_FASTSEEK 0 /* 禁用快速查找 */
#define _USE_LABEL 0    /* 禁用卷标功能 */
#define _USE_FORWARD 0  /* 禁用数据流功能 */

/* 代码页设置 */
#define _CODE_PAGE 1    /* ASCII，无扩展字符 */

/* 长文件名设置 */
#define _USE_LFN 0      /* 禁用长文件名 */
#define _MAX_LFN 0      /* 不需要 LFN 缓冲区 */

/* Unicode 设置 */
#define _LFN_UNICODE 0  /* 使用 ANSI/OEM 编码 */

/* 路径设置 */
#define _FS_RPATH 0     /* 禁用相对路径 */

/* 驱动器/卷设置 */
#define _VOLUMES 1      /* 只使用一个驱动器 */
#define _MAX_SS 512     /* 固定扇区大小为 512 字节 */
#define _MIN_SS 512

/* 系统设置 */
#define _FS_TINY 1      /* 使用精简模式 */
// #define _FS_NORTC 1     /* 禁用时间戳 */
#define _FS_LOCK 0      /* 禁用文件锁 */
#define _FS_REENTRANT 0 /* 禁用重入支持 */

/* 字节访问设置 */
#define _WORD_ACCESS 0  /* 使用字节访问，更兼容 */

