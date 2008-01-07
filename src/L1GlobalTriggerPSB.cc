/**
 * \class L1GlobalTriggerPSB
 * 
 * 
 * Description: Pipelined Synchronising Buffer, see header file for details.  
 *
 * Implementation:
 *    <TODO: enter implementation details>
 *   
 * \author: M. Fierro            - HEPHY Vienna - ORCA version 
 * \author: Vasile Mihai Ghete   - HEPHY Vienna - CMSSW version 
 * 
 * $Date$
 * $Revision$
 *
 */

// this class header
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerPSB.h"

// system include files
#include <bitset>

// user include files
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctCand.h"

#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctEmCand.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctJetCand.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctEtSums.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctJetCounts.h"

#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctCollections.h"

#include "L1Trigger/GlobalTrigger/interface/L1GlobalTrigger.h"
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerSetup.h"
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerConfig.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "CondFormats/L1TObjects/interface/L1GtParameters.h"
#include "CondFormats/DataRecord/interface/L1GtParametersRcd.h"

#include "CondFormats/L1TObjects/interface/L1GtFwd.h"
#include "CondFormats/L1TObjects/interface/L1GtBoard.h"
#include "CondFormats/L1TObjects/interface/L1GtBoardMaps.h"
#include "CondFormats/DataRecord/interface/L1GtBoardMapsRcd.h"

// forward declarations

// constructor
L1GlobalTriggerPSB::L1GlobalTriggerPSB(L1GlobalTrigger& gt)
        : m_GT(gt),
        m_candL1NoIsoEG        ( new L1GctCandVector(L1GlobalTriggerReadoutSetup::NumberL1Electrons) ),
        m_candL1IsoEG( new L1GctCandVector(L1GlobalTriggerReadoutSetup::NumberL1IsolatedElectrons) ),
        m_candL1CenJet      ( new L1GctCandVector(L1GlobalTriggerReadoutSetup::NumberL1CentralJets) ),
        m_candL1ForJet      ( new L1GctCandVector(L1GlobalTriggerReadoutSetup::NumberL1ForwardJets) ),
        m_candL1TauJet          ( new L1GctCandVector(L1GlobalTriggerReadoutSetup::NumberL1TauJets) ),
        m_candETM(0),
        m_candETT(0),
        m_candHTT(0),
        m_candJetCounts(0)
{

    m_candL1NoIsoEG->reserve(L1GlobalTriggerReadoutSetup::NumberL1Electrons);
    m_candL1IsoEG->reserve(L1GlobalTriggerReadoutSetup::NumberL1IsolatedElectrons);
    m_candL1CenJet->reserve(L1GlobalTriggerReadoutSetup::NumberL1CentralJets);
    m_candL1ForJet->reserve(L1GlobalTriggerReadoutSetup::NumberL1ForwardJets);
    m_candL1TauJet->reserve(L1GlobalTriggerReadoutSetup::NumberL1TauJets);

}

// destructor
L1GlobalTriggerPSB::~L1GlobalTriggerPSB()
{

    reset();
    m_candL1NoIsoEG->clear();
    m_candL1IsoEG->clear();
    m_candL1CenJet->clear();
    m_candL1ForJet->clear();
    m_candL1TauJet->clear();

    delete m_candL1NoIsoEG;
    delete m_candL1IsoEG;
    delete m_candL1CenJet;
    delete m_candL1ForJet;
    delete m_candL1TauJet;

    if ( m_candETM )
        delete m_candETM;
    if ( m_candETT )
        delete m_candETT;
    if ( m_candHTT )
        delete m_candHTT;

    if ( m_candJetCounts )
        delete m_candJetCounts;

}

// operations

// receive input data

