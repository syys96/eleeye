// begin of base.h

#include <assert.h>
#include <sys/timeb.h>

#ifndef BASE_H
#define BASE_H

#ifdef _MSC_VER
typedef signed   __int64  int64_t; // ll
  typedef unsigned __int64 uint64_t; // qw
  typedef signed   __int32  int32_t; // l
  typedef unsigned __int32 uint32_t; // dw
  typedef signed   __int16  int16_t; // s
  typedef unsigned __int16 uint16_t; // w
  typedef signed   __int8   int8_t;  // c
  typedef unsigned __int8  uint8_t;  // uc
  #define FORMAT_I64 "I64"
#else
#include <stdint.h>
#define FORMAT_I64 "ll"
#endif

#define __ASSERT(a) assert(a)
#define __ASSERT_BOUND(a, b, c) assert((a) <= (b) && (b) <= (c))
#define __ASSERT_BOUND_2(a, b, c, d) assert((a) <= (b) && (b) <= (c) && (c) <= (d))

inline bool EQV(bool bArg1, bool bArg2) {
    return bArg1 ? bArg2 : !bArg2;
}

inline bool XOR(bool bArg1, bool bArg2) {
    return bArg1 ? !bArg2 : bArg2;
}

template <typename T> inline T MIN(T Arg1, T Arg2) {
    return Arg1 < Arg2 ? Arg1 : Arg2;
}

template <typename T> inline T MAX(T Arg1, T Arg2) {
    return Arg1 > Arg2 ? Arg1 : Arg2;
}

template <typename T> inline T ABS(T Arg) {
    return Arg < 0 ? -Arg : Arg;
}

template <typename T> inline T SQR(T Arg) {
    return Arg * Arg;
}

template <typename T> inline void SWAP(T &Arg1, T &Arg2) {
    T Temp;
    Temp = Arg1;
    Arg1 = Arg2;
    Arg2 = Temp;
}

inline int PopCnt8(uint8_t uc) {
    int n;
    n = ((uc >> 1) & 0x55) + (uc & 0x55);
    n = ((n >> 2) & 0x33) + (n & 0x33);
    return (n >> 4) + (n & 0x0f);
}

inline int PopCnt16(uint16_t w) {
    int n;
    n = ((w >> 1) & 0x5555) + (w & 0x5555);
    n = ((n >> 2) & 0x3333) + (n & 0x3333);
    n = ((n >> 4) & 0x0f0f) + (n & 0x0f0f);
    return (n >> 8) + (n & 0x00ff);
}

inline int PopCnt32(uint32_t dw) {
    int n;
    n = ((dw >> 1) & 0x55555555) + (dw & 0x55555555);
    n = ((n >> 2) & 0x33333333) + (n & 0x33333333);
    n = ((n >> 4) & 0x0f0f0f0f) + (n & 0x0f0f0f0f);
    n = ((n >> 8) & 0x00ff00ff) + (n & 0x00ff00ff);
    return (n >> 16) + (n & 0x0000ffff);
}

inline int64_t GetTime() {
    timeb tb;
    ftime(&tb);
    return (int64_t) tb.time * 1000 + tb.millitm;
}

#endif


// end of base.h

// begin of base2.h

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#endif
#include <string.h>

#ifndef BASE2_H
#define BASE2_H

const int PATH_MAX_CHAR = 1024;

#ifdef _WIN32

inline void Idle(void) {
  Sleep(1);
}

const int PATH_SEPARATOR = '\\';

inline bool AbsolutePath(const char *sz) {
  return sz[0] == '\\' || (((sz[0] >= 'A' && sz[0] <= 'Z') || (sz[0] >= 'a' && sz[0] <= 'z')) && sz[1] == ':');
}

inline void GetSelfExe(char *szDst) {
  GetModuleFileName(NULL, szDst, PATH_MAX_CHAR);
}

inline void StartThread(void *ThreadEntry(void *), void *lpParameter) {
  DWORD dwThreadId;
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadEntry, (LPVOID) lpParameter, 0, &dwThreadId);
}

#else

inline void Idle(void) {
    usleep(1000);
}

const int PATH_SEPARATOR = '/';

inline bool AbsolutePath(const char *sz) {
    return sz[0] == '/' || (sz[0] == '~' && sz[1] == '/');
}

inline void GetSelfExe(char *szDst) {
    readlink("/proc/self/exe", szDst, PATH_MAX_CHAR);
}

inline void StartThread(void *ThreadEntry(void *), void *lpParameter) {
    pthread_t pthread;
    pthread_attr_t pthread_attr;
    pthread_attr_init(&pthread_attr);
    pthread_attr_setscope(&pthread_attr, PTHREAD_SCOPE_SYSTEM);
    pthread_create(&pthread, &pthread_attr, ThreadEntry, lpParameter);
}

#endif

inline void LocatePath(char *szDst, const char *szSrc) {
    char *lpSeparator;
    if (AbsolutePath(szSrc)) {
        strcpy(szDst, szSrc);
    } else {
        GetSelfExe(szDst);
        lpSeparator = strrchr(szDst, PATH_SEPARATOR);
        if (lpSeparator == NULL) {
            strcpy(szDst, szSrc);
        } else {
            strcpy(lpSeparator + 1, szSrc);
        }
    }
}

#endif

// end of base2.h

// begin of parse.h

#include <stdio.h>
#include <string.h>

#ifndef PARSE_H
#define PARSE_H

#ifdef _WIN32

#include <windows.h>
#include <shlwapi.h>

inline char *strcasestr(const char *sz1, const char *sz2) {
  return StrStrI(sz1, sz2);
}

#endif

#ifdef _MSC_VER
inline int strncasecmp(const char *sz1, const char *sz2, size_t n) {
    return strnicmp(sz1, sz2, n);
  }
#endif

inline void StrCutCrLf(char *sz) {
    char *lpsz;
    lpsz = strchr(sz, '\r');
    if (lpsz != NULL) {
        *lpsz = '\0';
    }
    lpsz = strchr(sz, '\n');
    if (lpsz != NULL) {
        *lpsz = '\0';
    }
}

inline bool StrEqv(const char *sz1, const char *sz2) {
    return strncasecmp(sz1, sz2, strlen(sz2)) == 0;
}

inline bool StrEqvSkip(const char *&sz1, const char *sz2) {
    if (strncasecmp(sz1, sz2, strlen(sz2)) == 0) {
        sz1 += strlen(sz2);
        return true;
    } else {
        return false;
    }
}

inline bool StrEqvSkip(char *&sz1, const char *sz2) {
    if (strncasecmp(sz1, sz2, strlen(sz2)) == 0) {
        sz1 += strlen(sz2);
        return true;
    } else {
        return false;
    }
}

inline bool StrScan(const char *sz1, const char *sz2) {
    return strcasestr(sz1, sz2) != NULL;
}

inline bool StrScanSkip(const char *&sz1, const char *sz2) {
    const char *lpsz;
    lpsz = strcasestr(sz1, sz2);
    if (lpsz == NULL) {
        return false;
    } else {
        sz1 = lpsz + strlen(sz2);
        return true;
    }
}

inline bool StrScanSkip(char *&sz1, const char *sz2) {
    char *lpsz;
    lpsz = strcasestr(sz1, sz2);
    if (lpsz == NULL) {
        return false;
    } else {
        sz1 = lpsz + strlen(sz2);
        return true;
    }
}

inline bool StrSplitSkip(const char *&szSrc, int nSeparator, char *szDst = NULL) {
    const char *lpsz;
    lpsz = strchr(szSrc, nSeparator);
    if (lpsz == NULL) {
        if (szDst != NULL) {
            strcpy(szDst, szSrc);
        }
        szSrc += strlen(szSrc);
        return false;
    } else {
        if (szDst != NULL) {
            strncpy(szDst, szSrc, lpsz - szSrc);
            szDst[lpsz - szSrc] = '\0';
        }
        szSrc = lpsz + 1;
        return true;
    }
}

inline bool StrSplitSkip(char *&szSrc, int nSeparator, char *szDst = NULL) {
    char *lpsz;
    lpsz = strchr(szSrc, nSeparator);
    if (lpsz == NULL) {
        if (szDst != NULL) {
            strcpy(szDst, szSrc);
        }
        szSrc += strlen(szSrc);
        return false;
    } else {
        if (szDst != NULL) {
            strncpy(szDst, szSrc, lpsz - szSrc);
            szDst[lpsz - szSrc] = '\0';
        }
        szSrc = lpsz + 1;
        return true;
    }
}

inline int Str2Digit(const char *sz, int nMin, int nMax) {
    int nRet;
    if (sscanf(sz, "%d", &nRet) > 0) {
        return MIN(MAX(nRet, nMin), nMax);
    } else {
        return nMin;
    }
}

#endif

// end of parse.h

// begin of ucci.h

/*
ucci.h/ucci.cpp - Source Code for ElephantEye, Part I

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.2, Last Modified: Sep. 2010
Copyright (C) 2004-2010 www.xqbase.com

This part (ucci.h/ucci.cpp only) of codes is NOT published under LGPL, and
can be used without restriction.
*/

#ifndef UCCI_H
#define UCCI_H

const int UCCI_MAX_DEPTH = 32; // UCCI引擎思考的极限深度

// 和UCCI指令中关键字有关的选项
enum UcciOptionEnum {
    UCCI_OPTION_UNKNOWN, UCCI_OPTION_BATCH, UCCI_OPTION_DEBUG, UCCI_OPTION_PONDER, UCCI_OPTION_USEHASH, UCCI_OPTION_USEBOOK, UCCI_OPTION_USEEGTB,
    UCCI_OPTION_BOOKFILES, UCCI_OPTION_EGTBPATHS, UCCI_OPTION_HASHSIZE, UCCI_OPTION_THREADS, UCCI_OPTION_PROMOTION,
    UCCI_OPTION_IDLE, UCCI_OPTION_PRUNING, UCCI_OPTION_KNOWLEDGE, UCCI_OPTION_RANDOMNESS, UCCI_OPTION_STYLE, UCCI_OPTION_NEWGAME
}; // 由"setoption"指定的选项
enum UcciRepetEnum {
    UCCI_REPET_ALWAYSDRAW, UCCI_REPET_CHECKBAN, UCCI_REPET_ASIANRULE, UCCI_REPET_CHINESERULE
}; // 选项"repetition"的设定值
enum UcciGradeEnum {
    UCCI_GRADE_NONE, UCCI_GRADE_TINY, UCCI_GRADE_SMALL, UCCI_GRADE_MEDIUM, UCCI_GRADE_LARGE, UCCI_GRADE_HUGE
}; // 选项"idle"、"pruning"、"knowledge"、"selectivity"的设定值
enum UcciStyleEnum {
    UCCI_STYLE_SOLID, UCCI_STYLE_NORMAL, UCCI_STYLE_RISKY
}; // 选项"style"的设定值
enum UcciGoEnum {
    UCCI_GO_DEPTH, UCCI_GO_NODES, UCCI_GO_TIME_MOVESTOGO, UCCI_GO_TIME_INCREMENT
}; // 由"go"指令指定的时间模式，分别是限定深度、限定结点数、时段制和加时制
enum UcciCommEnum {
    UCCI_COMM_UNKNOWN, UCCI_COMM_UCCI, UCCI_COMM_ISREADY, UCCI_COMM_PONDERHIT, UCCI_COMM_PONDERHIT_DRAW, UCCI_COMM_STOP,
    UCCI_COMM_SETOPTION, UCCI_COMM_POSITION, UCCI_COMM_BANMOVES, UCCI_COMM_GO, UCCI_COMM_PROBE, UCCI_COMM_QUIT
}; // UCCI指令类型

// UCCI指令可以解释成以下这个抽象的结构
union UcciCommStruct {

    /* 可得到具体信息的UCCI指令只有以下4种类型
     *
     * 1. "setoption"指令传递的信息，适合于"UCCI_COMM_SETOPTION"指令类型
     *    "setoption"指令用来设定选项，因此引擎接受到的信息有“选项类型”和“选项值”
     *    例如，"setoption batch on"，选项类型就是"UCCI_OPTION_DEBUG"，值(Value.bCheck)就是"true"
     */
    struct {
        UcciOptionEnum Option; // 选项类型
        union {                // 选项值
            int nSpin;           // "spin"类型的选项的值
            bool bCheck;         // "check"类型的选项的值
            UcciRepetEnum Repet; // "combo"类型的选项"repetition"的值
            UcciGradeEnum Grade; // "combo"类型的选项"pruning"、"knowledge"和"selectivity"的值
            UcciStyleEnum Style; // "combo"类型的选项"style"的值
            char *szOption;      // "string"类型的选项的值
        };                     // "button"类型的选项没有值
    };

    /* 2. "position"指令传递的信息，适合于"e_CommPosition"指令类型
     *    "position"指令用来设置局面，包括初始局面连同后续着法构成的局面
     *    例如，position startpos moves h2e2 h9g8，FEN串就是"startpos"代表的FEN串，着法数(MoveNum)就是2
     */
    struct {
        const char *szFenStr;     // FEN串，特殊局面(如"startpos"等)也由解释器最终转换成FEN串
        int nMoveNum;             // 后续着法数
        uint32_t *lpdwMovesCoord; // 后续着法，指向程序"IdleLine()"中的一个静态数组，但可以把"CoordList"本身看成数组
    };

    /* 3. "banmoves"指令传递的信息，适合于"e_CommBanMoves"指令类型
     *    "banmoves"指令用来设置禁止着法，数据结构时类似于"position"指令的后续着法，但没有FEN串
     */
    struct {
        int nBanMoveNum;
        uint32_t *lpdwBanMovesCoord;
    };

    /* 4. "go"指令传递的信息，适合于"UCCI_COMM_GO指令类型
     *    "go"指令让引擎思考(搜索)，同时设定思考模式，即固定深度、时段制还是加时制
     */
    struct {
        UcciGoEnum Go; // 思考模式
        bool bPonder;  // 后台思考
        bool bDraw;    // 提和
        union {
            int nDepth, nNodes, nTime;
        }; // 深度、结点数或时间
        union {
            int nMovesToGo, nIncrement;
        }; // 限定时间内要走多少步棋(加时制)或走完该步后限定时间加多少(时段制)
    };
};

// 下面三个函数用来解释UCCI指令，但适用于不同场合
UcciCommEnum BootLine(void);                                  // UCCI引擎启动的第一条指令，只接收"ucci"
UcciCommEnum IdleLine(UcciCommStruct &UcciComm, bool bDebug); // 引擎空闲时接收指令
UcciCommEnum BusyLine(UcciCommStruct &UcciComm, bool bDebug); // 引擎思考时接收指令，只允许接收"stop"、"ponderhit"和"probe"

#endif

// end of ucci.h

// begin of x86asm.h

#ifndef X86ASM_H
#define X86ASM_H

inline uint32_t LOW_LONG(uint64_t Operand) {
    return (uint32_t) Operand;
}

inline uint32_t HIGH_LONG(uint64_t Operand) {
    return (uint32_t) (Operand >> 32);
}

inline uint64_t MAKE_LONG_LONG(uint32_t LowLong, uint32_t HighLong) {
    return (uint64_t) LowLong | ((uint64_t) HighLong << 32);
}

#if defined __arm__ || defined __mips__

static int cnBitScanTable[64] = {
  32,  0,  1, 12,  2,  6, -1, 13,  3, -1,  7, -1, -1, -1, -1, 14,
  10,  4, -1, -1,  8, -1, -1, 25, -1, -1, -1, -1, -1, 21, 27, 15,
  31, 11,  5, -1, -1, -1, -1, -1,  9, -1, -1, 24, -1, -1, 20, 26,
  30, -1, -1, -1, -1, 23, -1, 19, 29, -1, 22, 18, 28, 17, 16, -1
};

inline int BitScan(uint32_t Operand) {
  uint32_t dw = (Operand << 4) + Operand;
  dw += dw << 6;
  dw = (dw << 16) - dw;
  return cnBitScanTable[dw >> 26];
}

inline int Bsf(uint32_t Operand) {
  return BitScan(Operand & -Operand);
}

inline int Bsr(uint32_t Operand) {
  uint32_t dw = Operand | (Operand >> 1);
  dw |= dw >> 2;
  dw |= dw >> 4;
  dw |= dw >> 8;
  dw |= dw >> 16;
  return BitScan(dw - (dw >> 1));
}

#else

#ifdef _MSC_VER

#pragma warning(disable: 4035)

__forceinline int Bsf(uint32_t Operand) {
  __asm {
    bsf eax, Operand;
  }
}

__forceinline int Bsr(uint32_t Operand) {
  __asm {
    bsr eax, Operand;
  }
}

__forceinline uint64_t TimeStampCounter(void) {
  __asm {
    rdtsc;
  }
}

__forceinline uint64_t LongMul(uint32_t Multiplier, uint32_t Multiplicand) {
  __asm {
    mov eax, Multiplier;
    mul Multiplicand;
  }
}

__forceinline uint64_t LongSqr(uint32_t Multiplier) {
  __asm {
    mov eax, Multiplier;
    mul Multiplier;
  }
}

__forceinline uint32_t LongDiv(uint64_t Dividend, uint32_t Divisor) {
  __asm {
    mov eax, dword ptr Dividend[0];
    mov edx, dword ptr Dividend[4];
    div Divisor;
  }
}

__forceinline uint32_t LongMod(uint64_t Dividend, uint32_t Divisor) {
  __asm {
    mov eax, dword ptr Dividend[0];
    mov edx, dword ptr Dividend[4];
    div Divisor;
    mov eax, edx;
  }
}

__forceinline uint32_t LongMulDiv(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
  __asm {
    mov eax, Multiplier;
    mul Multiplicand;
    div Divisor;
  }
}

__forceinline uint32_t LongMulMod(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
  __asm {
    mov eax, Multiplier;
    mul Multiplicand;
    div Divisor;
    mov eax, edx;
  }
}

__forceinline uint32_t Shld(uint32_t HighLong, uint32_t LowLong, uint32_t Count) {
  __asm {
    mov eax, HighLong;
    mov edx, LowLong;
    mov ecx, Count;
    shld eax, edx, cl;
  }
}

__forceinline uint32_t Shrd(uint32_t LowLong, uint32_t HighLong, uint32_t Count) {
  __asm {
    mov eax, LowLong;
    mov edx, HighLong;
    mov ecx, Count;
    shrd eax, edx, cl;
  }
}

#pragma warning(default: 4035)

#else

static __inline__ int Bsf(uint32_t Operand) {
    int eax;
    asm __volatile__ (
    "bsfl %0, %0" "\n\t"
    : "=a" (eax)
    : "0" (Operand)
    );
    return eax;
}

static __inline__ int Bsr(uint32_t Operand) {
    int eax;
    asm __volatile__ (
    "bsrl %0, %0" "\n\t"
    : "=a" (eax)
    : "0" (Operand)
    );
    return eax;
}

static __inline__ uint64_t TimeStampCounter(void) {
    uint32_t eax, edx;
    asm __volatile__ (
    "rdtsc" "\n\t"
    : "=a" (eax), "=d" (edx)
    :
    );
    return MAKE_LONG_LONG(eax, edx);
}

static __inline__ uint64_t LongMul(uint32_t Multiplier, uint32_t Multiplicand) {
    uint32_t eax, edx;
    asm __volatile__ (
    "mull %1" "\n\t"
    : "=a" (eax), "=d" (edx)
    : "0" (Multiplier), "1" (Multiplicand)
    );
    return MAKE_LONG_LONG(eax, edx);
}

static __inline__ uint64_t LongSqr(uint32_t Multiplier) {
    uint32_t eax, edx;
    asm __volatile__ (
    "mull %1" "\n\t"
    : "=a" (eax), "=d" (edx)
    : "0" (Multiplier), "1" (Multiplier)
    );
    return MAKE_LONG_LONG(eax, edx);
}

static __inline__ uint32_t LongDiv(uint64_t Dividend, uint32_t Divisor) {
    uint32_t eax, edx, dummy;
    asm __volatile__ (
    "divl %2" "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (LOW_LONG(Dividend)), "1" (HIGH_LONG(Dividend)), "2" (Divisor)
    );
    return eax;
}

static __inline__ uint32_t LongMod(uint64_t Dividend, uint32_t Divisor) {
    uint32_t eax, edx, dummy;
    asm __volatile__ (
    "divl %2"     "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (LOW_LONG(Dividend)), "1" (HIGH_LONG(Dividend)), "2" (Divisor)
    );
    return edx;
}

static __inline__ uint32_t LongMulDiv(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
    uint32_t eax, edx, dummy;
    asm __volatile__ (
    "mull %1" "\n\t"
    "divl %2" "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (Multiplier), "1" (Multiplicand), "2" (Divisor)
    );
    return eax;
}

static __inline__ uint32_t LongMulMod(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
    uint32_t eax, edx, dummy;
    asm __volatile__ (
    "mull %1"     "\n\t"
    "divl %2"     "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (Multiplier), "1" (Multiplicand), "2" (Divisor)
    );
    return edx;
}

static __inline uint32_t Shld(uint32_t High, uint32_t Low, uint32_t Count) {
    uint32_t eax, edx, ecx;
    asm __volatile__ (
    "shldl %%cl, %1, %0" "\n\t"
    : "=a" (eax), "=d" (edx), "=c" (ecx)
    : "0" (High), "1" (Low), "2" (Count)
    );
    return eax;
}

static __inline uint32_t Shrd(uint32_t Low, uint32_t High, uint32_t Count) {
    uint32_t eax, edx, ecx;
    asm __volatile__ (
    "shrdl %%cl, %1, %0" "\n\t"
    : "=a" (eax), "=d" (edx), "=c" (ecx)
    : "0" (Low), "1" (High), "2" (Count)
    );
    return eax;
}

#endif

inline uint64_t LongShl(uint64_t Operand, uint32_t Count) {
    if (Count < 32) {
        return MAKE_LONG_LONG(LOW_LONG(Operand) << Count, Shld(HIGH_LONG(Operand), LOW_LONG(Operand), Count));
    } else if (Count < 64) {
        return MAKE_LONG_LONG(0, LOW_LONG(Operand) << (Count - 32));
    } else {
        return MAKE_LONG_LONG(0, 0);
    }
}

inline uint64_t LongShr(uint64_t Operand, uint32_t Count) {
    if (Count < 32) {
        return MAKE_LONG_LONG(Shrd(LOW_LONG(Operand), HIGH_LONG(Operand), Count), HIGH_LONG(Operand) >> Count);
    } else if (Count < 64) {
        return MAKE_LONG_LONG(HIGH_LONG(Operand) >> (Count - 32), 0);
    } else {
        return MAKE_LONG_LONG(0, 0);
    }
}

#endif

#endif

// end of x86asm.h

// begin of rc4prng.h

#ifndef RC4PRNG_H
#define RC4PRNG_H

struct RC4Struct {
    uint8_t s[256];
    int x, y;

    void Init(void *lpKey, int nKeyLen) {
        int i, j;
        x = y = j = 0;
        for (i = 0; i < 256; i ++) {
            s[i] = i;
        }
        for (i = 0; i < 256; i ++) {
            j = (j + s[i] + ((uint8_t *) lpKey)[i % nKeyLen]) & 255;
            SWAP(s[i], s[j]);
        }
    }

    void InitZero(void) {
        uint32_t dwKey;
        dwKey = 0;
        Init(&dwKey, 4);
    }

    void InitRand(void) {
        union {
            uint32_t dw[2];
            uint64_t qw;
        } Seed;
        timeb tb;
        ftime(&tb);
#if defined __arm__ || defined __mips__
        Seed.qw = 0;
#else
        Seed.qw = TimeStampCounter();
#endif
        Seed.dw[1] ^= (uint32_t) GetTime();
        Init(&Seed, 8);
    }

    uint8_t NextByte(void) {
        x = (x + 1) & 255;
        y = (y + s[x]) & 255;
        SWAP(s[x], s[y]);
        return s[(s[x] + s[y]) & 255];
    }

    uint32_t NextLong(void) {
        union {
            uint8_t uc[4];
            uint32_t dw;
        } Ret;
        Ret.uc[0] = NextByte();
        Ret.uc[1] = NextByte();
        Ret.uc[2] = NextByte();
        Ret.uc[3] = NextByte();
        return Ret.dw;
    }
};

#endif

// end of rc4prng.h

// begin of pregen.h

/*
pregen.h/pregen.cpp - Source Code for ElephantEye, Part II

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.0, Last Modified: Nov. 2007
Copyright (C) 2004-2007 www.elephantbase.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PREGEN_H
#define PREGEN_H

#define __ASSERT_PIECE(pc) __ASSERT((pc) >= 16 && (pc) <= 47)
#define __ASSERT_SQUARE(sq) __ASSERT(IN_BOARD(sq))
#define __ASSERT_BITRANK(w) __ASSERT((w) < (1 << 9))
#define __ASSERT_BITFILE(w) __ASSERT((w) < (1 << 10))

const int RANK_TOP = 3;
const int RANK_BOTTOM = 12;
const int FILE_LEFT = 3;
const int FILE_CENTER = 7;
const int FILE_RIGHT = 11;

extern const bool cbcInBoard[256];    // 棋盘区域表
extern const bool cbcInFort[256];     // 城池区域表
extern const bool cbcCanPromote[256]; // 升变区域表
extern const int8_t ccLegalSpanTab[512];   // 合理着法跨度表
extern const int8_t ccKnightPinTab[512];   // 马腿表

inline bool IN_BOARD(int sq) {
    return cbcInBoard[sq];
}

inline bool IN_FORT(int sq) {
    return cbcInFort[sq];
}

inline bool CAN_PROMOTE(int sq) {
    return cbcCanPromote[sq];
}

inline int8_t LEGAL_SPAN_TAB(int nDisp) {
    return ccLegalSpanTab[nDisp];
}

inline int8_t KNIGHT_PIN_TAB(int nDisp) {
    return ccKnightPinTab[nDisp];
}

inline int RANK_Y(int sq) {
    return sq >> 4;
}

inline int FILE_X(int sq) {
    return sq & 15;
}

inline int COORD_XY(int x, int y) {
    return x + (y << 4);
}

inline int SQUARE_FLIP(int sq) {
    return 254 - sq;
}

inline int FILE_FLIP(int x) {
    return 14 - x;
}

inline int RANK_FLIP(int y) {
    return 15 - y;
}

inline int OPP_SIDE(int sd) {
    return 1 - sd;
}

inline int SQUARE_FORWARD(int sq, int sd) {
    return sq - 16 + (sd << 5);
}

inline int SQUARE_BACKWARD(int sq, int sd) {
    return sq + 16 - (sd << 5);
}

inline bool KING_SPAN(int sqSrc, int sqDst) {
    return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 1;
}

inline bool ADVISOR_SPAN(int sqSrc, int sqDst) {
    return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 2;
}

inline bool BISHOP_SPAN(int sqSrc, int sqDst) {
    return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 3;
}

inline int BISHOP_PIN(int sqSrc, int sqDst) {
    return (sqSrc + sqDst) >> 1;
}

inline int KNIGHT_PIN(int sqSrc, int sqDst) {
    return sqSrc + KNIGHT_PIN_TAB(sqDst - sqSrc + 256);
}

inline bool WHITE_HALF(int sq) {
    return (sq & 0x80) != 0;
}

inline bool BLACK_HALF(int sq) {
    return (sq & 0x80) == 0;
}

inline bool HOME_HALF(int sq, int sd) {
    return (sq & 0x80) != (sd << 7);
}

inline bool AWAY_HALF(int sq, int sd) {
    return (sq & 0x80) == (sd << 7);
}

inline bool SAME_HALF(int sqSrc, int sqDst) {
    return ((sqSrc ^ sqDst) & 0x80) == 0;
}

inline bool DIFF_HALF(int sqSrc, int sqDst) {
    return ((sqSrc ^ sqDst) & 0x80) != 0;
}

inline int RANK_DISP(int y) {
    return y << 4;
}

inline int FILE_DISP(int x) {
    return x;
}

// 借助“位行”和“位列”生成车炮着法的预置结构
struct SlideMoveStruct {
    uint8_t ucNonCap[2];    // 不吃子能走到的最大一格/最小一格
    uint8_t ucRookCap[2];   // 车吃子能走到的最大一格/最小一格
    uint8_t ucCannonCap[2]; // 炮吃子能走到的最大一格/最小一格
    uint8_t ucSuperCap[2];  // 超级炮(隔两子吃子)能走到的最大一格/最小一格
}; // smv

// 借助“位行”和“位列”判断车炮着法合理性的预置结构
struct SlideMaskStruct {
    uint16_t wNonCap, wRookCap, wCannonCap, wSuperCap;
}; // sms

struct ZobristStruct {
    uint32_t dwKey, dwLock0, dwLock1;
    void InitZero(void) {
        dwKey = dwLock0 = dwLock1 = 0;
    }
    void InitRC4(RC4Struct &rc4) {
        dwKey = rc4.NextLong();
        dwLock0 = rc4.NextLong();
        dwLock1 = rc4.NextLong();
    }
    void Xor(const ZobristStruct &zobr) {
        dwKey ^= zobr.dwKey;
        dwLock0 ^= zobr.dwLock0;
        dwLock1 ^= zobr.dwLock1;
    }
    void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
        dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
        dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
        dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
    }
}; // zobr

extern struct PreGenStruct {
    // Zobrist键值表，分Zobrist键值和Zobrist校验锁两部分
    ZobristStruct zobrPlayer;
    ZobristStruct zobrTable[14][256];

    uint16_t wBitRankMask[256]; // 每个格子的位行的屏蔽位
    uint16_t wBitFileMask[256]; // 每个格子的位列的屏蔽位

    /* 借助“位行”和“位列”生成车炮着法和判断车炮着法合理性的预置数组
     *
     * “位行”和“位列”技术是ElephantEye的核心技术，用来处理车和炮的着法生成、将军判断和局面分析。
     * 以初始局面红方右边的炮在该列的行动为例，首先必须知道该列的“位列”，即"1010000101b"，
     * ElephantEye有两种预置数组，即"...MoveTab"和"...MaskTab"，用法分别是：
     * 一、如果要知道该子向前吃子的目标格(起始格是2，目标格是9)，那么希望查表就能知道这个格子，
     * 　　预先生成一个数组"FileMoveTab_CannonCap[10][1024]"，使得"FileMoveTab_CannonCap[2][1010000101b] == 9"就可以了。
     * 二、如果要判断该子能否吃到目标格(同样以起始格是2，目标格是9为例)，那么需要知道目标格的位列，即"0000000001b"，
     * 　　只要把"...MoveTab"的格子以“屏蔽位”的形式重新记作数组"...MaskTab"就可以了，用“与”操作来判断能否吃到目标格，
     * 　　通常一个"...MaskTab"单元会包括多个屏蔽位，判断能否吃到同行或同列的某个格子时，只需要做一次判断就可以了。
     */
    SlideMoveStruct smvRankMoveTab[9][512];   // 36,864 字节
    SlideMoveStruct smvFileMoveTab[10][1024]; // 81,920 字节
    SlideMaskStruct smsRankMaskTab[9][512];   // 36,864 字节
    SlideMaskStruct smsFileMaskTab[10][1024]; // 81,920 字节
    // 共计:  237,568 字节

    /* 其余棋子(不适合用“位行”和“位列”)的着法预生成数组
     *
     * 这部分数组是真正意义上的“着法预生成”数组，可以根据某个棋子的起始格直接查数组，得到所有的目标格。
     * 使用数组时，可以根据起始格来确定一个指针"g_...Moves[Square]"，这个指针指向一系列目标格，以0结束。
     * 为了对齐地址，数组[256][n]中n总是4的倍数，而且必须大于n(因为数组包括了结束标识符0)，除了象眼和马腿数组。
     */
    uint8_t ucsqKingMoves[256][8];
    uint8_t ucsqAdvisorMoves[256][8];
    uint8_t ucsqBishopMoves[256][8];
    uint8_t ucsqBishopPins[256][4];
    uint8_t ucsqKnightMoves[256][12];
    uint8_t ucsqKnightPins[256][8];
    uint8_t ucsqPawnMoves[2][256][4];
} PreGen;

// 局面预评价结构
extern struct PreEvalStruct {
    bool bPromotion;
    int vlAdvanced;
    uint8_t ucvlWhitePieces[7][256];
    uint8_t ucvlBlackPieces[7][256];
} PreEval;

void PreGenInit(void);

#endif

// end of pregen.h

// begin of position.h

