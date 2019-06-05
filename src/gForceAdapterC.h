#ifndef _GFORCEADAPTERC_H
#define _GFORCEADAPTERC_H

#include "gForceDataTypes.h"


#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE ((BOOL)1)
#endif

#ifndef FALSE
#define FALSE ((BOOL)0)
#endif


#ifdef __cplusplus
extern "C" {
#endif


/*  Implement me: get char that usart2 receve from gforcejoint */
int SYS_GetChar(unsigned char *data);

/* Implement me: get system run time */
unsigned long SYS_GetTick(void);

enum GF_Ret GFC_GetgForcedata(struct GF_Data *gForceData, unsigned long timeout);

enum GF_Ret GFC_QuaternionToEuler(const struct GF_Quaternion *quat, struct GF_Euler *euler);

BOOL GFC_GetGesture(enum GF_Gesture gesture, unsigned long timeout);


#ifdef __cplusplus
}
#endif


#endif
