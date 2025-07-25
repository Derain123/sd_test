/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.11a (C)ChaN, 2015
/---------------------------------------------------------------------------*/

#define _FFCONF 64180	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define _FS_READONLY 1  /* 设为只读模式 */
#define _FS_MINIMIZE 3  /* 最大程度精简功能 */
#define _USE_STRFUNC 0  /* 禁用字符串函数 */
#define _USE_FIND 0     /* 禁用查找功能 */
#define _USE_MKFS 0     /* 禁用格式化功能 */
#define _USE_FASTSEEK 0 /* 禁用快速查找 */
#define _USE_LABEL 0    /* 禁用卷标功能 */
#define _USE_FORWARD 0  /* 禁用数据流功能 */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define _CODE_PAGE	1
/* This option specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   1   - ASCII (No extended character. Non-LFN cfg. only)
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   771 - KBL
/   775 - Baltic
/   850 - Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   860 - Portuguese
/   861 - Icelandic
/   862 - Hebrew
/   863 - Canadian French
/   864 - Arabic
/   865 - Nordic
/   866 - Russian
/   869 - Greek 2
/   932 - Japanese (DBCS)
/   936 - Simplified Chinese (DBCS)
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
*/


#define	_USE_LFN	0
#define	_MAX_LFN	255
/* The _USE_LFN option switches the LFN feature.
/
/   0: Disable LFN feature. _MAX_LFN has no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT thread-safe.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  When enable the LFN feature, Unicode handling functions (option/unicode.c) must
/  be added to the project. The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes.
/  When use stack for the working buffer, take care on stack overflow. When use heap
/  memory for the working buffer, memory management functions, ff_memalloc() and
/  ff_memfree(), must be added to the project. */


#define	_LFN_UNICODE	0
/* This option switches character encoding on the API. (0:ANSI/OEM or 1:Unicode)
/  To use Unicode string for the path name, enable LFN feature and set _LFN_UNICODE
/  to 1. This option also affects behavior of string I/O functions. */


#define _STRF_ENCODE	3
/* When _LFN_UNICODE is 1, this option selects the character encoding on the file to
/  be read/written via string I/O functions, f_gets(), f_putc(), f_puts and f_printf().
/
/  0: ANSI/OEM
/  1: UTF-16LE
/  2: UTF-16BE
/  3: UTF-8
/
/  When _LFN_UNICODE is 0, this option has no effect. */


#define _FS_RPATH	2
/* This option configures relative path feature.
/
/   0: Disable relative path feature and remove related functions.
/   1: Enable relative path feature. f_chdir() and f_chdrive() are available.
/   2: f_getcwd() function is available in addition to 1.
/
/  Note that directory items read via f_readdir() are affected by this option. */


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define _VOLUMES	2
/* Number of volumes (logical drives) to be used. */


#define _STR_VOLUME_ID	0
#define _VOLUME_STRS	"CF","SD"
/* _STR_VOLUME_ID option switches string volume ID feature.
/  When _STR_VOLUME_ID is set to 1, also pre-defined strings can be used as drive
/  number in the path name. _VOLUME_STRS defines the drive ID strings for each
/  logical drives. Number of items must be equal to _VOLUMES. Valid characters for
/  the drive ID strings are: A-Z and 0-9. */


#define	_MULTI_PARTITION	0
/* This option switches multi-partition feature. By default (0), each logical drive
/  number is bound to the same physical drive number and only an FAT volume found on
/  the physical drive will be mounted. When multi-partition feature is enabled (1),
/  each logical drive number is bound to arbitrary physical drive and partition
/  listed in the VolToPart[]. Also f_fdisk() funciton will be available. */


#define	_MIN_SS		512
#define	_MAX_SS		512
/* These options configure the range of sector size to be supported. (512, 1024,
/  2048 or 4096) Always set both 512 for most systems, all type of memory cards and
/  harddisk. But a larger value may be required for on-board flash memory and some
/  type of optical media. When _MAX_SS is larger than _MIN_SS, FatFs is configured
/  to variable sector size and GET_SECTOR_SIZE command must be implemented to the
/  disk_ioctl() function. */


#define	_USE_TRIM	0
/* This option switches ATA-TRIM feature. (0:Disable or 1:Enable)
/  To enable Trim feature, also CTRL_TRIM command should be implemented to the
/  disk_ioctl() function. */


#define _FS_NOFSINFO	0
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this
/  option, and f_getfree() function at first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/
/  bit0=0: Use free cluster count in the FSINFO if available.
/  bit0=1: Do not trust free cluster count in the FSINFO.
/  bit1=0: Use last allocated cluster number in the FSINFO if available.
/  bit1=1: Do not trust last allocated cluster number in the FSINFO.
*/



/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define _FS_TINY 1     /* 使用精简模式 */
// #define _FS_NORTC 1    /* 禁用时间戳功能 */
#define _FS_LOCK 0     /* 禁用文件锁 */
#define _FS_REENTRANT 0 /* 禁用重入支持 */
#define _FS_TIMEOUT		1000
#define	_SYNC_t			HANDLE
/* The _FS_REENTRANT option switches the re-entrancy (thread safe) of the FatFs
/  module itself. Note that regardless of this option, file access to different
/  volume is always re-entrant and volume control functions, f_mount(), f_mkfs()
/  and f_fdisk() function, are always not re-entrant. Only file/directory access
/  to the same volume is under control of this feature.
/
/   0: Disable re-entrancy. _FS_TIMEOUT and _SYNC_t have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function, must be added to the project. Samples are available in
/      option/syscall.c.
/
/  The _FS_TIMEOUT defines timeout period in unit of time tick.
/  The _SYNC_t defines O/S dependent sync object type. e.g. HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc.. A header file for O/S definitions needs to be
/  included somewhere in the scope of ff.c. */


#define _WORD_ACCESS	0
/* The _WORD_ACCESS option is an only platform dependent option. It defines
/  which access method is used to the word data on the FAT volume.
/
/   0: Byte-by-byte access. Always compatible with all platforms.
/   1: Word access. Do not choose this unless under both the following conditions.
/
/  * Address misaligned memory access is always allowed to ALL instructions.
/  * Byte order on the memory is little-endian.
/
/  If it is the case, _WORD_ACCESS can also be set to 1 to reduce code size.
/  Following table shows allowable settings of some type of processors.
/
/  ARM7TDMI   0   *2          ColdFire   0    *1         V850E      0    *2
/  Cortex-M3  0   *3          Z80        0/1             V850ES     0/1
/  Cortex-M0  0   *2          x86        0/1             TLCS-870   0/1
/  AVR        0/1             RX600(LE)  0/1             TLCS-900   0/1
/  AVR32      0   *1          RL78       0    *2         R32C       0    *2
/  PIC18      0/1             SH-2       0    *1         M16C       0/1
/  PIC24      0   *2          H8S        0    *1         MSP430     0    *2
/  PIC32      0   *1          H8/300H    0    *1         8051       0/1
/
/  *1:Big-endian.
/  *2:Unaligned memory access is not supported.
/  *3:Some compilers generate LDM/STM for mem_cpy function.
*/

