import FWCore.ParameterSet.Config as cms

process = cms.Process("H2TestBeam")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

#
#	Command Line Input(Copied from DQM for now)
#
import sys
#if len(sys.argv)!= 3:
#	print "### ERROR: No Run File has been provided"
#	print "### Use: cmsRun h2testbeamanalyzer_cfg.py <run number>"
#	sys.exit(1)

#
#	Change the filename to process
#
#runNumber = sys.argv[2]

process.source = cms.Source("HcalTBSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
#		'root://eoscms//eos/cms/store/group/comm_hcal/LS1/USC_226003.root'
#		'root://eoscms//eos/cms/store/group/comm_hcal/LS1/USC_227057.root'
#		'file:/afs/cern.ch/work/v/vkhriste/CMSSW/CMSSW_6_0_1/src/UserCode/H2TestBeamAnalyzer/data_H2_TB/HTB_007385.root' 
#		'file:/data/spool/HTB_' + runNumber + '.root'
#		'file:/afs/cern.ch/user/j/jhirsch/HTB_007480.root'
		'file:./HTBqc_000008-2.root'
	)
)

process.options = cms.untracked.PSet(
		wantSummary = cms.untracked.bool(False),
                SkipEvent = cms.untracked.vstring('ProductNotFound')
		)

process.tbunpack = cms.EDProducer("HcalTBObjectUnpacker",
		IncludeUnmatchedHits = cms.untracked.bool(False),
		ConfigurationFile=cms.untracked.string('configQADCTDC.txt'),
		#HcalSlowDataFED = cms.untracked.int32(3),
		HcalTriggerFED = cms.untracked.int32(1),
		#HcalTDCFED = cms.untracked.int32(8),
		#HcalQADCFED = cms.untracked.int32(8),
	        fedRawDataCollectionTag = cms.InputTag('source')
)

process.hcalDigis = cms.EDProducer("HcalRawToDigi",
#		UnpackHF = cms.untracked.bool(True),
		### Falg to enable unpacking of TTP channels(default = false)
		### UnpackTTP = cms.untracked.bool(True),
		FilterDataQuality = cms.bool(False),
		InputLabel = cms.InputTag('source'),
		HcalFirstFED = cms.untracked.int32(700),
		ComplainEmptyData = cms.untracked.bool(False),
#		UnpackCalib = cms.untracked.bool(True),
		firstSample = cms.int32(0),
		lastSample = cms.int32(9)
)

process.hcalDigis.FEDs = cms.untracked.vint32(700,928)


process.hcalAnalyzer = cms.EDAnalyzer('H2TestBeamAnalyzer',
		OutFileName = cms.untracked.string('ana_h2_tb_run_b28.root'),
		Verbosity = cms.untracked.int32(0)
)

#
#	For Debugging: Create a Pool Output Module
#
process.output = cms.OutputModule(
		'PoolOutputModule',
		fileName = cms.untracked.string('h2testbeamanalyzer_cfg_b28.root')
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

#	EMAP Needed for H2 DATA
process.es_ascii = cms.ESSource('HcalTextCalibrations',
		input = cms.VPSet(
			cms.PSet(
				object = cms.string('ElectronicsMap'),
#				file = cms.FileInPath('UserCode/H2TestBeamAnalyzer/emap_H2_validatedSIC_jul2013.txt')
				file = cms.FileInPath(
					'UserCode/H2TestBeamAnalyzer/EMAP_b28.txt'
				)
			)
		)
)
process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')

process.dump = cms.EDAnalyzer("HcalDigiDump")

#process.p = cms.Path(process.tbunpack*process.hcalDigis*process.dump*process.hcalAnalyzer)
process.p = cms.Path(process.tbunpack*process.hcalDigis*process.dump)
process.outpath = cms.EndPath(process.output)












