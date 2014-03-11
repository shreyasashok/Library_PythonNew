

#ifndef PATTERNPROCSTRUCTURES_H

#define PATTERNPROCSTRUCTURES_H

#include "hashDefs.h"

typedef struct segInfo
{
    float str;
    float end;
}segInfo_t;

typedef struct segInfoInterp
{
    float str;
    float end[MAXNTEMPOFACTORS];
    
}segInfoInterp_t;


typedef struct motifInfo
{
  
    DISTTYPE dist;
    INDTYPE ind1;
    INDTYPE ind2;
    
}motifInfo_t;





typedef struct procParams
{
    int binsPOct;
    int dsFactor;
    int removeTaniSegs;
    float minPossiblePitch;
    float allowedSilDur;
    float varDur;
    float threshold;
    float flatThreshold;
    float maxPauseDur;
    float factorLow;
    float factorHigh;
    float durMotif;
    float blackDur;
    float DTWBand;
    INDTYPE nPitchSamples;
    float interpFac[MAXNTEMPOFACTORS];
    int nInterpFac;
    int motifLengths[MAXNTEMPOFACTORS];
    int motifLengthsM1[MAXNTEMPOFACTORS];
    int indexMotifLenReal;
    int indexMotifLenLongest;
    
    int lenMotifReal;
    int lenMotifRealM1;
    int lenMotifInterpH;
    int lenMotifInterpL;
    int lenMotifInterpHM1;
    int lenMotifInterpLM1;
    
    
}procParams_t;



#endif //PATTERNPROCSTRUCTURES_H