void L1GlobalTriggerPSB::receiveGctObjectData(
    edm::Event& iEvent,
    const edm::InputTag& caloGctInputTag, const int iBxInEvent,
    const bool receiveNoIsoEG, const unsigned int nrL1NoIsoEG,
    const bool receiveIsoEG, const unsigned int nrL1IsoEG,
    const bool receiveCenJet, const unsigned int nrL1CenJet,
    const bool receiveForJet, const unsigned int nrL1ForJet,
    const bool receiveTauJet, const unsigned int nrL1TauJet,
    const bool receiveETM, const bool receiveETT, const bool receiveHTT,
    const bool receiveJetCounts)
{

    reset();

    if ( edm::isDebugEnabled() && (iBxInEvent != 0) ) {

        LogDebug("L1GlobalTriggerPSB")
        << "\n**** Temporary fix for bunch cross treatment in GCT"
        << "\n     Bunch cross " << iBxInEvent
        << ": all candidates empty." << "\n**** \n"
        << std::endl;

    }

    if (receiveNoIsoEG && (iBxInEvent == 0)) {

        // get GCT NoIsoEG
        edm::Handle<L1GctEmCandCollection> emCands;
        iEvent.getByLabel(caloGctInputTag.label(), "nonIsoEm", emCands);

        for ( unsigned int i = 0; i < nrL1NoIsoEG; i++ ) {

            CaloDataWord dataword = 0;
            unsigned int nElec = 0;

            for (L1GctEmCandCollection::const_iterator
                    it = emCands->begin(); it != emCands->end(); it++) {

                if ( nElec == i ) {
                    dataword = (*it).raw();
                    break;
                }
                nElec++;

            }

            bool isolation = false;
            (*m_candL1NoIsoEG)[i] = new L1GctEmCand( dataword, isolation );

        }


    } else {

        // empty GCT NoIsoEG
        for ( unsigned int i = 0; i < nrL1NoIsoEG; i++ ) {

            CaloDataWord dataword = 0;

            bool isolation = false;
            (*m_candL1NoIsoEG)[i] = new L1GctEmCand( dataword, isolation );

        }
    }



    if (receiveIsoEG && (iBxInEvent == 0)) {

        // get GCT IsoEG
        edm::Handle<L1GctEmCandCollection> isoEmCands;
        iEvent.getByLabel(caloGctInputTag.label(), "isoEm",    isoEmCands);

        for ( unsigned int i = 0; i < nrL1IsoEG; i++ ) {

            CaloDataWord dataword = 0;
            unsigned int nElec = 0;

            for (L1GctEmCandCollection::const_iterator
                    it = isoEmCands->begin(); it != isoEmCands->end(); it++) {

                if ( nElec == i ) {
                    dataword = (*it).raw();
                    break;
                }
                nElec++;
            }

            bool isolation = true;
            (*m_candL1IsoEG)[i] = new L1GctEmCand( dataword, isolation );

        }

    } else {

        // empty GCT IsoEG

        for ( unsigned int i = 0; i < nrL1IsoEG; i++ ) {

            CaloDataWord dataword = 0;

            bool isolation = true;
            (*m_candL1IsoEG)[i] = new L1GctEmCand( dataword, isolation );

        }
    }



    if (receiveCenJet && (iBxInEvent == 0)) {

        // get GCT CenJet
        edm::Handle<L1GctJetCandCollection> cenJets;
        iEvent.getByLabel(caloGctInputTag.label(), "cenJets", cenJets);

        // central jets
        for ( unsigned int i = 0; i < nrL1CenJet; i++ ) {

            CaloDataWord dataword = 0;
            unsigned int nJet = 0;

            for (L1GctJetCandCollection::const_iterator
                    it = cenJets->begin(); it != cenJets->end(); it++) {

                if ( nJet == i ) {
                    dataword = (*it).raw();
                    break;
                }
                nJet++;
            }

            bool isTau = false;
            bool isFor = false;
            (*m_candL1CenJet)[i] = new L1GctJetCand( dataword, isTau, isFor );

        }

    } else {

        // empty GCT CenJet
        for ( unsigned int i = 0; i < nrL1CenJet; i++ ) {

            CaloDataWord dataword = 0;

            bool isTau = false;
            bool isFor = false;
            (*m_candL1CenJet)[i] = new L1GctJetCand( dataword, isTau, isFor );

        }

    }


    if (receiveForJet && (iBxInEvent == 0)) {

        // get GCT ForJet
        edm::Handle<L1GctJetCandCollection> forJets;
        iEvent.getByLabel(caloGctInputTag.label(), "forJets", forJets);

        // forward jets
        for ( unsigned int i = 0; i < nrL1ForJet; i++ ) {

            CaloDataWord dataword = 0;
            unsigned int nJet = 0;

            for (L1GctJetCandCollection::const_iterator
                    it = forJets->begin(); it != forJets->end(); it++) {

                if ( nJet == i ) {
                    dataword = (*it).raw();
                    break;
                }
                nJet++;
            }

            bool isTau = false;
            bool isFor = true;
            (*m_candL1ForJet)[i] = new L1GctJetCand( dataword, isTau, isFor );

        }

    } else {

        // empty GCT ForJet
        for ( unsigned int i = 0; i < nrL1ForJet; i++ ) {

            CaloDataWord dataword = 0;

            bool isTau = false;
            bool isFor = true;
            (*m_candL1ForJet)[i] = new L1GctJetCand( dataword, isTau, isFor );

        }

    }


    if (receiveTauJet && (iBxInEvent == 0)) {

        // get GCT TauJet
        edm::Handle<L1GctJetCandCollection> tauJets;
        iEvent.getByLabel(caloGctInputTag.label(), "tauJets", tauJets);

        for ( unsigned int i = 0; i < nrL1TauJet; i++ ) {

            CaloDataWord dataword = 0;
            unsigned int nJet = 0;

            for (L1GctJetCandCollection::const_iterator
                    it = tauJets->begin(); it != tauJets->end(); it++) {

                if ( nJet == i ) {
                    dataword = (*it).raw();
                    break;
                }
                nJet++;
            }

            bool isTau = true;
            bool isFor = false;
            (*m_candL1TauJet)[i] = new L1GctJetCand( dataword, isTau, isFor );

        }

    } else {

        // empty GCT TauJet
        for ( unsigned int i = 0; i < nrL1TauJet; i++ ) {

            CaloDataWord dataword = 0;

            bool isTau = true;
            bool isFor = false;
            (*m_candL1TauJet)[i] = new L1GctJetCand( dataword, isTau, isFor );

        }

    }


    if (receiveETM && (iBxInEvent == 0)) {

        // get GCT ETM
        edm::Handle<L1GctEtMiss>  missEt;
        iEvent.getByLabel(caloGctInputTag.label(), missEt);

        m_candETM = new L1GctEtMiss(  (*missEt).raw()  );

    } else {

        // null values for energy sums and jet counts
        m_candETM = new L1GctEtMiss();

    }


    if (receiveETT && (iBxInEvent == 0)) {

        // get GCT ETT
        edm::Handle<L1GctEtTotal> totalEt;
        iEvent.getByLabel(caloGctInputTag.label(), totalEt);

        m_candETT   = new L1GctEtTotal( (*totalEt).raw() );

    } else {

        // null values for energy sums and jet counts
        m_candETT   = new L1GctEtTotal();

    }

    if (receiveHTT && (iBxInEvent == 0)) {

        // get GCT HTT
        edm::Handle<L1GctEtHad>   totalHt;
        iEvent.getByLabel(caloGctInputTag.label(), totalHt);

        m_candHTT   = new L1GctEtHad(   (*totalHt).raw() );

    } else {

        // null values for energy sums and jet counts
        m_candHTT   = new L1GctEtHad();

    }


    if (receiveJetCounts && (iBxInEvent == 0)) {

        // get GCT JetCounts
        edm::Handle<L1GctJetCounts> jetCounts;
        iEvent.getByLabel(caloGctInputTag.label(), jetCounts);

        m_candJetCounts = new L1GctJetCounts( (*jetCounts) );

    } else {

        // null values for energy sums and jet counts
        m_candJetCounts = new L1GctJetCounts();

    }


    if ( edm::isDebugEnabled() ) {
        LogDebug("L1GlobalTriggerPSB")
        << "**** L1GlobalTriggerPSB received calorimeter data from input tag "
        << caloGctInputTag.label()
        << std::endl;

        printGctObjectData();
    }


}

