import essentia as ess
import yaml
import essentia.standard as es
import sys, os, copy
import numpy as np
import scipy.ndimage.filters as filters

sys.path.append(os.path.join(os.path.dirname(__file__), '../batchProcessing'))
sys.path.append(os.path.join(os.path.dirname(__file__), '../machineLearning'))


import batchProcessing as BP
import mlWrapper as mlw


def feature_extractor_standard(filename, frameSize, hopSize, aggLen):
    
    #print('Starting Feature Extraction for %s',filename)
    
    #loading the audio file into an array
    audio_in=es.MonoLoader(filename=filename)()
    
    #creating algorithm objects and pool objects
    pool=ess.Pool()
    pool2=ess.Pool()
    win=es.Windowing()
    spec=es.Spectrum()
    centroid = es.Centroid()
    flatness = es.Flatness()
    flux = es.Flux()
    mfcc=es.MFCC(lowFrequencyBound=40)
    
    #Compute features frame by frame
    for frame in es.FrameGenerator(audio_in, frameSize = frameSize, hopSize = hopSize):
        spectrum = spec(win(frame))
        band_eneg, mfcc_ftrs=mfcc(spectrum)
        sCentroid = centroid(spectrum)
        sFlatness = flatness(spectrum)
        sFlux = flux(spectrum)
        
        pool.add('lowlevel.mfcc', mfcc_ftrs)
        pool.add('lowlevel.sCentroid', [sCentroid])
        pool.add('lowlevel.sFlatness', [sFlatness])
        
    for ii in xrange(0,pool['lowlevel.mfcc'].shape[0]-aggLen,aggLen):
        pool2.add('meanMFCC', np.mean(pool['lowlevel.mfcc'][ii:ii+aggLen,:],axis=0))
        pool2.add('varMFCC', np.var(pool['lowlevel.mfcc'][ii:ii+aggLen,:],axis=0))
        pool2.add('varCent', np.var(pool['lowlevel.sCentroid'][ii:ii+aggLen],axis=0))
        pool2.add('varFlat', np.var(pool['lowlevel.sFlatness'][ii:ii+aggLen],axis=0))

    pool.clear()
    
    meanMFCC = copy.deepcopy(pool2['meanMFCC'])
    varMFCC = copy.deepcopy(pool2['varMFCC'])
    varCent = copy.deepcopy(pool2['varCent'])
    varFlat = copy.deepcopy(pool2['varFlat'])
    pool2.clear()
    return meanMFCC, varMFCC, varCent, varFlat



def generateBinaryAggMFCCARFF(class1Folder, class2Folder, class1, class2, arffFile, frameDur, hopDur, aggDur):
    """
    This function generates an arff file of MFCC features for two classes class1 and class2 for which MFCCs are extracted from the audio files kept in appropriate folders (first two args)
    """
    
    #features extracted and features to use
    features = ['m1', 'm2', 'm3', 'm4', 'm5', 'm6', 'm7', 'm8', 'm9', 'm10', 'm11', 'm12', 'm13', 'v1', 'v2', 'v3', 'v4', 'v5', 'v6', 'v7', 'v8', 'v9', 'v10', 'v11', 'v12', 'v13', 'vCent', 'vFlat']
    features2Use = ['m1', 'm5', 'm6', 'm7', 'm8', 'm9', 'm10', 'm13', 'v1', 'v2', 'v4', 'v12', 'v13', 'vCent', 'vFlat']
    
    #array of class labels
    classes = [class1, class2]
    
    #finding index of features to be used in classification
    ind_features = []
    for feat in features2Use:
        ind_features.append(features.index(feat))

    #selecting those features
    features = np.array(features)
    features = features[ind_features]
    features = features.tolist()
    
    #writing header for arff file
    fid = open(arffFile,'w')
    fid.write("@relation 'ToWeka_sectionSegmentation'\n")
    for feature in features:
        fid.write("@attribute %s numeric\n"%feature)
    fid.write("@attribute class  {")
    for clas in classes:
        fid.write("%s,\t"%clas)
    fid.write("}\n")
    fid.write("@data\n")
    
    #start extracting features and write
    class1audiofiles = BP.GetFileNamesInDir(class1Folder,'wav')
    for audiofile in class1audiofiles:
        
        #computing dynamically fs, aggLen based on provided hop size
        fs=float(es.MetadataReader(filename=audiofile)()[9])
        framesize = int(np.round(fs*frameDur))
        if framesize%2 ==1:
            framesize=framesize+1
        hopsize = int(np.round(fs*hopDur))
        aggLen = int(np.round(aggDur*fs/hopsize))
        
        meanMFCC, varMFCC, varCent, varFlat  = feature_extractor_standard(audiofile, framesize, hopsize, aggLen);
        featuresAll = np.concatenate((meanMFCC, varMFCC, varCent, varFlat),axis=1)[:,ind_features]
        for ftr in featuresAll:
            fid.write("%f,"*len(features)%tuple(ftr))
            fid.write("%s\n"%classes[0])
        
    
    class2audiofiles = BP.GetFileNamesInDir(class2Folder,'wav')
    for audiofile in class2audiofiles:
        
        #computing dynamically fs, aggLen based on provided hop size
        fs=float(es.MetadataReader(filename=audiofile)()[9])
        framesize = int(np.round(fs*frameDur))
        if framesize%2 ==1:
            framesize=framesize+1
        hopsize = int(np.round(fs*hopDur))
        aggLen = int(np.round(aggDur*fs/hopsize))
        
        meanMFCC, varMFCC, varCent, varFlat  = feature_extractor_standard(audiofile, framesize, hopsize, aggLen)
        featuresAll = np.concatenate((meanMFCC, varMFCC, varCent, varFlat),axis=1)[:,ind_features]
        for ftr in featuresAll:
            fid.write("%f,"*len(features)%tuple(ftr))
            fid.write("%s\n"%classes[1])
        
    fid.close()
    
    
