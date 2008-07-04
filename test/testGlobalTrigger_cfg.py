#
# cfg file to run L1 Global Trigger emulator on a file containing the output of the 
# GCT system and GMT system
#

import FWCore.ParameterSet.Config as cms

# process
process = cms.Process("TestGlobalTrigger")

# number of events to be processed and source file
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(25)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/g/ghete/scratch0/CmsswTestFiles/testGt_Emulator_GctGmtFile_source.root')
)

process.PoolSource.fileNames = [
    '/store/relval/2008/6/25/RelVal-RelValQCD_Pt_120_170-1214239099-STARTUP_V1-2nd/0007/049232E8-AF42-DD11-BC5B-000423D9880C.root',
    '/store/relval/2008/6/25/RelVal-RelValQCD_Pt_120_170-1214239099-STARTUP_V1-2nd/0007/04A4CD1C-B542-DD11-8ACE-000423D98C20.root',
    '/store/relval/2008/6/25/RelVal-RelValQCD_Pt_120_170-1214239099-STARTUP_V1-2nd/0007/0AF90409-AF42-DD11-BA95-001617DBCF6A.root'
]

# load and configure modules

# L1 EventSetup
process.load("L1Trigger.Configuration.L1DummyConfig_cff")

# prescaled menu    
process.load("L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1x1032.L1Menu2007_cff")
#process.load("L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1030.L1Menu2008_2E30_cff")
#process.load("L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1031.L1Menu2008_2E31_cff")

# uprescaled menu - change prescale factors to 1
#process.load("L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1x1032.L1Menu2007_Unprescaled_cff")
#process.load("L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1030.L1Menu2008_2E30_Unprescaled_cff")
#process.load("L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1031.L1Menu2008_2E31_Unprescaled_cff")
#process.load("L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1x1032.L1MenuTestCondCorrelation_cff")

# Global Trigger emulator
process.load("L1Trigger.GlobalTrigger.gtDigis_cfi")

# input tag for GMT readout collection: 
#     gmtDigis = GMT emulator (default)
#     l1GtUnpack     = GT unpacker (common GT/GMT unpacker)
process.gtDigis.GmtInputTag = 'hltGtDigis'

# input tag for GCT readout collections: 
#     gctDigis = GCT emulator (default) 
process.gtDigis.GctInputTag = 'hltGctDigis'

# logical flag to produce the L1 GT DAQ readout record
#     if true, produce the record (default)
#process.gtDigis.ProduceL1GtDaqRecord = False
    
# logical flag to produce the L1 GT EVM readout record
#     if true, produce the record (default)
#process.gtDigis.ProduceL1GtEvmRecord = False

# logical flag to produce the L1 GT object map record
#     if true, produce the record (default)
#process.gtDigis.ProduceL1GtObjectMapRecord = False

# logical flag to write the PSB content in the  L1 GT DAQ record
#     if true, write the PSB content in the record (default)
#process.gtDigis.WritePsbL1GtDaqRecord = False

# logical flag to read the technical trigger records
#     if true, it will read via getMany the available records (default)
#process.gtDigis.ReadTechnicalTriggerRecords = False

# number of "bunch crossing in the event" (BxInEvent) to be emulated
# symmetric around L1Accept (BxInEvent = 0):
#    1 (BxInEvent = 0); 3 (F 0 1) (standard record); 5 (E F 0 1 2) (debug record)
# even numbers (except 0) "rounded" to the nearest lower odd number
# negative value: emulate TotalBxInEvent as given in EventSetup  
#process.gtDigis.EmulateBxInEvent = 3

# length of BST record (in bytes) from parameter set
# negative value: take the value from EventSetup      
#process.gtDigis.BstLengthBytes = 52

# path to be run
process.p = cms.Path(process.gtDigis)

# services

# Message Logger
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories = ['*']
process.MessageLogger.destinations = ['cout']
process.MessageLogger.cout = cms.untracked.PSet(
    #threshold = cms.untracked.string('ERROR'),
    #threshold = cms.untracked.string('INFO'),
    #INFO = cms.untracked.PSet(
    #    limit = cms.untracked.int32(-1)
    #)#,
    threshold = cms.untracked.string('DEBUG'),
    DEBUG = cms.untracked.PSet(
        limit = cms.untracked.int32(-1)     ## DEBUG, all messages
    )
)
process.MessageLogger.debugModules = ['gtDigis']

# summary
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

# output 

# keep GMT, GCT and GT output records
# update labels if InputTags are replaced above

process.outputL1GlobalTrigger = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('testGt_Emulator_GctGmtFile_output.root'),
    outputCommands = cms.untracked.vstring('drop *', 
        'keep *_hltGtDigis_*_*', 
        'keep *_hltGctDigis_*_*', 
        'keep *_gtDigis_*_*')
)

process.outpath = cms.EndPath(process.outputL1GlobalTrigger)