/*
position.h/position.cpp - Source Code for ElephantEye, Part III

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上四个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

#ifndef POSITION_H
#define POSITION_H

const int MAX_MOVE_NUM = 1024;  // 局面能容纳的回滚着法数
const int MAX_GEN_MOVES = 128;  // 搜索的最大着法数，中国象棋的任何局面都不会超过120个着法
const int DRAW_MOVES = 100;     // 默认的和棋着法数，ElephantEye设定在50回合即100步，但将军和应将不计入其中
const int REP_HASH_MASK = 4095; // 判断重复局面的迷你置换表长度，即4096个表项

const int MATE_VALUE = 10000;           // 最高分值，即将死的分值
const int BAN_VALUE = MATE_VALUE - 100; // 长将判负的分值，低于该值将不写入置换表(参阅"hash.cpp")
const int WIN_VALUE = MATE_VALUE - 200; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int NULLOKAY_MARGIN = 200;        // 空着裁剪可以不检验的子力价值边界
const int NULLSAFE_MARGIN = 400;        // 允许使用空着裁剪的条件的子力价值边界
const int DRAW_VALUE = 20;              // 和棋时返回的分数(取负值)

const bool CHECK_LAZY = true;   // 偷懒检测将军
const int CHECK_MULTI = 48;     // 被多个子将军

// 每种子力的类型编号
const int KING_TYPE = 0;
const int ADVISOR_TYPE = 1;
const int BISHOP_TYPE = 2;
const int KNIGHT_TYPE = 3;
const int ROOK_TYPE = 4;
const int CANNON_TYPE = 5;
const int PAWN_TYPE = 6;

// 每种子力的开始序号和结束序号
const int KING_FROM = 0;
const int ADVISOR_FROM = 1;
const int ADVISOR_TO = 2;
const int BISHOP_FROM = 3;
const int BISHOP_TO = 4;
const int KNIGHT_FROM = 5;
const int KNIGHT_TO = 6;
const int ROOK_FROM = 7;
const int ROOK_TO = 8;
const int CANNON_FROM = 9;
const int CANNON_TO = 10;
const int PAWN_FROM = 11;
const int PAWN_TO = 15;

// 各种子力的屏蔽位
const int KING_BITPIECE = 1 << KING_FROM;
const int ADVISOR_BITPIECE = (1 << ADVISOR_FROM) | (1 << ADVISOR_TO);
const int BISHOP_BITPIECE = (1 << BISHOP_FROM) | (1 << BISHOP_TO);
const int KNIGHT_BITPIECE = (1 << KNIGHT_FROM) | (1 << KNIGHT_TO);
const int ROOK_BITPIECE = (1 << ROOK_FROM) | (1 << ROOK_TO);
const int CANNON_BITPIECE = (1 << CANNON_FROM) | (1 << CANNON_TO);
const int PAWN_BITPIECE = (1 << PAWN_FROM) | (1 << (PAWN_FROM + 1)) |
                          (1 << (PAWN_FROM + 2)) | (1 << (PAWN_FROM + 3)) | (1 << PAWN_TO);
const int ATTACK_BITPIECE = KNIGHT_BITPIECE | ROOK_BITPIECE | CANNON_BITPIECE | PAWN_BITPIECE;

inline uint32_t BIT_PIECE(int pc) {
    return 1 << (pc - 16);
}

inline uint32_t WHITE_BITPIECE(int nBitPiece) {
    return nBitPiece;
}

inline uint32_t BLACK_BITPIECE(int nBitPiece) {
    return nBitPiece << 16;
}

inline uint32_t BOTH_BITPIECE(int nBitPiece) {
    return nBitPiece + (nBitPiece << 16);
}

// "RepStatus()"返回的重复标记位
const int REP_NONE = 0;
const int REP_DRAW = 1;
const int REP_LOSS = 3;
const int REP_WIN = 5;

/* ElephantEye的很多代码中都用到"SIDE_TAG()"这个量，红方设为16，黑方设为32。
 * 用"SIDE_TAG() + i"可以方便地选择棋子的类型，"i"从0到15依次是：
 * 帅仕仕相相马马车车炮炮兵兵兵兵兵(将士士象象马马车车炮炮卒卒卒卒卒)
 * 例如"i"取"KNIGHT_FROM"到"KNIGHT_TO"，则表示依次检查两个马的位置
 */
inline int SIDE_TAG(int sd) {
    return 16 + (sd << 4);
}

inline int OPP_SIDE_TAG(int sd) {
    return 32 - (sd << 4);
}

inline int SIDE_VALUE(int sd, int vl) {
    return (sd == 0 ? vl : -vl);
}

inline int PIECE_INDEX(int pc) {
    return pc & 15;
}

extern const char *const cszStartFen;     // 起始局面的FEN串
extern const char *const cszPieceBytes;   // 棋子类型对应的棋子符号
extern const int cnPieceTypes[48];        // 棋子序号对应的棋子类型
extern const int cnSimpleValues[48];      // 棋子的简单分值
extern const uint8_t cucsqMirrorTab[256]; // 坐标的镜像(左右对称)数组

inline char PIECE_BYTE(int pt) {
    return cszPieceBytes[pt];
}

inline int PIECE_TYPE(int pc) {
    return cnPieceTypes[pc];
}

inline int SIMPLE_VALUE(int pc) {
    return cnSimpleValues[pc];
}

inline uint8_t SQUARE_MIRROR(int sq) {
    return cucsqMirrorTab[sq];
}

// FEN串中棋子标识
int FenPiece(int Arg);

// 复杂着法结构
union MoveStruct {
    uint32_t dwmv;           // 填满整个结构用
    struct {
        uint16_t wmv, wvl;     // 着法和分值
    };
    struct {
        uint8_t Src, Dst;      // 起始格和目标格
        int8_t CptDrw, ChkChs; // 被吃子(+)/和棋着法数(-)、将军子(+)/被捉子(-)
    };
}; // mvs

// 着法结构
inline int SRC(int mv) { // 得到着法的起点
    return mv & 255;
}

inline int DST(int mv) { // 得到着法的终点
    return mv >> 8;
}

inline int MOVE(int sqSrc, int sqDst) {   // 由起点和终点得到着法
    return sqSrc + (sqDst << 8);
}

inline uint32_t MOVE_COORD(int mv) {      // 把着法转换成字符串
    union {
        char c[4];
        uint32_t dw;
    } Ret;
    Ret.c[0] = FILE_X(SRC(mv)) - FILE_LEFT + 'a';
    Ret.c[1] = '9' - RANK_Y(SRC(mv)) + RANK_TOP;
    Ret.c[2] = FILE_X(DST(mv)) - FILE_LEFT + 'a';
    Ret.c[3] = '9' - RANK_Y(DST(mv)) + RANK_TOP;
    // 断言输出着法的合理性
    __ASSERT_BOUND('a', Ret.c[0], 'i');
    __ASSERT_BOUND('0', Ret.c[1], '9');
    __ASSERT_BOUND('a', Ret.c[2], 'i');
    __ASSERT_BOUND('0', Ret.c[3], '9');
    return Ret.dw;
}

inline int COORD_MOVE(uint32_t dwMoveStr) { // 把字符串转换成着法
    int sqSrc, sqDst;
    char *lpArgPtr;
    lpArgPtr = (char *) &dwMoveStr;
    sqSrc = COORD_XY(lpArgPtr[0] - 'a' + FILE_LEFT, '9' - lpArgPtr[1] + RANK_TOP);
    sqDst = COORD_XY(lpArgPtr[2] - 'a' + FILE_LEFT, '9' - lpArgPtr[3] + RANK_TOP);
    // 对输入着法的合理性不作断言
    // __ASSERT_SQUARE(sqSrc);
    // __ASSERT_SQUARE(sqDst);
    return (IN_BOARD(sqSrc) && IN_BOARD(sqDst) ? MOVE(sqSrc, sqDst) : 0);
}

inline int MOVE_MIRROR(int mv) {          // 对着法做镜像
    return MOVE(SQUARE_MIRROR(SRC(mv)), SQUARE_MIRROR(DST(mv)));
}

// 回滚结构
struct RollbackStruct {
    ZobristStruct zobr;   // Zobrist
    int vlWhite, vlBlack; // 红方和黑方的子力价值
    MoveStruct mvs;       // 着法
}; // rbs

const bool DEL_PIECE = true; // 函数"PositionStruct::AddPiece()"的选项

// 局面结构
struct PositionStruct {
    // 基本成员
    int sdPlayer;             // 轮到哪方走，0表示红方，1表示黑方
    uint8_t ucpcSquares[256]; // 每个格子放的棋子，0表示没有棋子
    uint8_t ucsqPieces[48];   // 每个棋子放的位置，0表示被吃
    ZobristStruct zobr;       // Zobrist

    // 位结构成员，用来增强棋盘的处理
    union {
        uint32_t dwBitPiece;    // 32位的棋子位，0到31位依次表示序号为16到47的棋子是否还在棋盘上
        uint16_t wBitPiece[2];  // 拆分成两个
    };
    uint16_t wBitRanks[16];   // 位行数组，注意用法是"wBitRanks[RANK_Y(sq)]"
    uint16_t wBitFiles[16];   // 位列数组，注意用法是"wBitFiles[FILE_X(sq)]"

    // 局面评价数据
    int vlWhite, vlBlack;   // 红方和黑方的子力价值

    // 回滚着法，用来检测循环局面
    int nMoveNum, nDistance;              // 回滚着法数和搜索深度
    RollbackStruct rbsList[MAX_MOVE_NUM]; // 回滚列表
    uint8_t ucRepHash[REP_HASH_MASK + 1]; // 判断重复局面的迷你置换表

    // 获取着法预生成信息
    SlideMoveStruct *RankMovePtr(int x, int y) const {
        return PreGen.smvRankMoveTab[x - FILE_LEFT] + wBitRanks[y];
    }
    SlideMoveStruct *FileMovePtr(int x, int y) const {
        return PreGen.smvFileMoveTab[y - RANK_TOP] + wBitFiles[x];
    }
    SlideMaskStruct *RankMaskPtr(int x, int y) const {
        return PreGen.smsRankMaskTab[x - FILE_LEFT] + wBitRanks[y];
    }
    SlideMaskStruct *FileMaskPtr(int x, int y) const {
        return PreGen.smsFileMaskTab[y - RANK_TOP] + wBitFiles[x];
    }

    // 棋盘处理过程
    void ClearBoard(void) { // 棋盘初始化
        sdPlayer = 0;
        memset(ucpcSquares, 0, 256);
        memset(ucsqPieces, 0, 48);
        zobr.InitZero();
        dwBitPiece = 0;
        memset(wBitRanks, 0, 16 * sizeof(uint16_t));
        memset(wBitFiles, 0, 16 * sizeof(uint16_t));
        vlWhite = vlBlack = 0;
        // "ClearBoard()"后面紧跟的是"SetIrrev()"，来初始化其它成员
    }
    void ChangeSide(void) { // 交换走棋方
        sdPlayer = OPP_SIDE(sdPlayer);
        zobr.Xor(PreGen.zobrPlayer);
    }
    void SaveStatus(void) { // 保存状态
        RollbackStruct *lprbs;
        lprbs = rbsList + nMoveNum;
        lprbs->zobr = zobr;
        lprbs->vlWhite = vlWhite;
        lprbs->vlBlack = vlBlack;
    }
    void Rollback(void) {   // 回滚
        RollbackStruct *lprbs;
        lprbs = rbsList + nMoveNum;
        zobr = lprbs->zobr;
        vlWhite = lprbs->vlWhite;
        vlBlack = lprbs->vlBlack;
    }
    void AddPiece(int sq, int pc, bool bDel = false); // 棋盘上增加棋子
    int MovePiece(int mv);                            // 移动棋子
    void UndoMovePiece(int mv, int pcCaptured);       // 撤消移动棋子
    int Promote(int sq);                              // 升变
    void UndoPromote(int sq, int pcCaptured);         // 撤消升变

    // 着法处理过程
    bool MakeMove(int mv);   // 执行一个着法
    void UndoMakeMove(void); // 撤消一个着法
    void NullMove(void);     // 执行一个空着
    void UndoNullMove(void); // 撤消一个空着
    void SetIrrev(void) {    // 把局面设成“不可逆”，即清除回滚着法
        rbsList[0].mvs.dwmv = 0; // wmv, Chk, CptDrw, ChkChs = 0
        rbsList[0].mvs.ChkChs = CheckedBy();
        nMoveNum = 1;
        nDistance = 0;
        memset(ucRepHash, 0, REP_HASH_MASK + 1);
    }

    // 局面处理过程
    void FromFen(const char *szFen); // FEN串识别
    void ToFen(char *szFen) const;   // 生成FEN串
    void Mirror(void);               // 局面镜像

