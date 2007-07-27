/**
 * \class L1GlobalTriggerSetup
 * 
 * 
 * Description: see header file.
 *  
 * Implementation:
 *    <TODO: enter implementation details>
 *   
 * \author: Vasile Mihai Ghete - HEPHY Vienna
 * 
 * $Date$
 * $Revision$
 *
 */

// this class header
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerSetup.h"

// system include files
#include <iostream>
#include <string>

// user include files
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerConfig.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/MessageLogger/interface/MessageDrop.h"

// forward declarations

// constructor
L1GlobalTriggerSetup::L1GlobalTriggerSetup(
    L1GlobalTrigger& gt,
    const edm::ParameterSet& iConfig)
        : m_GT(gt)
{
    LogDebug ("Trace") << "Entering " << __PRETTY_FUNCTION__ << std::endl;

    m_pSet = &iConfig;

    // input tag for muon collection from GMT
    m_muGmtInputTag = m_pSet->getUntrackedParameter<edm::InputTag>(
                          "GmtInputTag", edm::InputTag("gmt"));

    LogDebug("L1GlobalTriggerSetup")
    << "\nInput tag for muon collection from GMT: "
    << m_muGmtInputTag.label() << " \n"
    << std::endl;

    // input tag for calorimeter collection from GCT
    m_caloGctInputTag = m_pSet->getUntrackedParameter<edm::InputTag>(
                            "GctInputTag", edm::InputTag("gct"));

    LogDebug("L1GlobalTriggerSetup")
    << "\nInput tag for calorimeter collection from GCT: "
    << m_caloGctInputTag.label() << " \n"
    << std::endl;

}

// destructor
L1GlobalTriggerSetup::~L1GlobalTriggerSetup()
{
    if (m_gtConfig) {
        delete m_gtConfig;
    }
    m_gtConfig = 0;
}


// methods


// input tag for muon collection from GMT
const edm::InputTag L1GlobalTriggerSetup::muGmtInputTag() const
{

    return m_muGmtInputTag;
}

const edm::InputTag L1GlobalTriggerSetup::caloGctInputTag() const
{

    return m_caloGctInputTag;
}


//
void L1GlobalTriggerSetup::setTriggerMenu(std::string& menuDir)
{

    LogDebug ("Trace") << "Entering " << __PRETTY_FUNCTION__ << std::endl;

    if ( menuDir == "" ) {
        menuDir = m_pSet->getParameter<std::string>("triggerMenu");
    }

    std::string defXmlFileName = m_pSet->getParameter<std::string>("xmlConfig");
    edm::FileInPath f1("L1Trigger/GlobalTrigger/data/" + menuDir + "/" + defXmlFileName);

    std::string defXmlFile = f1.fullPath();

    edm::LogInfo("L1GlobalTriggerSetup")
    << "\n\n  Trigger menu and configuration: XML file: \n  " << defXmlFile << "\n\n"
    << std::endl;

    std::string vmeXmlFile;

    std::string vmeXmlFileName = m_pSet->getParameter<std::string>("xmlVME");
    if (vmeXmlFileName != "") {
        edm::FileInPath f2("L1Trigger/GlobalTrigger/data/" + menuDir + "/" + vmeXmlFileName);

        vmeXmlFile = f2.fullPath();

        LogDebug("L1GlobalTriggerSetup")
        << "FileInPath: XML File for VME-bus preamble: \n  " << vmeXmlFile
        << std::endl;
    }

    // get the new trigger configuration
    if (!m_gtConfig) {
        m_gtConfig = new L1GlobalTriggerConfig(&m_GT, defXmlFile, vmeXmlFile);
    }

    // set the trigger mask: block the corresponding algorithm if bit value is 1
    // one converts from vector as there is no "bitset" parameter type
    std::vector<unsigned int> trigMaskV =
        m_pSet->getParameter<std::vector<unsigned int> >("triggerMask");

    std::bitset<L1GlobalTriggerConfig::MaxNumberAlgorithms> trigMask;
    for (unsigned int i = 0; i < L1GlobalTriggerConfig::MaxNumberAlgorithms; ++i) {
        if ( trigMaskV[i] ) {
            trigMask.set(i);
        }
    }

    m_gtConfig->setTriggerMask(trigMask);

    if ( edm::isDebugEnabled() ) {

        LogDebug("L1GlobalTriggerSetup")
        << "Trigger mask: bit number  & mask value \n  "
        << std::endl;

        for (unsigned int i = 0; i < L1GlobalTriggerConfig::MaxNumberAlgorithms; ++i) {

            LogTrace("L1GlobalTriggerSetup")
            << "Bit number " << i << "\t" << m_gtConfig->getTriggerMask()[i]
            << std::endl;
        }

    }

    // set the input mask: bit 0 GCT, bit 1 GMT
    // one converts from vector as there is no "bitset" parameter type
    std::vector<unsigned int> inMaskV =
        m_pSet->getParameter<std::vector<unsigned int> >("inputMask");

    std::bitset<L1GlobalTriggerConfig::NumberInputModules> inMask;
    for (unsigned int i = 0; i < L1GlobalTriggerConfig::NumberInputModules; ++i) {
        if ( inMaskV[i] ) {
            inMask.set(i);
        }
    }

    m_gtConfig->setInputMask(inMask);

    if ( m_gtConfig->getInputMask()[0] ) {

        edm::LogVerbatim("L1GlobalTriggerSetup")
        << "\n**** Calorimeter input disabled! \n     inputMask[0] = "
        << m_gtConfig->getInputMask()[0]
        << "     All candidates empty." << "\n**** \n"
        << std::endl;
    }

    if ( m_gtConfig->getInputMask()[1] ) {

        edm::LogVerbatim("L1GlobalTriggerSetup")
        << "\n**** Global Muon input disabled! \n  inputMask[1] = "
        << m_gtConfig->getInputMask()[1]
        << "     All candidates empty." << "\n**** \n"
        << std::endl;
    }


}

// static class members
const edm::ParameterSet* L1GlobalTriggerSetup::m_pSet = 0;
L1GlobalTriggerConfig* L1GlobalTriggerSetup::m_gtConfig = 0;
