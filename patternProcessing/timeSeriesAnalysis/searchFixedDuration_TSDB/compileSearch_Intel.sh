g++ -I. ../../../similarityMeasures/dtw/dtw.c ../../../basicDSPFuncs/basicDSPCFuncs.c ../../../similarityMeasures/dtw/tables.c ../TSATables.cpp ../TSApool.cpp ../TSAsimilarity.cpp ../TSAdataIO.cpp searchPatterns.cpp ../TSAlogs.cpp -O3 -o searchPatterns_O3