    // 着法检测过程
    bool GoodCap(int mv) const {     // 好的吃子着法检测，这样的着法不记录到历史表和杀手着法表中
        int pcMoved, pcCaptured;
        pcCaptured = ucpcSquares[DST(mv)];
        if (pcCaptured == 0) {
            return false;
        }
        if (!Protected(OPP_SIDE(sdPlayer), DST(mv))) {
            return true;
        }
        pcMoved = ucpcSquares[SRC(mv)];
        return SIMPLE_VALUE(pcCaptured) > SIMPLE_VALUE(pcMoved);
    }
    bool LegalMove(int mv) const;            // 着法合理性检测，仅用在“杀手着法”的检测中
    int CheckedBy(bool bLazy = false) const; // 被哪个子将军
    bool IsMate(void);                       // 判断是已被将死
    MoveStruct LastMove(void) const {        // 前一步着法，该着法保存了局面的将军状态
        return rbsList[nMoveNum - 1].mvs;
    }
    bool CanPromote(void) const {            // 判断是否能升变
        return (wBitPiece[sdPlayer] & PAWN_BITPIECE) != PAWN_BITPIECE && LastMove().ChkChs <= 0;
    }
    bool NullOkay(void) const {              // 允许使用空着裁剪的条件
        return (sdPlayer == 0 ? vlWhite : vlBlack) > NULLOKAY_MARGIN;
    }
    bool NullSafe(void) const {              // 空着裁剪可以不检验的条件
        return (sdPlayer == 0 ? vlWhite : vlBlack) > NULLSAFE_MARGIN;
    }
    bool IsDraw(void) const {                // 和棋判断
        return (!PreEval.bPromotion && (dwBitPiece & BOTH_BITPIECE(ATTACK_BITPIECE)) == 0) ||
               -LastMove().CptDrw >= DRAW_MOVES || nMoveNum == MAX_MOVE_NUM;
    }
    int RepStatus(int nRecur = 1) const;     // 重复局面检测
    int DrawValue(void) const {              // 和棋的分值
        return (nDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
    }
    int RepValue(int vlRep) const {          // 重复局面的分值
        // return vlRep == REP_LOSS ? nDistance - MATE_VALUE : vlRep == REP_WIN ? MATE_VALUE - nDistance : DrawValue();
        // 长将判负的分值，低于BAN_VALUE将不写入置换表(参阅"hash.cpp")
        return vlRep == REP_LOSS ? nDistance - BAN_VALUE : vlRep == REP_WIN ? BAN_VALUE - nDistance : DrawValue();
    }
    int Material(void) const {               // 子力平衡，包括先行权因素
        return SIDE_VALUE(sdPlayer, vlWhite - vlBlack) + PreEval.vlAdvanced;
    }

    // 着法生成过程，由于这些过程代码量特别大，所以把他们都集中在"genmoves.cpp"中
    bool Protected(int sd, int sqSrc, int sqExcept = 0) const; // 棋子保护判断
    int ChasedBy(int mv) const;                                // 捉哪个子
    int MvvLva(int sqDst, int pcCaptured, int nLva) const;     // 计算MVV(LVA)值
    int GenCapMoves(MoveStruct *lpmvs) const;                  // 吃子着法生成器
    int GenNonCapMoves(MoveStruct *lpmvs) const;               // 不吃子着法生成器
    int GenAllMoves(MoveStruct *lpmvs) const {                 // 全部着法生成器
        int nCapNum;
        nCapNum = GenCapMoves(lpmvs);
        return nCapNum + GenNonCapMoves(lpmvs + nCapNum);
    }

    // 着法生成过程，由于这些过程代码量特别大，所以把他们都集中在"preeval.cpp"和"evaluate.cpp"中
    void PreEvaluate(void);
    int AdvisorShape(void) const;
    int StringHold(void) const;
    int RookMobility(void) const;
    int KnightTrap(void) const;
    int Evaluate(int vlAlpha, int vlBeta) const;
}; // pos

#endif

// end of position.h

// begin of hash.h

/*
hash.h/hash.cpp - Source Code for ElephantEye, Part V

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.31, Last Modified: May 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>

#ifndef HASH_H
#define HASH_H

// 置换表标志，只用在"RecordHash()"函数中
const int HASH_BETA = 1;
const int HASH_ALPHA = 2;
const int HASH_PV = HASH_ALPHA | HASH_BETA;

const int HASH_LAYERS = 2;   // 置换表的层数
const int NULL_DEPTH = 2;    // 空着裁剪的深度

// 置换表结构，置换表信息夹在两个Zobrist校验锁中间，可以防止存取冲突
struct HashStruct {
    uint32_t dwZobristLock0;           // Zobrist校验锁，第一部分
    uint16_t wmv;                      // 最佳着法
    uint8_t ucAlphaDepth, ucBetaDepth; // 深度(上边界和下边界)
    int16_t svlAlpha, svlBeta;         // 分值(上边界和下边界)
    uint32_t dwZobristLock1;           // Zobrist校验锁，第二部分
}; // hsh

// 置换表信息
extern int nHashMask;              // 置换表的大小
extern HashStruct *hshItems;       // 置换表的指针，ElephantEye采用多层的置换表
#ifdef HASH_QUIESC
extern HashStruct *hshItemsQ;
#endif

inline void ClearHash(void) {         // 清空置换表
    memset(hshItems, 0, (nHashMask + 1) * sizeof(HashStruct));
#ifdef HASH_QUIESC
    memset(hshItemsQ, 0, (nHashMask + 1) * sizeof(HashStruct));
#endif
}

inline void NewHash(int nHashScale) { // 分配置换表，大小是 2^nHashScale 字节
    nHashMask = ((1 << nHashScale) / sizeof(HashStruct)) - 1;
    hshItems = new HashStruct[nHashMask + 1];
#ifdef HASH_QUIESC
    hshItemsQ = new HashStruct[nHashMask + 1];
#endif
    ClearHash();
}

inline void DelHash(void) {           // 释放置换表
    delete[] hshItems;
#ifdef HASH_QUIESC
    delete[] hshItemsQ;
#endif
}

// 判断置换表是否符合局面(Zobrist锁是否相等)
inline bool HASH_POS_EQUAL(const HashStruct &hsh, const PositionStruct &pos) {
    return hsh.dwZobristLock0 == pos.zobr.dwLock0 && hsh.dwZobristLock1 == pos.zobr.dwLock1;
}

// 按局面和层数获取置换表项(返回一个引用，可以对其赋值)
inline HashStruct &HASH_ITEM(const PositionStruct &pos, int nLayer) {
    return hshItems[(pos.zobr.dwKey + nLayer) & nHashMask];
}

// 置换表的管理过程
void RecordHash(const PositionStruct &pos, int nFlag, int vl, int nDepth, int mv);                    // 存储置换表局面信息
int ProbeHash(const PositionStruct &pos, int vlAlpha, int vlBeta, int nDepth, bool bNoNull, int &mv); // 获取置换表局面信息
#ifdef HASH_QUIESC
void RecordHashQ(const PositionStruct &pos, int vlBeta, int vlAlpha); // 存储置换表局面信息(静态搜索)
  int ProbeHashQ(const PositionStruct &pos, int vlAlpha, int vlBeta);   // 获取置换表局面信息(静态搜索)
#endif

#ifndef CCHESS_A3800
// UCCI支持 - 输出Hash表中的局面信息
bool PopHash(const PositionStruct &pos);
#endif

#endif

// end of hash.h

// begin of search.h

/*
search.h/search.cpp - Source Code for ElephantEye, Part VIII

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.31, Last Modified: May 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef CCHESS_A3800
//#include "ucci.h"
#endif

#include "jsoncpp/json.h"
#include <iostream>

#ifndef SEARCH_H
#define SEARCH_H

// 搜索模式
const int GO_MODE_INFINITY = 0;
const int GO_MODE_NODES = 1;
const int GO_MODE_TIMER = 2;

// 搜索前可设置的全局变量，指定搜索参数
struct SearchStruct {
    PositionStruct pos;                // 有待搜索的局面
    bool bQuit, bPonder, bDraw;        // 是否收到退出指令、后台思考模式和提和模式
    bool bBatch, bDebug;               // 是否批处理模式和调试模式
    bool bUseHash, bUseBook;           // 是否使用置换表裁剪和开局库
    bool bNullMove, bKnowledge;        // 是否空着裁剪和使用局面评价知识
    bool bIdle;                        // 是否空闲
    RC4Struct rc4Random;               // 随机数
    int nGoMode, nNodes, nCountMask;   // 搜索模式、结点数和
    int nProperTimer, nMaxTimer;       // 计划使用时间
    int nRandomMask, nBanMoves;        // 随机性屏蔽位和禁着数
    uint16_t wmvBanList[MAX_MOVE_NUM]; // 禁着列表
    char szBookFile[1024];             // 开局库
#ifdef CCHESS_A3800
    int mvResult;                      // 返回着法
#endif
};

extern SearchStruct Search;

#ifndef CCHESS_A3800

// UCCI局面构造过程
void BuildPos(PositionStruct &pos, const UcciCommStruct &UcciComm);

// UCCI支持 - 输出叶子结点的局面信息
void PopLeaf(PositionStruct &pos);

#endif

// 搜索的启动过程
void SearchMain(int nDepth);

// 搜索的启动过程
void SearchMainBotzone(int nDepth);

void OutputBotzoneNone();
#endif

// end of search.h

// begin of book.h

/*
book.h/book.cpp - Source Code for ElephantEye, Part VI

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.25, Last Modified: Apr. 2011
Copyright (C) 2004-2011 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef BOOK_H
#define BOOK_H

#include <stdio.h>

struct BookStruct {
    union {
        uint32_t dwZobristLock;
        int nPtr;
    };
    uint16_t wmv, wvl;
}; // bk

inline int BOOK_POS_CMP(const BookStruct &bk, const PositionStruct &pos) {
    return bk.dwZobristLock < pos.zobr.dwLock1 ? -1 :
           bk.dwZobristLock > pos.zobr.dwLock1 ? 1 : 0;
}

struct BookFileStruct {
    FILE *fp;
    int nLen;
    bool Open(const char *szFileName, bool bEdit = false) {
        fp = fopen(szFileName, bEdit ? "r+b" : "rb");
        if (fp == NULL) {
            return false;
        } else {
            fseek(fp, 0, SEEK_END);
            nLen = ftell(fp) / sizeof(BookStruct);
            return true;
        }
    }
    void Close(void) const {
        fclose(fp);
    }
    void Read(BookStruct &bk, int nPtr) const {
        fseek(fp, nPtr * sizeof(BookStruct), SEEK_SET);
        fread(&bk, sizeof(BookStruct), 1, fp);
    }
    void Write(const BookStruct &bk, int nPtr) const {
        fseek(fp, nPtr * sizeof(BookStruct), SEEK_SET);
        fwrite(&bk, sizeof(BookStruct), 1, fp);
    }
};

// 获取开局库着法
int GetBookMoves(const PositionStruct &pos, const char *szBookFile, BookStruct *lpbks);

#endif

// end of book.h

// begin of movesort.h

/*
movesort.h/movesort.cpp - Source Code for ElephantEye, Part VII

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.11, Last Modified: Dec. 2007
Copyright (C) 2004-2007 www.elephantbase.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MOVESORT_H
#define MOVESORT_H

#include <string.h>

const int LIMIT_DEPTH = 64;       // 搜索的极限深度
const int SORT_VALUE_MAX = 65535; // 着法序列最大值

extern const int FIBONACCI_LIST[32];

// "nHistory"只在"movesort.cpp"一个模块中使用
extern int nHistory[65536]; // 历史表

// 着法顺序的若干阶段(参阅"NextFull()"函数)
const int PHASE_HASH = 0;
const int PHASE_GEN_CAP = 1;
const int PHASE_GOODCAP = 2;
const int PHASE_KILLER1 = 3;
const int PHASE_KILLER2 = 4;
const int PHASE_GEN_NONCAP = 5;
const int PHASE_REST = 6;

const bool NEXT_ALL = true;    // 着法顺序函数"MoveSortStruct::NextQuiesc()"选项
const bool ROOT_UNIQUE = true; // 着法顺序函数"MoveSortStruct::ResetRoot()"选项

// 着法序列结构
struct MoveSortStruct {
    int nPhase, nMoveIndex, nMoveNum;
    int mvHash, mvKiller1, mvKiller2;
    MoveStruct mvs[MAX_GEN_MOVES];

    void SetHistory(void); // 根据历史表对着法列表赋值
    void ShellSort(void);  // 着法排序过程
    // 好的吃子着法(包括没有着法，都不更新历史表和杀手着法表)
    bool GoodCap(const PositionStruct &pos, int mv) {
        return mv == 0 || nPhase == PHASE_GOODCAP || (nPhase < PHASE_GOODCAP && pos.GoodCap(mv));
    }

    // 静态搜索的着法顺序控制
    void InitAll(const PositionStruct &pos) {
        nMoveIndex = 0;
        nMoveNum = pos.GenAllMoves(mvs);
        SetHistory();
        ShellSort();
    }
    void InitQuiesc(const PositionStruct &pos) {
        nMoveIndex = 0;
        nMoveNum = pos.GenCapMoves(mvs);
        ShellSort();
    }
    void InitQuiesc2(const PositionStruct &pos) {
        nMoveNum += pos.GenNonCapMoves(mvs);
        SetHistory();
        ShellSort();
    }
    int NextQuiesc(bool bNextAll = false) {
        if (nMoveIndex < nMoveNum && (bNextAll || mvs[nMoveIndex].wvl > 0)) {
            nMoveIndex ++;
            return mvs[nMoveIndex - 1].wmv;
        } else {
            return 0;
        }
    }

    // 完全搜索的着法顺序控制
    void InitFull(const PositionStruct &pos, int mv, const uint16_t *lpwmvKiller) {
        nPhase = PHASE_HASH;
        mvHash = mv;
        mvKiller1 = lpwmvKiller[0];
        mvKiller2 = lpwmvKiller[1];
    }
    int InitEvade(PositionStruct &pos, int mv, const uint16_t *lpwmvKiller);
    int NextFull(const PositionStruct &pos);

    // 根结点着法顺序控制
    void InitRoot(const PositionStruct &pos, int nBanMoves, const uint16_t *lpwmvBanList);
    void ResetRoot(bool bUnique = false) {
        nMoveIndex = 0;
        ShellSort();
        nMoveIndex = (bUnique ? 1 : 0);
    }
    int NextRoot(void) {
        if (nMoveIndex < nMoveNum) {
            nMoveIndex ++;
            return mvs[nMoveIndex - 1].wmv;
        } else {
            return 0;
        }
    }
    void UpdateRoot(int mv);
};

// 清空历史表
inline void ClearHistory(void) {
    memset(nHistory, 0, sizeof(int[65536]));
}

// 清空杀手着法表
inline void ClearKiller(uint16_t (*lpwmvKiller)[2]) {
    memset(lpwmvKiller, 0, LIMIT_DEPTH * sizeof(uint16_t[2]));
}

// 复制杀手着法表
inline void CopyKiller(uint16_t (*lpwmvDst)[2], const uint16_t (*lpwmvSrc)[2]) {
    memcpy(lpwmvDst, lpwmvSrc, LIMIT_DEPTH * sizeof(uint16_t[2]));
}

/* 找到最佳着法时采取的措施
 *
 * 历史表的深度相关增量有以下几种选择：
 * 1. 平方关系(n^2)；
 * 2. 指数关系(2^n)；
 * 3. Fibonacci数列；
 * 4. 以上几种情况的组合，例如：n^2 + 2^n，等等。
 * ElephantEye使用最传统的平方关系。
 */
inline void SetBestMove(int mv, int nDepth, uint16_t *lpwmvKiller) {
    nHistory[mv] += SQR(nDepth);
    if (lpwmvKiller[0] != mv) {
        lpwmvKiller[1] = lpwmvKiller[0];
        lpwmvKiller[0] = mv;
    }
}

#endif

// end of movesort.h

// begin of preeval.h

/*
preeval.h/preeval.cpp - Source Code for ElephantEye, Part X

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PREEVAL_H
#define PREEVAL_H

// 扩展的局面预评价结构
extern struct PreEvalStructEx {
    int vlBlackAdvisorLeakage, vlWhiteAdvisorLeakage;
    int vlHollowThreat[16], vlCentralThreat[16];
    int vlWhiteBottomThreat[16], vlBlackBottomThreat[16];
    char cPopCnt16[65536]; // 加速PopCnt16的数组，只需要初始化一次
} PreEvalEx;

#endif

// end of preeval.h

// begin of pipe.h

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef PIPE_H
#define PIPE_H

const int LINE_INPUT_MAX_CHAR = 8192;

struct PipeStruct {
#ifdef _WIN32
    HANDLE hInput, hOutput;
  BOOL bConsole;
  int nBytesLeft;
#else
    int nInput, nOutput;
#endif
    int nEof;
    int nReadEnd;
    char szBuffer[LINE_INPUT_MAX_CHAR];

    void Open(const char *szExecFile = NULL);
    void Close(void) const;
    void ReadInput(void);
    bool CheckInput(void);
    bool GetBuffer(char *szLineStr);
    bool LineInput(char *szLineStr);
    void LineOutput(const char *szLineStr) const;
}; // pipe

#endif


// end of pipe.h

#include <string>
#include <iostream>

using std::string;
using std::cin;
using std::cout;

const int INTERRUPT_COUNT = 4096; // 搜索若干结点后调用中断

inline void PrintLn(const char *sz) {
    printf("%s\n", sz);
    fflush(stdout);
}

int main() {
    int i;
    bool bPonderTime;
    UcciCommStruct UcciComm;
    PositionStruct posProbe;

    LocatePath(Search.szBookFile, "BOOK.DAT");
    bPonderTime = false;
    PreGenInit();
    NewHash(24); // 24=16MB, 25=32MB, 26=64MB, ...
    Search.pos.FromFen(cszStartFen);
    Search.pos.nDistance = 0;
    Search.pos.PreEvaluate();
    Search.nBanMoves = 0;
    Search.bQuit = Search.bBatch = Search.bDebug = false;
    Search.bUseHash = Search.bNullMove = Search.bKnowledge = true;
    Search.bUseBook = false;
    Search.bIdle = false;
    Search.nCountMask = INTERRUPT_COUNT - 1;
    Search.nRandomMask = 0;
    Search.rc4Random.InitRand();

    string str;

    getline(cin, str);
//    str = string("{\"requests\":[{\"source\":\"-1\",\"target\":\"-1\"}],\"responses\":[]}");

    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    int turnID = input["responses"].size();
    // 第一回合收到的起点是"-1", 说明我是红方
    int currBotColor =
            input["requests"][(Json::Value::UInt) 0]["source"].asString() == "-1" ?  1 : 0;
    string curSource, curTarget;
    for (int j = 0; j < turnID; j++)
    {
        // 根据这些输入输出逐渐恢复状态到当前回合
        curSource = input["requests"][j]["source"].asString();
        curTarget = input["requests"][j]["target"].asString();
        if (curSource != "-1") {
            int sqSrc, sqDst;
            sqSrc = COORD_XY(curSource[0] - 'a' + FILE_LEFT, '9' - curSource[1] + RANK_TOP);
            sqDst = COORD_XY(curTarget[0] - 'a' + FILE_LEFT, '9' - curTarget[1] + RANK_TOP);
            if (IN_BOARD(sqSrc) && IN_BOARD(sqDst)) {
                int mv = MOVE(sqSrc, sqDst);
                if (mv != 0) {
                    if (Search.pos.LegalMove(mv) && Search.pos.MakeMove(mv) && Search.pos.LastMove().CptDrw > 0) {
                        // 始终让pos.nMoveNum反映没吃子的步数
                        Search.pos.SetIrrev();
                    } else {
                        throw std::runtime_error("input is not valid!");
                    }
                } else {
                    throw std::runtime_error("input is not valid!");
                }
            } else {
                throw std::runtime_error("input is not valid!");
            }
        }

        curSource = input["responses"][j]["source"].asString();
        curTarget = input["responses"][j]["target"].asString();
        {
            int sqSrc, sqDst;
            sqSrc = COORD_XY(curSource[0] - 'a' + FILE_LEFT, '9' - curSource[1] + RANK_TOP);
            sqDst = COORD_XY(curTarget[0] - 'a' + FILE_LEFT, '9' - curTarget[1] + RANK_TOP);
            if (IN_BOARD(sqSrc) && IN_BOARD(sqDst)) {
                int mv = MOVE(sqSrc, sqDst);
                if (mv != 0) {
                    if (Search.pos.LegalMove(mv) && Search.pos.MakeMove(mv) && Search.pos.LastMove().CptDrw > 0) {
                        // 始终让pos.nMoveNum反映没吃子的步数
                        Search.pos.SetIrrev();
                    } else {
                        throw std::runtime_error("input is not valid!");
                    }
                } else {
                    throw std::runtime_error("input is not valid!");
                }
            } else {
                throw std::runtime_error("input is not valid!");
            }
        }
    }
    curSource = input["requests"][turnID]["source"].asString();
    curTarget = input["requests"][turnID]["target"].asString();
    if (curSource != "-1") {
        int sqSrc, sqDst;
        sqSrc = COORD_XY(curSource[0] - 'a' + FILE_LEFT, '9' - curSource[1] + RANK_TOP);
        sqDst = COORD_XY(curTarget[0] - 'a' + FILE_LEFT, '9' - curTarget[1] + RANK_TOP);
        if (IN_BOARD(sqSrc) && IN_BOARD(sqDst)) {
            int mv = MOVE(sqSrc, sqDst);
            if (mv != 0) {
                if (Search.pos.LegalMove(mv) && Search.pos.MakeMove(mv) && Search.pos.LastMove().CptDrw > 0) {
                    // 始终让pos.nMoveNum反映没吃子的步数
                    Search.pos.SetIrrev();
                } else {
                    throw std::runtime_error("input is not valid!");
                }
            } else {
                throw std::runtime_error("input is not valid!");
            }
        } else {
            throw std::runtime_error("input is not valid!");
        }
    }

    Search.pos.nDistance = 0;
    Search.pos.PreEvaluate();
    Search.nBanMoves = 0;
    Search.nGoMode = GO_MODE_TIMER;
    Search.nProperTimer = 950;
    Search.nMaxTimer = 950;
    SearchMainBotzone(UCCI_MAX_DEPTH);

    DelHash();
    return 0;
}

// begin of pipe.cpp

#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

inline void ParseDir(char *szDir, const char *szPath) {
    char *lpSeparator;
    strcpy(szDir, szPath);
    lpSeparator = strrchr(szDir, PATH_SEPARATOR);
    if (lpSeparator == NULL) {
        szDir[0] = '\0';
    } else {
        *lpSeparator = '\0';
    }
}

#ifdef _WIN32

#ifdef PIPE_DLL

extern "C" __declspec(dllexport) VOID WINAPI PipeOpen(PipeStruct *lppipe, LPCSTR szProcFile);
extern "C" __declspec(dllexport) VOID WINAPI PipeClose(PipeStruct *lppipe);
extern "C" __declspec(dllexport) LPSTR WINAPI PipeLineInput(PipeStruct *lppipe);
extern "C" __declspec(dllexport) VOID WINAPI PipeLineOutput(PipeStruct *lppipe, LPCSTR szLineStr);
extern "C" __declspec(dllexport) BOOL WINAPI PipeEof(PipeStruct *lppipe);

VOID WINAPI PipeOpen(PipeStruct *lppipe, LPCSTR szProcFile) {
  lppipe->Open(szProcFile);
}

VOID WINAPI PipeClose(PipeStruct *lppipe) {
  lppipe->Close();
}

LPSTR WINAPI PipeLineInput(PipeStruct *lppipe) {
  static char szBuffer[LINE_INPUT_MAX_CHAR];
  if (lppipe->LineInput(szBuffer)) {
    return szBuffer;
  } else {
    return NULL;
  }
}

VOID WINAPI PipeLineOutput(PipeStruct *lppipe, LPCSTR szLineStr) {
  lppipe->LineOutput(szLineStr);
}

BOOL WINAPI PipeEof(PipeStruct *lppipe) {
  return lppipe->nEof != 0;
}

#endif

void PipeStruct::Open(const char *szProcFile) {
  DWORD dwMode;
  HANDLE hStdinRead, hStdinWrite, hStdoutRead, hStdoutWrite;
  SECURITY_ATTRIBUTES sa;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  char szDir[PATH_MAX_CHAR], szCurDir[PATH_MAX_CHAR];

  nEof = 0;
  if (szProcFile == NULL) {
    hInput = GetStdHandle(STD_INPUT_HANDLE);
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    bConsole = GetConsoleMode(hInput, &dwMode);
  } else {
    GetCurrentDirectory(PATH_MAX_CHAR, szCurDir);
    ParseDir(szDir, szProcFile);
    SetCurrentDirectory(szDir);

    memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    CreatePipe(&hStdinRead, &hStdinWrite, &sa, 0);
    CreatePipe(&hStdoutRead, &hStdoutWrite, &sa, 0);
    memset(&si, 0, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdinRead;
    si.hStdOutput = hStdoutWrite;
    si.hStdError = hStdoutWrite;
    if (CreateProcess(NULL, (LPSTR) szProcFile, NULL, NULL, TRUE, DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP | IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
    } else {
      nEof = 1;
    }
    CloseHandle(hStdinRead);
    CloseHandle(hStdoutWrite);
    hInput = hStdoutRead;
    hOutput = hStdinWrite;
    bConsole = FALSE;

    SetCurrentDirectory(szCurDir);
  }
  if (bConsole) {
    SetConsoleMode(hInput, dwMode & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
    FlushConsoleInputBuffer(hInput);
  }
  nBytesLeft = 0;
  nReadEnd = 0;
}

void PipeStruct::Close(void) const {
  CloseHandle(hInput);
  CloseHandle(hOutput);
}

void PipeStruct::ReadInput(void) {
  DWORD dwBytes;
  if (ReadFile(hInput, szBuffer + nReadEnd, LINE_INPUT_MAX_CHAR - nReadEnd, &dwBytes, NULL)) {
    nReadEnd += dwBytes;
    if (nBytesLeft > 0) {
      nBytesLeft -= dwBytes;
    }
  } else {
    nEof = 1;
  }
}

bool PipeStruct::CheckInput(void) {
  DWORD dwEvents, dwBytes;
  if (bConsole) { // a tty, or an un-redirected handle
    GetNumberOfConsoleInputEvents(hInput, &dwEvents);
    return dwEvents > 1;
  } else if (nBytesLeft > 0) { // a pipe with remainder data
    return true;
  } else if (PeekNamedPipe(hInput, NULL, 0, NULL, &dwBytes, NULL)) { // a pipe without remainder data
    nBytesLeft = dwBytes;
    return nBytesLeft > 0;
  } else { // a file, always true
    return true;
  }
}

void PipeStruct::LineOutput(const char *szLineStr) const {
  DWORD dwBytes;
  int nStrLen;
  char szWriteBuffer[LINE_INPUT_MAX_CHAR];
  nStrLen = strlen(szLineStr);
  memcpy(szWriteBuffer, szLineStr, nStrLen);
  szWriteBuffer[nStrLen] = '\r';
  szWriteBuffer[nStrLen + 1] = '\n';
  WriteFile(hOutput, szWriteBuffer, nStrLen + 2, &dwBytes, NULL);
}

#else

#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

void PipeStruct::Open(const char *szProcFile) {
    int nStdinPipe[2], nStdoutPipe[2];
    char szDir[PATH_MAX_CHAR];

    nEof = 0;
    if (szProcFile == NULL) {
        nInput = STDIN_FILENO;
        nOutput = STDOUT_FILENO;
    } else {
        pipe(nStdinPipe);
        pipe(nStdoutPipe);
        if (fork() == 0) {
            close(nStdinPipe[1]);
            close(nStdoutPipe[0]);
            dup2(nStdinPipe[0], STDIN_FILENO);
            close(nStdinPipe[0]);
            dup2(nStdoutPipe[1], STDOUT_FILENO);
            dup2(nStdoutPipe[1], STDERR_FILENO);
            close(nStdoutPipe[1]);

            ParseDir(szDir, szProcFile);
            chdir(szDir);

            nice(20);
            execl(szProcFile, szProcFile, NULL);
            exit(EXIT_FAILURE);
        }
        close(nStdinPipe[0]);
        close(nStdoutPipe[1]);
        nInput = nStdoutPipe[0];
        nOutput = nStdinPipe[1];
    }
    nReadEnd = 0;
}

void PipeStruct::Close(void) const {
    close(nInput);
    close(nOutput);
}

void PipeStruct::ReadInput(void) {
    int n;
    n = read(nInput, szBuffer + nReadEnd, LINE_INPUT_MAX_CHAR - nReadEnd);
    if (n < 0) {
        nEof = 1;
    } else {
        nReadEnd += n;
    }
}

bool PipeStruct::CheckInput(void) {
    fd_set set;
    timeval tv;
    int val;
    FD_ZERO(&set);
    FD_SET(nInput, &set);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    val = select(nInput + 1, &set, NULL, NULL, &tv);
    return (val > 0 && FD_ISSET(nInput, &set) > 0);
}

void PipeStruct::LineOutput(const char *szLineStr) const {
    int nStrLen;
    char szWriteBuffer[LINE_INPUT_MAX_CHAR];
    nStrLen = strlen(szLineStr);
    memcpy(szWriteBuffer, szLineStr, nStrLen);
    szWriteBuffer[nStrLen] = '\n';
    write(nOutput, szWriteBuffer, nStrLen + 1);
}

#endif

bool PipeStruct::GetBuffer(char *szLineStr) {
    char *lpFeedEnd;
    int nFeedEnd;
    lpFeedEnd = (char *) memchr(szBuffer, '\n', nReadEnd);
    if (lpFeedEnd == NULL) {
        return false;
    } else {
        nFeedEnd = lpFeedEnd - szBuffer;
        memcpy(szLineStr, szBuffer, nFeedEnd);
        szLineStr[nFeedEnd] = '\0';
        nFeedEnd ++;
        nReadEnd -= nFeedEnd;
        memcpy(szBuffer, szBuffer + nFeedEnd, nReadEnd);
        lpFeedEnd = (char *) strchr(szLineStr, '\r');
        if (lpFeedEnd != NULL) {
            *lpFeedEnd = '\0';
        }
        return true;
    }
}

bool PipeStruct::LineInput(char *szLineStr) {
    if (GetBuffer(szLineStr)) {
        return true;
    } else if (CheckInput()) {
        ReadInput();
        if (GetBuffer(szLineStr)) {
            return true;
        } else if (nReadEnd == LINE_INPUT_MAX_CHAR) {
            memcpy(szLineStr, szBuffer, LINE_INPUT_MAX_CHAR - 1);
            szLineStr[LINE_INPUT_MAX_CHAR - 1] = '\0';
            szBuffer[0] = szBuffer[LINE_INPUT_MAX_CHAR - 1];
            nReadEnd = 1;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


// end of pipe.cpp

// begin of ucci.cpp

/*
ucci.h/ucci.cpp - Source Code for ElephantEye, Part I

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This part (ucci.h/ucci.cpp only) of codes is NOT published under LGPL, and
can be used without restriction.
*/

#include <stdio.h>

/* UCCI指令分析模块由三各UCCI指令解释器组成。
 *
 * 其中第一个解释器"BootLine()"最简单，只用来接收引擎启动后的第一行指令
 * 输入"ucci"时就返回"UCCI_COMM_UCCI"，否则一律返回"UCCI_COMM_UNKNOWN"
 * 前两个解释器都等待是否有输入，如果没有输入则执行待机指令"Idle()"
 * 而第三个解释器("BusyLine()"，只用在引擎思考时)则在没有输入时直接返回"UCCI_COMM_UNKNOWN"
 */
static PipeStruct pipeStd;

//const int MAX_MOVE_NUM = 1024;

static char szFen[LINE_INPUT_MAX_CHAR];
static uint32_t dwCoordList[MAX_MOVE_NUM];

static bool ParsePos(UcciCommStruct &UcciComm, char *lp) {
    int i;
    // 首先判断是否指定了FEN串
    if (StrEqvSkip(lp, "fen ")) {
        strcpy(szFen, lp);
        UcciComm.szFenStr = szFen;
        // 然后判断是否是startpos
    } else if (StrEqv(lp, "startpos")) {
        UcciComm.szFenStr = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";
        // 如果两者都不是，就立即返回
    } else {
        return false;
    }
    // 然后寻找是否指定了后续着法，即是否有"moves"关键字
    UcciComm.nMoveNum = 0;
    if (StrScanSkip(lp, " moves ")) {
        *(lp - strlen(" moves ")) = '\0';
        UcciComm.nMoveNum = MIN((int) (strlen(lp) + 1) / 5, MAX_MOVE_NUM); // 提示："moves"后面的每个着法都是1个空格和4个字符
        for (i = 0; i < UcciComm.nMoveNum; i ++) {
            dwCoordList[i] = *(uint32_t *) lp; // 4个字符可转换为一个"uint32_t"，存储和处理起来方便
            lp += sizeof(uint32_t) + 1;
        }
        UcciComm.lpdwMovesCoord = dwCoordList;
    }
    return true;
}

UcciCommEnum BootLine(void) {
    char szLineStr[LINE_INPUT_MAX_CHAR];
    pipeStd.Open();
    while (!pipeStd.LineInput(szLineStr)) {
        Idle();
    }
    if (StrEqv(szLineStr, "ucci")) {
        return UCCI_COMM_UCCI;
    } else {
        return UCCI_COMM_UNKNOWN;
    }
}

UcciCommEnum IdleLine(UcciCommStruct &UcciComm, bool bDebug) {
    char szLineStr[LINE_INPUT_MAX_CHAR];
    char *lp;
    int i;
    bool bGoTime;

    while (!pipeStd.LineInput(szLineStr)) {
        Idle();
    }
    lp = szLineStr;
    if (bDebug) {
        printf("info idleline [%s]\n", lp);
        fflush(stdout);
    }
    if (false) {
        // "IdleLine()"是最复杂的UCCI指令解释器，大多数的UCCI指令都由它来解释，包括：

        // 1. "isready"指令
    } else if (StrEqv(lp, "isready")) {
        return UCCI_COMM_ISREADY;

        // 2. "setoption <option> [<arguments>]"指令
    } else if (StrEqvSkip(lp, "setoption ")) {
        if (false) {

            // (1) "batch"选项
        } else if (StrEqvSkip(lp, "batch ")) {
            UcciComm.Option = UCCI_OPTION_BATCH;
            if (StrEqv(lp, "on")) {
                UcciComm.bCheck = true;
            } else if (StrEqv(lp, "true")) {
                UcciComm.bCheck = true;
            } else {
                UcciComm.bCheck = false;
            } // 由于"batch"选项默认是关闭的，所以只有设定"on"或"true"时才打开，下同

            // (2) "debug"选项
        } else if (StrEqvSkip(lp, "debug ")) {
            UcciComm.Option = UCCI_OPTION_DEBUG;
            if (StrEqv(lp, "on")) {
                UcciComm.bCheck = true;
            } else if (StrEqv(lp, "true")) {
                UcciComm.bCheck = true;
            } else {
                UcciComm.bCheck = false;
            }

            // (3) "ponder"选项
        } else if (StrEqvSkip(lp, "ponder ")) {
            UcciComm.Option = UCCI_OPTION_PONDER;
            if (StrEqv(lp, "on")) {
                UcciComm.bCheck = true;
            } else if (StrEqv(lp, "true")) {
                UcciComm.bCheck = true;
            } else {
                UcciComm.bCheck = false;
            }

            // (4) "usehash"选项
        } else if (StrEqvSkip(lp, "usehash ")) {
            UcciComm.Option = UCCI_OPTION_USEHASH;
            if (StrEqv(lp, "off")) {
                UcciComm.bCheck = false;
            } else if (StrEqv(lp, "false")) {
                UcciComm.bCheck = false;
            } else {
                UcciComm.bCheck = true;
            }

            // (5) "usebook"选项
        } else if (StrEqvSkip(lp, "usebook ")) {
            UcciComm.Option = UCCI_OPTION_USEBOOK;
            if (StrEqv(lp, "off")) {
                UcciComm.bCheck = false;
            } else if (StrEqv(lp, "false")) {
                UcciComm.bCheck = false;
            } else {
                UcciComm.bCheck = true;
            }

            // (6) "useegtb"选项
        } else if (StrEqvSkip(lp, "useegtb ")) {
            UcciComm.Option = UCCI_OPTION_USEEGTB;
            if (StrEqv(lp, "off")) {
                UcciComm.bCheck = false;
            } else if (StrEqv(lp, "false")) {
                UcciComm.bCheck = false;
            } else {
                UcciComm.bCheck = true;
            }

            // (7) "bookfiles"选项
        } else if (StrEqvSkip(lp, "bookfiles ")) {
            UcciComm.Option = UCCI_OPTION_BOOKFILES;
            UcciComm.szOption = lp;

            // (8) "egtbpaths"选项
        } else if (StrEqvSkip(lp, "egtbpaths ")) {
            UcciComm.Option = UCCI_OPTION_EGTBPATHS;
            UcciComm.szOption = lp;

            // (9) "evalapi"选项，3.3以后不再支持

            // (10) "hashsize"选项
        } else if (StrEqvSkip(lp, "hashsize ")) {
            UcciComm.Option = UCCI_OPTION_HASHSIZE;
            UcciComm.nSpin = Str2Digit(lp, 0, 1024);

            // (11) "threads"选项
        } else if (StrEqvSkip(lp, "threads ")) {
            UcciComm.Option = UCCI_OPTION_THREADS;
            UcciComm.nSpin = Str2Digit(lp, 0, 32);

            // (12) "promotion"选项
        } else if (StrEqvSkip(lp, "promotion ")) {
            UcciComm.Option = UCCI_OPTION_PROMOTION;
            if (StrEqv(lp, "on")) {
                UcciComm.bCheck = true;
            } else if (StrEqv(lp, "true")) {
                UcciComm.bCheck = true;
            } else {
                UcciComm.bCheck = false;
            }

            // (13) "idle"选项
        } else if (StrEqvSkip(lp, "idle ")) {
            UcciComm.Option = UCCI_OPTION_IDLE;
            if (false) {
            } else if (StrEqv(lp, "none")) {
                UcciComm.Grade = UCCI_GRADE_NONE;
            } else if (StrEqv(lp, "small")) {
                UcciComm.Grade = UCCI_GRADE_SMALL;
            } else if (StrEqv(lp, "medium")) {
                UcciComm.Grade = UCCI_GRADE_MEDIUM;
            } else if (StrEqv(lp, "large")) {
                UcciComm.Grade = UCCI_GRADE_LARGE;
            } else {
                UcciComm.Grade = UCCI_GRADE_NONE;
            }

            // (14) "pruning"选项
        } else if (StrEqvSkip(lp, "pruning ")) {
            UcciComm.Option = UCCI_OPTION_PRUNING;
            if (false) {
            } else if (StrEqv(lp, "none")) {
                UcciComm.Grade = UCCI_GRADE_NONE;
            } else if (StrEqv(lp, "small")) {
                UcciComm.Grade = UCCI_GRADE_SMALL;
            } else if (StrEqv(lp, "medium")) {
                UcciComm.Grade = UCCI_GRADE_MEDIUM;
            } else if (StrEqv(lp, "large")) {
                UcciComm.Grade = UCCI_GRADE_LARGE;
            } else {
                UcciComm.Grade = UCCI_GRADE_LARGE;
            }

            // (15) "knowledge"选项
        } else if (StrEqvSkip(lp, "knowledge ")) {
            UcciComm.Option = UCCI_OPTION_KNOWLEDGE;
            if (false) {
            } else if (StrEqv(lp, "none")) {
                UcciComm.Grade = UCCI_GRADE_NONE;
            } else if (StrEqv(lp, "small")) {
                UcciComm.Grade = UCCI_GRADE_SMALL;
            } else if (StrEqv(lp, "medium")) {
                UcciComm.Grade = UCCI_GRADE_MEDIUM;
            } else if (StrEqv(lp, "large")) {
                UcciComm.Grade = UCCI_GRADE_LARGE;
            } else {
                UcciComm.Grade = UCCI_GRADE_LARGE;
            }

            // (16) "randomness"选项
        } else if (StrEqvSkip(lp, "randomness ")) {
            UcciComm.Option = UCCI_OPTION_RANDOMNESS;
            if (false) {
            } else if (StrEqv(lp, "none")) {
                UcciComm.Grade = UCCI_GRADE_NONE;
            } else if (StrEqv(lp, "tiny")) {
                UcciComm.Grade = UCCI_GRADE_TINY;
            } else if (StrEqv(lp, "small")) {
                UcciComm.Grade = UCCI_GRADE_SMALL;
            } else if (StrEqv(lp, "medium")) {
                UcciComm.Grade = UCCI_GRADE_MEDIUM;
            } else if (StrEqv(lp, "large")) {
                UcciComm.Grade = UCCI_GRADE_LARGE;
            } else if (StrEqv(lp, "huge")) {
                UcciComm.Grade = UCCI_GRADE_HUGE;
            } else {
                UcciComm.Grade = UCCI_GRADE_NONE;
            }

            // (17) "style"选项
        } else if (StrEqvSkip(lp, "style ")) {
            UcciComm.Option = UCCI_OPTION_STYLE;
            if (false) {
            } else if (StrEqv(lp, "solid")) {
                UcciComm.Style = UCCI_STYLE_SOLID;
            } else if (StrEqv(lp, "normal")) {
                UcciComm.Style = UCCI_STYLE_NORMAL;
            } else if (StrEqv(lp, "risky")) {
                UcciComm.Style = UCCI_STYLE_RISKY;
            } else {
                UcciComm.Style = UCCI_STYLE_NORMAL;
            }

            // (18) "newgame"选项
        } else if (StrEqv(lp, "newgame")) {
            UcciComm.Option = UCCI_OPTION_NEWGAME;

            // (19) 无法识别的选项，有扩充的余地
        } else {
            UcciComm.Option = UCCI_OPTION_UNKNOWN;
        }
        return UCCI_COMM_SETOPTION;

        // 3. "position {<special_position> | fen <fen_string>} [moves <move_list>]"指令
    } else if (StrEqvSkip(lp, "position ")) {
        return ParsePos(UcciComm, lp) ? UCCI_COMM_POSITION : UCCI_COMM_UNKNOWN;

        // 4. "banmoves <move_list>"指令，处理起来和"position ... moves ..."是一样的
    } else if (StrEqvSkip(lp, "banmoves ")) {
        UcciComm.nBanMoveNum = MIN((int) (strlen(lp) + 1) / 5, MAX_MOVE_NUM);
        for (i = 0; i < UcciComm.nBanMoveNum; i ++) {
            dwCoordList[i] = *(uint32_t *) lp;
            lp += sizeof(uint32_t) + 1;
        }
        UcciComm.lpdwBanMovesCoord = dwCoordList;
        return UCCI_COMM_BANMOVES;

        // 5. "go [ponder | draw] <mode>"指令
    } else if (StrEqvSkip(lp, "go ")) {
        UcciComm.bPonder = UcciComm.bDraw = false;
        // 首先判断到底是"go"、"go ponder"还是"go draw"
        if (StrEqvSkip(lp, "ponder ")) {
            UcciComm.bPonder = true;
        } else if (StrEqvSkip(lp, "draw ")) {
            UcciComm.bDraw = true;
        }
        // 然后判断思考模式
        bGoTime = false;
        if (false) {
        } else if (StrEqvSkip(lp, "depth ")) {
            UcciComm.Go = UCCI_GO_DEPTH;
            UcciComm.nDepth = Str2Digit(lp, 0, UCCI_MAX_DEPTH);
        } else if (StrEqvSkip(lp, "nodes ")) {
            UcciComm.Go = UCCI_GO_NODES;
            UcciComm.nDepth = Str2Digit(lp, 0, 2000000000);
        } else if (StrEqvSkip(lp, "time ")) {
            UcciComm.nTime = Str2Digit(lp, 0, 2000000000);
            bGoTime = true;
            // 如果没有说明是固定深度还是设定时限，就固定深度为"UCCI_MAX_DEPTH"
        } else {
            UcciComm.Go = UCCI_GO_DEPTH;
            UcciComm.nDepth = UCCI_MAX_DEPTH;
        }
        // 如果是设定时限，就要判断是时段制还是加时制
        if (bGoTime) {
            if (false) {
            } else if (StrScanSkip(lp, " movestogo ")) {
                UcciComm.Go = UCCI_GO_TIME_MOVESTOGO;
                UcciComm.nMovesToGo = Str2Digit(lp, 1, 999);
            } else if (StrScanSkip(lp, " increment ")) {
                UcciComm.Go = UCCI_GO_TIME_INCREMENT;
                UcciComm.nIncrement = Str2Digit(lp, 0, 999999);
                // 如果没有说明是时段制还是加时制，就设定为步数是1的时段制
            } else {
                UcciComm.Go = UCCI_GO_TIME_MOVESTOGO;
                UcciComm.nMovesToGo = 1;
            }
        }
        return UCCI_COMM_GO;

        // 6. "stop"指令
    } else if (StrEqv(lp, "stop")) {
        return UCCI_COMM_STOP;

        // 7. "probe {<special_position> | fen <fen_string>} [moves <move_list>]"指令
    } else if (StrEqvSkip(lp, "probe ")) {
        return ParsePos(UcciComm, lp) ? UCCI_COMM_PROBE : UCCI_COMM_UNKNOWN;

        // 8. "quit"指令
    } else if (StrEqv(lp, "quit")) {
        return UCCI_COMM_QUIT;

        // 9. 无法识别的指令
    } else {
        return UCCI_COMM_UNKNOWN;
    }
}

UcciCommEnum BusyLine(UcciCommStruct &UcciComm, bool bDebug) {
    char szLineStr[LINE_INPUT_MAX_CHAR];
    char *lp;
    if (pipeStd.LineInput(szLineStr)) {
        if (bDebug) {
            printf("info busyline [%s]\n", szLineStr);
            fflush(stdout);
        }
        // "BusyLine"只能接收"isready"、"ponderhit"和"stop"这三条指令
        if (false) {
        } else if (StrEqv(szLineStr, "isready")) {
            return UCCI_COMM_ISREADY;
        } else if (StrEqv(szLineStr, "ponderhit draw")) {
            return UCCI_COMM_PONDERHIT_DRAW;
            // 注意：必须首先判断"ponderhit draw"，再判断"ponderhit"
        } else if (StrEqv(szLineStr, "ponderhit")) {
            return UCCI_COMM_PONDERHIT;
        } else if (StrEqv(szLineStr, "stop")) {
            return UCCI_COMM_STOP;
        } else if (StrEqv(szLineStr, "quit")) {
            return UCCI_COMM_QUIT;
        } else {
            lp = szLineStr;
            if (StrEqvSkip(lp, "probe ")) {
                return ParsePos(UcciComm, lp) ? UCCI_COMM_PROBE : UCCI_COMM_UNKNOWN;
            } else {
                return UCCI_COMM_UNKNOWN;
            }
        }
    } else {
        return UCCI_COMM_UNKNOWN;
    }
}


// end of ucci.cpp

// begin of evaluate.cpp

/*
evaluate.cpp - Source Code for ElephantEye, Part XI

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上五个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

// 偷懒评价的边界
const int EVAL_MARGIN1 = 160;
const int EVAL_MARGIN2 = 80;
const int EVAL_MARGIN3 = 40;
const int EVAL_MARGIN4 = 20;

// 本模块只涉及到"PositionStruct"中的"sdPlayer"、"ucpcSquares"、"ucsqPieces"和"wBitPiece"四个成员，故省略前面的"this->"

/* ElephantEye的局面评价内容共4有4部分
 * 1. 跟仕(士)有关的特殊棋型的评价，见"AdvisorShape()"；
 * 2. 车或炮牵制帅(将)或车的棋型的评价，见"StringHold()"；
 * 3. 车的灵活性的评价，见"RookMobility()"；
 * 4. 马受到阻碍的评价，见"KnightTrap()"。
 */

// 以下是第一部分，特殊棋型的评价

/* 仕(士)的形状对于局面评价，特别是判断空头炮、沉底炮等棋型有重大作用，为此ElephantEye给出四种形状：
 * 1. 帅(将)在原位，双仕(士)都在底线，编为1号，这种情况要判断空头炮和炮镇窝心马；
 * 2. 帅(将)在原位，双仕(士)从左边包围帅(将)，编为2号，这种情况要判断右边的沉底炮和车封右边的帅(将)门；
 * 3. 帅(将)在原位，双仕(士)从右边包围帅(将)，编为3号，这种情况要判断左边的沉底炮和车封左边的帅(将)门；
 * 4. 其他情况，包括帅(将)不在原位或缺仕(士)，都编号0。
 * 注：以“红下黑上”这个固定的棋盘方位来规定左右。
 */
const int WHITE_KING_BITFILE = 1 << (RANK_BOTTOM - RANK_TOP);
const int BLACK_KING_BITFILE = 1 << (RANK_TOP - RANK_TOP);
const int KING_BITRANK = 1 << (FILE_CENTER - FILE_LEFT);

const int SHAPE_NONE = 0;
const int SHAPE_CENTER = 1;
const int SHAPE_LEFT = 2;
const int SHAPE_RIGHT = 3;

int PositionStruct::AdvisorShape(void) const {
    int pcCannon, pcRook, sq, sqAdv1, sqAdv2, x, y, nShape;
    int vlWhitePenalty, vlBlackPenalty;
    SlideMaskStruct *lpsms;
    vlWhitePenalty = vlBlackPenalty = 0;
    if ((this->wBitPiece[0] & ADVISOR_BITPIECE) == ADVISOR_BITPIECE) {
        if (this->ucsqPieces[SIDE_TAG(0) + KING_FROM] == 0xc7) {
            sqAdv1 = this->ucsqPieces[SIDE_TAG(0) + ADVISOR_FROM];
            sqAdv2 = this->ucsqPieces[SIDE_TAG(0) + ADVISOR_TO];
            if (false) {
            } else if (sqAdv1 == 0xc6) { // 红方一个仕在左侧底线
                nShape = (sqAdv2 == 0xc8 ? SHAPE_CENTER : sqAdv2 == 0xb7 ? SHAPE_LEFT : SHAPE_NONE);
            } else if (sqAdv1 == 0xc8) { // 红方一个仕在右侧底线
                nShape = (sqAdv2 == 0xc6 ? SHAPE_CENTER : sqAdv2 == 0xb7 ? SHAPE_RIGHT : SHAPE_NONE);
            } else if (sqAdv1 == 0xb7) { // 红方一个仕在花心
                nShape = (sqAdv2 == 0xc6 ? SHAPE_LEFT : sqAdv2 == 0xc8 ? SHAPE_RIGHT : SHAPE_NONE);
            } else {
                nShape = SHAPE_NONE;
            }
            switch (nShape) {
                case SHAPE_NONE:
                    break;
                case SHAPE_CENTER:
                    for (pcCannon = SIDE_TAG(1) + CANNON_FROM; pcCannon <= SIDE_TAG(1) + CANNON_TO; pcCannon ++) {
                        sq = this->ucsqPieces[pcCannon];
                        if (sq != 0) {
                            x = FILE_X(sq);
                            if (x == FILE_CENTER) {
                                y = RANK_Y(sq);
                                lpsms = this->FileMaskPtr(x, y);
                                if ((lpsms->wRookCap & WHITE_KING_BITFILE) != 0) {
                                    // 计算空头炮的威胁
                                    vlWhitePenalty += PreEvalEx.vlHollowThreat[RANK_FLIP(y)];
                                } else if ((lpsms->wSuperCap & WHITE_KING_BITFILE) != 0 &&
                                           (this->ucpcSquares[0xb7] == 21 || this->ucpcSquares[0xb7] == 22)) {
                                    // 计算炮镇窝心马的威胁
                                    vlWhitePenalty += PreEvalEx.vlCentralThreat[RANK_FLIP(y)];
                                }
                            }
                        }
                    }
                    break;
                case SHAPE_LEFT:
                case SHAPE_RIGHT:
                    for (pcCannon = SIDE_TAG(1) + CANNON_FROM; pcCannon <= SIDE_TAG(1) + CANNON_TO; pcCannon ++) {
                        sq = this->ucsqPieces[pcCannon];
                        if (sq != 0) {
                            x = FILE_X(sq);
                            y = RANK_Y(sq);
                            if (x == FILE_CENTER) {
                                if ((this->FileMaskPtr(x, y)->wSuperCap & WHITE_KING_BITFILE) != 0) {
                                    // 计算一般中炮的威胁，帅(将)门被对方控制的还有额外罚分
                                    vlWhitePenalty += (PreEvalEx.vlCentralThreat[RANK_FLIP(y)] >> 2) +
                                                      (this->Protected(1, nShape == SHAPE_LEFT ? 0xc8 : 0xc6) ? 20 : 0);
                                    // 如果车在底线保护帅(将)，则给予更大的罚分！
                                    for (pcRook = SIDE_TAG(0) + ROOK_FROM; pcRook <= SIDE_TAG(0) + ROOK_TO; pcRook ++) {
                                        sq = this->ucsqPieces[pcRook];
                                        if (sq != 0) {
                                            y = RANK_Y(sq);
                                            if (y == RANK_BOTTOM) {
                                                x = FILE_X(sq);
                                                if ((this->RankMaskPtr(x, y)->wRookCap & KING_BITRANK) != 0) {
                                                    vlWhitePenalty += 80;
                                                }
                                            }
                                        }
                                    }
                                }
                            } else if (y == RANK_BOTTOM) {
                                if ((this->RankMaskPtr(x, y)->wRookCap & KING_BITRANK) != 0) {
                                    // 计算沉底炮的威胁
                                    vlWhitePenalty += PreEvalEx.vlWhiteBottomThreat[x];
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        } else if (this->ucsqPieces[SIDE_TAG(0) + KING_FROM] == 0xb7) {
            // 有双仕(士)但花心被帅(将)占领，要罚分
            vlWhitePenalty += 20;
        }
    } else {
        if ((this->wBitPiece[1] & ROOK_BITPIECE) == ROOK_BITPIECE) {
            // 缺仕(士)怕双车，有罚分
            vlWhitePenalty += PreEvalEx.vlWhiteAdvisorLeakage;
        }
    }
    if ((this->wBitPiece[1] & ADVISOR_BITPIECE) == ADVISOR_BITPIECE) {
        if (this->ucsqPieces[SIDE_TAG(1) + KING_FROM] == 0x37) {
            sqAdv1 = this->ucsqPieces[SIDE_TAG(1) + ADVISOR_FROM];
            sqAdv2 = this->ucsqPieces[SIDE_TAG(1) + ADVISOR_TO];
            if (false) {
            } else if (sqAdv1 == 0x36) { // 黑方一个士在左侧底线
                nShape = (sqAdv2 == 0x38 ? SHAPE_CENTER : sqAdv2 == 0x47 ? SHAPE_LEFT : SHAPE_NONE);
            } else if (sqAdv1 == 0x38) { // 黑方一个士在右侧底线
                nShape = (sqAdv2 == 0x36 ? SHAPE_CENTER : sqAdv2 == 0x47 ? SHAPE_RIGHT : SHAPE_NONE);
            } else if (sqAdv1 == 0x47) { // 黑方一个士在花心
                nShape = (sqAdv2 == 0x36 ? SHAPE_LEFT : sqAdv2 == 0x38 ? SHAPE_RIGHT : SHAPE_NONE);
            } else {
                nShape = SHAPE_NONE;
            }
            switch (nShape) {
                case SHAPE_NONE:
                    break;
                case SHAPE_CENTER:
                    for (pcCannon = SIDE_TAG(0) + CANNON_FROM; pcCannon <= SIDE_TAG(0) + CANNON_TO; pcCannon ++) {
                        sq = this->ucsqPieces[pcCannon];
                        if (sq != 0) {
                            x = FILE_X(sq);
                            if (x == FILE_CENTER) {
                                y = RANK_Y(sq);
                                lpsms = this->FileMaskPtr(x, y);
                                if ((lpsms->wRookCap & BLACK_KING_BITFILE) != 0) {
                                    // 计算空头炮的威胁
                                    vlBlackPenalty += PreEvalEx.vlHollowThreat[y];
                                } else if ((lpsms->wSuperCap & BLACK_KING_BITFILE) != 0 &&
                                           (this->ucpcSquares[0x47] == 37 || this->ucpcSquares[0x47] == 38)) {
                                    // 计算炮镇窝心马的威胁
                                    vlBlackPenalty += PreEvalEx.vlCentralThreat[y];
                                }
                            }
                        }
                    }
                    break;
                case SHAPE_LEFT:
                case SHAPE_RIGHT:
                    for (pcCannon = SIDE_TAG(0) + CANNON_FROM; pcCannon <= SIDE_TAG(0) + CANNON_TO; pcCannon ++) {
                        sq = this->ucsqPieces[pcCannon];
                        if (sq != 0) {
                            x = FILE_X(sq);
                            y = RANK_Y(sq);
                            if (x == FILE_CENTER) {
                                if ((this->FileMaskPtr(x, y)->wSuperCap & BLACK_KING_BITFILE) != 0) {
                                    // 计算一般中炮的威胁，帅(将)门被对方控制的还有额外罚分
                                    vlBlackPenalty += (PreEvalEx.vlCentralThreat[y] >> 2) +
                                                      (this->Protected(0, nShape == SHAPE_LEFT ? 0x38 : 0x36) ? 20 : 0);
                                    // 如果车在底线保护帅(将)，则给予更大的罚分！
                                    for (pcRook = SIDE_TAG(1) + ROOK_FROM; pcRook <= SIDE_TAG(1) + ROOK_TO; pcRook ++) {
                                        sq = this->ucsqPieces[pcRook];
                                        if (sq != 0) {
                                            y = RANK_Y(sq);
                                            if (y == RANK_TOP) {
                                                x = FILE_X(sq);
                                                if ((this->RankMaskPtr(x, y)->wRookCap & KING_BITRANK) != 0) {
                                                    vlBlackPenalty += 80;
                                                }
                                            }
                                        }
                                    }
                                }
                            } else if (y == RANK_TOP) {
                                if ((this->RankMaskPtr(x, y)->wRookCap & KING_BITRANK) != 0) {
                                    // 计算沉底炮的威胁
                                    vlBlackPenalty += PreEvalEx.vlBlackBottomThreat[x];
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        } else if (this->ucsqPieces[SIDE_TAG(1) + KING_FROM] == 0x47) {
            // 有双仕(士)但花心被帅(将)占领，要罚分
            vlBlackPenalty += 20;
        }
    } else {
        if ((this->wBitPiece[0] & ROOK_BITPIECE) == ROOK_BITPIECE) {
            // 缺仕(士)怕双车，有罚分
            vlBlackPenalty += PreEvalEx.vlBlackAdvisorLeakage;
        }
    }
    return SIDE_VALUE(this->sdPlayer, vlBlackPenalty - vlWhitePenalty);
}

// 以上是第一部分，特殊棋型的评价

// 以下是第二部分，牵制的评价

// 常数表"cnValuableStringPieces"用判断牵制是否有价值
// 大于0的项是对于车来说的，牵制马和炮(被牵制)都有价值，大于1的项是对于炮来说只有牵制马才有价值
static const int cnValuableStringPieces[48] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0
};

// "ccvlStringValueTab"是类似"KNIGHT_PIN_TAB"的常数表(参阅"pregen.h")，决定牵制价值
// 中间子和被牵制子的距离越近，牵制的价值就越大
static const char ccvlStringValueTab[512] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0,  0,
        12, 16, 20, 24, 28, 32, 36,  0, 36, 32, 28, 24, 20, 16, 12,  0,
        0,  0,  0,  0,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0
};

// 车或炮牵制帅(将)或车的棋型的评价
int PositionStruct::StringHold(void) const {
    int sd, i, j, nDir, sqSrc, sqDst, sqStr;
    int x, y, nSideTag, nOppSideTag;
    int vlString[2];
    SlideMoveStruct *lpsmv;

    for (sd = 0; sd < 2; sd ++) {
        vlString[sd] = 0;
        nSideTag = SIDE_TAG(sd);
        nOppSideTag = OPP_SIDE_TAG(sd);
        // 考查用车来牵制的情况
        for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
            sqSrc = this->ucsqPieces[nSideTag + i];
            if (sqSrc != 0) {
                __ASSERT_SQUARE(sqSrc);
                // 考查牵制目标是帅(将)的情况
                sqDst = this->ucsqPieces[nOppSideTag + KING_FROM];
                if (sqDst != 0) {
                    __ASSERT_SQUARE(sqDst);
                    x = FILE_X(sqSrc);
                    y = RANK_Y(sqSrc);
                    if (x == FILE_X(sqDst)) {
                        lpsmv = this->FileMovePtr(x, y);
                        nDir = (sqSrc < sqDst ? 0 : 1);
                        // 如果车用炮的吃法(炮用超级炮的着法)能吃到目标子"sqDst"，牵制就成立了，下同
                        if (sqDst == lpsmv->ucCannonCap[nDir] + FILE_DISP(x)) {
                            // 被牵制子"sqStr"是车(炮)本身能吃到的棋子，下同
                            sqStr = lpsmv->ucRookCap[nDir] + FILE_DISP(x);
                            __ASSERT_SQUARE(sqStr);
                            // 被牵制子必须是对方的子，下同
                            if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                // 如果被牵制子是有价值的，而且被牵制子没有保护(被目标子保护不算)，那么牵制是有价值的，下同
                                if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 0 &&
                                    !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                    vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                }
                            }
                        }
                    } else if (y == RANK_Y(sqDst)) {
                        lpsmv = this->RankMovePtr(x, y);
                        nDir = (sqSrc < sqDst ? 0 : 1);
                        if (sqDst == lpsmv->ucCannonCap[nDir] + RANK_DISP(y)) {
                            sqStr = lpsmv->ucRookCap[nDir] + RANK_DISP(y);
                            __ASSERT_SQUARE(sqStr);
                            if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 0 &&
                                    !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                    vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                }
                            }
                        }
                    }
                }
                // 考查牵制目标是车的情况
                for (j = ROOK_FROM; j <= ROOK_TO; j ++) {
                    sqDst = this->ucsqPieces[nOppSideTag + j];
                    if (sqDst != 0) {
                        __ASSERT_SQUARE(sqDst);
                        x = FILE_X(sqSrc);
                        y = RANK_Y(sqSrc);
                        if (x == FILE_X(sqDst)) {
                            lpsmv = this->FileMovePtr(x, y);
                            nDir = (sqSrc < sqDst ? 0 : 1);
                            if (sqDst == lpsmv->ucCannonCap[nDir] + FILE_DISP(x)) {
                                sqStr = lpsmv->ucRookCap[nDir] + FILE_DISP(x);
                                __ASSERT_SQUARE(sqStr);
                                if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                    // 目标子是车，不同于帅(将)，要求车也没有保护时才有牵制价值，下同
                                    if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 0 &&
                                        !this->Protected(OPP_SIDE(sd), sqDst) && !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                        vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                    }
                                }
                            }
                        } else if (y == RANK_Y(sqDst)) {
                            lpsmv = this->RankMovePtr(x, y);
                            nDir = (sqSrc < sqDst ? 0 : 1);
                            if (sqDst == lpsmv->ucCannonCap[nDir] + RANK_DISP(y)) {
                                sqStr = lpsmv->ucRookCap[nDir] + RANK_DISP(y);
                                __ASSERT_SQUARE(sqStr);
                                if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                    if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 0 &&
                                        !this->Protected(OPP_SIDE(sd), sqDst) && !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                        vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // 考查用炮来牵制的情况
        for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
            sqSrc = this->ucsqPieces[nSideTag + i];
            if (sqSrc != 0) {
                __ASSERT_SQUARE(sqSrc);
                // 考查牵制目标是帅(将)的情况
                sqDst = this->ucsqPieces[nOppSideTag + KING_FROM];
                if (sqDst != 0) {
                    __ASSERT_SQUARE(sqDst);
                    x = FILE_X(sqSrc);
                    y = RANK_Y(sqSrc);
                    if (x == FILE_X(sqDst)) {
                        lpsmv = this->FileMovePtr(x, y);
                        nDir = (sqSrc < sqDst ? 0 : 1);
                        if (sqDst == lpsmv->ucSuperCap[nDir] + FILE_DISP(x)) {
                            sqStr = lpsmv->ucCannonCap[nDir] + FILE_DISP(x);
                            __ASSERT_SQUARE(sqStr);
                            if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 1 &&
                                    !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                    vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                }
                            }
                        }
                    } else if (y == RANK_Y(sqDst)) {
                        lpsmv = this->RankMovePtr(x, y);
                        nDir = (sqSrc < sqDst ? 0 : 1);
                        if (sqDst == lpsmv->ucSuperCap[nDir] + RANK_DISP(y)) {
                            sqStr = lpsmv->ucCannonCap[nDir] + RANK_DISP(y);
                            __ASSERT_SQUARE(sqStr);
                            if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 1 &&
                                    !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                    vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                }
                            }
                        }
                    }
                }
                // 考查牵制目标是车的情况
                for (j = ROOK_FROM; j <= ROOK_TO; j ++) {
                    sqDst = this->ucsqPieces[nOppSideTag + j];
                    if (sqDst != 0) {
                        __ASSERT_SQUARE(sqDst);
                        x = FILE_X(sqSrc);
                        y = RANK_Y(sqSrc);
                        if (x == FILE_X(sqDst)) {
                            lpsmv = this->FileMovePtr(x, y);
                            nDir = (sqSrc < sqDst ? 0 : 1);
                            if (sqDst == lpsmv->ucSuperCap[nDir] + FILE_DISP(x)) {
                                sqStr = lpsmv->ucCannonCap[nDir] + FILE_DISP(x);
                                __ASSERT_SQUARE(sqStr);
                                if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                    if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 1 &&
                                        !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                        vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                    }
                                }
                            }
                        } else if (y == RANK_Y(sqDst)) {
                            lpsmv = this->RankMovePtr(x, y);
                            nDir = (sqSrc < sqDst ? 0 : 1);
                            if (sqDst == lpsmv->ucSuperCap[nDir] + RANK_DISP(y)) {
                                sqStr = lpsmv->ucCannonCap[nDir] + RANK_DISP(y);
                                __ASSERT_SQUARE(sqStr);
                                if ((this->ucpcSquares[sqStr] & nOppSideTag) != 0) {
                                    if (cnValuableStringPieces[this->ucpcSquares[sqStr]] > 1 &&
                                        !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                                        vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return SIDE_VALUE(this->sdPlayer, vlString[0] - vlString[1]);
}

// 以上是第二部分，牵制的评价

// 以下是第三部分，车的灵活性的评价

int PositionStruct::RookMobility(void) const {
    int sd, i, sqSrc, nSideTag, x, y;
    int vlRookMobility[2];
    for (sd = 0; sd < 2; sd ++) {
        vlRookMobility[sd] = 0;
        nSideTag = SIDE_TAG(sd);
        for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
            sqSrc = this->ucsqPieces[nSideTag + i];
            if (sqSrc != 0) {
                __ASSERT_SQUARE(sqSrc);
                x = FILE_X(sqSrc);
                y = RANK_Y(sqSrc);
                vlRookMobility[sd] += PreEvalEx.cPopCnt16[this->RankMaskPtr(x, y)->wNonCap] +
                                      PreEvalEx.cPopCnt16[this->FileMaskPtr(x, y)->wNonCap];
            }
        }
        __ASSERT(vlRookMobility[sd] <= 34);
    }
    return SIDE_VALUE(this->sdPlayer, vlRookMobility[0] - vlRookMobility[1]) >> 1;
}

// 以上是第三部分，车的灵活性的评价

// 以下是第四部分，马受到阻碍的评价

// 常数表"cbcEdgeSquares"给定了不利于马的位置，处于棋盘边缘和两个花心位置的马都是坏马
static const bool cbcEdgeSquares[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int PositionStruct::KnightTrap(void) const {
    int sd, i, sqSrc, sqDst, nSideTag, nMovable;
    uint8_t *lpucsqDst, *lpucsqPin;
    int vlKnightTraps[2];

    for (sd = 0; sd < 2; sd ++) {
        vlKnightTraps[sd] = 0;
        nSideTag = SIDE_TAG(sd);
        // 考虑马可以走的位置，走到棋盘边缘上，或者走到对方的控制格，都必须排除
        for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
            sqSrc = this->ucsqPieces[nSideTag + i];
            if (sqSrc != 0) {
                __ASSERT_SQUARE(sqSrc);
                nMovable = 0;
                lpucsqDst = PreGen.ucsqKnightMoves[sqSrc];
                lpucsqPin = PreGen.ucsqKnightPins[sqSrc];
                sqDst = *lpucsqDst;
                while (sqDst != 0) {
                    __ASSERT_SQUARE(sqDst);
                    // 以下的判断区别于"genmoves.cpp"中的着法生成器，排除了走到棋盘边缘和走到对方控制格的着法
                    if (!cbcEdgeSquares[sqDst] && this->ucpcSquares[sqDst] == 0 &&
                        this->ucpcSquares[*lpucsqPin] == 0 && !this->Protected(OPP_SIDE(sd), sqDst)) {
                        nMovable ++;
                        if (nMovable > 1) {
                            break;
                        }
                    }
                    lpucsqDst ++;
                    sqDst = *lpucsqDst;
                    lpucsqPin ++;
                }
                // 没有好的着法的马给予10分罚分，只有一个好的着法的马给予5分罚分
                if (nMovable == 0) {
                    vlKnightTraps[sd] += 10;
                } else if (nMovable == 1) {
                    vlKnightTraps[sd] += 5;
                }
            }
            __ASSERT(vlKnightTraps[sd] <= 20);
        }
    }
    return SIDE_VALUE(this->sdPlayer, vlKnightTraps[1] - vlKnightTraps[0]);
}

// 以上是第四部分，马受到阻碍的评价

// 局面评价过程
int PositionStruct::Evaluate(int vlAlpha, int vlBeta) const {
    int vl;
    // 偷懒的局面评价函数分以下几个层次：

    // 1. 四级偷懒评价(彻底偷懒评价)，只包括子力平衡；
    vl = this->Material();
    if (vl + EVAL_MARGIN1 <= vlAlpha) {
        return vl + EVAL_MARGIN1;
    } else if (vl - EVAL_MARGIN1 >= vlBeta) {
        return vl - EVAL_MARGIN1;
    }

    // 2. 三级偷懒评价，包括特殊棋型；
    vl += this->AdvisorShape();
    if (vl + EVAL_MARGIN2 <= vlAlpha) {
        return vl + EVAL_MARGIN2;
    } else if (vl - EVAL_MARGIN2 >= vlBeta) {
        return vl - EVAL_MARGIN2;
    }

    // 3. 二级偷懒评价，包括牵制；
    vl += this->StringHold();
    if (vl + EVAL_MARGIN3 <= vlAlpha) {
        return vl + EVAL_MARGIN3;
    } else if (vl - EVAL_MARGIN3 >= vlBeta) {
        return vl - EVAL_MARGIN3;
    }

    // 4. 一级偷懒评价，包括车的灵活性；
    vl += this->RookMobility();
    if (vl + EVAL_MARGIN4 <= vlAlpha) {
        return vl + EVAL_MARGIN4;
    } else if (vl - EVAL_MARGIN4 >= vlBeta) {
        return vl - EVAL_MARGIN4;
    }

    // 5. 零级偷懒评价(完全评价)，包括马的阻碍。
    return vl + this->KnightTrap();
}


// end of evaluate.cpp

// begin of book.cpp

/*
book.h/book.cpp - Source Code for ElephantEye, Part VI

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.25, Last Modified: Apr. 2011
Copyright (C) 2004-2011 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


int GetBookMoves(const PositionStruct &pos, const char *szBookFile, BookStruct *lpbks) {
    BookFileStruct BookFile;
    PositionStruct posScan;
    BookStruct bk;
    int nScan, nLow, nHigh, nPtr;
    int i, j, nMoves;
    // 从开局库中搜索着法的例程，有以下几个步骤：

    // 1. 打开开局库，如果打开失败，则返回空值；
    if (!BookFile.Open(szBookFile)) {
        return 0;
    }

    // 2. 用拆半查找法搜索局面；
    posScan = pos;
    for (nScan = 0; nScan < 2; nScan ++) {
        nPtr = nLow = 0;
        nHigh = BookFile.nLen - 1;
        while (nLow <= nHigh) {
            nPtr = (nLow + nHigh) / 2;
            BookFile.Read(bk, nPtr);
            if (BOOK_POS_CMP(bk, posScan) < 0) {
                nLow = nPtr + 1;
            } else if (BOOK_POS_CMP(bk, posScan) > 0) {
                nHigh = nPtr - 1;
            } else {
                break;
            }
        }
        if (nLow <= nHigh) {
            break;
        }
        // 原局面和镜像局面各搜索一趟
        posScan.Mirror();
    }

    // 3. 如果不到局面，则返回空着；
    if (nScan == 2) {
        BookFile.Close();
        return 0;
    }
    __ASSERT_BOUND(0, nPtr, BookFile.nLen - 1);

    // 4. 如果找到局面，则向前查找第一个着法；
    for (nPtr --; nPtr >= 0; nPtr --) {
        BookFile.Read(bk, nPtr);
        if (BOOK_POS_CMP(bk, posScan) < 0) {
            break;
        }
    }

    // 5. 向后依次读入属于该局面的每个着法；
    nMoves = 0;
    for (nPtr ++; nPtr < BookFile.nLen; nPtr ++) {
        BookFile.Read(bk, nPtr);
        if (BOOK_POS_CMP(bk, posScan) > 0) {
            break;
        }
        if (posScan.LegalMove(bk.wmv)) {
            // 如果局面是第二趟搜索到的，则着法必须做镜像
            lpbks[nMoves].nPtr = nPtr;
            lpbks[nMoves].wmv = (nScan == 0 ? bk.wmv : MOVE_MIRROR(bk.wmv));
            lpbks[nMoves].wvl = bk.wvl;
            nMoves ++;
            if (nMoves == MAX_GEN_MOVES) {
                break;
            }
        }
    }
    BookFile.Close();

    // 6. 对着法按分值排序
    for (i = 0; i < nMoves - 1; i ++) {
        for (j = nMoves - 1; j > i; j --) {
            if (lpbks[j - 1].wvl < lpbks[j].wvl) {
                SWAP(lpbks[j - 1], lpbks[j]);
            }
        }
    }
    return nMoves;
}


// end of book.cpp

// begin of genmoves.cpp

/*
genmoves.cpp - Source Code for ElephantEye, Part IV

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.0, Last Modified: Nov. 2007
Copyright (C) 2004-2007 www.elephantbase.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上四个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

// 本模块只涉及到"PositionStruct"中的"sdPlayer"、"ucpcSquares"和"ucsqPieces"三个成员，故省略前面的"this->"

// 棋子保护判断
bool PositionStruct::Protected(int sd, int sqSrc, int sqExcept) const {
    // 参数"sqExcept"表示排除保护的棋子(指格子编号)，考虑被牵制子的保护时，需要排除牵制目标子的保护
    int i, sqDst, sqPin, pc, x, y, nSideTag;
    SlideMaskStruct *lpsmsRank, *lpsmsFile;
    // 棋子保护判断包括以下几个步骤：

    __ASSERT_SQUARE(sqSrc);
    nSideTag = SIDE_TAG(sd);
    if (HOME_HALF(sqSrc, sd)) {
        if (IN_FORT(sqSrc)) {

            // 1. 判断受到帅(将)的保护
            sqDst = ucsqPieces[nSideTag + KING_FROM];
            if (sqDst != 0 && sqDst != sqExcept) {
                __ASSERT_SQUARE(sqDst);
                if (KING_SPAN(sqSrc, sqDst)) {
                    return true;
                }
            }

            // 2. 判断受到仕(士)的保护
            for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
                sqDst = ucsqPieces[nSideTag + i];
                if (sqDst != 0 && sqDst != sqExcept) {
                    __ASSERT_SQUARE(sqDst);
                    if (ADVISOR_SPAN(sqSrc, sqDst)) {
                        return true;
                    }
                }
            }
        }

        // 3. 判断受到相(象)的保护
        for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
            sqDst = ucsqPieces[nSideTag + i];
            if (sqDst != 0 && sqDst != sqExcept) {
                __ASSERT_SQUARE(sqDst);
                if (BISHOP_SPAN(sqSrc, sqDst) && ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0) {
                    return true;
                }
            }
        }
    } else {

        // 4. 判断受到过河兵(卒)横向的保护
        for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2) {
            // 如果棋子在边线，那么断言不成立
            // __ASSERT_SQUARE(sqDst);
            if (sqDst != sqExcept) {
                pc = ucpcSquares[sqDst];
                if ((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
                    return true;
                }
            }
        }
    }

    // 5. 判断受到兵(卒)纵向的保护
    sqDst = SQUARE_BACKWARD(sqSrc, sd);
    // 如果棋子在底线，那么断言不成立
    // __ASSERT_SQUARE(sqDst);
    if (sqDst != sqExcept) {
        pc = ucpcSquares[sqDst];
        if ((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
            return true;
        }
    }

    // 6. 判断受到马的保护
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
        sqDst = ucsqPieces[nSideTag + i];
        if (sqDst != 0 && sqDst != sqExcept) {
            __ASSERT_SQUARE(sqDst);
            sqPin = KNIGHT_PIN(sqDst, sqSrc); // 注意，sqSrc和sqDst是反的！
            if (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
                return true;
            }
        }
    }

    x = FILE_X(sqSrc);
    y = RANK_Y(sqSrc);
    lpsmsRank = RankMaskPtr(x, y);
    lpsmsFile = FileMaskPtr(x, y);

    // 7. 判断受到车的保护，参阅"position.cpp"里的"CheckedBy()"函数
    for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
        sqDst = ucsqPieces[nSideTag + i];
        if (sqDst != 0 && sqDst != sqSrc && sqDst != sqExcept) {
            if (x == FILE_X(sqDst)) {
                if ((lpsmsFile->wRookCap & PreGen.wBitFileMask[sqDst]) != 0) {
                    return true;
                }
            } else if (y == RANK_Y(sqDst)) {
                if ((lpsmsRank->wRookCap & PreGen.wBitRankMask[sqDst]) != 0) {
                    return true;
                }
            }
        }
    }

    // 8. 判断受到炮的保护，参阅"position.cpp"里的"CheckedBy()"函数
    for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
        sqDst = ucsqPieces[nSideTag + i];
        if (sqDst && sqDst != sqSrc && sqDst != sqExcept) {
            if (x == FILE_X(sqDst)) {
                if ((lpsmsFile->wCannonCap & PreGen.wBitFileMask[sqDst]) != 0) {
                    return true;
                }
            } else if (y == RANK_Y(sqDst)) {
                if ((lpsmsRank->wCannonCap & PreGen.wBitRankMask[sqDst]) != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

/* 计算MVV(LVA)值的函数
 *
 * MVV(LVA)指的是：如果被吃子无保护，那么取值MVV，否则取值MVV-LVA。
 * 由于ElephantEye的MVV(LVA)值在计算完毕后再加了1，并且有其它考虑，因此有以下几种含义：
 * a. MVV(LVA)大于1，说明被吃子价值大于攻击子(表面上是赚的)，这种吃子将首先搜索，静态搜索也将考虑这种吃子；
 * b. MVV(LVA)等于1，说明被吃子有一定价值(吃车马炮或吃过河兵卒，即便表面上是亏的，也值得一试)，静态搜索也将考虑这种吃子；
 * c. MVV(LVA)等于0，说明被吃子没有价值，静态搜索将不考虑这种吃子。
 *
 * MVV价值表"SIMPLE_VALUE"是按照帅(将)=5、车=4、马炮=3、兵(卒)=2、仕(士)相(象)=1设定的；
 * LVA价值直接体现在吃子着法生成器中。
 */
int PositionStruct::MvvLva(int sqDst, int pcCaptured, int nLva) const {
    int nMvv, nLvaAdjust;
    nMvv = SIMPLE_VALUE(pcCaptured);
    nLvaAdjust = (Protected(OPP_SIDE(sdPlayer), sqDst) ? nLva : 0);
    if (nMvv >= nLvaAdjust) {
        return nMvv - nLvaAdjust + 1;
    } else {
        return (nMvv >= 3 || HOME_HALF(sqDst, sdPlayer)) ? 1 : 0;
    }
}

// 吃子着法生成器，按MVV(LVA)设定分值
int PositionStruct::GenCapMoves(MoveStruct *lpmvs) const {
    int i, sqSrc, sqDst, pcCaptured;
    int x, y, nSideTag, nOppSideTag;
    bool bCanPromote;
    SlideMoveStruct *lpsmv;
    uint8_t *lpucsqDst, *lpucsqPin;
    MoveStruct *lpmvsCurr;
    // 生成吃子着法的过程包括以下几个步骤：

    lpmvsCurr = lpmvs;
    nSideTag = SIDE_TAG(sdPlayer);
    nOppSideTag = OPP_SIDE_TAG(sdPlayer);
    bCanPromote = PreEval.bPromotion && CanPromote();

    // 1. 生成帅(将)的着法
    sqSrc = ucsqPieces[nSideTag + KING_FROM];
    if (sqSrc != 0) {
        __ASSERT_SQUARE(sqSrc);
        lpucsqDst = PreGen.ucsqKingMoves[sqSrc];
        sqDst = *lpucsqDst;
        while (sqDst != 0) {
            __ASSERT_SQUARE(sqDst);
            // 找到一个着法后，首先判断吃到的棋子是否是对方棋子，技巧是利用"nOppSideTag"的标志(16和32颠倒)，
            // 如果是对方棋子，则保存MVV(LVA)值，即如果被吃子无保护，则只记MVV，否则记MVV-LVA(如果MVV>LVA的话)。
            pcCaptured = ucpcSquares[sqDst];
            if ((pcCaptured & nOppSideTag) != 0) {
                __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 5); // 帅(将)的价值是5
                lpmvsCurr ++;
            }
            lpucsqDst ++;
            sqDst = *lpucsqDst;
        }
    }

    // 2. 生成仕(士)的着法
    for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqAdvisorMoves[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 1); // 仕(士)的价值是1
                    lpmvsCurr ++;
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
            }
            if (bCanPromote && CAN_PROMOTE(sqSrc)) {
                lpmvsCurr->wmv = MOVE(sqSrc, sqSrc);
                lpmvsCurr->wvl = 0;
                lpmvsCurr ++;
            }
        }
    }

    // 3. 生成相(象)的着法
    for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqBishopMoves[sqSrc];
            lpucsqPin = PreGen.ucsqBishopPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                if (ucpcSquares[*lpucsqPin] == 0) {
                    pcCaptured = ucpcSquares[sqDst];
                    if ((pcCaptured & nOppSideTag) != 0) {
                        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                        lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                        lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 1); // 相(象)的价值是1
                        lpmvsCurr ++;
                    }
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
                lpucsqPin ++;
            }
            if (bCanPromote && CAN_PROMOTE(sqSrc)) {
                lpmvsCurr->wmv = MOVE(sqSrc, sqSrc);
                lpmvsCurr->wvl = 0;
                lpmvsCurr ++;
            }
        }
    }

    // 4. 生成马的着法
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqKnightMoves[sqSrc];
            lpucsqPin = PreGen.ucsqKnightPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                if (ucpcSquares[*lpucsqPin] == 0) {
                    pcCaptured = ucpcSquares[sqDst];
                    if ((pcCaptured & nOppSideTag) != 0) {
                        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                        lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                        lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 马的价值是3
                        lpmvsCurr ++;
                    }
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
                lpucsqPin ++;
            }
        }
    }

