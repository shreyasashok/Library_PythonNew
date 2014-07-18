
#ifndef  TSA_DATAIO_H

#define TSA_DATAIO_H

#include "TSAhashDefs.h"
#include "TSAdataStructs.h"


class TSAparamHandle
{
    
public:
    procParams_t procParams;
    fileExts_t fileExts;
    
    TSAparamHandle();
    
    int readParamsFromFile(char *paramFile);
    int readFileExtsInfoFile(char* infoFileExts);
    
    procParams_t* getParamPtr();
    fileExts_t* getExtPtr();
};

class fileNameHandler
{
    
public:
    char fileName[MAX_FNAME_CHARS];
    fileExts_t *fileExtPtr;
    char *baseName;
    
    fileNameHandler();
    int initialize(char *bName, fileExts_t *fExtPtr);
    char *getTSFileName();
    char *getTonicFileName();
    char *getBlackListSegFileName();
    char *getLogFileName();
    char *getParamDumpFileName();
    char *getOutFileName();
};


class TSAdataHandler
{
    
public:
    
    procLogs_t *procLogPtr;
    fileExts_t *fileExtPtr;
    procParams_t procParams;
    char *baseName;
    fileNameHandler fHandle;
    
    
    TSAIND lenTS;
    TSAIND nSubSeqs;
    TSAIND nLinesFile;
    
    
    int *blacklist;
    
    
    
    
    TSAsam_t *samPtr;
    TSAsubSeq_t *subSeqPtr;
    
    float pHop;
    
    int         readTSData(char *fileName);
    void*       readTSSubSeq(char* fileName, void *subSeq, int len, int sizeSample);
    int       readHopSizeTS(char *fileName);
    int         dumpMotifInfo();
    int         countNumberLines();
    int         genTemplate1SubSeqs();
    TSAIND      getNumLines(const char *file);
    int         downSampleTS();
    int         filterSamplesTS();
    int         convertHz2Cents(char *tonicFileName);
    int         initializeBlackList();
    int         updateBLDurThsld();
    int         updateBLStdThsld();
    int         updateBLInvalidSegment(char *fileName);
    int         calculateDiffMotifLengths();
    int         genSlidingWindowSubSeqs();
    int         computeStdTSLocal(float **std, int varSam);
    int         filterBlackListedSubSeqs();
    int         genUniScaledSubSeqs();
    int         dumpDiscMotifInfo(char *motifFile, TSAmotifInfo_t *priorityQDisc, int K, int verbos);
    
    TSAdataHandler(char *bName, procLogs_t *procLogs, fileExts_t *fileExts, procParams_t *pParams);
};



//MISC functions
int initializeLogCounts(procLogs_t *myProcLogs);



#endif //TSA_DATAIO_H