//
// Created by JinWen on 2019/4/29.
//

#ifndef JETANALYSOR_JETSELECTOR_H
#define JETANALYSOR_JETSELECTOR_H

#include <vector>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/Selector.hh>

#include "utils.h"

using namespace std;
using namespace fastjet;

namespace JetAnalysor {

    vector<PseudoJet> SelectJet(const vector<PseudoJet> &hadrons, JetDefinition jet_def, Selector select);

    vector<PseudoJet> SelectJet(const vector<PseudoJet> &hadrons, JetDefinition jet_def, Selector select) {
        ClusterSequence cluster(hadrons, jet_def);
        vector<PseudoJet> jets = sorted_by_pt(select(cluster.inclusive_jets()));
        return jets;
    }
}


#endif //JETANALYSOR_JETSELECTOR_H