    // 5. 生成车的着法
    for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            x = FILE_X(sqSrc);
            y = RANK_Y(sqSrc);

            lpsmv = RankMovePtr(x, y);
            sqDst = lpsmv->ucRookCap[0] + RANK_DISP(y);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr ++;
                }
            }
            sqDst = lpsmv->ucRookCap[1] + RANK_DISP(y);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr ++;
                }
            }

            lpsmv = FileMovePtr(x, y);
            sqDst = lpsmv->ucRookCap[0] + FILE_DISP(x);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr ++;
                }
            }
            sqDst = lpsmv->ucRookCap[1] + FILE_DISP(x);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
                    lpmvsCurr ++;
                }
            }
        }
    }

    // 6. 生成炮的着法
    for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            x = FILE_X(sqSrc);
            y = RANK_Y(sqSrc);

            lpsmv = RankMovePtr(x, y);
            sqDst = lpsmv->ucCannonCap[0] + RANK_DISP(y);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr ++;
                }
            }
            sqDst = lpsmv->ucCannonCap[1] + RANK_DISP(y);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr ++;
                }
            }

            lpsmv = FileMovePtr(x, y);
            sqDst = lpsmv->ucCannonCap[0] + FILE_DISP(x);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr ++;
                }
            }
            sqDst = lpsmv->ucCannonCap[1] + FILE_DISP(x);
            __ASSERT_SQUARE(sqDst);
            if (sqDst != sqSrc) {
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
                    lpmvsCurr ++;
                }
            }
        }
    }

    // 7. 生成兵(卒)的着法
    for (i = PAWN_FROM; i <= PAWN_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqPawnMoves[sdPlayer][sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                pcCaptured = ucpcSquares[sqDst];
                if ((pcCaptured & nOppSideTag) != 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 2); // 兵(卒)的价值是2
                    lpmvsCurr ++;
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
            }
        }
    }
    return lpmvsCurr - lpmvs;
}

// 不吃子着法生成器
int PositionStruct::GenNonCapMoves(MoveStruct *lpmvs) const {
    int i, sqSrc, sqDst, x, y, nSideTag;
    SlideMoveStruct *lpsmv;
    uint8_t *lpucsqDst, *lpucsqPin;
    MoveStruct *lpmvsCurr;
    // 生成不吃子着法的过程包括以下几个步骤：

    lpmvsCurr = lpmvs;
    nSideTag = SIDE_TAG(sdPlayer);

    // 1. 生成帅(将)的着法
    sqSrc = ucsqPieces[nSideTag + KING_FROM];
    if (sqSrc != 0) {
        __ASSERT_SQUARE(sqSrc);
        lpucsqDst = PreGen.ucsqKingMoves[sqSrc];
        sqDst = *lpucsqDst;
        while (sqDst != 0) {
            __ASSERT_SQUARE(sqDst);
            // 找到一个着法后，首先判断是否吃到棋子
            if (ucpcSquares[sqDst] == 0) {
                __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                lpmvsCurr ++;
            }
            lpucsqDst ++;
            sqDst = *lpucsqDst;
        }
    }

    // 2. 生成仕(士)的着法
    for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqAdvisorMoves[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                if (ucpcSquares[sqDst] == 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr ++;
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
            }
        }
    }

    // 3. 生成相(象)的着法
    for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqBishopMoves[sqSrc];
            lpucsqPin = PreGen.ucsqBishopPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                if (ucpcSquares[*lpucsqPin] == 0 && ucpcSquares[sqDst] == 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr ++;
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
                lpucsqPin ++;
            }
        }
    }

    // 4. 生成马的着法
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqKnightMoves[sqSrc];
            lpucsqPin = PreGen.ucsqKnightPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                if (ucpcSquares[*lpucsqPin] == 0 && ucpcSquares[sqDst] == 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr ++;
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
                lpucsqPin ++;
            }
        }
    }

    // 5. 生成车和炮的着法，没有必要判断是否吃到本方棋子
    for (i = ROOK_FROM; i <= CANNON_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            x = FILE_X(sqSrc);
            y = RANK_Y(sqSrc);

            lpsmv = RankMovePtr(x, y);
            sqDst = lpsmv->ucNonCap[0] + RANK_DISP(y);
            __ASSERT_SQUARE(sqDst);
            while (sqDst != sqSrc) {
                __ASSERT(ucpcSquares[sqDst] == 0);
                __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                lpmvsCurr ++;
                sqDst --;
            }
            sqDst = lpsmv->ucNonCap[1] + RANK_DISP(y);
            __ASSERT_SQUARE(sqDst);
            while (sqDst != sqSrc) {
                __ASSERT(ucpcSquares[sqDst] == 0);
                __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                lpmvsCurr ++;
                sqDst ++;
            }

            lpsmv = FileMovePtr(x, y);
            sqDst = lpsmv->ucNonCap[0] + FILE_DISP(x);
            __ASSERT_SQUARE(sqDst);
            while (sqDst != sqSrc) {
                __ASSERT(ucpcSquares[sqDst] == 0);
                __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                lpmvsCurr ++;
                sqDst -= 16;
            }
            sqDst = lpsmv->ucNonCap[1] + FILE_DISP(x);
            __ASSERT_SQUARE(sqDst);
            while (sqDst != sqSrc) {
                __ASSERT(ucpcSquares[sqDst] == 0);
                __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                lpmvsCurr ++;
                sqDst += 16;
            }
        }
    }

    // 6. 生成兵(卒)的着法
    for (i = PAWN_FROM; i <= PAWN_TO; i ++) {
        sqSrc = ucsqPieces[nSideTag + i];
        if (sqSrc != 0) {
            __ASSERT_SQUARE(sqSrc);
            lpucsqDst = PreGen.ucsqPawnMoves[sdPlayer][sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                if (ucpcSquares[sqDst] == 0) {
                    __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
                    lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
                    lpmvsCurr ++;
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
            }
        }
    }
    return lpmvsCurr - lpmvs;
}

