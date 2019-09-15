//example hepmc reader

#include "utils.h"
#include "jetSelector.h"
#include "calcDeltaPhiDist.h"
#include "distCalculator.h"

#include <HepMC/IO_GenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
// #include <fastjet/Selector.hh>

#include <iostream>
#include <fstream>
// #include <iomanip>
#include <vector>

using namespace std;
using namespace fastjet;

// 用于 vector 排序
// static bool CompareDouble(double u, double v) { return (u < v); }

const static double R_jet = 0.3;
JetDefinition jet_def(antikt_algorithm, R_jet);
Selector select_akt = SelectorAbsEtaMax(1.6) && SelectorPtMin(30.);

JetAnalysor::distCalculator mDistOfDeltaPhi(0., JetAnalysor::PI, 0.15);

const static PseudoJet mInitParton(0., 0., 0., 0.);

int main(int argc, char *argv[]) {

    if (argc == 1) {
        cout << "    " << "ERROR: Please supply HepMC file as argument..." << endl;
        return 1;
    }

    for (int file_num = 1; file_num < argc; file_num++) {

        char *hepmc_file_name = argv[file_num];
        HepMC::IO_GenEvent ascii_in(hepmc_file_name, std::ios::in);
        HepMC::GenEvent* evt = ascii_in.read_next_event();

        while ( evt ) {

            vector<PseudoJet> pseudo_jets;
            vector<PseudoJet> pseudo_Z_bosons;
            PseudoJet Z_boson;
            PseudoJet pseudo_Z_from_electron(0., 0., 0., 0.);
            PseudoJet pseudo_Z_from_mu(0., 0., 0., 0.);
            bool wait_anti_electron = false, wait_anti_mu = false;
            bool findZBoson = false;
            
            pseudo_jets.clear();

            for (HepMC::GenEvent::particle_iterator p = evt->particles_begin();
                    p != evt->particles_end(); ++p) {

                // get final state particles
                if ( isFinal(*p) && 
                        !( abs((*p)->pdg_id()) == 11 || abs((*p)->pdg_id()) == 13 )) {
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), 
                        (*p)->momentum().pz(), (*p)->momentum().e());
                    pseudo_jets.emplace_back(tmp);
                }

                if ( (*p)->pdg_id() == 11 && isFinal(*p) ) {
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), 
                        (*p)->momentum().pz(), (*p)->momentum().e());
                    if ( ( fabs(tmp.eta()) < 1.44 || ( fabs(tmp.eta()) > 1.57 && 
                        fabs(tmp.eta()) < 2.5 ) ) && tmp.pt() > 20.0 ) {
                        //
                        pseudo_Z_from_electron = tmp;
                        wait_anti_electron = true;
                    } else {
                        break;
                    }
                    
                }

                if ( (*p)->pdg_id() == -11 && isFinal(*p) && wait_anti_electron ) {
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), 
                        (*p)->momentum().pz(), (*p)->momentum().e());
                    if ( ( fabs(tmp.eta()) < 1.44 || ( fabs(tmp.eta()) > 1.57 && 
                        fabs(tmp.eta()) < 2.5 ) ) && tmp.pt() > 20.0 ) {
                        //
                        pseudo_Z_from_electron += tmp;
                        pseudo_Z_from_electron.set_user_index(11);
                        pseudo_Z_bosons.emplace_back(pseudo_Z_from_electron);
                        wait_anti_electron = false;
                        pseudo_Z_from_electron = mInitParton;
                    } else {
                        break;
                    }
                }

                if ( (*p)->pdg_id() == 13 && isFinal(*p)  ) {
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), 
                        (*p)->momentum().pz(), (*p)->momentum().e());
                    if ( fabs(tmp.eta()) < 2.4 && tmp.pt() > 10.0 ) {
                        pseudo_Z_from_mu += tmp;
                        wait_anti_mu = true;
                    } else {
                        break;
                    }
                }

                if ( (*p)->pdg_id() == -13 && isFinal(*p) && wait_anti_mu ) {
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), 
                        (*p)->momentum().pz(), (*p)->momentum().e());
                    if (fabs(tmp.eta()) < 2.4 && tmp.pt() > 10.0) {
                        pseudo_Z_from_mu += tmp;
                        pseudo_Z_from_mu.set_user_index(13);
                        pseudo_Z_bosons.emplace_back(pseudo_Z_from_mu);
                        wait_anti_mu = false;
                        pseudo_Z_from_mu = mInitParton;
                    } else {
                        break;
                    }
                }

            }

            
            vector<PseudoJet> tmp_jets = JetAnalysor::SelectJet(pseudo_jets, 
                jet_def, select_akt);

            Selector select_Z = SelectorPtMin(60.) && SelectorMassRange(70, 110) && SelectorAbsRapMax(2.5);
            auto Z_Bosons_By_Select = select_Z(pseudo_Z_bosons); 

            // cout << evt->event_number() << endl;
            if (Z_Bosons_By_Select.size() == 1) {
                Z_boson = Z_Bosons_By_Select[0];
                findZBoson = true;
            } else if (Z_Bosons_By_Select.size() > 1) {
                Z_boson = sorted_by_E(Z_Bosons_By_Select)[0];
                findZBoson = true;
            } else {
                // cout << "ERROR: we don't find any Z boson at this event." << endl;
                findZBoson = false;
            }
            
            
            if (tmp_jets.size() != 0 && findZBoson) {
                // cout << "Z boson: " << evt->event_number() << ", " << 
                //     Z_boson.px() << ", " << Z_boson.py() << ", " << Z_boson.pz() 
                //     << ", " << Z_boson.e() << ", " << Z_boson.m() << ", "
                //     << Z_boson.pt() << ", " << Z_boson.eta() << endl; 
                for (unsigned jet_i = 0; jet_i < tmp_jets.size(); jet_i++) {
                        double delta_phi_value = fabs(tmp_jets[jet_i].phi() - 
                            Z_boson.phi());
                        delta_phi_value = delta_phi_value > pi ? 
                            2 * pi - delta_phi_value : delta_phi_value;
                        mDistOfDeltaPhi.addEventNorm(
                            (evt->weights())[0] / (evt->weights())[2]
                        );
                        mDistOfDeltaPhi.addEventNum(delta_phi_value, 
                            (evt->weights())[0] / (evt->weights())[2]);
                }
            }

            delete evt;
            ascii_in >> evt;
        }
    }

        std::ofstream delta_phi_dist_file;
        const char *out_delta_phi_dist_file = "deltaPhiDist.dat";
        delta_phi_dist_file.open(out_delta_phi_dist_file);
        auto temp = mDistOfDeltaPhi.getDistList();
        for(unsigned i = 0; i < temp.size(); i++) {
            delta_phi_dist_file << temp[i].variable 
                << ", " << temp[i].dist_value << endl;
        }

    return 0;
} //end main