// fill the content of active PSB boards
void L1GlobalTriggerPSB::fillPsbBlock(
    edm::Event& iEvent,
    const edm::EventSetup& evSetup,
    const int iBxInEvent,
    std::auto_ptr<L1GlobalTriggerReadoutRecord>& gtDaqReadoutRecord)
{

    // get parameters
    edm::ESHandle< L1GtParameters > l1GtPar ;
    evSetup.get< L1GtParametersRcd >().get( l1GtPar ) ;

    //    active boards in L1 GT DAQ record
    boost::uint16_t activeBoardsGtDaq = l1GtPar->gtDaqActiveBoards();

    // get board maps
    edm::ESHandle< L1GtBoardMaps > l1GtBM;
    evSetup.get< L1GtBoardMapsRcd >().get( l1GtBM );

    const std::vector<L1GtBoard> boardMaps = l1GtBM->gtBoardMaps();
    typedef std::vector<L1GtBoard>::const_iterator CItBoardMaps;

    // loop over PSB blocks in the GT DAQ record and fill them
    // with the content of the object list

    for (CItBoardMaps
            itBoard = boardMaps.begin();
            itBoard != boardMaps.end(); ++itBoard) {

        int iPosition = itBoard->gtPositionDaqRecord();
        if (iPosition > 0) {

            int iActiveBit = itBoard->gtBitDaqActiveBoards();
            bool activeBoard = false;

            if (iActiveBit >= 0) {
                activeBoard = activeBoardsGtDaq & (1 << iActiveBit);
            }

            if (activeBoard && (itBoard->gtBoardType() == PSB)) {

                L1GtPsbWord psbWordValue;

                // set board ID
                psbWordValue.setBoardId(itBoard->gtBoardId());

                // set bunch cross in the GT event record
                psbWordValue.setBxInEvent(iBxInEvent);

                // set bunch cross number of the actual bx
                boost::uint16_t bxNrValue = 0; // FIXME
                psbWordValue.setBxNr(bxNrValue);


                // set event number since last L1 reset generated in PSB
                psbWordValue.setEventNr(
                    static_cast<boost::uint32_t>(iEvent.id().event()) );

                // set A_DATA_CH_IA
                //psbWordValue.setAData(boost::uint16_t aDataVal, int iA);

                // set B_DATA_CH_IB
                //psbWordValue.setBData(boost::uint16_t bDataVal, int iB);

                // set local bunch cross number of the actual bx
                boost::uint16_t localBxNrValue = 0; // FIXME
                psbWordValue.setLocalBxNr(localBxNrValue);

                // ** fill L1GtfeWord in GT DAQ record

                gtDaqReadoutRecord->setGtPsbWord(psbWordValue);


                // get the objects coming to this PSB
                std::vector<L1GtPsbQuad> quadInPsb = itBoard->gtQuadInPsb();
                for (std::vector<L1GtPsbQuad>::const_iterator
                        itQuad = quadInPsb.begin();
                        itQuad != quadInPsb.end(); ++itQuad) {

                    switch (*itQuad) {

                        case TechTr: {
                                // FIXME write TechTr
                            }

                            break;
                        case NoIsoEGQ: {
                                // FIXME write objects in PSB;
                            }

                            break;
                        case IsoEGQ: {
                                // FIXME write objects in PSB;
                            }

                            break;
                        case CenJetQ: {
                                // FIXME write objects in PSB;
                            }

                            break;
                        case ForJetQ: {
                                // FIXME write objects in PSB;
                            }

                            break;
                        case TauJetQ: {
                                // FIXME write objects in PSB;
                            }

                            break;
                        case ESumsQ: {
                                // FIXME write objects in PSB;
                            }

                            break;
                        case JetCountsQ: {
                                // FIXME write objects in PSB;
                            }

                            break;
                            // FIXME add MIP/Iso bits
                        default: {
                                // do nothing
                            }

                            break;
                    } // end switch (*itQuad)

                } // end for: (itQuad)

            } // end if (active && PSB)

        } // end if (iPosition)

    } // end for (itBoard

}