// “捉”的检测
int PositionStruct::ChasedBy(int mv) const {
    int i, nSideTag, pcMoved, pcCaptured;
    int sqSrc, sqDst, x, y;
    uint8_t *lpucsqDst, *lpucsqPin;
    SlideMoveStruct *lpsmv;

    sqSrc = DST(mv);
    pcMoved = this->ucpcSquares[sqSrc];
    nSideTag = SIDE_TAG(this->sdPlayer);
    __ASSERT_SQUARE(sqSrc);
    __ASSERT_PIECE(pcMoved);
    __ASSERT_BOUND(0, pcMoved - OPP_SIDE_TAG(this->sdPlayer), 15);

    // “捉”的判断包括以下几部分内容：
    switch (pcMoved - OPP_SIDE_TAG(this->sdPlayer)) {

        // 1. 走了马，判断是否捉车或捉有根的炮兵(卒)
        case KNIGHT_FROM:
        case KNIGHT_TO:
            // 逐一检测马踩的八个位置
            lpucsqDst = PreGen.ucsqKnightMoves[sqSrc];
            lpucsqPin = PreGen.ucsqKnightPins[sqSrc];
            sqDst = *lpucsqDst;
            while (sqDst != 0) {
                __ASSERT_SQUARE(sqDst);
                if (ucpcSquares[*lpucsqPin] == 0) {
                    pcCaptured = this->ucpcSquares[sqDst];
                    if ((pcCaptured & nSideTag) != 0) {
                        pcCaptured -= nSideTag;
                        __ASSERT_BOUND(0, pcCaptured, 15);
                        // 技巧：优化兵种判断的分枝
                        if (pcCaptured <= ROOK_TO) {
                            // 马捉仕(士)、相(象)和马的情况不予考虑
                            if (pcCaptured >= ROOK_FROM) {
                                // 马捉到了车
                                return pcCaptured;
                            }
                        } else {
                            if (pcCaptured <= CANNON_TO) {
                                // 马捉到了炮，要判断炮是否受保护
                                if (!Protected(this->sdPlayer, sqDst)) {
                                    return pcCaptured;
                                }
                            } else {
                                // 马捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                                    return pcCaptured;
                                }
                            }
                        }
                    }
                }
                lpucsqDst ++;
                sqDst = *lpucsqDst;
                lpucsqPin ++;
            }
            break;

            // 2. 走了车，判断是否捉有根的马炮兵(卒)
        case ROOK_FROM:
        case ROOK_TO:
            x = FILE_X(sqSrc);
            y = RANK_Y(sqSrc);
            if (((SRC(mv) ^ sqSrc) & 0xf) == 0) {
                // 如果车纵向移动了，则判断车横向吃到的子
                lpsmv = RankMovePtr(x, y);
                for (i = 0; i < 2; i ++) {
                    sqDst = lpsmv->ucRookCap[i] + RANK_DISP(y);
                    __ASSERT_SQUARE(sqDst);
                    if (sqDst != sqSrc) {
                        pcCaptured = this->ucpcSquares[sqDst];
                        if ((pcCaptured & nSideTag) != 0) {
                            pcCaptured -= nSideTag;
                            __ASSERT_BOUND(0, pcCaptured, 15);
                            // 技巧：优化兵种判断的分枝
                            if (pcCaptured <= ROOK_TO) {
                                // 车捉仕(士)、相(象)的情况不予考虑
                                if (pcCaptured >= KNIGHT_FROM) {
                                    if (pcCaptured <= KNIGHT_TO) {
                                        // 车捉到了马，要判断马是否受保护
                                        if (!Protected(this->sdPlayer, sqDst)) {
                                            return pcCaptured;
                                        }
                                    }
                                    // 车捉车的情况不予考虑
                                }
                            } else {
                                if (pcCaptured <= CANNON_TO) {
                                    // 车捉到了炮，要判断炮是否受保护
                                    if (!Protected(this->sdPlayer, sqDst)) {
                                        return pcCaptured;
                                    }
                                } else {
                                    // 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                    if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                                        return pcCaptured;
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                // 如果车横向移动了，则判断车纵向吃到的子
                lpsmv = FileMovePtr(x, y);
                for (i = 0; i < 2; i ++) {
                    sqDst = lpsmv->ucRookCap[i] + FILE_DISP(x);
                    __ASSERT_SQUARE(sqDst);
                    if (sqDst != sqSrc) {
                        pcCaptured = this->ucpcSquares[sqDst];
                        if ((pcCaptured & nSideTag) != 0) {
                            pcCaptured -= nSideTag;
                            __ASSERT_BOUND(0, pcCaptured, 15);
                            // 技巧：优化兵种判断的分枝
                            if (pcCaptured <= ROOK_TO) {
                                // 车捉仕(士)、相(象)的情况不予考虑
                                if (pcCaptured >= KNIGHT_FROM) {
                                    if (pcCaptured <= KNIGHT_TO) {
                                        // 车捉到了马，要判断马是否受保护
                                        if (!Protected(this->sdPlayer, sqDst)) {
                                            return pcCaptured;
                                        }
                                    }
                                    // 车捉车的情况不予考虑
                                }
                            } else {
                                if (pcCaptured <= CANNON_TO) {
                                    // 车捉到了炮，要判断炮是否受保护
                                    if (!Protected(this->sdPlayer, sqDst)) {
                                        return pcCaptured;
                                    }
                                } else {
                                    // 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                    if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                                        return pcCaptured;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;

            // 3. 走了炮，判断是否捉车或捉有根的马兵(卒)
        case CANNON_FROM:
        case CANNON_TO:
            x = FILE_X(sqSrc);
            y = RANK_Y(sqSrc);
            if (((SRC(mv) ^ sqSrc) & 0xf) == 0) {
                // 如果炮纵向移动了，则判断炮横向吃到的子
                lpsmv = RankMovePtr(x, y);
                for (i = 0; i < 2; i ++) {
                    sqDst = lpsmv->ucCannonCap[i] + RANK_DISP(y);
                    __ASSERT_SQUARE(sqDst);
                    if (sqDst != sqSrc) {
                        pcCaptured = this->ucpcSquares[sqDst];
                        if ((pcCaptured & nSideTag) != 0) {
                            pcCaptured -= nSideTag;
                            __ASSERT_BOUND(0, pcCaptured, 15);
                            // 技巧：优化兵种判断的分枝
                            if (pcCaptured <= ROOK_TO) {
                                // 炮捉仕(士)、相(象)的情况不予考虑
                                if (pcCaptured >= KNIGHT_FROM) {
                                    if (pcCaptured <= KNIGHT_TO) {
                                        // 炮捉到了马，要判断马是否受保护
                                        if (!Protected(this->sdPlayer, sqDst)) {
                                            return pcCaptured;
                                        }
                                    } else {
                                        // 炮捉到了车
                                        return pcCaptured;
                                    }
                                }
                            } else {
                                // 炮捉炮的情况不予考虑
                                if (pcCaptured >= PAWN_FROM) {
                                    // 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                    if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                                        return pcCaptured;
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                // 如果炮横向移动了，则判断炮纵向吃到的子
                lpsmv = FileMovePtr(x, y);
                for (i = 0; i < 2; i ++) {
                    sqDst = lpsmv->ucCannonCap[i] + FILE_DISP(x);
                    __ASSERT_SQUARE(sqDst);
                    if (sqDst != sqSrc) {
                        pcCaptured = this->ucpcSquares[sqDst];
                        if ((pcCaptured & nSideTag) != 0) {
                            pcCaptured -= nSideTag;
                            __ASSERT_BOUND(0, pcCaptured, 15);
                            // 技巧：优化兵种判断的分枝
                            if (pcCaptured <= ROOK_TO) {
                                // 炮捉仕(士)、相(象)的情况不予考虑
                                if (pcCaptured >= KNIGHT_FROM) {
                                    if (pcCaptured <= KNIGHT_TO) {
                                        // 炮捉到了马，要判断马是否受保护
                                        if (!Protected(this->sdPlayer, sqDst)) {
                                            return pcCaptured;
                                        }
                                    } else {
                                        // 炮捉到了车
                                        return pcCaptured;
                                    }
                                }
                            } else {
                                // 炮捉炮的情况不予考虑
                                if (pcCaptured >= PAWN_FROM) {
                                    // 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                                    if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                                        return pcCaptured;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
    }

    return 0;
}


// end of genmoves.cpp

// begin of movesort.cpp

/*
movesort.h/movesort.cpp - Source Code for ElephantEye, Part VII

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.11, Last Modified: Dec. 2007
Copyright (C) 2004-2007 www.elephantbase.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


int nHistory[65536]; // 历史表

// 根据历史表对着法列表赋值
void MoveSortStruct::SetHistory(void) {
    int i, j, vl, nShift, nNewShift;
    nShift = 0;
    for (i = nMoveIndex; i < nMoveNum; i ++) {
        // 如果着法的分值超过65536，就必需对所有着法的分值作缩减，使它们都不超过65536
        vl = nHistory[mvs[i].wmv] >> nShift;
        if (vl > 65535) {
            nNewShift = Bsr(vl) - 15;
            for (j = nMoveIndex; j < i; j ++) {
                mvs[j].wvl >>= nNewShift;
            }
            vl >>= nNewShift;
            __ASSERT_BOUND(32768, vl, 65535);
            nShift += nNewShift;
        }
        mvs[i].wvl = vl;
    }
}

// Shell排序法，这里用"1, 4, 13, 40 ..."的序列，这样要比"1, 2, 4, 8, ..."好
static const int cnShellStep[8] = {0, 1, 4, 13, 40, 121, 364, 1093};

void MoveSortStruct::ShellSort(void) {
    int i, j, nStep, nStepLevel;
    MoveStruct mvsBest;
    nStepLevel = 1;
    while (cnShellStep[nStepLevel] < nMoveNum - nMoveIndex) {
        nStepLevel ++;
    }
    nStepLevel --;
    while (nStepLevel > 0) {
        nStep = cnShellStep[nStepLevel];
        for (i = nMoveIndex + nStep; i < nMoveNum; i ++) {
            mvsBest = mvs[i];
            j = i - nStep;
            while (j >= nMoveIndex && mvsBest.wvl > mvs[j].wvl) {
                mvs[j + nStep] = mvs[j];
                j -= nStep;
            }
            mvs[j + nStep] = mvsBest;
        }
        nStepLevel --;
    }
}

/* 生成解将着法，返回唯一应将着法(有多个应将着法则返回零)
 *
 * 解将着法的顺序如下：
 * 1. 置换表着法(SORT_VALUE_MAX)；
 * 2. 两个杀手着法(SORT_VALUE_MAX - 1或2)；
 * 3. 其他着法，按历史表排序(从1到SORT_VALUE_MAX - 3)；
 * 4. 不能解将的着法(0)，这些着法会过滤掉。
 */
int MoveSortStruct::InitEvade(PositionStruct &pos, int mv, const uint16_t *lpwmvKiller) {
    int i, nLegal;
    nPhase = PHASE_REST;
    nMoveIndex = 0;
    nMoveNum = pos.GenAllMoves(mvs);
    SetHistory();
    nLegal = 0;
    for (i = nMoveIndex; i < nMoveNum; i ++) {
        if (mvs[i].wmv == mv) {
            nLegal ++;
            mvs[i].wvl = SORT_VALUE_MAX;
        } else if (pos.MakeMove(mvs[i].wmv)) {
            pos.UndoMakeMove();
            nLegal ++;
            if (mvs[i].wmv == lpwmvKiller[0]) {
                mvs[i].wvl = SORT_VALUE_MAX - 1;
            } else if (mvs[i].wmv == lpwmvKiller[1]) {
                mvs[i].wvl = SORT_VALUE_MAX - 2;
            } else {
                mvs[i].wvl = MIN(mvs[i].wvl + 1, SORT_VALUE_MAX - 3);
            }
        } else {
            mvs[i].wvl = 0;
        }
    }
    ShellSort();
    nMoveNum = nMoveIndex + nLegal;
    return (nLegal == 1 ? mvs[0].wmv : 0);
}

// 给出下一个即将搜索的着法
int MoveSortStruct::NextFull(const PositionStruct &pos) {
    switch (nPhase) {
        // "nPhase"表示着法启发的若干阶段，依次为：

        // 0. 置换表着法启发，完成后立即进入下一阶段；
        case PHASE_HASH:
            nPhase = PHASE_GEN_CAP;
            if (mvHash != 0) {
                __ASSERT(pos.LegalMove(mvHash));
                return mvHash;
            }
            // 技巧：这里没有"break"，表示"switch"的上一个"case"执行完后紧接着做下一个"case"，下同

            // 1. 生成所有吃子着法，完成后立即进入下一阶段；
        case PHASE_GEN_CAP:
            nPhase = PHASE_GOODCAP;
            nMoveIndex = 0;
            nMoveNum = pos.GenCapMoves(mvs);
            ShellSort();

            // 2. MVV(LVA)启发，可能要循环若干次；
        case PHASE_GOODCAP:
            if (nMoveIndex < nMoveNum && mvs[nMoveIndex].wvl > 1) {
                // 注意：MVV(LVA)值不超过1，则说明吃子不是直接能获得优势的，这些着法将留在以后搜索
                nMoveIndex ++;
                __ASSERT_PIECE(pos.ucpcSquares[DST(mvs[nMoveIndex - 1].wmv)]);
                return mvs[nMoveIndex - 1].wmv;
            }

            // 3. 杀手着法启发(第一个杀手着法)，完成后立即进入下一阶段；
        case PHASE_KILLER1:
            nPhase = PHASE_KILLER2;
            if (mvKiller1 != 0 && pos.LegalMove(mvKiller1)) {
                // 注意：杀手着法必须检验着法合理性，下同
                return mvKiller1;
            }

            // 4. 杀手着法启发(第二个杀手着法)，完成后立即进入下一阶段；
        case PHASE_KILLER2:
            nPhase = PHASE_GEN_NONCAP;
            if (mvKiller2 != 0 && pos.LegalMove(mvKiller2)) {
                return mvKiller2;
            }

            // 5. 生成所有不吃子着法，完成后立即进入下一阶段；
        case PHASE_GEN_NONCAP:
            nPhase = PHASE_REST;
            nMoveNum += pos.GenNonCapMoves(mvs + nMoveNum);
            SetHistory();
            ShellSort();

            // 6. 对剩余着法做历史表启发(包括返回解将着法)；
        case PHASE_REST:
            if (nMoveIndex < nMoveNum) {
                nMoveIndex ++;
                return mvs[nMoveIndex - 1].wmv;
            }

            // 7. 没有着法了，返回零。
        default:
            return 0;
    }
}

// 生成根结点的着法
void MoveSortStruct::InitRoot(const PositionStruct &pos, int nBanMoves, const uint16_t *lpwmvBanList) {
    int i, j, nBanned;
    nMoveIndex = 0;
    nMoveNum = pos.GenAllMoves(mvs);
    nBanned = 0;
    for (i = 0; i < nMoveNum; i ++) {
        mvs[i].wvl = 1;
        for (j = 0; j < nBanMoves; j ++) {
            if (mvs[i].wmv == lpwmvBanList[j]) {
                mvs[i].wvl = 0;
                nBanned ++;
                break;
            }
        }
    }
    ShellSort();
    nMoveNum -= nBanned;
}

// 更新根结点的着法排序列表
void MoveSortStruct::UpdateRoot(int mv) {
    int i;
    for (i = 0; i < nMoveNum; i ++) {
        if (mvs[i].wmv == mv) {
            mvs[i].wvl = SORT_VALUE_MAX;
        } else if (mvs[i].wvl > 0) {
            mvs[i].wvl --;
        }
    }
}


// end of movesort.cpp

// begin of pregen.cpp

/*
pregen.h/pregen.cpp - Source Code for ElephantEye, Part II

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.0, Last Modified: Nov. 2007
Copyright (C) 2004-2007 www.elephantbase.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>

const bool cbcInBoard[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const bool cbcInFort[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const bool cbcCanPromote[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const int8_t ccLegalSpanTab[512] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0
};

const int8_t ccKnightPinTab[512] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0
};

PreGenStruct PreGen;
PreEvalStruct PreEval;

// 这四个数组用来判断棋子的走子方向，以马为例就是：sqDst = sqSrc + cnKnightMoveTab[i]
static const int cnKingMoveTab[4]    = {-0x10, -0x01, +0x01, +0x10};
static const int cnAdvisorMoveTab[4] = {-0x11, -0x0f, +0x0f, +0x11};
static const int cnBishopMoveTab[4]  = {-0x22, -0x1e, +0x1e, +0x22};
static const int cnKnightMoveTab[8]  = {-0x21, -0x1f, -0x12, -0x0e, +0x0e, +0x12, +0x1f, +0x21};

void PreGenInit(void) {
    int i, j, k, n, sqSrc, sqDst;
    RC4Struct rc4;
    SlideMoveStruct smv;
    SlideMaskStruct sms;

    // 首先初始化Zobrist键值表
    rc4.InitZero();
    PreGen.zobrPlayer.InitRC4(rc4);
    for (i = 0; i < 14; i ++) {
        for (j = 0; j < 256; j ++) {
            PreGen.zobrTable[i][j].InitRC4(rc4);
        }
    }

    // 然后初始化屏蔽位行和屏蔽位列
    // 注：位行和位列不包括棋盘以外的位，所以就会频繁使用"+/- RANK_TOP/FILE_LEFT"
    for (sqSrc = 0; sqSrc < 256; sqSrc ++) {
        if (IN_BOARD(sqSrc)) {
            PreGen.wBitRankMask[sqSrc] = 1 << (FILE_X(sqSrc) - FILE_LEFT);
            PreGen.wBitFileMask[sqSrc] = 1 << (RANK_Y(sqSrc) - RANK_TOP);
        } else {
            PreGen.wBitRankMask[sqSrc] = 0;
            PreGen.wBitFileMask[sqSrc] = 0;
        }
    }

    // 然后生成车炮横向的预置数组(数组的应用参阅"pregen.h")
    for (i = 0; i < 9; i ++) {
        for (j = 0; j < 512; j ++) {
            // 初始化借助于“位行”的车和炮的着法预生成数组，包括以下几个步骤：
            // 1. 初始化临时变量"SlideMoveTab"，假设没有着法，用起始格填充
            smv.ucNonCap[0] = smv.ucNonCap[1] = smv.ucRookCap[0] = smv.ucRookCap[1] =
            smv.ucCannonCap[0] = smv.ucCannonCap[1] = smv.ucSuperCap[0] = smv.ucSuperCap[1] = i + FILE_LEFT;
            sms.wNonCap = sms.wRookCap = sms.wCannonCap = sms.wSuperCap = 0;
            // 提示：参阅"pregen.h"，...[0]表示最大一格，向右移动和下移动都用[0]，反之亦然
            // 2. 考虑向右移动的目标格，填充...[0]，
            for (k = i + 1; k <= 8; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.ucRookCap[0] = FILE_DISP(k + FILE_LEFT);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[0] = FILE_DISP(k + FILE_LEFT);
                sms.wNonCap |= 1 << k;
            }
            for (k ++; k <= 8; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.ucCannonCap[0] = FILE_DISP(k + FILE_LEFT);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k ++; k <= 8; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.ucSuperCap[0] = FILE_DISP(k + FILE_LEFT);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 3. 考虑向左移动的目标格，填充...[1]
            for (k = i - 1; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.ucRookCap[1] = FILE_DISP(k + FILE_LEFT);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[1] = FILE_DISP(k + FILE_LEFT);
                sms.wNonCap |= 1 << k;
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.ucCannonCap[1] = FILE_DISP(k + FILE_LEFT);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.ucSuperCap[1] = FILE_DISP(k + FILE_LEFT);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 4. 为"smv"和"sms"的值作断言
            __ASSERT_BOUND_2(3, smv.ucNonCap[1], smv.ucNonCap[0], 11);
            __ASSERT_BOUND_2(3, smv.ucRookCap[1], smv.ucRookCap[0], 11);
            __ASSERT_BOUND_2(3, smv.ucCannonCap[1], smv.ucCannonCap[0], 11);
            __ASSERT_BOUND_2(3, smv.ucSuperCap[1], smv.ucSuperCap[0], 11);
            __ASSERT_BITRANK(sms.wNonCap);
            __ASSERT_BITRANK(sms.wRookCap);
            __ASSERT_BITRANK(sms.wCannonCap);
            __ASSERT_BITRANK(sms.wSuperCap);
            // 5. 将临时变量"smv"和"sms"拷贝到着法预生成数组中
            PreGen.smvRankMoveTab[i][j] = smv;
            PreGen.smsRankMaskTab[i][j] = sms;
        }
    }

    // 然后生成车炮纵向的预置数组(数组的应用参阅"pregen.h")
    for (i = 0; i < 10; i ++) {
        for (j = 0; j < 1024; j ++) {
            // 初始化借助于“位列”的车和炮的着法预生成数组，包括以下几个步骤：
            // 1. 初始化临时变量"smv"，假设没有着法，用起始格填充
            smv.ucNonCap[0] = smv.ucNonCap[1] = smv.ucRookCap[0] = smv.ucRookCap[1] =
            smv.ucCannonCap[0] = smv.ucCannonCap[1] = smv.ucSuperCap[0] = smv.ucSuperCap[1] = (i + RANK_TOP) * 16;
            sms.wNonCap = sms.wRookCap = sms.wCannonCap = sms.wSuperCap = 0;
            // 2. 考虑向下移动的目标格，填充...[0]
            for (k = i + 1; k <= 9; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.ucRookCap[0] = RANK_DISP(k + RANK_TOP);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[0] = RANK_DISP(k + RANK_TOP);
                sms.wNonCap |= 1 << k;
            }
            for (k ++; k <= 9; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.ucCannonCap[0] = RANK_DISP(k + RANK_TOP);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k ++; k <= 9; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.ucSuperCap[0] = RANK_DISP(k + RANK_TOP);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 3. 考虑向上移动的目标格，填充...[1]
            for (k = i - 1; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.ucRookCap[1] = RANK_DISP(k + RANK_TOP);
                    sms.wRookCap |= 1 << k;
                    break;
                }
                smv.ucNonCap[1] = RANK_DISP(k + RANK_TOP);
                sms.wNonCap |= 1 << k;
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.ucCannonCap[1] = RANK_DISP(k + RANK_TOP);
                    sms.wCannonCap |= 1 << k;
                    break;
                }
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.ucSuperCap[1] = RANK_DISP(k + RANK_TOP);
                    sms.wSuperCap |= 1 << k;
                    break;
                }
            }
            // 4. 为"smv"和"sms"的值作断言
            __ASSERT_BOUND_2(3, smv.ucNonCap[1] >> 4, smv.ucNonCap[0] >> 4, 12);
            __ASSERT_BOUND_2(3, smv.ucRookCap[1] >> 4, smv.ucRookCap[0] >> 4, 12);
            __ASSERT_BOUND_2(3, smv.ucCannonCap[1] >> 4, smv.ucCannonCap[0] >> 4, 12);
            __ASSERT_BOUND_2(3, smv.ucSuperCap[1] >> 4, smv.ucSuperCap[0] >> 4, 12);
            __ASSERT_BITFILE(sms.wNonCap);
            __ASSERT_BITFILE(sms.wRookCap);
            __ASSERT_BITFILE(sms.wCannonCap);
            __ASSERT_BITFILE(sms.wSuperCap);
            // 5. 将临时变量"smv"和"sms"拷贝到着法预生成数组中
            PreGen.smvFileMoveTab[i][j] = smv;
            PreGen.smsFileMaskTab[i][j] = sms;
        }
    }

    // 接下来生成着法预生成数组，连同将军预判数组
    for (sqSrc = 0; sqSrc < 256; sqSrc ++) {
        if (IN_BOARD(sqSrc)) {
            // 生成帅(将)的着法预生成数组
            n = 0;
            for (i = 0; i < 4; i ++) {
                sqDst = sqSrc + cnKingMoveTab[i];
                if (IN_FORT(sqDst)) {
                    PreGen.ucsqKingMoves[sqSrc][n] = sqDst;
                    n ++;
                }
            }
            __ASSERT(n <= 4);
            PreGen.ucsqKingMoves[sqSrc][n] = 0;
            // 生成仕(士)的着法预生成数组
            n = 0;
            for (i = 0; i < 4; i ++) {
                sqDst = sqSrc + cnAdvisorMoveTab[i];
                if (IN_FORT(sqDst)) {
                    PreGen.ucsqAdvisorMoves[sqSrc][n] = sqDst;
                    n ++;
                }
            }
            __ASSERT(n <= 4);
            PreGen.ucsqAdvisorMoves[sqSrc][n] = 0;
            // 生成相(象)的着法预生成数组，包括象眼数组
            n = 0;
            for (i = 0; i < 4; i ++) {
                sqDst = sqSrc + cnBishopMoveTab[i];
                if (IN_BOARD(sqDst) && SAME_HALF(sqSrc, sqDst)) {
                    PreGen.ucsqBishopMoves[sqSrc][n] = sqDst;
                    PreGen.ucsqBishopPins[sqSrc][n] = BISHOP_PIN(sqSrc, sqDst);
                    n ++;
                }
            }
            __ASSERT(n <= 4);
            PreGen.ucsqBishopMoves[sqSrc][n] = 0;
            // 生成马的着法预生成数组，包括马腿数组
            n = 0;
            for (i = 0; i < 8; i ++) {
                sqDst = sqSrc + cnKnightMoveTab[i];
                if (IN_BOARD(sqDst)) {
                    PreGen.ucsqKnightMoves[sqSrc][n] = sqDst;
                    PreGen.ucsqKnightPins[sqSrc][n] = KNIGHT_PIN(sqSrc, sqDst);
                    n ++;
                }
            }
            __ASSERT(n <= 8);
            PreGen.ucsqKnightMoves[sqSrc][n] = 0;
            // 生成兵(卒)的着法预生成数组
            for (i = 0; i < 2; i ++) {
                n = 0;
                sqDst = SQUARE_FORWARD(sqSrc, i);
                sqDst = sqSrc + (i == 0 ? -16 : 16);
                if (IN_BOARD(sqDst)) {
                    PreGen.ucsqPawnMoves[i][sqSrc][n] = sqDst;
                    n ++;
                }
                if (AWAY_HALF(sqSrc, i)) {
                    for (j = -1; j <= 1; j += 2) {
                        sqDst = sqSrc + j;
                        if (IN_BOARD(sqDst)) {
                            PreGen.ucsqPawnMoves[i][sqSrc][n] = sqDst;
                            n ++;
                        }
                    }
                }
                __ASSERT(n <= 3);
                PreGen.ucsqPawnMoves[i][sqSrc][n] = 0;
            }
        }
    }

    // 最后清空局面预评价结构
    memset(&PreEval, 0, sizeof(PreEvalStruct));
    PreEval.bPromotion = false; // 缺省是不允许升变的
}


// end of pregen.cpp

// begin of position.cpp

/*
position.h/position.cpp - Source Code for ElephantEye, Part III

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.0, Last Modified: Nov. 2007
Copyright (C) 2004-2007 www.elephantbase.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上五个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

// 本模块涉及多个"PositionStruct"的成员，用"this->"标记出来以方便辨认

// 起始局面的FEN串
const char *const cszStartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";

// 棋子类型对应的棋子符号
const char *const cszPieceBytes = "KABNRCP";

/* 棋子序号对应的棋子类型
 *
 * ElephantEye的棋子序号从0到47，其中0到15不用，16到31表示红子，32到47表示黑子。
 * 每方的棋子顺序依次是：帅仕仕相相马马车车炮炮兵兵兵兵兵(将士士象象马马车车炮炮卒卒卒卒卒)
 * 提示：判断棋子是红子用"pc < 32"，黑子用"pc >= 32"
 */
const int cnPieceTypes[48] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6,
        0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
};

// 棋子的简单分值，只在简单比较时作参考
const int cnSimpleValues[48] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
        5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
};

// 该数组很方便地实现了坐标的镜像(左右对称)
const uint8_t cucsqMirrorTab[256] = {
        0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0, 0, 0, 0,
        0, 0, 0, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0, 0, 0, 0,
        0, 0, 0, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54, 0x53, 0, 0, 0, 0,
        0, 0, 0, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0, 0, 0, 0,
        0, 0, 0, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0, 0, 0, 0,
        0, 0, 0, 0x8b, 0x8a, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0, 0, 0, 0,
        0, 0, 0, 0x9b, 0x9a, 0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0, 0, 0, 0,
        0, 0, 0, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0, 0, 0, 0,
        0, 0, 0, 0xbb, 0xba, 0xb9, 0xb8, 0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 0, 0, 0, 0,
        0, 0, 0, 0xcb, 0xca, 0xc9, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0, 0, 0, 0,
        0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
};

// FEN串中棋子标识，注意这个函数只能识别大写字母，因此用小写字母时，首先必须转换为大写
int FenPiece(int nArg) {
    switch (nArg) {
        case 'K':
            return 0;
        case 'A':
            return 1;
        case 'B':
        case 'E':
            return 2;
        case 'N':
        case 'H':
            return 3;
        case 'R':
            return 4;
        case 'C':
            return 5;
        case 'P':
            return 6;
        default:
            return 7;
    }
}

// 以下是一些棋盘处理过程

// 棋盘上增加棋子
void PositionStruct::AddPiece(int sq, int pc, bool bDel) {
    int pt;

    __ASSERT_SQUARE(sq);
    __ASSERT_PIECE(pc);
    if (bDel) {
        this->ucpcSquares[sq] = 0;
        this->ucsqPieces[pc] = 0;
    } else {
        this->ucpcSquares[sq] = pc;
        this->ucsqPieces[pc] = sq;
    }
    this->wBitRanks[RANK_Y(sq)] ^= PreGen.wBitRankMask[sq];
    this->wBitFiles[FILE_X(sq)] ^= PreGen.wBitFileMask[sq];
    __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sq)]);
    __ASSERT_BITFILE(this->wBitRanks[FILE_X(sq)]);
    this->dwBitPiece ^= BIT_PIECE(pc);
    pt = PIECE_TYPE(pc);
    if (pc < 32) {
        if (bDel) {
            this->vlWhite -= PreEval.ucvlWhitePieces[pt][sq];
        } else {
            this->vlWhite += PreEval.ucvlWhitePieces[pt][sq];
        }
    } else {
        if (bDel) {
            this->vlBlack -= PreEval.ucvlBlackPieces[pt][sq];
        } else {
            this->vlBlack += PreEval.ucvlBlackPieces[pt][sq];
        }
        pt += 7;
    }
    __ASSERT_BOUND(0, pt, 13);
    this->zobr.Xor(PreGen.zobrTable[pt][sq]);
}

// 移动棋子
int PositionStruct::MovePiece(int mv) {
    int sqSrc, sqDst, pcMoved, pcCaptured, pt;
    uint8_t *lpucvl;
    // 移动棋子包括以下几个步骤：

    // 1. 得到移动的棋子序号和被吃的棋子序号；
    sqSrc = SRC(mv);
    sqDst = DST(mv);
    pcMoved = this->ucpcSquares[sqSrc];
    __ASSERT_SQUARE(sqSrc);
    __ASSERT_SQUARE(sqDst);
    __ASSERT_PIECE(pcMoved);
    pcCaptured = this->ucpcSquares[sqDst];
    if (pcCaptured == 0) {

        // 2. 如果没有被吃的棋子，那么更新目标格的位行和位列。
        //    换句话说，有被吃的棋子，目标格的位行和位列就不必更新了。
        this->wBitRanks[RANK_Y(sqDst)] ^= PreGen.wBitRankMask[sqDst];
        this->wBitFiles[FILE_X(sqDst)] ^= PreGen.wBitFileMask[sqDst];
        __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqDst)]);
        __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqDst)]);
    } else {

        __ASSERT_PIECE(pcCaptured);
        // 3. 如果有被吃的棋子，那么更新棋子位，从"ucsqPieces"数组中清除被吃棋子
        //    同时更新子力价值、位行位列、Zobrist键值和校验锁
        this->ucsqPieces[pcCaptured] = 0;
        this->dwBitPiece ^= BIT_PIECE(pcCaptured);
        pt = PIECE_TYPE(pcCaptured);
        if (pcCaptured < 32) {
            this->vlWhite -= PreEval.ucvlWhitePieces[pt][sqDst];
        } else {
            this->vlBlack -= PreEval.ucvlBlackPieces[pt][sqDst];
            pt += 7;
        }
        __ASSERT_BOUND(0, pt, 13);
        this->zobr.Xor(PreGen.zobrTable[pt][sqDst]);
    }

    // 4. 从"ucpcSquares"和"ucsqPieces"数组中移动棋子，注意“格子-棋子联系数组”移动棋子的方法
    //    同时更新位行、位列、子力价值、位行位列、Zobrist键值和校验锁
    this->ucpcSquares[sqSrc] = 0;
    this->ucpcSquares[sqDst] = pcMoved;
    this->ucsqPieces[pcMoved] = sqDst;
    this->wBitRanks[RANK_Y(sqSrc)] ^= PreGen.wBitRankMask[sqSrc];
    this->wBitFiles[FILE_X(sqSrc)] ^= PreGen.wBitFileMask[sqSrc];
    __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqSrc)]);
    __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqSrc)]);
    pt = PIECE_TYPE(pcMoved);
    if (pcMoved < 32) {
        lpucvl = PreEval.ucvlWhitePieces[pt];
        this->vlWhite += lpucvl[sqDst] - lpucvl[sqSrc];
    } else {
        lpucvl = PreEval.ucvlBlackPieces[pt];
        this->vlBlack += lpucvl[sqDst] - lpucvl[sqSrc];
        pt += 7;
    }
    __ASSERT_BOUND(0, pt, 13);
    this->zobr.Xor(PreGen.zobrTable[pt][sqDst], PreGen.zobrTable[pt][sqSrc]);
    return pcCaptured;
}

// 撤消移动棋子
void PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
    int sqSrc, sqDst, pcMoved;
    sqSrc = SRC(mv);
    sqDst = DST(mv);
    pcMoved = this->ucpcSquares[sqDst];
    __ASSERT_SQUARE(sqSrc);
    __ASSERT_SQUARE(sqDst);
    __ASSERT_PIECE(pcMoved);
    this->ucpcSquares[sqSrc] = pcMoved;
    this->ucsqPieces[pcMoved] = sqSrc;
    this->wBitRanks[RANK_Y(sqSrc)] ^= PreGen.wBitRankMask[sqSrc];
    this->wBitFiles[FILE_X(sqSrc)] ^= PreGen.wBitFileMask[sqSrc];
    __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqSrc)]);
    __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqSrc)]);
    if (pcCaptured > 0) {
        __ASSERT_PIECE(pcCaptured);
        this->ucpcSquares[sqDst] = pcCaptured;
        this->ucsqPieces[pcCaptured] = sqDst;
        this->dwBitPiece ^= BIT_PIECE(pcCaptured);
    } else {
        this->ucpcSquares[sqDst] = 0;
        this->wBitRanks[RANK_Y(sqDst)] ^= PreGen.wBitRankMask[sqDst];
        this->wBitFiles[FILE_X(sqDst)] ^= PreGen.wBitFileMask[sqDst];
        __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqDst)]);
        __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqDst)]);
    }
}

