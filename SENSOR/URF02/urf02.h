#ifndef __URF02_H
#define __URF02_H
#include "sys.h"
void URF_Init( void );
void Read_URF( u8 no );
extern u8 counting[2];
extern u8 over_time[2];
extern float URF_DIST[2];
#endif
