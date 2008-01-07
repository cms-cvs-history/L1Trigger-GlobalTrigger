#ifndef GlobalTrigger_L1GtMuonCondition_h
#define GlobalTrigger_L1GtMuonCondition_h

/**
 * \class L1GtMuonCondition
 * 
 * 
 * Description: evaluation of a CondMuon condition.
 * 
 * Implementation:
 *    <TODO: enter implementation details>
 *   
 * \author: Vasile Mihai Ghete   - HEPHY Vienna 
 * 
 * $Date$
 * $Revision$
 *
 */

// system include files
#include <iosfwd>
#include <string>

// user include files
//   base classes
#include "L1Trigger/GlobalTrigger/interface/L1GtConditionEvaluation.h"

#include "FWCore/Framework/interface/EventSetup.h"

// forward declarations
class L1GtCondition;
class L1GtMuonTemplate;

class L1MuGMTCand;

class L1GlobalTriggerGTL;

// class declaration
class L1GtMuonCondition : public L1GtConditionEvaluation
{

public:

    /// constructors
    ///     default
    L1GtMuonCondition();

    ///     from base template condition (from event setup usually)
    L1GtMuonCondition(L1GtCondition*, const L1GlobalTriggerGTL*,
        const edm::EventSetup& evSetup);

    // copy constructor
    L1GtMuonCondition(const L1GtMuonCondition&);

    // destructor
    virtual ~L1GtMuonCondition();

    // assign operator
    L1GtMuonCondition& operator=(const L1GtMuonCondition&);

public:

    /// the core function to check if the condition matches
    virtual const bool evaluateCondition() const;

    /// print condition
    virtual void print(std::ostream& myCout) const;

public:

    ///   get / set the pointer to a L1GtCondition
    inline const L1GtMuonTemplate* gtMuonTemplate() const {
        return m_gtMuonTemplate;
    }

    void setGtMuonTemplate(const L1GtMuonTemplate*);
    
    ///   get / set the number of bits for eta of muon objects
    inline int gtIfMuEtaNumberBits() const {
        return m_ifMuEtaNumberBits;
    }

    void setGtIfMuEtaNumberBits(const int&);    

    ///   get / set the pointer to GTL
    inline const L1GlobalTriggerGTL* gtGTL() const {
        return m_gtGTL;
    }

    void setGtGTL(const L1GlobalTriggerGTL*);

private:

    /// copy function for copy constructor and operator=
    void copy(const L1GtMuonCondition& cp);

    /// load muon candidates
    virtual L1MuGMTCand* getCandidate(const int indexCand) const;

    /// function to check a single object if it matches a condition
    const bool checkObjectParameter(const int iCondition,
        const L1MuGMTCand& cand) const;

private:

    /// pointer to a L1GtMuonTemplate
    const L1GtMuonTemplate* m_gtMuonTemplate;

    /// pointer to GTL, to be able to get the trigger objects
    const L1GlobalTriggerGTL* m_gtGTL;

    /// number of bits for eta of muon objects
    int m_ifMuEtaNumberBits;

};

#endif