// 升变
int PositionStruct::Promote(int sq) {
    int pcCaptured, pcPromoted, pt;
    // 升变包括以下几个步骤：

    // 1. 得到升变前棋子的序号；
    __ASSERT_SQUARE(sq);
    __ASSERT(CanPromote());
    __ASSERT(CAN_PROMOTE(sq));
    pcCaptured = this->ucpcSquares[sq];
    __ASSERT_PIECE(pcCaptured);
    pcPromoted = SIDE_TAG(this->sdPlayer) + Bsf(~this->wBitPiece[this->sdPlayer] & PAWN_BITPIECE);
    __ASSERT_PIECE(pcPromoted);
    __ASSERT(this->ucsqPieces[pcPromoted] == 0);

    // 2. 去掉升变前棋子，同时更新子力价值、Zobrist键值和校验锁
    this->dwBitPiece ^= BIT_PIECE(pcPromoted) ^ BIT_PIECE(pcCaptured);
    this->ucsqPieces[pcCaptured] = 0;
    pt = PIECE_TYPE(pcCaptured);
    if (pcCaptured < 32) {
        this->vlWhite -= PreEval.ucvlWhitePieces[pt][sq];
    } else {
        this->vlBlack -= PreEval.ucvlBlackPieces[pt][sq];
        pt += 7;
    }
    __ASSERT_BOUND(0, pt, 13);
    this->zobr.Xor(PreGen.zobrTable[pt][sq]);

    // 3. 加上升变后棋子，同时更新子力价值、Zobrist键值和校验锁
    this->ucpcSquares[sq] = pcPromoted;
    this->ucsqPieces[pcPromoted] = sq;
    pt = PIECE_TYPE(pcPromoted);
    if (pcPromoted < 32) {
        this->vlWhite += PreEval.ucvlWhitePieces[pt][sq];
    } else {
        this->vlBlack += PreEval.ucvlBlackPieces[pt][sq];
        pt += 7;
    }
    __ASSERT_BOUND(0, pt, 13);
    this->zobr.Xor(PreGen.zobrTable[pt][sq]);
    return pcCaptured;
}

// 撤消升变
void PositionStruct::UndoPromote(int sq, int pcCaptured) {
    int pcPromoted;
    __ASSERT_SQUARE(sq);
    __ASSERT_PIECE(pcCaptured);
    pcPromoted = this->ucpcSquares[sq];
    __ASSERT(PIECE_TYPE(pcPromoted) == 6);
    this->ucsqPieces[pcPromoted] = 0;
    this->ucpcSquares[sq] = pcCaptured;
    this->ucsqPieces[pcCaptured] = sq;
    this->dwBitPiece ^= BIT_PIECE(pcPromoted) ^ BIT_PIECE(pcCaptured);
}

// 以上是一些棋盘处理过程

// 以下是一些着法处理过程

// 执行一个着法
bool PositionStruct::MakeMove(int mv) {
    int sq, pcCaptured;
    uint32_t dwOldZobristKey;
    RollbackStruct *lprbs;

    // 如果达到最大着法数，那么判定为非法着法
    if (this->nMoveNum == MAX_MOVE_NUM) {
        return false;
    }
    __ASSERT(this->nMoveNum < MAX_MOVE_NUM);
    // 执行一个着法要包括以下几个步骤：

    // 1. 保存原来的Zobrist键值
    dwOldZobristKey = this->zobr.dwKey;
    SaveStatus();

    // 2. 移动棋子，记住吃掉的子(如果有的话)
    sq = SRC(mv);
    if (sq == DST(mv)) {
        pcCaptured = Promote(sq);
    } else {
        pcCaptured = MovePiece(mv);

        // 3. 如果移动后被将军了，那么着法是非法的，撤消该着法
        if (CheckedBy(CHECK_LAZY) > 0) {
            UndoMovePiece(mv, pcCaptured);
            Rollback();
            return false;
        }
    }

    // 4. 交换走子方
    ChangeSide();

    // 5. 把原来的Zobrist键值记录到检测重复的迷你置换表中
    if (this->ucRepHash[dwOldZobristKey & REP_HASH_MASK] == 0) {
        this->ucRepHash[dwOldZobristKey & REP_HASH_MASK] = this->nMoveNum;
    }

    // 6. 把着法保存到历史着法列表中，并记住吃掉的子和将军状态
    lprbs = this->rbsList + this->nMoveNum;
    lprbs->mvs.wmv = mv;
    lprbs->mvs.ChkChs = CheckedBy();

    // 7. 设置和棋着法数(将军和应将不计入)
    if (pcCaptured == 0) {
        if (lprbs->mvs.ChkChs == 0) {
            lprbs->mvs.ChkChs = -ChasedBy(mv);
        }
        if (LastMove().CptDrw == -100) {
            lprbs->mvs.CptDrw = -100;
        } else {
            lprbs->mvs.CptDrw = MIN((int) LastMove().CptDrw, 0) - (lprbs->mvs.ChkChs > 0 || LastMove().ChkChs > 0 ? 0 : 1);
        }
        __ASSERT_BOUND(-100, lprbs->mvs.CptDrw, 0);
    } else {
        lprbs->mvs.CptDrw = pcCaptured;
        __ASSERT_PIECE(pcCaptured);
    }
    this->nMoveNum ++;
    this->nDistance ++;

    return true;
}

// 撤消一个着法
void PositionStruct::UndoMakeMove(void) {
    int sq;
    RollbackStruct *lprbs;
    this->nMoveNum --;
    this->nDistance --;
    lprbs = this->rbsList + this->nMoveNum;
    sq = SRC(lprbs->mvs.wmv);
    if (sq == DST(lprbs->mvs.wmv)) {
        __ASSERT_BOUND(ADVISOR_TYPE, PIECE_TYPE(lprbs->mvs.CptDrw), BISHOP_TYPE);
        UndoPromote(sq, lprbs->mvs.CptDrw);
    } else {
        UndoMovePiece(lprbs->mvs.wmv, lprbs->mvs.CptDrw);
    }
    this->sdPlayer = OPP_SIDE(this->sdPlayer);
    Rollback();
    if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == this->nMoveNum) {
        this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] = 0;
    }
    __ASSERT(this->nMoveNum > 0);
}

// 执行一个空着
void PositionStruct::NullMove(void) {
    __ASSERT(this->nMoveNum < MAX_MOVE_NUM);
    if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == 0) {
        this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] = this->nMoveNum;
    }
    SaveStatus();
    ChangeSide();
    this->rbsList[nMoveNum].mvs.dwmv = 0; // wmv, Chk, CptDrw, ChkChs = 0
    this->nMoveNum ++;
    this->nDistance ++;
}

// 撤消一个空着
void PositionStruct::UndoNullMove(void) {
    this->nMoveNum --;
    this->nDistance --;
    this->sdPlayer = OPP_SIDE(this->sdPlayer);
    Rollback();
    if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == this->nMoveNum) {
        this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] = 0;
    }
    __ASSERT(this->nMoveNum > 0);
}

// 以上是一些着法处理过程

// 以下是一些局面处理过程

// FEN串识别
void PositionStruct::FromFen(const char *szFen) {
    int i, j, k;
    int pcWhite[7];
    int pcBlack[7];
    const char *lpFen;
    // FEN串的识别包括以下几个步骤：
    // 1. 初始化，清空棋盘
    pcWhite[0] = SIDE_TAG(0) + KING_FROM;
    pcWhite[1] = SIDE_TAG(0) + ADVISOR_FROM;
    pcWhite[2] = SIDE_TAG(0) + BISHOP_FROM;
    pcWhite[3] = SIDE_TAG(0) + KNIGHT_FROM;
    pcWhite[4] = SIDE_TAG(0) + ROOK_FROM;
    pcWhite[5] = SIDE_TAG(0) + CANNON_FROM;
    pcWhite[6] = SIDE_TAG(0) + PAWN_FROM;
    for (i = 0; i < 7; i ++) {
        pcBlack[i] = pcWhite[i] + 16;
    }
    /* 数组"pcWhite[7]"和"pcBlack[7]"分别代表红方和黑方每个兵种即将占有的序号，
     * 以"pcWhite[7]"为例，由于棋子16到31依次代表“帅仕仕相相马马车车炮炮兵兵兵兵兵”，
     * 所以最初应该是"pcWhite[7] = {16, 17, 19, 21, 23, 25, 27}"，每添加一个棋子，该项就增加1，
     * 这种做法允许添加多余的棋子(例如添加第二个帅，就变成仕了)，但添加前要做边界检测
     */
    ClearBoard();
    lpFen = szFen;
    if (*lpFen == '\0') {
        SetIrrev();
        return;
    }
    // 2. 读取棋盘上的棋子
    i = RANK_TOP;
    j = FILE_LEFT;
    while (*lpFen != ' ') {
        if (*lpFen == '/') {
            j = FILE_LEFT;
            i ++;
            if (i > RANK_BOTTOM) {
                break;
            }
        } else if (*lpFen >= '1' && *lpFen <= '9') {
            j += (*lpFen - '0');
        } else if (*lpFen >= 'A' && *lpFen <= 'Z') {
            if (j <= FILE_RIGHT) {
                k = FenPiece(*lpFen);
                if (k < 7) {
                    if (pcWhite[k] < 32) {
                        if (this->ucsqPieces[pcWhite[k]] == 0) {
                            AddPiece(COORD_XY(j, i), pcWhite[k]);
                            pcWhite[k] ++;
                        }
                    }
                }
                j ++;
            }
        } else if (*lpFen >= 'a' && *lpFen <= 'z') {
            if (j <= FILE_RIGHT) {
                k = FenPiece(*lpFen + 'A' - 'a');
                if (k < 7) {
                    if (pcBlack[k] < 48) {
                        if (this->ucsqPieces[pcBlack[k]] == 0) {
                            AddPiece(COORD_XY(j, i), pcBlack[k]);
                            pcBlack[k] ++;
                        }
                    }
                }
                j ++;
            }
        }
        lpFen ++;
        if (*lpFen == '\0') {
            SetIrrev();
            return;
        }
    }
    lpFen ++;
    // 3. 确定轮到哪方走
    if (*lpFen == 'b') {
        ChangeSide();
    }
    // 4. 把局面设成“不可逆”
    SetIrrev();
}

// 生成FEN串
void PositionStruct::ToFen(char *szFen) const {
    int i, j, k, pc;
    char *lpFen;

    lpFen = szFen;
    for (i = RANK_TOP; i <= RANK_BOTTOM; i ++) {
        k = 0;
        for (j = FILE_LEFT; j <= FILE_RIGHT; j ++) {
            pc = this->ucpcSquares[COORD_XY(j, i)];
            if (pc != 0) {
                if (k > 0) {
                    *lpFen = k + '0';
                    lpFen ++;
                    k = 0;
                }
                *lpFen = PIECE_BYTE(PIECE_TYPE(pc)) + (pc < 32 ? 0 : 'a' - 'A');
                lpFen ++;
            } else {
                k ++;
            }
        }
        if (k > 0) {
            *lpFen = k + '0';
            lpFen ++;
        }
        *lpFen = '/';
        lpFen ++;
    }
    *(lpFen - 1) = ' '; // 把最后一个'/'替换成' '
    *lpFen = (this->sdPlayer == 0 ? 'w' : 'b');
    lpFen ++;
    *lpFen = '\0';
}

// 局面镜像
void PositionStruct::Mirror(void) {
    int i, sq, nMoveNumSave;
    uint16_t wmvList[MAX_MOVE_NUM];
    uint8_t ucsqList[32];
    // 局面镜像需要按以下步骤依次进行：

    // 1. 记录所有历史着法
    nMoveNumSave = this->nMoveNum;
    for (i = 1; i < nMoveNumSave; i ++) {
        wmvList[i] = this->rbsList[i].mvs.wmv;
    }

    // 2. 撤消所有着法
    for (i = 1; i < nMoveNumSave; i ++) {
        UndoMakeMove();
    }

    // 3. 把所有棋子从棋盘上拿走，位置记录到"ucsqList"数组；
    for (i = 16; i < 48; i ++) {
        sq = this->ucsqPieces[i];
        ucsqList[i - 16] = sq;
        if (sq != 0) {
            AddPiece(sq, i, DEL_PIECE);
        }
    }

    // 4. 把拿走的棋子按照镜像位置重新放到棋盘上；
    for (i = 16; i < 48; i ++) {
        sq = ucsqList[i - 16];
        if (sq != 0) {
            AddPiece(SQUARE_MIRROR(sq), i);
        }
    }

    // 6. 还原镜像着法
    SetIrrev();
    for (i = 1; i < nMoveNumSave; i ++) {
        MakeMove(MOVE_MIRROR(wmvList[i]));
    }
}

// 以上是一些局面处理过程

// 以下是一些着法检测过程

// 着法合理性检测，仅用在“杀手着法”的检测中
bool PositionStruct::LegalMove(int mv) const {
    int sqSrc, sqDst, sqPin, pcMoved, pcCaptured, x, y, nSideTag;
    // 着法合理性检测包括以下几个步骤：

    // 1. 检查要走的子是否存在
    nSideTag = SIDE_TAG(this->sdPlayer);
    sqSrc = SRC(mv);
    sqDst = DST(mv);
    pcMoved = this->ucpcSquares[sqSrc];
    if ((pcMoved & nSideTag) == 0) {
        return false;
    }
    __ASSERT_SQUARE(sqSrc);
    __ASSERT_SQUARE(sqDst);
    __ASSERT_PIECE(pcMoved);

    // 2. 检查吃到的子是否为对方棋子(如果有吃子并且没有升变的话)
    pcCaptured = this->ucpcSquares[sqDst];
    if (sqSrc != sqDst && (pcCaptured & nSideTag) != 0) {
        return false;
    }
    __ASSERT_BOUND(0, PIECE_INDEX(pcMoved), 15);
    switch (PIECE_INDEX(pcMoved)) {

        // 3. 如果是帅(将)或仕(士)，则先看是否在九宫内，再看是否是合理位移
        case KING_FROM:
            return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
        case ADVISOR_FROM:
        case ADVISOR_TO:
            if (sqSrc == sqDst) {
                // 考虑升变，在底线并且兵(卒)不全时，才可升变
                return CAN_PROMOTE(sqSrc) && CanPromote();
            } else {
                return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
            }

            // 4. 如果是相(象)，则先看是否过河，再看是否是合理位移，最后看有没有被塞象眼
        case BISHOP_FROM:
        case BISHOP_TO:
            if (sqSrc == sqDst) {
                // 考虑升变，在底线并且兵(卒)不全时，才可升变
                return CAN_PROMOTE(sqSrc) && CanPromote();
            } else {
                return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) && this->ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
            }

            // 5. 如果是马，则先看看是否是合理位移，再看有没有被蹩马腿
        case KNIGHT_FROM:
        case KNIGHT_TO:
            sqPin = KNIGHT_PIN(sqSrc, sqDst);
            return sqPin != sqSrc && this->ucpcSquares[sqPin] == 0;

            // 6. 如果是车，则先看是横向移动还是纵向移动，再读取位行或位列的着法预生成数组
        case ROOK_FROM:
        case ROOK_TO:
            x = FILE_X(sqSrc);
            y = RANK_Y(sqSrc);
            if (x == FILE_X(sqDst)) {
                if (pcCaptured == 0) {
                    return (FileMaskPtr(x, y)->wNonCap & PreGen.wBitFileMask[sqDst]) != 0;
                } else {
                    return (FileMaskPtr(x, y)->wRookCap & PreGen.wBitFileMask[sqDst]) != 0;
                }
            } else if (y == RANK_Y(sqDst)) {
                if (pcCaptured == 0) {
                    return (RankMaskPtr(x, y)->wNonCap & PreGen.wBitRankMask[sqDst]) != 0;
                } else {
                    return (RankMaskPtr(x, y)->wRookCap & PreGen.wBitRankMask[sqDst]) != 0;
                }
            } else {
                return false;
            }

            // 7. 如果是炮，判断起来和车一样
        case CANNON_FROM:
        case CANNON_TO:
            x = FILE_X(sqSrc);
            y = RANK_Y(sqSrc);
            if (x == FILE_X(sqDst)) {
                if (pcCaptured == 0) {
                    return (FileMaskPtr(x, y)->wNonCap & PreGen.wBitFileMask[sqDst]) != 0;
                } else {
                    return (FileMaskPtr(x, y)->wCannonCap & PreGen.wBitFileMask[sqDst]) != 0;
                }
            } else if (y == RANK_Y(sqDst)) {
                if (pcCaptured == 0) {
                    return (RankMaskPtr(x, y)->wNonCap & PreGen.wBitRankMask[sqDst]) != 0;
                } else {
                    return (RankMaskPtr(x, y)->wCannonCap & PreGen.wBitRankMask[sqDst]) != 0;
                }
            } else {
                return false;
            }

            // 8. 如果是兵(卒)，则按红方和黑方分情况讨论
        default:
            if (AWAY_HALF(sqDst, this->sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
                return true;
            } else {
                return sqDst == SQUARE_FORWARD(sqSrc, this->sdPlayer);
            }
    }
}

// 将军检测
int PositionStruct::CheckedBy(bool bLazy) const {
    int pcCheckedBy, i, sqSrc, sqDst, sqPin, pc, x, y, nOppSideTag;
    SlideMaskStruct *lpsmsRank, *lpsmsFile;

    pcCheckedBy = 0;
    nOppSideTag = OPP_SIDE_TAG(this->sdPlayer);
    // 将军判断包括以下几部分内容：

    // 1. 判断帅(将)是否在棋盘上
    sqSrc = this->ucsqPieces[SIDE_TAG(this->sdPlayer)];
    if (sqSrc == 0) {
        return 0;
    }
    __ASSERT_SQUARE(sqSrc);

    // 2. 获得帅(将)所在格子的位行和位列
    x = FILE_X(sqSrc);
    y = RANK_Y(sqSrc);
    lpsmsRank = RankMaskPtr(x, y);
    lpsmsFile = FileMaskPtr(x, y);

    // 3. 判断是否将帅对脸
    sqDst = this->ucsqPieces[nOppSideTag + KING_FROM];
    if (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        if (x == FILE_X(sqDst) && (lpsmsFile->wRookCap & PreGen.wBitFileMask[sqDst]) != 0) {
            return CHECK_MULTI;
        }
    }

    // 4. 判断是否被马将军
    for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
        sqDst = this->ucsqPieces[nOppSideTag + i];
        if (sqDst != 0) {
            __ASSERT_SQUARE(sqDst);
            sqPin = KNIGHT_PIN(sqDst, sqSrc); // 注意，sqSrc和sqDst是反的！
            if (sqPin != sqDst && this->ucpcSquares[sqPin] == 0) {
                if (bLazy || pcCheckedBy > 0) {
                    return CHECK_MULTI;
                }
                pcCheckedBy = nOppSideTag + i;
                __ASSERT_PIECE(pcCheckedBy);
            }
        }
    }

    // 5. 判断是否被车将军或将帅对脸
    for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
        sqDst = this->ucsqPieces[nOppSideTag + i];
        if (sqDst != 0) {
            __ASSERT_SQUARE(sqDst);
            if (x == FILE_X(sqDst)) {
                if ((lpsmsFile->wRookCap & PreGen.wBitFileMask[sqDst]) != 0) {
                    if (bLazy || pcCheckedBy > 0) {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE(pcCheckedBy);
                }
            } else if (y == RANK_Y(sqDst)) {
                if ((lpsmsRank->wRookCap & PreGen.wBitRankMask[sqDst]) != 0) {
                    if (bLazy || pcCheckedBy > 0) {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE(pcCheckedBy);
                }
            }
        }
    }

    // 6. 判断是否被炮将军
    for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
        sqDst = this->ucsqPieces[nOppSideTag + i];
        if (sqDst != 0) {
            __ASSERT_SQUARE(sqDst);
            if (x == FILE_X(sqDst)) {
                if ((lpsmsFile->wCannonCap & PreGen.wBitFileMask[sqDst]) != 0) {
                    if (bLazy || pcCheckedBy > 0) {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE(pcCheckedBy);
                }
            } else if (y == RANK_Y(sqDst)) {
                if ((lpsmsRank->wCannonCap & PreGen.wBitRankMask[sqDst]) != 0) {
                    if (bLazy || pcCheckedBy > 0) {
                        return CHECK_MULTI;
                    }
                    pcCheckedBy = nOppSideTag + i;
                    __ASSERT_PIECE(pcCheckedBy);
                }
            }
        }
    }

    // 7. 判断是否被兵(卒)将军
    for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2) {
        // 如果帅(将)在边线(ElephantEye允许)，那么断言不成立
        // __ASSERT_SQUARE(sqDst);
        pc = this->ucpcSquares[sqDst];
        if ((pc & nOppSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
            if (bLazy || pcCheckedBy > 0) {
                return CHECK_MULTI;
            }
            pcCheckedBy = nOppSideTag + i;
            __ASSERT_PIECE(pcCheckedBy);
        }
    }
    pc = this->ucpcSquares[SQUARE_FORWARD(sqSrc, this->sdPlayer)];
    if ((pc & nOppSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
        if (bLazy || pcCheckedBy > 0) {
            return CHECK_MULTI;
        }
        pcCheckedBy = nOppSideTag + i;
        __ASSERT_PIECE(pcCheckedBy);
    }
    return pcCheckedBy;
}

// 判断是否被将死
bool PositionStruct::IsMate(void) {
    int i, nGenNum;
    MoveStruct mvsGen[MAX_GEN_MOVES];
    nGenNum = GenCapMoves(mvsGen);
    for (i = 0; i < nGenNum; i ++) {
        if (MakeMove(mvsGen[i].wmv)) {
            UndoMakeMove();
            return false;
        }
    }
    // 着法生成分两部分做，这样可以节约时间
    nGenNum = GenNonCapMoves(mvsGen);
    for (i = 0; i < nGenNum; i ++) {
        if (MakeMove(mvsGen[i].wmv)) {
            UndoMakeMove();
            return false;
        }
    }
    return true;
}

// 设置将军状态位
inline void SetPerpCheck(uint32_t &dwPerpCheck, int nChkChs) {
    if (nChkChs == 0) {
        dwPerpCheck = 0;
    } else if (nChkChs > 0) {
        dwPerpCheck &= 0x10000;
    } else {
        dwPerpCheck &= (1 << -nChkChs);
    }
}

// 重复局面检测
int PositionStruct::RepStatus(int nRecur) const {
    // 参数"nRecur"指重复次数，在搜索中取1以提高搜索效率(默认值)，根结点处取3以适应规则
    int sd;
    uint32_t dwPerpCheck, dwOppPerpCheck;
    const RollbackStruct *lprbs;
    /* 重复局面检测包括以下几个步骤：
     *
     * 1. 首先判断检测重复的迷你置换表中是否可能有当前局面，如果没有可能，就用不着判断了
     *    置换表"ucRepHash"是ElephantEye的一个特色，局面每执行一个着法时，就会在置换表项中记录下当前的"nMoveNum"
     *    如果置换表项已经填有其他局面，就不必覆盖了，参阅"MakeMove()"函数
     *    因此撤消着法时，只要查找置换表项的值是否等于当前的"nMoveNum"，如果相等则清空该项
     *    如果不等于当前的"nMoveNum"，则说明之前还有局面占有这个置换表项，不必清空该项，参阅"position.h"中的"UndoMakeMove()"函数
     */
    if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == 0) {
        return REP_NONE;
    }

    // 2. 初始化
    sd = OPP_SIDE(this->sdPlayer);
    dwPerpCheck = dwOppPerpCheck = 0x1ffff;
    lprbs = this->rbsList + this->nMoveNum - 1;

    // 3. 检查上一个着法，如果是空着或吃子着法，就不可能有重复了
    while (lprbs->mvs.wmv != 0 && lprbs->mvs.CptDrw <= 0) {
        __ASSERT(lprbs >= this->rbsList);

        // 4. 判断双方的长打级别，0表示无长打，0xffff表示长捉，0x10000表示长将
        if (sd == this->sdPlayer) {
            SetPerpCheck(dwPerpCheck, lprbs->mvs.ChkChs);

            // 5. 寻找重复局面，如果重复次数达到预定次数，则返回重复记号
            if (lprbs->zobr.dwLock0 == this->zobr.dwLock0 && lprbs->zobr.dwLock1 == this->zobr.dwLock1) {
                nRecur --;
                if (nRecur == 0) {
                    dwPerpCheck = ((dwPerpCheck & 0xffff) == 0 ? dwPerpCheck : 0xffff);
                    dwOppPerpCheck = ((dwOppPerpCheck & 0xffff) == 0 ? dwOppPerpCheck : 0xffff);
                    return dwPerpCheck > dwOppPerpCheck ? REP_LOSS : dwPerpCheck < dwOppPerpCheck ? REP_WIN : REP_DRAW;
                }
            }

        } else {
            SetPerpCheck(dwOppPerpCheck, lprbs->mvs.ChkChs);
        }

        sd = OPP_SIDE(sd);
        lprbs --;
    }
    return REP_NONE;
}

// 以上是一些着法检测过程


// end of position.cpp

// begin of hash.cpp

/*
hash.h/hash.cpp - Source Code for ElephantEye, Part V

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.31, Last Modified: May 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef CCHESS_A3800
#include <stdio.h>
#endif

int nHashMask;
HashStruct *hshItems;
#ifdef HASH_QUIESC
HashStruct *hshItemsQ;
#endif

// 存储置换表局面信息
void RecordHash(const PositionStruct &pos, int nFlag, int vl, int nDepth, int mv) {
    HashStruct hsh;
    int i, nHashDepth, nMinDepth, nMinLayer;
    // 存储置换表局面信息的过程包括以下几个步骤：

    // 1. 对分值做杀棋步数调整；
    __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
    __ASSERT(mv == 0 || pos.LegalMove(mv));
    if (vl > WIN_VALUE) {
        if (mv == 0 && vl <= BAN_VALUE) {
            return; // 导致长将的局面(不进行置换裁剪)如果连最佳着法也没有，那么没有必要写入置换表
        }
        vl += pos.nDistance;
    } else if (vl < -WIN_VALUE) {
        if (mv == 0 && vl >= -BAN_VALUE) {
            return; // 同理
        }
        vl -= pos.nDistance;
    } else if (vl == pos.DrawValue() && mv == 0) {
        return;   // 同理
    }

    // 2. 逐层试探置换表；
    nMinDepth = 512;
    nMinLayer = 0;
    for (i = 0; i < HASH_LAYERS; i ++) {
        hsh = HASH_ITEM(pos, i);

        // 3. 如果试探到一样的局面，那么更新置换表信息即可；
        if (HASH_POS_EQUAL(hsh, pos)) {
            // 如果深度更深，或者边界缩小，都可更新置换表的值
            if ((nFlag & HASH_ALPHA) != 0 && (hsh.ucAlphaDepth <= nDepth || hsh.svlAlpha >= vl)) {
                hsh.ucAlphaDepth = nDepth;
                hsh.svlAlpha = vl;
            }
            // Beta结点要注意：不要用Null-Move的结点覆盖正常的结点
            if ((nFlag & HASH_BETA) != 0 && (hsh.ucBetaDepth <= nDepth || hsh.svlBeta <= vl) && (mv != 0 || hsh.wmv == 0)) {
                hsh.ucBetaDepth = nDepth;
                hsh.svlBeta = vl;
            }
            // 最佳着法是始终覆盖的
            if (mv != 0) {
                hsh.wmv = mv;
            }
            HASH_ITEM(pos, i) = hsh;
            return;
        }

        // 4. 如果不是一样的局面，那么获得深度最小的置换表项；
        nHashDepth = MAX((hsh.ucAlphaDepth == 0 ? 0 : hsh.ucAlphaDepth + 256),
                         (hsh.wmv == 0 ? hsh.ucBetaDepth : hsh.ucBetaDepth + 256));
        __ASSERT(nHashDepth < 512);
        if (nHashDepth < nMinDepth) {
            nMinDepth = nHashDepth;
            nMinLayer = i;
        }
    }

    // 5. 记录置换表。
    hsh.dwZobristLock0 = pos.zobr.dwLock0;
    hsh.dwZobristLock1 = pos.zobr.dwLock1;
    hsh.wmv = mv;
    hsh.ucAlphaDepth = hsh.ucBetaDepth = 0;
    hsh.svlAlpha = hsh.svlBeta = 0;
    if ((nFlag & HASH_ALPHA) != 0) {
        hsh.ucAlphaDepth = nDepth;
        hsh.svlAlpha = vl;
    }
    if ((nFlag & HASH_BETA) != 0) {
        hsh.ucBetaDepth = nDepth;
        hsh.svlBeta = vl;
    }
    HASH_ITEM(pos, nMinLayer) = hsh;
}

/* 判断获取置换表要符合哪些条件，置换表的分值针对四个不同的区间有不同的处理：
 * 一、如果分值在"WIN_VALUE"以内(即介于"-WIN_VALUE"到"WIN_VALUE"之间，下同)，则只获取满足搜索深度要求的局面；
 * 二、如果分值在"WIN_VALUE"和"BAN_VALUE"之间，则不能获取置换表中的值(只能获取最佳着法仅供参考)，目的是防止由于长将而导致的“置换表的不稳定性”；
 * 三、如果分值在"BAN_VALUE"以外，则获取局面时不必考虑搜索深度要求，因为这些局面已经被证明是杀棋了；
 * 四、如果分值是"DrawValue()"(是第一种情况的特殊情况)，则不能获取置换表中的值(原因与第二种情况相同)。
 * 注意：对于第三种情况，要对杀棋步数进行调整！
 */
inline int ValueAdjust(const PositionStruct &pos, bool &bBanNode, bool &bMateNode, int vl) {
    bBanNode = bMateNode = false;
    if (vl > WIN_VALUE) {
        if (vl <= BAN_VALUE) {
            bBanNode = true;
        } else {
            bMateNode = true;
            vl -= pos.nDistance;
        }
    } else if (vl < -WIN_VALUE) {
        if (vl >= -BAN_VALUE) {
            bBanNode = true;
        } else {
            bMateNode = true;
            vl += pos.nDistance;
        }
    } else if (vl == pos.DrawValue()) {
        bBanNode = true;
    }
    return vl;
}

// 检测下一个着法是否稳定，有助于减少置换表的不稳定性
inline bool MoveStable(PositionStruct &pos, int mv) {
    // 判断下一个着法是否稳定的依据是：
    // 1. 没有后续着法，则假定是稳定的；
    if (mv == 0) {
        return true;
    }
    // 2. 吃子着法是稳定的；
    __ASSERT(pos.LegalMove(mv));
    if (pos.ucpcSquares[DST(mv)] != 0) {
        return true;
    }
    // 3. 可能因置换表引起路线迁移，使得路线超过"MAX_MOVE_NUM"，此时应立刻终止路线，并假定是稳定的。
    if (!pos.MakeMove(mv)) {
        return true;
    }
    return false;
}

// 检测后续路线是否稳定(不是循环路线)，有助于减少置换表的不稳定性
static bool PosStable(const PositionStruct &pos, int mv) {
    HashStruct hsh;
    int i, nMoveNum;
    bool bStable;
    // pos会沿着路线变化，但最终会还原，所以被视为"const"，而让"posMutable"承担非"const"的角色
    PositionStruct &posMutable = (PositionStruct &) pos;

    __ASSERT(mv != 0);
    nMoveNum = 0;
    bStable = true;
    while (!MoveStable(posMutable, mv)) {
        nMoveNum ++; // "!MoveStable()"表明已经执行了一个着法，以后需要撤消
        // 执行这个着法，如果产生循环，那么终止后续路线，并确认该路线不稳定
        if (posMutable.RepStatus() > 0) {
            bStable = false;
            break;
        }
        // 逐层获取置换表项，方法同"ProbeHash()"
        for (i = 0; i < HASH_LAYERS; i ++) {
            hsh = HASH_ITEM(posMutable, i);
            if (HASH_POS_EQUAL(hsh, posMutable)) {
                break;
            }
        }
        mv = (i == HASH_LAYERS ? 0 : hsh.wmv);
    }
    // 撤消前面执行过的所有着法
    for (i = 0; i < nMoveNum; i ++) {
        posMutable.UndoMakeMove();
    }
    return bStable;
}