// clear PSB

void L1GlobalTriggerPSB::reset()
{

    L1GctCandVector::iterator iter;

    for ( iter = m_candL1NoIsoEG->begin(); iter < m_candL1NoIsoEG->end(); iter++ ) {
        if (*iter) {
            delete (*iter);
            *iter = 0;
        }
    }

    for ( iter = m_candL1IsoEG->begin(); iter < m_candL1IsoEG->end(); iter++ ) {
        if (*iter) {
            delete (*iter);
            *iter = 0;
        }
    }

    for ( iter = m_candL1CenJet->begin(); iter < m_candL1CenJet->end(); iter++ ) {
        if (*iter) {
            delete (*iter);
            *iter = 0;
        }
    }

    for ( iter = m_candL1ForJet->begin(); iter < m_candL1ForJet->end(); iter++ ) {
        if (*iter) {
            delete (*iter);
            *iter = 0;
        }
    }

    for ( iter = m_candL1TauJet->begin(); iter < m_candL1TauJet->end(); iter++ ) {
        if (*iter) {
            delete (*iter);
            *iter = 0;
        }
    }

    if ( m_candETM ) {
        delete m_candETM;
        m_candETM = 0;
    }

    if ( m_candETT )   {
        delete m_candETT;
        m_candETT = 0;
    }

    if ( m_candHTT )   {
        delete m_candHTT;
        m_candHTT = 0;
    }

    if ( m_candJetCounts ) {
        delete m_candJetCounts;
        m_candJetCounts = 0;
    }

}

