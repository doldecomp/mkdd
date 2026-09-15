#ifndef JAUDIO_JASFAKEMATCH14_H
#define JAUDIO_JASFAKEMATCH14_H

#ifdef MATCHING

#include "JSystem/JAudio/JASFakeMatch.h"

class JAIStream;
class JAISeq;
class JAISe;

typedef JAUDopplerAudible<4> TDopplerAudible4;

JASMemPool<TDopplerAudible4> JASPoolAllocObject<TDopplerAudible4>::memPool_;
JASMemPool<JAISeq> JASPoolAllocObject<JAISeq>::memPool_;
JASMemPool<JAISe> JASPoolAllocObject<JAISe>::memPool_;

#endif

#endif
