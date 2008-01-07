/**
 * \class L1GtEnergySumCondition
 * 
 * 
 * Description: evaluation of a CondEnergySum condition.
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

// this class header
#include "L1Trigger/GlobalTrigger/interface/L1GtEnergySumCondition.h"

// system include files
#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <algorithm>

// user include files
//   base classes
#include "CondFormats/L1TObjects/interface/L1GtEnergySumTemplate.h"
#include "L1Trigger/GlobalTrigger/interface/L1GtConditionEvaluation.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"

#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctEtSums.h"

#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerFunctions.h"
#include "L1Trigger/GlobalTrigger/interface/L1GlobalTriggerPSB.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

// constructors
//     default
L1GtEnergySumCondition::L1GtEnergySumCondition() :
    L1GtConditionEvaluation() {

    //empty

}

//     from base template condition (from event setup usually)
L1GtEnergySumCondition::L1GtEnergySumCondition(L1GtCondition* eSumTemplate,
    const L1GlobalTriggerPSB* ptrPSB, const edm::EventSetup& evSetup) :
    L1GtConditionEvaluation()

{

    m_gtEnergySumTemplate = static_cast<L1GtEnergySumTemplate*>(eSumTemplate);

    m_gtPSB = ptrPSB;

    // maximum number of objects received for the evaluation of the condition
    // energy sums are global quantities - one object per event 

    m_condMaxNumberObjects = 1;

}

// copy constructor
void L1GtEnergySumCondition::copy(const L1GtEnergySumCondition &cp) {

    m_gtEnergySumTemplate = cp.gtEnergySumTemplate();
    m_gtPSB = cp.gtPSB();

    m_condMaxNumberObjects = cp.condMaxNumberObjects();
    m_condLastResult = cp.condLastResult();
    m_combinationsInCond = cp.getCombinationsInCond();

}

L1GtEnergySumCondition::L1GtEnergySumCondition(const L1GtEnergySumCondition& cp) :
    L1GtConditionEvaluation() {

    copy(cp);

}

// destructor
L1GtEnergySumCondition::~L1GtEnergySumCondition() {

    // empty

}

// equal operator
L1GtEnergySumCondition& L1GtEnergySumCondition::operator= (const L1GtEnergySumCondition& cp)
{
    copy(cp);
    return *this;
}

// methods
void L1GtEnergySumCondition::setGtEnergySumTemplate(
    const L1GtEnergySumTemplate* eSumTempl) {

    m_gtEnergySumTemplate = eSumTempl;

}

///   set the pointer to PSB
void L1GtEnergySumCondition::setGtPSB(const L1GlobalTriggerPSB* ptrPSB) {

    m_gtPSB = ptrPSB;

}

// try all object permutations and check spatial correlations, if required
const bool L1GtEnergySumCondition::evaluateCondition() const {

    // number of trigger objects in the condition
    // in fact, there is only one object
    int iCondition = 0;

    // condition result condResult set to true if the energy sum
    // passes all requirements
    bool condResult = false;

    // store the indices of the calorimeter objects
    // from the combination evaluated in the condition
    SingleCombInCond objectsInComb;

    // clear the m_combinationsInCond vector
    (*m_combinationsInCond).clear();

    // clear the indices in the combination
    objectsInComb.clear();

    // get energy and phi (ETM only) for the trigger object

    unsigned int candEt = 0;
    unsigned int candPhi = 0;

    switch ((m_gtEnergySumTemplate->objectType())[0]) {
        case ETT: {
            L1GctEtTotal* cand1 = m_gtPSB->getCandL1ETT();

            if (cand1 == 0) {
                return false;
            }

            candEt = cand1->et();

            break;
        }
        case ETM: {
            L1GctEtMiss* cand2 = m_gtPSB->getCandL1ETM();

            if (cand2 == 0) {
                return false;
            }

            candEt = cand2->et();
            candPhi = cand2->phi();

            break;
        }
        case HTT: {
            L1GctEtHad* cand3 = m_gtPSB->getCandL1HTT();

            if (cand3 == 0) {
                return false;
            }

            candEt = cand3->et();

            break;
        }
        default: {
            // should not arrive here
            return false;

            break;
        }
    }

    const L1GtEnergySumTemplate::ObjectParameter objPar = ( *(m_gtEnergySumTemplate->objectParameter()) )[iCondition];

    // check energy threshold
    if ( !checkThreshold(objPar.etThreshold, candEt,
        m_gtEnergySumTemplate->condGEq()) ) {

        return false;
    }

    /// for ETM check phi also
    if ((m_gtEnergySumTemplate->objectType())[0] == ETM) {

        // phi bitmask is saved in two u_int64_t (see parser)
        if (candPhi < 64) {
            if (!checkBit(objPar.phiRange0Word, candPhi) ) {

                return false;
            }
        }
        else {
            if (!checkBit(objPar.phiRange1Word, candPhi - 64)) {

                return false;
            }
        }

    }

    // index is always zero, as they are global quantities (there is only one object)
    int indexObj = 0;

    objectsInComb.push_back(indexObj);
    (*m_combinationsInCond).push_back(objectsInComb);

    // if we get here all checks were successfull for this combination
    // set the general result for evaluateCondition to "true"

    condResult = true;
    return condResult;

}

void L1GtEnergySumCondition::print(std::ostream& myCout) const {

    m_gtEnergySumTemplate->print(myCout);
    L1GtConditionEvaluation::print(myCout);

}

