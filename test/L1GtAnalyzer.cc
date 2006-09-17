/**
 * \class L1GtAnalyzer
 * 
 * 
 * 
 * Description: see header file 
 * Implementation:
 *    <TODO: enter implementation details>
 *   
 * \author: Vasile Mihai Ghete - HEPHY Vienna
 * 
 * $Date:$
 * $Revision:$
 *
 */

// this class header
#include "L1Trigger/GlobalTrigger/test/L1GtAnalyzer.h"

// system include files
#include <memory>

// user include files
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1TriggerObject.h"

#include "L1Trigger/GlobalTrigger/interface/L1GlobalTrigger.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"

#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctEmCand.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctJetCand.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctEtSums.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctJetCounts.h"

#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerSetup.h"

#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerPSB.h"
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerGTL.h"
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerFDL.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"



// constructor(s)
L1GtAnalyzer::L1GtAnalyzer(const edm::ParameterSet& iConfig) {
   // initialization

}

// destructor
L1GtAnalyzer::~L1GtAnalyzer() {
 
}

// member functions

// analyze each event: event loop 
void L1GtAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
   using namespace edm;

    LogDebug("L1GtAnalyzer") << "\n**** L1GtAnalyzer::analyze starting ****\n" 
        << std::endl;
  
}


// method called once each job just before starting event loop
void L1GtAnalyzer::beginJob(const edm::EventSetup&)
{
}

// method called once each job just after ending the event loop
void L1GtAnalyzer::endJob() {

}

// static data members 


//define this as a plug-in
DEFINE_FWK_MODULE(L1GtAnalyzer)