def exportTREEModel(arffFile, modelFile, normFile):
    
    objML = mlw.experimenter()
    objML.readArffFile(arffFile)
    objML.exportTREEModel(objML.features, objML.classLabelsInt, modelFile, normFile)
    
    
def extractSoloPercussion(audiofile, segFile, modelFile, normFile, frameDur, hopDur, aggDur, medianDur=20):
    
    #extactly same set of features used in training of the model
    features = ['m1', 'm2', 'm3', 'm4', 'm5', 'm6', 'm7', 'm8', 'm9', 'm10', 'm11', 'm12', 'm13', 'v1', 'v2', 'v3', 'v4', 'v5', 'v6', 'v7', 'v8', 'v9', 'v10', 'v11', 'v12', 'v13', 'vCent', 'vFlat']
    features2Use = ['m1', 'm5', 'm6', 'm7', 'm8', 'm9', 'm10', 'm13', 'v1', 'v2', 'v4', 'v12', 'v13', 'vCent', 'vFlat']
    
    # indexes of the chosen features
    ind_features = []
    for feat in features2Use:
        ind_features.append(features.index(feat))
    
    
    #computing dynamically fs, aggLen based on provided hop size
    fs=float(es.MetadataReader(filename=audiofile)()[9])
    framesize = int(np.round(fs*frameDur))
    if framesize%2 ==1:
            framesize=framesize+1
    hopsize = int(np.round(fs*hopDur))
    aggLen = int(np.round(aggDur*fs/hopsize))
        
    #computing features
    meanMFCC, varMFCC, varCent, varFlat  = feature_extractor_standard(audiofile, framesize, hopsize, aggLen)
    features = np.concatenate((meanMFCC, varMFCC, varCent, varFlat),axis=1)[:,ind_features]
    
    #normalization step, read the values used to normalize features while building the model
    fid=file(normFile,'r')
    normVals = yaml.load(fid)
    fid.close()
    for ii in np.arange(features.shape[1]):
            features[:,ii] = features[:,ii]-normVals[ii]['mean']
            features[:,ii] = features[:,ii]/normVals[ii]['var']
            
    #initializing object to predic classes using built model
    perc = 0 #= solo percussion
    nperc = 1 #= all except solo percussion
    objML = mlw.experimenter()
    prediction = objML.predicByModel(modelFile, features)
    
    ### CRUCIAL STEP
    # There are few frames in between voice section which are labelled as tani (either becauseof strong (or solo) mridandam part or missclassification or some other factor), these are generally 1 or 2 aggLen marked as tani. 
    #AS we don't want to loose any vocal segment we would like to do a median filering here roughly duration of 20 seconds. Which means only those segments longer than 10 seconds in continuam would be marked as same label.
    median_length = int(np.round(medianDur/aggDur))
    prediction = filters.median_filter(prediction,size= median_length)
    prediction = filters.median_filter(prediction,size= median_length)
    

    #array in which we store start and ending of every solo percussion segment
    perc_sec = []
    
    #flag
    perStr = 0
    for ii,val in enumerate(prediction):
        if val == perc and perStr == 0:
            perStr = 1
            strInd=ii
        if val == nperc and perStr ==1:
            perStr = 0
            perc_sec.append([strInd, ii])
    if perStr==1:
         perc_sec.append([strInd, prediction.shape[0]])
    
    #converting array to float values
    perc_sec = np.array(perc_sec).astype(np.float)
    #converting from indexes to time stamps
    perc_sec = perc_sec*float(aggLen)*float(hopsize)/float(fs)
    
    for sec in perc_sec:
        if(sec[1]-sec[0])<120 or sec[1] < 0.5*float(prediction.shape[0])*float(aggLen)*float(hopsize)/float(fs):
            print "WIERD AUDIO FILE IS HERE: %s"%audiofile
            
    np.savetxt(segFile, perc_sec,  fmt='%.3f',)
    
    return 1

def extractSoloPercussionBATCHPROC(root_dir, modelFile, normFile, frameDur, hopDur, aggDur, medianDur=20):
    
    
    audiofiles = BP.GetFileNamesInDir(root_dir,'mp3')
    
    for ii, audiofile in enumerate(audiofiles):
        print "pricessing %d of %d files"%(ii+1, len(audiofiles))
        print "File being processed %s\n"%audiofile
        fname, ext = os.path.splitext(audiofile)
        segFile = fname + '.taniSeg'
        extractSoloPercussion(audiofile, segFile, modelFile, normFile, frameDur, hopDur, aggDur, medianDur)