// 获取置换表局面信息(没有命中时，返回"-MATE_VALUE")
int ProbeHash(const PositionStruct &pos, int vlAlpha, int vlBeta, int nDepth, bool bNoNull, int &mv) {
    HashStruct hsh;
    int i, vl;
    bool bBanNode, bMateNode;
    // 获取置换表局面信息的过程包括以下几个步骤：

    // 1. 逐层获取置换表项
    mv = 0;
    for (i = 0; i < HASH_LAYERS; i ++) {
        hsh = HASH_ITEM(pos, i);
        if (HASH_POS_EQUAL(hsh, pos)) {
            mv = hsh.wmv;
            __ASSERT(mv == 0 || pos.LegalMove(mv));
            break;
        }
    }
    if (i == HASH_LAYERS) {
        return -MATE_VALUE;
    }

    // 2. 判断是否符合Beta边界
    if (hsh.ucBetaDepth > 0) {
        vl = ValueAdjust(pos, bBanNode, bMateNode, hsh.svlBeta);
        if (!bBanNode && !(hsh.wmv == 0 && bNoNull) && (hsh.ucBetaDepth >= nDepth || bMateNode) && vl >= vlBeta) {
            __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
            if (hsh.wmv == 0 || PosStable(pos, hsh.wmv)) {
                return vl;
            }
        }
    }

    // 3. 判断是否符合Alpha边界
    if (hsh.ucAlphaDepth > 0) {
        vl = ValueAdjust(pos, bBanNode, bMateNode, hsh.svlAlpha);
        if (!bBanNode && (hsh.ucAlphaDepth >= nDepth || bMateNode) && vl <= vlAlpha) {
            __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
            if (hsh.wmv == 0 || PosStable(pos, hsh.wmv)) {
                return vl;
            }
        }
    }
    return -MATE_VALUE;
}

#ifdef HASH_QUIESC

// 存储置换表局面信息(静态搜索)
void RecordHashQ(const PositionStruct &pos, int vlBeta, int vlAlpha) {
  volatile HashStruct *lphsh;
  __ASSERT((vlBeta > -WIN_VALUE && vlBeta < WIN_VALUE) || (vlAlpha > -WIN_VALUE && vlAlpha < WIN_VALUE));
  lphsh = hshItemsQ + (pos.zobr.dwKey & nHashMask);
  lphsh->dwZobristLock0 = pos.zobr.dwLock0;
  lphsh->svlAlpha = vlAlpha;
  lphsh->svlBeta = vlBeta;
  lphsh->dwZobristLock1 = pos.zobr.dwLock1;
}

// 获取置换表局面信息(静态搜索)
int ProbeHashQ(const PositionStruct &pos, int vlAlpha, int vlBeta) {
  volatile HashStruct *lphsh;
  int vlHashAlpha, vlHashBeta;

  lphsh = hshItemsQ + (pos.zobr.dwKey & nHashMask);
  if (lphsh->dwZobristLock0 == pos.zobr.dwLock0) {
    vlHashAlpha = lphsh->svlAlpha;
    vlHashBeta = lphsh->svlBeta;
    if (lphsh->dwZobristLock1 == pos.zobr.dwLock1) {
      if (vlHashBeta >= vlBeta) {
        __ASSERT(vlHashBeta > -WIN_VALUE && vlHashBeta < WIN_VALUE);
        return vlHashBeta;
      }
      if (vlHashAlpha <= vlAlpha) {
        __ASSERT(vlHashAlpha > -WIN_VALUE && vlHashAlpha < WIN_VALUE);
        return vlHashAlpha;
      }
    }
  }
  return -MATE_VALUE;
}

#endif

#ifndef CCHESS_A3800

// UCCI支持 - 输出Hash表中的局面信息
bool PopHash(const PositionStruct &pos) {
    HashStruct hsh;
    uint32_t dwMoveStr;
    int i;

    for (i = 0; i < HASH_LAYERS; i ++) {
        hsh = HASH_ITEM(pos, i);
        if (HASH_POS_EQUAL(hsh, pos)) {
            printf("pophash");
            if (hsh.wmv != 0) {
                __ASSERT(pos.LegalMove(hsh.wmv));
                dwMoveStr = MOVE_COORD(hsh.wmv);
                printf(" bestmove %.4s", (const char *) &dwMoveStr);
            }
            if (hsh.ucBetaDepth > 0) {
                printf(" lowerbound %d depth %d", hsh.svlBeta, hsh.ucBetaDepth);
            }
            if (hsh.ucAlphaDepth > 0) {
                printf(" upperbound %d depth %d", hsh.svlAlpha, hsh.ucAlphaDepth);
            }
            printf("\n");
            fflush(stdout);
            return true;
        }
    }
    return false;
}

#endif

// end of hash.cpp

// begin of preeval.cpp

/*
preeval.h/preeval.cpp - Source Code for ElephantEye, Part X

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上五个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

/* 子力位置价值表
 * ElephantEye的子力位置价值表对局面评价的导向起了很大的作用，在参照“梦入神蛋”程序的基础上，作了如下改动：
 * 1. 把棋力基本分和位置相关分组合在一起，以利于快速运算；
 * 2. 一九路的兵(卒)在巡河位置分值减少了5分，以减少盲目进边兵(卒)的情况；
 * 3. 过河兵(卒)(底线除外)多加10分，以减少过河兵(卒)盲目换仕(士)相(象)的情况；
 * 4. 一九路车在横车的位置分值减少了5分，以减少上仕(士)时还起无意义的横车的情况。
 */

