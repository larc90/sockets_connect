#ifndef WININET_H
#define WININET_H

#ifdef __WIN32__
/********* Macros definition **********/
#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

/********* Global function definition **********/
s32 inet_pton(s32 af, const s8 *src, s8 *dst);
s32 inet_pton4(const s8 *src, s8 *dst);
s32 inet_pton6(const s8 *src, s8 *dst);
#endif

#endif // WININET_H
