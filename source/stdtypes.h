/** @file stdtypes.h
 **
 ** Standard types for skycatch test.
 **
 ** @author: Leonel Robles
 ** @since: Jul-2018
 **/

#ifndef STDTYPES_H
#define STDTYPES_H

/********* Typedef definitions **********/
typedef unsigned char       u8;
typedef char                s8;
typedef unsigned short      u16;
typedef short               s16;
typedef unsigned int        u32;
typedef unsigned char       boolean;
typedef int                 s32;
typedef float               f32;
typedef double              f64;

#ifndef TRUE
#define TRUE  (1U)
#endif

#ifndef FALSE
#define FALSE  (0U)
#endif

#endif // STDTYPES_H