// print Global Calorimeter Trigger data
// use int to bitset conversion to print
void L1GlobalTriggerPSB::printGctObjectData() const
{

    LogTrace("L1GlobalTriggerPSB")
    << "\n L1GlobalTrigger Calorimeter input data\n" << std::endl;

    L1GctCandVector::const_iterator iter;

    LogTrace("L1GlobalTriggerPSB") << "   GCT NoIsoEG " << std::endl;
    for ( iter = m_candL1NoIsoEG->begin(); iter < m_candL1NoIsoEG->end(); iter++ ) {

        LogTrace("L1GlobalTriggerPSB")
        << "Rank = " << (*iter)->rank()
        << " Eta index = " << (*iter)->etaIndex()
        << " Phi index = " << (*iter)->phiIndex()
        << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT IsoEG " << std::endl;
    for ( iter = m_candL1IsoEG->begin(); iter < m_candL1IsoEG->end(); iter++ ) {
        LogTrace("L1GlobalTriggerPSB")
        << "Rank = " << (*iter)->rank()
        << " Eta index = " << (*iter)->etaIndex()
        << " Phi index = " << (*iter)->phiIndex()
        << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT CenJet " << std::endl;
    for ( iter = m_candL1CenJet->begin(); iter < m_candL1CenJet->end(); iter++ ) {
        LogTrace("L1GlobalTriggerPSB")
        << "Rank = " << (*iter)->rank()
        << " Eta index = " << (*iter)->etaIndex()
        << " Phi index = " << (*iter)->phiIndex()
        << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT ForJet " << std::endl;
    for ( iter = m_candL1ForJet->begin(); iter < m_candL1ForJet->end(); iter++ ) {
        LogTrace("L1GlobalTriggerPSB")
        << "Rank = " << (*iter)->rank()
        << " Eta index = " << (*iter)->etaIndex()
        << " Phi index = " << (*iter)->phiIndex()
        << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT TauJet " << std::endl;
    for ( iter = m_candL1TauJet->begin(); iter < m_candL1TauJet->end(); iter++ ) {
        LogTrace("L1GlobalTriggerPSB")
        << "Rank = " << (*iter)->rank()
        << " Eta index = " << (*iter)->etaIndex()
        << " Phi index = " << (*iter)->phiIndex()
        << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT ETM " << std::endl;
    if ( m_candETM ) {
        LogTrace("L1GlobalTriggerPSB") << "ET  = " << m_candETM->et() << std::endl;
        LogTrace("L1GlobalTriggerPSB") << "phi = " << m_candETM->phi() << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT ETT " << std::endl;
    if ( m_candETT )   {
        LogTrace("L1GlobalTriggerPSB") <<  "ET  = " << m_candETT->et() << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT HTT " << std::endl;
    if ( m_candHTT )   {
        LogTrace("L1GlobalTriggerPSB") <<  "ET  = " << m_candHTT->et() << std::endl;
    }

    LogTrace("L1GlobalTriggerPSB") << "   GCT JetCounts " << std::endl;
    if ( m_candJetCounts ) {
        LogTrace("L1GlobalTriggerPSB") << (*m_candJetCounts) << std::endl;
    }


}



// static data members