// 1. 开中局、有进攻机会的帅(将)和兵(卒)，参照“梦入神蛋”
static const uint8_t cucvlKingPawnMidgameAttacking[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
        0,  0,  0, 39, 49, 69, 84, 89, 84, 69, 49, 39,  0,  0,  0,  0,
        0,  0,  0, 39, 49, 64, 74, 74, 74, 64, 49, 39,  0,  0,  0,  0,
        0,  0,  0, 39, 46, 54, 59, 61, 59, 54, 46, 39,  0,  0,  0,  0,
        0,  0,  0, 29, 37, 41, 54, 59, 54, 41, 37, 29,  0,  0,  0,  0,
        0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
        0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 2. 开中局、没有进攻机会的帅(将)和兵(卒)
static const uint8_t cucvlKingPawnMidgameAttackless[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
        0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,
        0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,
        0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
        0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
        0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
        0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 3. 残局、有进攻机会的帅(将)和兵(卒)
static const uint8_t cucvlKingPawnEndgameAttacking[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 10, 10, 10, 15, 15, 15, 10, 10, 10,  0,  0,  0,  0,
        0,  0,  0, 50, 55, 60, 85,100, 85, 60, 55, 50,  0,  0,  0,  0,
        0,  0,  0, 65, 70, 70, 75, 75, 75, 70, 70, 65,  0,  0,  0,  0,
        0,  0,  0, 75, 80, 80, 80, 80, 80, 80, 80, 75,  0,  0,  0,  0,
        0,  0,  0, 70, 70, 65, 70, 70, 70, 65, 70, 70,  0,  0,  0,  0,
        0,  0,  0, 45,  0, 40, 45, 45, 45, 40,  0, 45,  0,  0,  0,  0,
        0,  0,  0, 40,  0, 35, 40, 40, 40, 35,  0, 40,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  5,  5, 15,  5,  5,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  3,  3, 13,  3,  3,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  1,  1, 11,  1,  1,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 4. 残局、没有进攻机会的帅(将)和兵(卒)
static const uint8_t cucvlKingPawnEndgameAttackless[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 10, 10, 10, 15, 15, 15, 10, 10, 10,  0,  0,  0,  0,
        0,  0,  0, 10, 15, 20, 45, 60, 45, 20, 15, 10,  0,  0,  0,  0,
        0,  0,  0, 25, 30, 30, 35, 35, 35, 30, 30, 25,  0,  0,  0,  0,
        0,  0,  0, 35, 40, 40, 45, 45, 45, 40, 40, 35,  0,  0,  0,  0,
        0,  0,  0, 25, 30, 30, 35, 35, 35, 30, 30, 25,  0,  0,  0,  0,
        0,  0,  0, 25,  0, 25, 25, 25, 25, 25,  0, 25,  0,  0,  0,  0,
        0,  0,  0, 20,  0, 20, 20, 20, 20, 20,  0, 20,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  5,  5, 13,  5,  5,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  3,  3, 12,  3,  3,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  1,  1, 11,  1,  1,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 5. 没受威胁的仕(士)和相(象)
static const uint8_t cucvlAdvisorBishopThreatless[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 18,  0,  0, 20, 23, 20,  0,  0, 18,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 20, 20,  0, 20, 20,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 5'. 可升变的，没受威胁的仕(士)和相(象)
static const uint8_t cucvlAdvisorBishopPromotionThreatless[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 30,  0,  0,  0, 30,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 28,  0,  0, 30, 33, 30,  0,  0, 28,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 33,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 30, 30,  0, 30, 30,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 6. 受到威胁的仕(士)和相(象)，参照“梦入神蛋”
static const uint8_t cucvlAdvisorBishopThreatened[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 40,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 38,  0,  0, 40, 43, 40,  0,  0, 38,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 43,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 40, 40,  0, 40, 40,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 7. 开中局的马，参照“梦入神蛋”
static const uint8_t cucvlKnightMidgame[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
        0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
        0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
        0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
        0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
        0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
        0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
        0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
        0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,
        0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 8. 残局的马
static const uint8_t cucvlKnightEndgame[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 92, 94, 96, 96, 96, 96, 96, 94, 92,  0,  0,  0,  0,
        0,  0,  0, 94, 96, 98, 98, 98, 98, 98, 96, 94,  0,  0,  0,  0,
        0,  0,  0, 96, 98,100,100,100,100,100, 98, 96,  0,  0,  0,  0,
        0,  0,  0, 96, 98,100,100,100,100,100, 98, 96,  0,  0,  0,  0,
        0,  0,  0, 96, 98,100,100,100,100,100, 98, 96,  0,  0,  0,  0,
        0,  0,  0, 94, 96, 98, 98, 98, 98, 98, 96, 94,  0,  0,  0,  0,
        0,  0,  0, 94, 96, 98, 98, 98, 98, 98, 96, 94,  0,  0,  0,  0,
        0,  0,  0, 92, 94, 96, 96, 96, 96, 96, 94, 92,  0,  0,  0,  0,
        0,  0,  0, 90, 92, 94, 92, 92, 92, 94, 92, 90,  0,  0,  0,  0,
        0,  0,  0, 88, 90, 92, 90, 90, 90, 92, 90, 88,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 9. 开中局的车，参照“梦入神蛋”
static const uint8_t cucvlRookMidgame[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
        0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
        0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
        0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
        0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
        0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
        0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
        0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
        0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
        0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 10. 残局的车
static const uint8_t cucvlRookEndgame[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,182,182,182,184,186,184,182,182,182,  0,  0,  0,  0,
        0,  0,  0,184,184,184,186,190,186,184,184,184,  0,  0,  0,  0,
        0,  0,  0,182,182,182,184,186,184,182,182,182,  0,  0,  0,  0,
        0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
        0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
        0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
        0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
        0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
        0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
        0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 11. 开中局的炮，参照“梦入神蛋”
static const uint8_t cucvlCannonMidgame[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
        0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
        0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
        0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
        0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
        0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
        0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
        0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
        0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
        0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 12. 残局的炮
static const uint8_t cucvlCannonEndgame[256] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,104,106,104,100,100,100,  0,  0,  0,  0,
        0,  0,  0,100,100,100,104,106,104,100,100,100,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 空头炮的威胁分值，指标是对红方来说的行号(即黑方要用15去减)，大体上空头炮位置越高威胁越大。进入残局时，该值要相应减少。
static const int cvlHollowThreat[16] = {
        0,  0,  0,  0,  0,  0, 60, 65, 70, 75, 80, 80, 80,  0,  0,  0
};

// 炮镇窝心马的威胁分值，指标同上，大体上高度越低威胁越大，没有窝心马时可取四分之一。进入残局时，取值似乎不应变化。
static const int cvlCentralThreat[16] = {
        0,  0,  0,  0,  0,  0, 50, 45, 40, 35, 30, 30, 30,  0,  0,  0
};

// 沉底炮的威胁分值，指标是列号，大体上越靠近边线威胁越大。威胁减少时，该值要相应减少。
static const int cvlBottomThreat[16] = {
        0,  0,  0, 40, 30,  0,  0,  0,  0,  0, 30, 40,  0,  0,  0,  0
};

// 本模块只涉及到"PositionStruct"中的"ucsqPieces"、"dwBitPiece/wBitPiece"、"vlWhite"和"vlBlack"四个成员，故省略前面的"this->"

/* 局面预评价就是初始化局面预评价数据(PreEval和PreEvalEx)的过程。
 * ElephantEye的局面预评价主要分以下两个方面：
 * 1. 判断局势处于开中局还是残局阶段；
 * 2. 判断每一方是否对对方形成威胁。
 */

const int ROOK_MIDGAME_VALUE = 6;
const int KNIGHT_CANNON_MIDGAME_VALUE = 3;
const int OTHER_MIDGAME_VALUE = 1;
const int TOTAL_MIDGAME_VALUE = ROOK_MIDGAME_VALUE * 4 + KNIGHT_CANNON_MIDGAME_VALUE * 8 + OTHER_MIDGAME_VALUE * 18;
const int TOTAL_ADVANCED_VALUE = 4;
const int TOTAL_ATTACK_VALUE = 8;
const int ADVISOR_BISHOP_ATTACKLESS_VALUE = 80;
const int TOTAL_ADVISOR_LEAKAGE = 80;

static bool bInit = false;

PreEvalStructEx PreEvalEx;

void PositionStruct::PreEvaluate(void) {
    int i, sq, nMidgameValue, nWhiteAttacks, nBlackAttacks, nWhiteSimpleValue, nBlackSimpleValue;
    uint8_t ucvlPawnPiecesAttacking[256], ucvlPawnPiecesAttackless[256];

    if (!bInit) {
        bInit = true;
        // 初始化"PreEvalEx.cPopCnt16"数组，只需要初始化一次
        for (i = 0; i < 65536; i ++) {
            PreEvalEx.cPopCnt16[i] = PopCnt16(i);
        }
    }

    // 首先判断局势处于开中局还是残局阶段，方法是计算各种棋子的数量，按照车=6、马炮=3、其它=1相加。
    nMidgameValue = PopCnt32(this->dwBitPiece & BOTH_BITPIECE(ADVISOR_BITPIECE | BISHOP_BITPIECE | PAWN_BITPIECE)) * OTHER_MIDGAME_VALUE;
    nMidgameValue += PopCnt32(this->dwBitPiece & BOTH_BITPIECE(KNIGHT_BITPIECE | CANNON_BITPIECE)) * KNIGHT_CANNON_MIDGAME_VALUE;
    nMidgameValue += PopCnt32(this->dwBitPiece & BOTH_BITPIECE(ROOK_BITPIECE)) * ROOK_MIDGAME_VALUE;
    // 使用二次函数，子力很少时才认为接近残局
    nMidgameValue = (2 * TOTAL_MIDGAME_VALUE - nMidgameValue) * nMidgameValue / TOTAL_MIDGAME_VALUE;
    __ASSERT_BOUND(0, nMidgameValue, TOTAL_MIDGAME_VALUE);
    PreEval.vlAdvanced = (TOTAL_ADVANCED_VALUE * nMidgameValue + TOTAL_ADVANCED_VALUE / 2) / TOTAL_MIDGAME_VALUE;
    __ASSERT_BOUND(0, PreEval.vlAdvanced, TOTAL_ADVANCED_VALUE);
    for (sq = 0; sq < 256; sq ++) {
        if (IN_BOARD(sq)) {
            PreEval.ucvlWhitePieces[0][sq] = PreEval.ucvlBlackPieces[0][SQUARE_FLIP(sq)] = (uint8_t)
                    ((cucvlKingPawnMidgameAttacking[sq] * nMidgameValue + cucvlKingPawnEndgameAttacking[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
            PreEval.ucvlWhitePieces[3][sq] = PreEval.ucvlBlackPieces[3][SQUARE_FLIP(sq)] = (uint8_t)
                    ((cucvlKnightMidgame[sq] * nMidgameValue + cucvlKnightEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
            PreEval.ucvlWhitePieces[4][sq] = PreEval.ucvlBlackPieces[4][SQUARE_FLIP(sq)] = (uint8_t)
                    ((cucvlRookMidgame[sq] * nMidgameValue + cucvlRookEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
            PreEval.ucvlWhitePieces[5][sq] = PreEval.ucvlBlackPieces[5][SQUARE_FLIP(sq)] = (uint8_t)
                    ((cucvlCannonMidgame[sq] * nMidgameValue + cucvlCannonEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
            ucvlPawnPiecesAttacking[sq] = PreEval.ucvlWhitePieces[0][sq];
            ucvlPawnPiecesAttackless[sq] = (uint8_t)
                    ((cucvlKingPawnMidgameAttackless[sq] * nMidgameValue + cucvlKingPawnEndgameAttackless[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
        }
    }
    for (i = 0; i < 16; i ++) {
        PreEvalEx.vlHollowThreat[i] = cvlHollowThreat[i] * (nMidgameValue + TOTAL_MIDGAME_VALUE) / (TOTAL_MIDGAME_VALUE * 2);
        __ASSERT_BOUND(0, PreEvalEx.vlHollowThreat[i], cvlHollowThreat[i]);
        PreEvalEx.vlCentralThreat[i] = cvlCentralThreat[i];
    }

    // 然后判断各方是否处于进攻状态，方法是计算各种过河棋子的数量，按照车马2炮兵1相加。
    nWhiteAttacks = nBlackAttacks = 0;
    for (i = SIDE_TAG(0) + KNIGHT_FROM; i <= SIDE_TAG(0) + ROOK_TO; i ++) {
        if (this->ucsqPieces[i] != 0 && BLACK_HALF(this->ucsqPieces[i])) {
            nWhiteAttacks += 2;
        }
    }
    for (i = SIDE_TAG(0) + CANNON_FROM; i <= SIDE_TAG(0) + PAWN_TO; i ++) {
        if (this->ucsqPieces[i] != 0 && BLACK_HALF(this->ucsqPieces[i])) {
            nWhiteAttacks ++;
        }
    }
    for (i = SIDE_TAG(1) + KNIGHT_FROM; i <= SIDE_TAG(1) + ROOK_TO; i ++) {
        if (this->ucsqPieces[i] != 0 && WHITE_HALF(this->ucsqPieces[i])) {
            nBlackAttacks += 2;
        }
    }
    for (i = SIDE_TAG(1) + CANNON_FROM; i <= SIDE_TAG(1) + PAWN_TO; i ++) {
        if (this->ucsqPieces[i] != 0 && WHITE_HALF(this->ucsqPieces[i])) {
            nBlackAttacks ++;
        }
    }
    // 如果本方轻子数比对方多，那么每多一个轻子(车算2个轻子)威胁值加2。威胁值最多不超过8。
    nWhiteSimpleValue = PopCnt16(this->wBitPiece[0] & ROOK_BITPIECE) * 2 + PopCnt16(this->wBitPiece[0] & (KNIGHT_BITPIECE | CANNON_BITPIECE));
    nBlackSimpleValue = PopCnt16(this->wBitPiece[1] & ROOK_BITPIECE) * 2 + PopCnt16(this->wBitPiece[1] & (KNIGHT_BITPIECE | CANNON_BITPIECE));
    if (nWhiteSimpleValue > nBlackSimpleValue) {
        nWhiteAttacks += (nWhiteSimpleValue - nBlackSimpleValue) * 2;
    } else {
        nBlackAttacks += (nBlackSimpleValue - nWhiteSimpleValue) * 2;
    }
    nWhiteAttacks = MIN(nWhiteAttacks, TOTAL_ATTACK_VALUE);
    nBlackAttacks = MIN(nBlackAttacks, TOTAL_ATTACK_VALUE);
    PreEvalEx.vlBlackAdvisorLeakage = TOTAL_ADVISOR_LEAKAGE * nWhiteAttacks / TOTAL_ATTACK_VALUE;
    PreEvalEx.vlWhiteAdvisorLeakage = TOTAL_ADVISOR_LEAKAGE * nBlackAttacks / TOTAL_ATTACK_VALUE;
    __ASSERT_BOUND(0, nWhiteAttacks, TOTAL_ATTACK_VALUE);
    __ASSERT_BOUND(0, nBlackAttacks, TOTAL_ATTACK_VALUE);
    __ASSERT_BOUND(0, PreEvalEx.vlBlackAdvisorLeakage, TOTAL_ADVISOR_LEAKAGE);
    __ASSERT_BOUND(0, PreEvalEx.vlBlackAdvisorLeakage, TOTAL_ADVISOR_LEAKAGE);
    for (sq = 0; sq < 256; sq ++) {
        if (IN_BOARD(sq)) {
            PreEval.ucvlWhitePieces[1][sq] = PreEval.ucvlWhitePieces[2][sq] = (uint8_t) ((cucvlAdvisorBishopThreatened[sq] * nBlackAttacks +
                                                                                          (PreEval.bPromotion ? cucvlAdvisorBishopPromotionThreatless[sq] : cucvlAdvisorBishopThreatless[sq]) * (TOTAL_ATTACK_VALUE - nBlackAttacks)) / TOTAL_ATTACK_VALUE);
            PreEval.ucvlBlackPieces[1][sq] = PreEval.ucvlBlackPieces[2][sq] = (uint8_t) ((cucvlAdvisorBishopThreatened[SQUARE_FLIP(sq)] * nWhiteAttacks +
                                                                                          (PreEval.bPromotion ? cucvlAdvisorBishopPromotionThreatless[SQUARE_FLIP(sq)] : cucvlAdvisorBishopThreatless[SQUARE_FLIP(sq)]) * (TOTAL_ATTACK_VALUE - nWhiteAttacks)) / TOTAL_ATTACK_VALUE);
            PreEval.ucvlWhitePieces[6][sq] = (uint8_t) ((ucvlPawnPiecesAttacking[sq] * nWhiteAttacks +
                                                         ucvlPawnPiecesAttackless[sq] * (TOTAL_ATTACK_VALUE - nWhiteAttacks)) / TOTAL_ATTACK_VALUE);
            PreEval.ucvlBlackPieces[6][sq] = (uint8_t) ((ucvlPawnPiecesAttacking[SQUARE_FLIP(sq)] * nBlackAttacks +
                                                         ucvlPawnPiecesAttackless[SQUARE_FLIP(sq)] * (TOTAL_ATTACK_VALUE - nBlackAttacks)) / TOTAL_ATTACK_VALUE);
        }
    }
    for (i = 0; i < 16; i ++) {
        PreEvalEx.vlWhiteBottomThreat[i] = cvlBottomThreat[i] * nBlackAttacks / TOTAL_ATTACK_VALUE;
        PreEvalEx.vlBlackBottomThreat[i] = cvlBottomThreat[i] * nWhiteAttacks / TOTAL_ATTACK_VALUE;
    }

    // 检查预评价是否对称
#ifndef NDEBUG
    for (sq = 0; sq < 256; sq ++) {
    if (IN_BOARD(sq)) {
      for (i = 0; i < 7; i ++) {
        __ASSERT(PreEval.ucvlWhitePieces[i][sq] == PreEval.ucvlWhitePieces[i][SQUARE_MIRROR(sq)]);
        __ASSERT(PreEval.ucvlBlackPieces[i][sq] == PreEval.ucvlBlackPieces[i][SQUARE_MIRROR(sq)]);
      }
    }
  }
  for (i = FILE_LEFT; i <= FILE_RIGHT; i ++) {
    __ASSERT(PreEvalEx.vlWhiteBottomThreat[i] == PreEvalEx.vlWhiteBottomThreat[FILE_FLIP(i)]);
    __ASSERT(PreEvalEx.vlBlackBottomThreat[i] == PreEvalEx.vlBlackBottomThreat[FILE_FLIP(i)]);
  }
#endif

    // 调整不受威胁方少掉的仕(士)相(象)分值
    this->vlWhite = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - nBlackAttacks) / TOTAL_ATTACK_VALUE;
    this->vlBlack = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - nWhiteAttacks) / TOTAL_ATTACK_VALUE;
    // 如果允许升变，那么不受威胁的仕(士)相(象)分值就少了一半
    if (PreEval.bPromotion) {
        this->vlWhite /= 2;
        this->vlBlack /= 2;
    }
    // 最后重新计算子力位置分
    for (i = 16; i < 32; i ++) {
        sq = this->ucsqPieces[i];
        if (sq != 0) {
            __ASSERT_SQUARE(sq);
            this->vlWhite += PreEval.ucvlWhitePieces[PIECE_TYPE(i)][sq];
        }
    }
    for (i = 32; i < 48; i ++) {
        sq = this->ucsqPieces[i];
        if (sq != 0) {
            __ASSERT_SQUARE(sq);
            this->vlBlack += PreEval.ucvlBlackPieces[PIECE_TYPE(i)][sq];
        }
    }
}


// end of preeval.cpp

// begin of search.cpp

/*
search.h/search.cpp - Source Code for ElephantEye, Part VIII

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.32, Last Modified: May 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef CCHESS_A3800
#include <stdio.h>
#endif
#ifndef CCHESS_A3800
//#include "ucci.h"
//#include "book.h"
#endif

const int IID_DEPTH = 2;         // 内部迭代加深的深度
const int SMP_DEPTH = 6;         // 并行搜索的深度
const int UNCHANGED_DEPTH = 4;   // 未改变最佳着法的深度

const int DROPDOWN_VALUE = 20;   // 落后的分值
const int RESIGN_VALUE = 300;    // 认输的分值
const int DRAW_OFFER_VALUE = 40; // 提和的分值

SearchStruct Search;

// 搜索信息，是封装在模块内部的
static struct {
    int64_t llTime;                     // 计时器
    bool bStop, bPonderStop;            // 中止信号和后台思考认为的中止信号
    bool bPopPv, bPopCurrMove;          // 是否输出pv和currmove
    int nPopDepth, vlPopValue;          // 输出的深度和分值
    int nAllNodes, nMainNodes;          // 总结点数和主搜索树的结点数
    int nUnchanged;                     // 未改变最佳着法的深度
    uint16_t wmvPvLine[MAX_MOVE_NUM];   // 主要变例路线上的着法列表
    uint16_t wmvKiller[LIMIT_DEPTH][2]; // 杀手着法表
    MoveSortStruct MoveSort;            // 根结点的着法序列
} Search2;

#ifndef CCHESS_A3800

void BuildPos(PositionStruct &pos, const UcciCommStruct &UcciComm) {
    int i, mv;
    pos.FromFen(UcciComm.szFenStr);
    for (i = 0; i < UcciComm.nMoveNum; i ++) {
        mv = COORD_MOVE(UcciComm.lpdwMovesCoord[i]);
        if (mv == 0) {
            break;
        }
        if (pos.LegalMove(mv) && pos.MakeMove(mv) && pos.LastMove().CptDrw > 0) {
            // 始终让pos.nMoveNum反映没吃子的步数
            pos.SetIrrev();
        }
    }
}

#endif

// 中断例程
static bool Interrupt(void) {
    if (Search.bIdle) {
        Idle();
    }
    if (Search.nGoMode == GO_MODE_NODES) {
        if (!Search.bPonder && Search2.nAllNodes > Search.nNodes * 4) {
            Search2.bStop = true;
            return true;
        }
    } else if (Search.nGoMode == GO_MODE_TIMER) {
        if (!Search.bPonder && (int) (GetTime() - Search2.llTime) > Search.nMaxTimer) {
            Search2.bStop = true;
            return true;
        }
    }
    if (Search.bBatch) {
        return false;
    }

#ifdef CCHESS_A3800
    return false;
#else
    UcciCommStruct UcciComm;
    PositionStruct posProbe;
    // 如果不是批处理模式，那么先调用UCCI解释程序，再判断是否中止
    switch (BusyLine(UcciComm, Search.bDebug)) {
        case UCCI_COMM_ISREADY:
            // "isready"指令实际上没有意义
            printf("readyok\n");
            fflush(stdout);
            return false;
        case UCCI_COMM_PONDERHIT:
            // "ponderhit"指令启动计时功能，如果"SearchMain()"例程认为已经搜索了足够的时间， 那么发出中止信号
            if (Search2.bPonderStop) {
                Search2.bStop = true;
                return true;
            } else {
                Search.bPonder = false;
                return false;
            }
        case UCCI_COMM_PONDERHIT_DRAW:
            // "ponderhit draw"指令启动计时功能，并设置提和标志
            Search.bDraw = true;
            if (Search2.bPonderStop) {
                Search2.bStop = true;
                return true;
            } else {
                Search.bPonder = false;
                return false;
            }
        case UCCI_COMM_STOP:
            // "stop"指令发送中止信号
            Search2.bStop = true;
            return true;
        case UCCI_COMM_PROBE:
            // "probe"指令输出Hash表信息
            BuildPos(posProbe, UcciComm);
            PopHash(posProbe);
            return false;
        case UCCI_COMM_QUIT:
            // "quit"指令发送退出信号
            Search.bQuit = Search2.bStop = true;
            return true;
        default:
            return false;
    }
#endif
}

#ifndef CCHESS_A3800

// 输出主要变例
static void PopPvLine(int nDepth = 0, int vl = 0) {
    uint16_t *lpwmv;
    uint32_t dwMoveStr;
    // 如果尚未达到需要输出的深度，那么记录该深度和分值，以后再输出
    if (nDepth > 0 && !Search2.bPopPv && !Search.bDebug) {
        Search2.nPopDepth = nDepth;
        Search2.vlPopValue = vl;
        return;
    }
    // 输出时间和搜索结点数
    printf("info time %d nodes %d\n", (int) (GetTime() - Search2.llTime), Search2.nAllNodes);
    fflush(stdout);
    if (nDepth == 0) {
        // 如果是搜索结束后的输出，并且已经输出过，那么不必再输出
        if (Search2.nPopDepth == 0) {
            return;
        }
        // 获取以前没有输出的深度和分值
        nDepth = Search2.nPopDepth;
        vl = Search2.vlPopValue;
    } else {
        // 达到需要输出的深度，那么以后不必再输出
        Search2.nPopDepth = Search2.vlPopValue = 0;
    }
    printf("info depth %d score %d pv", nDepth, vl);
    lpwmv = Search2.wmvPvLine;
    while (*lpwmv != 0) {
        dwMoveStr = MOVE_COORD(*lpwmv);
        printf(" %.4s", (const char *) &dwMoveStr);
        lpwmv ++;
    }
    printf("\n");
    fflush(stdout);
}

// 输出主要变例
static void PopPvLineBotzone(int nDepth = 0, int vl = 0) {
    uint16_t *lpwmv;
    uint32_t dwMoveStr;
    // 如果尚未达到需要输出的深度，那么记录该深度和分值，以后再输出
    if (nDepth > 0 && !Search2.bPopPv && !Search.bDebug) {
        Search2.nPopDepth = nDepth;
        Search2.vlPopValue = vl;
        return;
    }
    // 输出时间和搜索结点数
//    printf("info time %d nodes %d\n", (int) (GetTime() - Search2.llTime), Search2.nAllNodes);
//    fflush(stdout);
    if (nDepth == 0) {
        // 如果是搜索结束后的输出，并且已经输出过，那么不必再输出
        if (Search2.nPopDepth == 0) {
            return;
        }
        // 获取以前没有输出的深度和分值
        nDepth = Search2.nPopDepth;
        vl = Search2.vlPopValue;
    } else {
        // 达到需要输出的深度，那么以后不必再输出
        Search2.nPopDepth = Search2.vlPopValue = 0;
    }
//    printf("info depth %d score %d pv", nDepth, vl);
    lpwmv = Search2.wmvPvLine;
    while (*lpwmv != 0) {
        dwMoveStr = MOVE_COORD(*lpwmv);
//        printf(" %.4s", (const char *) &dwMoveStr);
        lpwmv ++;
    }
//    printf("\n");
//    fflush(stdout);
}

#endif

// 无害裁剪
static int HarmlessPruning(const PositionStruct &pos, int vlBeta) {
    int vl, vlRep;

    // 1. 杀棋步数裁剪；
    vl = pos.nDistance - MATE_VALUE;
    if (vl >= vlBeta) {
        return vl;
    }

    // 2. 和棋裁剪；
    if (pos.IsDraw()) {
        return 0; // 安全起见，这里不用"pos.DrawValue()";
    }

    // 3. 重复裁剪；
    vlRep = pos.RepStatus();
    if (vlRep > 0) {
        return pos.RepValue(vlRep);
    }

    return -MATE_VALUE;
}

// 调整型局面评价函数
inline int Evaluate(const PositionStruct &pos, int vlAlpha, int vlBeta) {
    int vl;
    vl = Search.bKnowledge ? pos.Evaluate(vlAlpha, vlBeta) : pos.Material();
    return vl == pos.DrawValue() ? vl - 1 : vl;
}

// 静态搜索例程
static int SearchQuiesc(PositionStruct &pos, int vlAlpha, int vlBeta) {
    int vlBest, vl, mv;
    bool bInCheck;
    MoveSortStruct MoveSort;
    // 静态搜索例程包括以下几个步骤：
    Search2.nAllNodes ++;

    // 1. 无害裁剪；
    vl = HarmlessPruning(pos, vlBeta);
    if (vl > -MATE_VALUE) {
        return vl;
    }

#ifdef HASH_QUIESC
    // 3. 置换裁剪；
  vl = ProbeHashQ(pos, vlAlpha, vlBeta);
  if (Search.bUseHash && vl > -MATE_VALUE) {
    return vl;
  }
#endif

    // 4. 达到极限深度，直接返回评价值；
    if (pos.nDistance == LIMIT_DEPTH) {
        return Evaluate(pos, vlAlpha, vlBeta);
    }
    __ASSERT(Search.pos.nDistance < LIMIT_DEPTH);

    // 5. 初始化；
    vlBest = -MATE_VALUE;
    bInCheck = (pos.LastMove().ChkChs > 0);

    // 6. 对于被将军的局面，生成全部着法；
    if (bInCheck) {
        MoveSort.InitAll(pos);
    } else {

        // 7. 对于未被将军的局面，在生成着法前首先尝试空着(空着启发)，即对局面作评价；
        vl = Evaluate(pos, vlAlpha, vlBeta);
        __ASSERT_BOUND(1 - WIN_VALUE, vl, WIN_VALUE - 1);
        __ASSERT(vl > vlBest);
        if (vl >= vlBeta) {
#ifdef HASH_QUIESC
            RecordHashQ(pos, vl, MATE_VALUE);
#endif
            return vl;
        }
        vlBest = vl;
        vlAlpha = MAX(vl, vlAlpha);

        // 8. 对于未被将军的局面，生成并排序所有吃子着法(MVV(LVA)启发)；
        MoveSort.InitQuiesc(pos);
    }

    // 9. 用Alpha-Beta算法搜索这些着法；
    while ((mv = MoveSort.NextQuiesc(bInCheck)) != 0) {
        __ASSERT(bInCheck || pos.ucpcSquares[DST(mv)] > 0);
        if (pos.MakeMove(mv)) {
            vl = -SearchQuiesc(pos, -vlBeta, -vlAlpha);
            pos.UndoMakeMove();
            if (vl > vlBest) {
                if (vl >= vlBeta) {
#ifdef HASH_QUIESC
                    if (vl > -WIN_VALUE && vl < WIN_VALUE) {
            RecordHashQ(pos, vl, MATE_VALUE);
          }
#endif
                    return vl;
                }
                vlBest = vl;
                vlAlpha = MAX(vl, vlAlpha);
            }
        }
    }

    // 10. 返回分值。
    if (vlBest == -MATE_VALUE) {
        __ASSERT(pos.IsMate());
        return pos.nDistance - MATE_VALUE;
    } else {
#ifdef HASH_QUIESC
        if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
      RecordHashQ(pos, vlBest > vlAlpha ? vlBest : -MATE_VALUE, vlBest);
    }
#endif
        return vlBest;
    }
}

#ifndef CCHESS_A3800

// UCCI支持 - 输出叶子结点的局面信息
void PopLeaf(PositionStruct &pos) {
    int vl;
    Search2.nAllNodes = 0;
    vl = SearchQuiesc(pos, -MATE_VALUE, MATE_VALUE);
    printf("pophash lowerbound %d depth 0 upperbound %d depth 0\n", vl, vl);
    fflush(stdout);
}

#endif

const bool NO_NULL = true; // "SearchCut()"的参数，是否禁止空着裁剪

// 零窗口完全搜索例程
static int SearchCut(int vlBeta, int nDepth, bool bNoNull = false) {
    int nNewDepth, vlBest, vl;
    int mvHash, mv, mvEvade;
    MoveSortStruct MoveSort;
    // 完全搜索例程包括以下几个步骤：

    // 1. 在叶子结点处调用静态搜索；
    if (nDepth <= 0) {
        __ASSERT(nDepth >= -NULL_DEPTH);
        return SearchQuiesc(Search.pos, vlBeta - 1, vlBeta);
    }
    Search2.nAllNodes ++;

    // 2. 无害裁剪；
    vl = HarmlessPruning(Search.pos, vlBeta);
    if (vl > -MATE_VALUE) {
        return vl;
    }

    // 3. 置换裁剪；
    vl = ProbeHash(Search.pos, vlBeta - 1, vlBeta, nDepth, bNoNull, mvHash);
    if (Search.bUseHash && vl > -MATE_VALUE) {
        return vl;
    }

    // 4. 达到极限深度，直接返回评价值；
    if (Search.pos.nDistance == LIMIT_DEPTH) {
        return Evaluate(Search.pos, vlBeta - 1, vlBeta);
    }
    __ASSERT(Search.pos.nDistance < LIMIT_DEPTH);

    // 5. 中断调用；
    Search2.nMainNodes ++;
    vlBest = -MATE_VALUE;
    if ((Search2.nMainNodes & Search.nCountMask) == 0 && Interrupt()) {
        return vlBest;
    }

    // 6. 尝试空着裁剪；
    if (Search.bNullMove && !bNoNull && Search.pos.LastMove().ChkChs <= 0 && Search.pos.NullOkay()) {
        Search.pos.NullMove();
        vl = -SearchCut(1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
        Search.pos.UndoNullMove();
        if (Search2.bStop) {
            return vlBest;
        }

        if (vl >= vlBeta) {
            if (Search.pos.NullSafe()) {
                // a. 如果空着裁剪不带检验，那么记录深度至少为(NULL_DEPTH + 1)；
                RecordHash(Search.pos, HASH_BETA, vl, MAX(nDepth, NULL_DEPTH + 1), 0);
                return vl;
            } else if (SearchCut(vlBeta, nDepth - NULL_DEPTH, NO_NULL) >= vlBeta) {
                // b. 如果空着裁剪带检验，那么记录深度至少为(NULL_DEPTH)；
                RecordHash(Search.pos, HASH_BETA, vl, MAX(nDepth, NULL_DEPTH), 0);
                return vl;
            }
        }
    }

    // 7. 初始化；
    if (Search.pos.LastMove().ChkChs > 0) {
        // 如果是将军局面，那么生成所有应将着法；
        mvEvade = MoveSort.InitEvade(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
    } else {
        // 如果不是将军局面，那么使用正常的着法列表。
        MoveSort.InitFull(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
        mvEvade = 0;
    }

    // 8. 按照"MoveSortStruct::NextFull()"例程的着法顺序逐一搜索；
    while ((mv = MoveSort.NextFull(Search.pos)) != 0) {
        if (Search.pos.MakeMove(mv)) {

            // 9. 尝试选择性延伸；
            nNewDepth = (Search.pos.LastMove().ChkChs > 0 || mvEvade != 0 ? nDepth : nDepth - 1);

            // 10. 零窗口搜索；
            vl = -SearchCut(1 - vlBeta, nNewDepth);
            Search.pos.UndoMakeMove();
            if (Search2.bStop) {
                return vlBest;
            }

            // 11. 截断判定；
            if (vl > vlBest) {
                vlBest = vl;
                if (vl >= vlBeta) {
                    RecordHash(Search.pos, HASH_BETA, vlBest, nDepth, mv);
                    if (!MoveSort.GoodCap(Search.pos, mv)) {
                        SetBestMove(mv, nDepth, Search2.wmvKiller[Search.pos.nDistance]);
                    }
                    return vlBest;
                }
            }
        }
    }

    // 12. 不截断措施。
    if (vlBest == -MATE_VALUE) {
        __ASSERT(Search.pos.IsMate());
        return Search.pos.nDistance - MATE_VALUE;
    } else {
        RecordHash(Search.pos, HASH_ALPHA, vlBest, nDepth, mvEvade);
        return vlBest;
    }
}

// 连接主要变例
static void AppendPvLine(uint16_t *lpwmvDst, uint16_t mv, const uint16_t *lpwmvSrc) {
    *lpwmvDst = mv;
    lpwmvDst ++;
    while (*lpwmvSrc != 0) {
        *lpwmvDst = *lpwmvSrc;
        lpwmvSrc ++;
        lpwmvDst ++;
    }
    *lpwmvDst = 0;
}

/* 主要变例完全搜索例程，和零窗口完全搜索的区别有以下几点：
 *
 * 1. 启用内部迭代加深启发；
 * 2. 不使用有负面影响的裁剪；
 * 3. Alpha-Beta边界判定复杂；
 * 4. PV结点要获取主要变例；
 * 5. 考虑PV结点处理最佳着法的情况。
 */
static int SearchPV(int vlAlpha, int vlBeta, int nDepth, uint16_t *lpwmvPvLine) {
    int nNewDepth, nHashFlag, vlBest, vl;
    int mvBest, mvHash, mv, mvEvade;
    MoveSortStruct MoveSort;
    uint16_t wmvPvLine[LIMIT_DEPTH];
    // 完全搜索例程包括以下几个步骤：

    // 1. 在叶子结点处调用静态搜索；
    *lpwmvPvLine = 0;
    if (nDepth <= 0) {
        __ASSERT(nDepth >= -NULL_DEPTH);
        return SearchQuiesc(Search.pos, vlAlpha, vlBeta);
    }
    Search2.nAllNodes ++;

    // 2. 无害裁剪；
    vl = HarmlessPruning(Search.pos, vlBeta);
    if (vl > -MATE_VALUE) {
        return vl;
    }

    // 3. 置换裁剪；
    vl = ProbeHash(Search.pos, vlAlpha, vlBeta, nDepth, NO_NULL, mvHash);
    if (Search.bUseHash && vl > -MATE_VALUE) {
        // 由于PV结点不适用置换裁剪，所以不会发生PV路线中断的情况
        return vl;
    }

    // 4. 达到极限深度，直接返回评价值；
    __ASSERT(Search.pos.nDistance > 0);
    if (Search.pos.nDistance == LIMIT_DEPTH) {
        return Evaluate(Search.pos, vlAlpha, vlBeta);
    }
    __ASSERT(Search.pos.nDistance < LIMIT_DEPTH);

    // 5. 中断调用；
    Search2.nMainNodes ++;
    vlBest = -MATE_VALUE;
    if ((Search2.nMainNodes & Search.nCountMask) == 0 && Interrupt()) {
        return vlBest;
    }

    // 6. 内部迭代加深启发；
    if (nDepth > IID_DEPTH && mvHash == 0) {
        __ASSERT(nDepth / 2 <= nDepth - IID_DEPTH);
        vl = SearchPV(vlAlpha, vlBeta, nDepth / 2, wmvPvLine);
        if (vl <= vlAlpha) {
            vl = SearchPV(-MATE_VALUE, vlBeta, nDepth / 2, wmvPvLine);
        }
        if (Search2.bStop) {
            return vlBest;
        }
        mvHash = wmvPvLine[0];
    }

    // 7. 初始化；
    mvBest = 0;
    nHashFlag = HASH_ALPHA;
    if (Search.pos.LastMove().ChkChs > 0) {
        // 如果是将军局面，那么生成所有应将着法；
        mvEvade = MoveSort.InitEvade(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
    } else {
        // 如果不是将军局面，那么使用正常的着法列表。
        MoveSort.InitFull(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
        mvEvade = 0;
    }

    // 8. 按照"MoveSortStruct::NextFull()"例程的着法顺序逐一搜索；
    while ((mv = MoveSort.NextFull(Search.pos)) != 0) {
        if (Search.pos.MakeMove(mv)) {

            // 9. 尝试选择性延伸；
            nNewDepth = (Search.pos.LastMove().ChkChs > 0 || mvEvade != 0 ? nDepth : nDepth - 1);

            // 10. 主要变例搜索；
            if (vlBest == -MATE_VALUE) {
                vl = -SearchPV(-vlBeta, -vlAlpha, nNewDepth, wmvPvLine);
            } else {
                vl = -SearchCut(-vlAlpha, nNewDepth);
                if (vl > vlAlpha && vl < vlBeta) {
                    vl = -SearchPV(-vlBeta, -vlAlpha, nNewDepth, wmvPvLine);
                }
            }
            Search.pos.UndoMakeMove();
            if (Search2.bStop) {
                return vlBest;
            }

            // 11. Alpha-Beta边界判定；
            if (vl > vlBest) {
                vlBest = vl;
                if (vl >= vlBeta) {
                    mvBest = mv;
                    nHashFlag = HASH_BETA;
                    break;
                }
                if (vl > vlAlpha) {
                    vlAlpha = vl;
                    mvBest = mv;
                    nHashFlag = HASH_PV;
                    AppendPvLine(lpwmvPvLine, mv, wmvPvLine);
                }
            }
        }
    }

    // 12. 更新置换表、历史表和杀手着法表。
    if (vlBest == -MATE_VALUE) {
        __ASSERT(Search.pos.IsMate());
        return Search.pos.nDistance - MATE_VALUE;
    } else {
        RecordHash(Search.pos, nHashFlag, vlBest, nDepth, mvEvade == 0 ? mvBest : mvEvade);
        if (mvBest != 0 && !MoveSort.GoodCap(Search.pos, mvBest)) {
            SetBestMove(mvBest, nDepth, Search2.wmvKiller[Search.pos.nDistance]);
        }
        return vlBest;
    }
}

/* 根结点搜索例程，和完全搜索的区别有以下几点：
 *
 * 1. 省略无害裁剪(也不获取置换表着法)；
 * 2. 不使用空着裁剪；
 * 3. 选择性延伸只使用将军延伸；
 * 4. 过滤掉禁止着法；
 * 5. 搜索到最佳着法时要做很多处理(包括记录主要变例、结点排序等)；
 * 6. 不更新历史表和杀手着法表。
 */
static int SearchRoot(int nDepth) {
    int nNewDepth, vlBest, vl, mv, nCurrMove;
#ifndef CCHESS_A3800
    uint32_t dwMoveStr;
#endif
    uint16_t wmvPvLine[LIMIT_DEPTH];
    // 根结点搜索例程包括以下几个步骤：

    // 1. 初始化
    vlBest = -MATE_VALUE;
    Search2.MoveSort.ResetRoot();

    // 2. 逐一搜索每个着法(要过滤禁止着法)
    nCurrMove = 0;
    while ((mv = Search2.MoveSort.NextRoot()) != 0) {
        if (Search.pos.MakeMove(mv)) {
#ifndef CCHESS_A3800
            if (Search2.bPopCurrMove || Search.bDebug) {
                dwMoveStr = MOVE_COORD(mv);
                nCurrMove ++;
                printf("info currmove %.4s currmovenumber %d\n", (const char *) &dwMoveStr, nCurrMove);
                fflush(stdout);
            }
#endif

            // 3. 尝试选择性延伸(只考虑将军延伸)
            nNewDepth = (Search.pos.LastMove().ChkChs > 0 ? nDepth : nDepth - 1);

            // 4. 主要变例搜索
            if (vlBest == -MATE_VALUE) {
                vl = -SearchPV(-MATE_VALUE, MATE_VALUE, nNewDepth, wmvPvLine);
                __ASSERT(vl > vlBest);
            } else {
                vl = -SearchCut(-vlBest, nNewDepth);
                if (vl > vlBest) { // 这里不需要" && vl < MATE_VALUE"了
                    vl = -SearchPV(-MATE_VALUE, -vlBest, nNewDepth, wmvPvLine);
                }
            }
            Search.pos.UndoMakeMove();
            if (Search2.bStop) {
                return vlBest;
            }

            // 5. Alpha-Beta边界判定("vlBest"代替了"SearchPV()"中的"vlAlpha")
            if (vl > vlBest) {

                // 6. 如果搜索到第一着法，那么"未改变最佳着法"的计数器加1，否则清零
                Search2.nUnchanged = (vlBest == -MATE_VALUE ? Search2.nUnchanged + 1 : 0);
                vlBest = vl;

                // 7. 搜索到最佳着法时记录主要变例
                AppendPvLine(Search2.wmvPvLine, mv, wmvPvLine);
#ifndef CCHESS_A3800
                PopPvLine(nDepth, vl);
#endif

                // 8. 如果要考虑随机性，则Alpha值要作随机浮动，但已搜索到杀棋时不作随机浮动
                if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
                    vlBest += (Search.rc4Random.NextLong() & Search.nRandomMask) -
                              (Search.rc4Random.NextLong() & Search.nRandomMask);
                    vlBest = (vlBest == Search.pos.DrawValue() ? vlBest - 1 : vlBest);
                }

                // 9. 更新根结点着法列表
                Search2.MoveSort.UpdateRoot(mv);
            }
        }
    }
    return vlBest;
}

// 唯一着法检验是ElephantEye在搜索上的一大特色，用来判断用以某种深度进行的搜索是否找到了唯一着法。
// 其原理是把找到的最佳着法设成禁止着法，然后以(-WIN_VALUE, 1 - WIN_VALUE)的窗口重新搜索，
// 如果低出边界则说明其他着法都将被杀。
static bool SearchUnique(int vlBeta, int nDepth) {
    int vl, mv;
    Search2.MoveSort.ResetRoot(ROOT_UNIQUE);
    // 跳过第一个着法
    while ((mv = Search2.MoveSort.NextRoot()) != 0) {
        if (Search.pos.MakeMove(mv)) {
            vl = -SearchCut(1 - vlBeta, Search.pos.LastMove().ChkChs > 0 ? nDepth : nDepth - 1);
            Search.pos.UndoMakeMove();
            if (Search2.bStop || vl >= vlBeta) {
                return false;
            }
        }
    }
    return true;
}

// 主搜索例程
void SearchMain(int nDepth) {
    int i, vl, vlLast, nDraw;
    int nCurrTimer, nLimitTimer, nLimitNodes;
    bool bUnique;
#ifndef CCHESS_A3800
    int nBookMoves;
    uint32_t dwMoveStr;
    BookStruct bks[MAX_GEN_MOVES];
#endif
    // 主搜索例程包括以下几个步骤：

    // 1. 遇到和棋则直接返回
    if (Search.pos.IsDraw() || Search.pos.RepStatus(3) > 0) {
#ifndef CCHESS_A3800
        printf("nobestmove\n");
        fflush(stdout);
#endif
        return;
    }

#ifndef CCHESS_A3800
    // 2. 从开局库中搜索着法
    if (Search.bUseBook) {
        // a. 获取开局库中的所有走法
        nBookMoves = GetBookMoves(Search.pos, Search.szBookFile, bks);
        if (nBookMoves > 0) {
            vl = 0;
            for (i = 0; i < nBookMoves; i ++) {
                vl += bks[i].wvl;
                dwMoveStr = MOVE_COORD(bks[i].wmv);
                printf("info depth 0 score %d pv %.4s\n", bks[i].wvl, (const char *) &dwMoveStr);
                fflush(stdout);
            }
            // b. 根据权重随机选择一个走法
            vl = Search.rc4Random.NextLong() % (uint32_t) vl;
            for (i = 0; i < nBookMoves; i ++) {
                vl -= bks[i].wvl;
                if (vl < 0) {
                    break;
                }
            }
            __ASSERT(vl < 0);
            __ASSERT(i < nBookMoves);
            // c. 如果开局库中的着法够成循环局面，那么不走这个着法
            Search.pos.MakeMove(bks[i].wmv);
            if (Search.pos.RepStatus(3) == 0) {
                dwMoveStr = MOVE_COORD(bks[i].wmv);
                printf("bestmove %.4s", (const char *) &dwMoveStr);
                // d. 给出后台思考的着法(开局库中第一个即权重最大的后续着法)
                nBookMoves = GetBookMoves(Search.pos, Search.szBookFile, bks);
                Search.pos.UndoMakeMove();
                if (nBookMoves > 0) {
                    dwMoveStr = MOVE_COORD(bks[0].wmv);
                    printf(" ponder %.4s", (const char *) &dwMoveStr);
                }
                printf("\n");
                fflush(stdout);
                return;
            }
            Search.pos.UndoMakeMove();
        }
    }
#endif

    // 3. 如果深度为零则返回静态搜索值
    if (nDepth == 0) {
#ifndef CCHESS_A3800
        printf("info depth 0 score %d\n", SearchQuiesc(Search.pos, -MATE_VALUE, MATE_VALUE));
        fflush(stdout);
        printf("nobestmove\n");
        fflush(stdout);
#endif
        return;
    }

    // 4. 生成根结点的每个着法
    Search2.MoveSort.InitRoot(Search.pos, Search.nBanMoves, Search.wmvBanList);

    // 5. 初始化时间和计数器
    Search2.bStop = Search2.bPonderStop = Search2.bPopPv = Search2.bPopCurrMove = false;
    Search2.nPopDepth = Search2.vlPopValue = 0;
    Search2.nAllNodes = Search2.nMainNodes = Search2.nUnchanged = 0;
    Search2.wmvPvLine[0] = 0;
    ClearKiller(Search2.wmvKiller);
    ClearHistory();
    ClearHash();
    // 由于 ClearHash() 需要消耗一定时间，所以计时从这以后开始比较合理
    Search2.llTime = GetTime();
    vlLast = 0;
    // 如果走了10回合无用着法，那么允许主动提和，以后每隔8回合提和一次
    nDraw = -Search.pos.LastMove().CptDrw;
    if (nDraw > 5 && ((nDraw - 4) / 2) % 8 == 0) {
        Search.bDraw = true;
    }
    bUnique = false;
    nCurrTimer = 0;

    // 6. 做迭代加深搜索
    for (i = 1; i <= nDepth; i ++) {
        // 需要输出主要变例时，第一个"info depth n"是不输出的
#ifndef CCHESS_A3800
        if (Search2.bPopPv || Search.bDebug) {
            printf("info depth %d\n", i);
            fflush(stdout);
        }

        // 7. 根据搜索的时间决定，是否需要输出主要变例和当前思考的着法
        Search2.bPopPv = (nCurrTimer > 300);
        Search2.bPopCurrMove = (nCurrTimer > 3000);
#endif

        // 8. 搜索根结点
        vl = SearchRoot(i);
        if (Search2.bStop) {
            if (vl > -MATE_VALUE) {
                vlLast = vl; // 跳出后，vlLast会用来判断认输或投降，所以需要给定最近一个值
            }
            break; // 没有跳出，则"vl"是可靠值
        }

        nCurrTimer = (int) (GetTime() - Search2.llTime);
        // 9. 如果搜索时间超过适当时限，则终止搜索
        if (Search.nGoMode == GO_MODE_TIMER) {
            // a. 如果没有使用空着裁剪，那么适当时限减半(因为分枝因子加倍了)
            nLimitTimer = (Search.bNullMove ? Search.nProperTimer : Search.nProperTimer / 2);
            // b. 如果当前搜索值没有落后前一层很多，那么适当时限减半
            nLimitTimer = (vl + DROPDOWN_VALUE >= vlLast ? nLimitTimer / 2 : nLimitTimer);
            // c. 如果最佳着法连续多层没有变化，那么适当时限减半
            nLimitTimer = (Search2.nUnchanged >= UNCHANGED_DEPTH ? nLimitTimer / 2 : nLimitTimer);
            if (nCurrTimer > nLimitTimer) {
                if (Search.bPonder) {
                    Search2.bPonderStop = true; // 如果处于后台思考模式，那么只是在后台思考命中后立即中止搜索。
                } else {
                    vlLast = vl;
                    break; // 不管是否跳出，"vlLast"都已更新
                }
            }
        } else if (Search.nGoMode == GO_MODE_NODES) {
            // nLimitNodes的计算方法与nLimitTimer是一样的
            nLimitNodes = (Search.bNullMove ? Search.nNodes : Search.nNodes / 2);
            nLimitNodes = (vl + DROPDOWN_VALUE >= vlLast ? nLimitNodes / 2 : nLimitNodes);
            nLimitNodes = (Search2.nUnchanged >= UNCHANGED_DEPTH ? nLimitNodes / 2 : nLimitNodes);
            // GO_MODE_NODES下是不延长后台思考时间的
            if (Search2.nAllNodes > nLimitNodes) {
                vlLast = vl;
                break;
            }
        }
        vlLast = vl;

        // 10. 搜索到杀棋则终止搜索
        if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
            break;
        }

        // 11. 是唯一着法，则终止搜索
        if (SearchUnique(1 - WIN_VALUE, i)) {
            bUnique = true;
            break;
        }
    }

#ifdef CCHESS_A3800
    Search.mvResult = Search2.wmvPvLine[0];
#else
    // 12. 输出最佳着法及其最佳应对(作为后台思考的猜测着法)
    if (Search2.wmvPvLine[0] != 0) {
        PopPvLine();
        dwMoveStr = MOVE_COORD(Search2.wmvPvLine[0]);
        printf("bestmove %.4s", (const char *) &dwMoveStr);
        if (Search2.wmvPvLine[1] != 0) {
            dwMoveStr = MOVE_COORD(Search2.wmvPvLine[1]);
            printf(" ponder %.4s", (const char *) &dwMoveStr);
        }

        // 13. 判断是否认输或提和，但是经过唯一着法检验的不适合认输或提和(因为搜索深度不够)
        if (!bUnique) {
            if (vlLast > -WIN_VALUE && vlLast < -RESIGN_VALUE) {
                printf(" resign");
            } else if (Search.bDraw && !Search.pos.NullSafe() && vlLast < DRAW_OFFER_VALUE * 2) {
                printf(" draw");
            }
        }
    } else {
        printf("nobestmove");
    }
    printf("\n");
    fflush(stdout);
#endif
}

// 主搜索例程
void SearchMainBotzone(int nDepth) {
    int i, vl, vlLast, nDraw;
    int nCurrTimer, nLimitTimer, nLimitNodes;
    bool bUnique;
#ifndef CCHESS_A3800
    int nBookMoves;
    uint32_t dwMoveStr;
    BookStruct bks[MAX_GEN_MOVES];
#endif
    // 主搜索例程包括以下几个步骤：

    // 1. 遇到和棋则直接返回
    if (Search.pos.IsDraw() || Search.pos.RepStatus(3) > 0) {
#ifndef CCHESS_A3800
//        printf("nobestmove\n");
//        fflush(stdout);
        OutputBotzoneNone();
#endif
        return;
    }

#ifndef CCHESS_A3800
    // 2. 从开局库中搜索着法
    if (Search.bUseBook) {
        // a. 获取开局库中的所有走法
        nBookMoves = GetBookMoves(Search.pos, Search.szBookFile, bks);
        if (nBookMoves > 0) {
            vl = 0;
            for (i = 0; i < nBookMoves; i ++) {
                vl += bks[i].wvl;
                dwMoveStr = MOVE_COORD(bks[i].wmv);
                printf("info depth 0 score %d pv %.4s\n", bks[i].wvl, (const char *) &dwMoveStr);
                fflush(stdout);
            }
            // b. 根据权重随机选择一个走法
            vl = Search.rc4Random.NextLong() % (uint32_t) vl;
            for (i = 0; i < nBookMoves; i ++) {
                vl -= bks[i].wvl;
                if (vl < 0) {
                    break;
                }
            }
            __ASSERT(vl < 0);
            __ASSERT(i < nBookMoves);
            // c. 如果开局库中的着法够成循环局面，那么不走这个着法
            Search.pos.MakeMove(bks[i].wmv);
            if (Search.pos.RepStatus(3) == 0) {
                dwMoveStr = MOVE_COORD(bks[i].wmv);
                printf("bestmove %.4s", (const char *) &dwMoveStr);
                // d. 给出后台思考的着法(开局库中第一个即权重最大的后续着法)
                nBookMoves = GetBookMoves(Search.pos, Search.szBookFile, bks);
                Search.pos.UndoMakeMove();
                if (nBookMoves > 0) {
                    dwMoveStr = MOVE_COORD(bks[0].wmv);
                    printf(" ponder %.4s", (const char *) &dwMoveStr);
                }
                printf("\n");
                fflush(stdout);
                return;
            }
            Search.pos.UndoMakeMove();
        }
    }
#endif

    // 3. 如果深度为零则返回静态搜索值
    if (nDepth == 0) {
#ifndef CCHESS_A3800
        printf("info depth 0 score %d\n", SearchQuiesc(Search.pos, -MATE_VALUE, MATE_VALUE));
        fflush(stdout);
        printf("nobestmove\n");
        fflush(stdout);
#endif
        return;
    }

    // 4. 生成根结点的每个着法
    Search2.MoveSort.InitRoot(Search.pos, Search.nBanMoves, Search.wmvBanList);

    // 5. 初始化时间和计数器
    Search2.bStop = Search2.bPonderStop = Search2.bPopPv = Search2.bPopCurrMove = false;
    Search2.nPopDepth = Search2.vlPopValue = 0;
    Search2.nAllNodes = Search2.nMainNodes = Search2.nUnchanged = 0;
    Search2.wmvPvLine[0] = 0;
    ClearKiller(Search2.wmvKiller);
    ClearHistory();
    ClearHash();
    // 由于 ClearHash() 需要消耗一定时间，所以计时从这以后开始比较合理
    Search2.llTime = GetTime();
    vlLast = 0;
    // 如果走了10回合无用着法，那么允许主动提和，以后每隔8回合提和一次
    nDraw = -Search.pos.LastMove().CptDrw;
    if (nDraw > 5 && ((nDraw - 4) / 2) % 8 == 0) {
        Search.bDraw = true;
    }
    bUnique = false;
    nCurrTimer = 0;

    // 6. 做迭代加深搜索
    for (i = 1; i <= nDepth; i ++) {
        // 需要输出主要变例时，第一个"info depth n"是不输出的
#ifndef CCHESS_A3800
        if (Search2.bPopPv || Search.bDebug) {
//            printf("info depth %d\n", i);
//            fflush(stdout);
        }

        // 7. 根据搜索的时间决定，是否需要输出主要变例和当前思考的着法
        Search2.bPopPv = (nCurrTimer > 300);
        Search2.bPopCurrMove = (nCurrTimer > 3000);
#endif

        // 8. 搜索根结点
        vl = SearchRoot(i);
        if (Search2.bStop) {
            if (vl > -MATE_VALUE) {
                vlLast = vl; // 跳出后，vlLast会用来判断认输或投降，所以需要给定最近一个值
            }
            break; // 没有跳出，则"vl"是可靠值
        }

        nCurrTimer = (int) (GetTime() - Search2.llTime);
        // 9. 如果搜索时间超过适当时限，则终止搜索
        if (Search.nGoMode == GO_MODE_TIMER) {
            // a. 如果没有使用空着裁剪，那么适当时限减半(因为分枝因子加倍了)
            nLimitTimer = (Search.bNullMove ? Search.nProperTimer : Search.nProperTimer / 2);
            // b. 如果当前搜索值没有落后前一层很多，那么适当时限减半
            nLimitTimer = (vl + DROPDOWN_VALUE >= vlLast ? nLimitTimer / 2 : nLimitTimer);
            // c. 如果最佳着法连续多层没有变化，那么适当时限减半
            nLimitTimer = (Search2.nUnchanged >= UNCHANGED_DEPTH ? nLimitTimer / 2 : nLimitTimer);
            if (nCurrTimer > nLimitTimer) {
                if (Search.bPonder) {
                    Search2.bPonderStop = true; // 如果处于后台思考模式，那么只是在后台思考命中后立即中止搜索。
                } else {
                    vlLast = vl;
                    break; // 不管是否跳出，"vlLast"都已更新
                }
            }
        } else if (Search.nGoMode == GO_MODE_NODES) {
            // nLimitNodes的计算方法与nLimitTimer是一样的
            nLimitNodes = (Search.bNullMove ? Search.nNodes : Search.nNodes / 2);
            nLimitNodes = (vl + DROPDOWN_VALUE >= vlLast ? nLimitNodes / 2 : nLimitNodes);
            nLimitNodes = (Search2.nUnchanged >= UNCHANGED_DEPTH ? nLimitNodes / 2 : nLimitNodes);
            // GO_MODE_NODES下是不延长后台思考时间的
            if (Search2.nAllNodes > nLimitNodes) {
                vlLast = vl;
                break;
            }
        }
        vlLast = vl;

        // 10. 搜索到杀棋则终止搜索
        if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
            break;
        }

        // 11. 是唯一着法，则终止搜索
        if (SearchUnique(1 - WIN_VALUE, i)) {
            bUnique = true;
            break;
        }
    }

#ifdef CCHESS_A3800
    Search.mvResult = Search2.wmvPvLine[0];
#else
    // 12. 输出最佳着法及其最佳应对(作为后台思考的猜测着法)
    if (Search2.wmvPvLine[0] != 0) {
        PopPvLineBotzone();
        dwMoveStr = MOVE_COORD(Search2.wmvPvLine[0]);
//        printf("bestmove %.4s", (const char *) &dwMoveStr);
        int best_mv = Search2.wmvPvLine[0];
        std::string retSource("-1"), retTarget("-1");
        retSource[0] = FILE_X(SRC(best_mv)) - FILE_LEFT + 'a';
        retSource[1] = '9' - RANK_Y(SRC(best_mv)) + RANK_TOP;
        retTarget[0] = FILE_X(DST(best_mv)) - FILE_LEFT + 'a';
        retTarget[1] = '9' - RANK_Y(DST(best_mv)) + RANK_TOP;
        Json::Value ret;
        ret["response"]["source"] = retSource;
        ret["response"]["target"] = retTarget;
        Json::FastWriter writer;
        std::cout << writer.write(ret) << std::endl;
        if (Search2.wmvPvLine[1] != 0) {
            dwMoveStr = MOVE_COORD(Search2.wmvPvLine[1]);
//            printf(" ponder %.4s", (const char *) &dwMoveStr);
        }

        // 13. 判断是否认输或提和，但是经过唯一着法检验的不适合认输或提和(因为搜索深度不够)
        if (!bUnique) {
            if (vlLast > -WIN_VALUE && vlLast < -RESIGN_VALUE) {
//                printf(" resign");
            } else if (Search.bDraw && !Search.pos.NullSafe() && vlLast < DRAW_OFFER_VALUE * 2) {
//                printf(" draw");
            }
        }
    } else {
        OutputBotzoneNone();
//        printf("nobestmove");
    }
//    printf("\n");
//    fflush(stdout);
#endif
}

void OutputBotzoneNone() {
    Json::Value ret;
    ret["response"]["source"] = std::string("-1");
    ret["response"]["target"] = std::string("-1");
    Json::FastWriter writer;
    std::cout << writer.write(ret) << std::endl;
}

// end of search.cpp