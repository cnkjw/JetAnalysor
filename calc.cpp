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
#include <fastjet/Selector.hh>

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace fastjet;

const static double PI = 3.1415926;

const static double R_jet = 0.3;
JetDefinition jet_def(antikt_algorithm, R_jet);
Selector select_akt = SelectorAbsEtaMax(1.6) && SelectorPtMin(30.);

double weight_total = 0.;
double phi_bin[9] = {0, 0.635, 1.25, 1.72, 2.20, 2.51, 2.82, 2.98, 3.14};
double weight_for_bin[9] = {0.};


int main(int argc, char *argv[]) {

    if (argc == 1) {
        cout << "    " << "ERROR: Please supply HepMC file as argument..." << endl;
        return 1;
    }

    ifstream in_file(argv[1]);
    if (!in_file.is_open()) {
        cout << "Can't open file." << endl;
        exit(EXIT_FAILURE);
    }

    while (!in_file.eof()) {
        int event_id, pdg_code, partons_num, Z_daughter_pdg_code;
        double Z_daughter_1_x, Z_daughter_1_y, Z_daughter_1_z, Z_daughter_1_energy;
        double Z_daughter_2_x, Z_daughter_2_y, Z_daughter_2_z, Z_daughter_2_energy;
        double px, py, pz, energy, temp; 
        double weight0, weight1, weight2, weight3, weight4;
        double eta, pT;
        bool isContinueFind = false;

        in_file >> event_id >> partons_num >> weight0 >> weight1 
            >> weight2 >> weight3 >> weight4;
        
        in_file >> Z_daughter_pdg_code >> Z_daughter_1_x >> Z_daughter_1_y 
            >> Z_daughter_1_z >> Z_daughter_1_energy >> temp;
        in_file >> Z_daughter_pdg_code >> Z_daughter_2_x >> Z_daughter_2_y
            >> Z_daughter_2_z >> Z_daughter_2_energy >> temp;

        double Z_daughter1_pT = sqrt(pow(Z_daughter_1_x, 2) + pow(Z_daughter_1_y, 2));
        double Z_daughter1_p = sqrt(pow(Z_daughter_1_x, 2) + pow(Z_daughter_1_y, 2) 
            + pow(Z_daughter_1_z, 2));
        double Z_daught1_eta = (1.0 / 2.0) * log((Z_daughter1_p + Z_daughter_1_z) / (
            Z_daughter1_p - Z_daughter_1_z
        ) );
        double Z_daughter1_phi = acos(Z_daughter_1_x / Z_daughter1_pT );
        if (Z_daughter_1_y < 0) Z_daughter1_phi = 2 * PI - Z_daughter1_phi;

        double Z_daughter2_pT = sqrt(pow(Z_daughter_2_x, 2) + pow(Z_daughter_2_y, 2));
        double Z_daughter2_p = sqrt(pow(Z_daughter_2_x, 2) + pow(Z_daughter_2_y, 2) 
            + pow(Z_daughter_2_z, 2));
        double Z_daught2_eta = (1.0 / 2.0) * log((Z_daughter2_p + Z_daughter_2_z) / (
            Z_daughter2_p - Z_daughter_2_z
        ) );
        double Z_daughter2_phi = acos(Z_daughter_2_x / Z_daughter2_pT );
        if (Z_daughter_2_y < 0) Z_daughter2_phi = 2 * PI - Z_daughter2_phi;

        double Z_pT = sqrt(pow(Z_daughter_1_x + Z_daughter_2_x, 2) + pow(
            Z_daughter_1_y + Z_daughter_2_y, 2
        ));
        double Z_p = sqrt(pow(Z_daughter_1_x + Z_daughter_2_x, 2) + pow(
            Z_daughter_1_y + Z_daughter_2_y, 2
        ) + pow(Z_daughter_1_z + Z_daughter_2_z, 2));
        double Z_rap = (1.0 / 2.0) * log(
            (Z_daughter_1_energy + Z_daughter_2_energy + Z_daughter_1_z + Z_daughter_2_z) / (
                Z_daughter_1_energy + Z_daughter_2_energy - Z_daughter_1_z - Z_daughter_2_z
            )
        );
        double Z_mass = sqrt(pow(Z_daughter_1_energy + Z_daughter_2_energy, 2) - 
            pow(Z_p, 2));
        
        double Z_eta = (1.0 / 2.0) * log(
            (Z_p + Z_daughter_1_z + Z_daughter_2_z) / (
                Z_p - Z_daughter_1_z - Z_daughter_2_z
            )
        );
        double Z_phi = acos((Z_daughter_1_x + Z_daughter_2_x) / Z_pT);
        if ((Z_daughter_2_x + Z_daughter_1_x) < 0) Z_phi = 2 * PI - Z_phi;

        // if (abs(Z_daughter_pdg_code) == 11 || abs(Z_daughter_pdg_code) == 13) {
        //     if (Z_mass >= 70.0 && Z_mass <=110 && Z_pT >= 60 ) {
        //         weight_total += weight0 / weight2;
        //         isContinueFind = true;
        //     }
        // }
        if (abs(Z_daughter_pdg_code) == 11) {
            if (fabs(Z_daught1_eta) < 1.44 || (fabs(Z_daught1_eta) > 1.57 && 
                fabs(Z_daught1_eta) < 2.5)  ) {
                //
                if (fabs(Z_daught2_eta) < 1.44 || (fabs(Z_daught2_eta) > 1.57 && 
                fabs(Z_daught2_eta) < 2.5) ) {
                    if (Z_daughter1_pT > 20. && Z_daughter2_pT > 20 && Z_mass > 70 && 
                        Z_mass < 110 && Z_pT > 60 && fabs(Z_rap) < 2.5 ) {
                        //
                        weight_total += weight0 / weight2;
                        isContinueFind = true;
                    }
                }
            }
        }

        if (abs(Z_daughter_pdg_code) == 13) {
            if (fabs(Z_daught1_eta) < 2.4) {
                if (fabs(Z_daught2_eta) < 2.4) {
                    if (Z_daughter1_pT > 10. && Z_daughter2_pT > 10 && Z_mass > 70 && 
                        Z_mass < 110 && Z_pT > 60 && fabs(Z_rap) < 2.5 ) {
                        //
                        weight_total += weight0 / weight2;
                        isContinueFind = true;
                    }
                }
            }
        }

        vector<PseudoJet> pseudo_jets;
        for ( unsigned i = 0; i < partons_num - 2; i++) {
            in_file >> pdg_code >> px >> py >> pz >> energy >> temp;
            PseudoJet tmp(px, py, pz, energy);
            pseudo_jets.emplace_back(tmp);
        }

        if (isContinueFind) {
            ClusterSequence cluster(pseudo_jets, jet_def);
            vector<PseudoJet> jets = sorted_by_pt(select_akt(cluster.inclusive_jets()));
            if (jets.size() > 0) {
                for (const auto jet : jets) {
                    double delta_phi = fabs(jet.phi() - Z_phi);
                    delta_phi = delta_phi > PI ? 2 * PI - delta_phi : delta_phi;
                    for (unsigned i = 0; i < 8; i++) {
                        if (delta_phi >= phi_bin[i] && delta_phi < phi_bin[i + 1]) {
                            weight_for_bin[i + 1] += weight0 / weight2;
                        }
                    }
                }
            }
            isContinueFind = false;
        }

    }

    std::ofstream delta_phi_dist_file;
    const char *out_delta_phi_dist_file = "deltaPhiDist.dat";
    delta_phi_dist_file.open(out_delta_phi_dist_file);

    for(unsigned i = 1; i < 9; i++) {
        cout << "weight_for_bin: " << weight_for_bin[i] 
            << ", phi_bin[i]: " << phi_bin[i] << ", "
            << "phi_bin[i - 1]: " << phi_bin[i - 1] << endl;
        delta_phi_dist_file << phi_bin[i]  
            << ", " << weight_for_bin[i] / weight_total / (phi_bin[i] - phi_bin[i - 1]) << endl;
    }

    return 0;
} //end main
