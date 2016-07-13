import FWCore.ParameterSet.Config as cms

DEBUG_OPTIONS = False

import sys
if len(sys.argv) != 9:
    print len(sys.argv)
    print "### ERROR: Wrong number of arguments.  Please use h2-tb-analyzer.py."
    print "### Usage: cmsRun h2-tb-analyzer-run.py <input file> <emap file> <verbosity> <num events> <run number> <shunt setting> <run adcHists only>"
    sys.exit(1)

inputFile = sys.argv[2]
emapFile = sys.argv[3]
runNumber = sys.argv[4]
numEvents = int(sys.argv[5])
verbosityLevel = int(sys.argv[6])
shuntSetting = float(sys.argv[7])
runadcHistsOnly = int(sys.argv[8])

if numEvents == 0: numEvents = -1

if DEBUG_OPTIONS:
    print "inputFile =",inputFile
    print "emapFile =",emapFile
    print "runNumber =",runNumber
    print "numEvents =",numEvents
    print "verbosityLevel =",verbosityLevel
    print "shuntSetting =",shuntSetting
    sys.exit(1)

process = cms.Process("H2TestBeam")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(numEvents) )

process.source = cms.Source("HcalTBSource",
    fileNames = cms.untracked.vstring(
        'file:'+inputFile
    )
)

process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(False)
        )

process.tbunpack = cms.EDProducer("HcalTBObjectUnpacker",
        IncludeUnmatchedHits = cms.untracked.bool(False),
        ConfigurationFile=cms.untracked.string('UserCode/H2TestBeamAnalyzer/configQADCTDC.txt'),
#        HcalSlowDataFED = cms.untracked.int32(3),
#        HcalTriggerFED = cms.untracked.int32(1),
#        HcalTDCFED = cms.untracked.int32(8),

        HcalTriggerFED = cms.untracked.int32(1),
#        HcalTDCFED = cms.untracked.int32(928), #original from Alexey
#        HcalQADCFED = cms.untracked.int32(928), #original from Alexey
        HcalTDCFED = cms.untracked.int32(1132), #temporary for CRF (USC)
        HcalQADCFED = cms.untracked.int32(1132), #temporary for CRF (USC)
            fedRawDataCollectionTag = cms.InputTag('source')
)

process.hcalDigis = cms.EDProducer("HcalRawToDigi",
#       UnpackHF = cms.untracked.bool(True),
        ### Flag to enable unpacking of TTP channels(default = false)
        ### UnpackTTP = cms.untracked.bool(True),
        FilterDataQuality = cms.bool(False),
        InputLabel = cms.InputTag('source'),
        HcalFirstFED = cms.untracked.int32(700),
        ComplainEmptyData = cms.untracked.bool(False),
#       UnpackCalib = cms.untracked.bool(True),
        firstSample = cms.int32(0),
        lastSample = cms.int32(9)
)

#process.hcalDigis.FEDs = cms.untracked.vint32(700,928) #original from Alexey
process.hcalDigis.FEDs = cms.untracked.vint32(700,1132) #temporary, for CRF (USC)

emapFileShort = emapFile.rsplit('.',1)[0].rsplit('/')[-1]

process.hcalAnalyzer = cms.EDAnalyzer('H2TestBeamAnalyzer',
        OutFileName = cms.untracked.string('ana_h2_tb_run'+runNumber+'_'+emapFileShort+'_processing.root'),
        Verbosity = cms.untracked.int32(verbosityLevel),
        Gain = cms.untracked.double(shuntSetting)
)

process.hcalADCHists = cms.EDAnalyzer('adcHists',
                                      gain = cms.untracked.double(shuntSetting)
)

#
#   For Debugging: Create a Pool Output Module
#
process.output = cms.OutputModule(
        'PoolOutputModule',
        fileName = cms.untracked.string('cmsrun_out_h2_tb_run'+runNumber+emapFileShort+'.root')
)

process.TFileService = cms.Service("TFileService",
       fileName = cms.string("analysis_run"+runNumber+"_"+emapFileShort+".root"),
)

process.load('Configuration.Geometry.GeometryIdeal_cff')
#process.load('RecoLocalCalo.Configuration.hcalLocalReco_cff')
#process.load('RecoLocalCalo.HcalRecProducers.HcalSimpleReconstructor_hf_cfi')

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.autoCond import autoCond
#process.GlobalTag.globaltag = autoCond['com10']
from CondCore.DBCommon.CondDBSetup_cfi import *

process.GlobalTag.globaltag = autoCond['startup'] 
#process.es_prefer_GlobalTag = cms.ESPrefer('PoolDBESSource', 'GlobalTag')

#   EMAP Needed for H2 DATA
process.es_ascii = cms.ESSource('HcalTextCalibrations',
        input = cms.VPSet(
               cms.PSet(
                object = cms.string('ElectronicsMap'),
                file = cms.FileInPath(emapFile)  # EMAP here!
               )
        )
)
process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')

process.dump = cms.EDAnalyzer("HcalDigiDump")

if runadcHistsOnly:
    process.p = cms.Path(process.tbunpack*process.hcalDigis*process.hcalADCHists)
else:
    if verbosityLevel > 0:
        process.p = cms.Path(process.tbunpack*process.hcalDigis*process.dump*process.hcalAnalyzer*process.hcalADCHists)
    else:
        process.p = cms.Path(process.tbunpack*process.hcalDigis*process.hcalAnalyzer*process.hcalADCHists)

# process.outpath = cms.EndPath(process.output)
