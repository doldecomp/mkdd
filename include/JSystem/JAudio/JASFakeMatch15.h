#ifndef JAUDIO_JASFAKEMATCH15_H
#define JAUDIO_JASFAKEMATCH15_H

#ifdef MATCHING

#include "JSystem/JAudio/JASFakeMatch.h"

class JAISe;

typedef JAUDopplerAudible<4> TDopplerAudible4;

JASMemPool<TDopplerAudible4> JASPoolAllocObject<TDopplerAudible4>::memPool_;
JASMemPool<JAISe> JASPoolAllocObject<JAISe>::memPool_;

#endif

#endif
