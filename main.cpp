//example hepmc reader

#include "utils.h"
#include "jetSelector.h"
#include "calcDeltaPhiDist.h"

#include <HepMC/IO_GenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/Selector.hh>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <utility>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace std;
using namespace fastjet;

// 用于 vector 排序
static bool CompareDouble(double u, double v) { return (u < v); }

int main(int argc, char *argv[]) {

    if (argc == 1) {
        cout << "    " << "ERROR: Please supply HepMC file as argument..." << endl;
        return 1;
    }

    vector<PseudoJet> jets, Z_Bosons;
    vector<double> deltaPhi(0);
    deltaPhi.clear();
    jets.clear();
    Z_Bosons.clear();

    double R_jet = 0.3;
    JetDefinition jet_def(antikt_algorithm, R_jet);
    Selector select_akt = SelectorAbsEtaMax(1.6) && SelectorPtMin(30.);

    for (int file_num = 1; file_num < argc; file_num++) {

        string hepmcfile = argv[file_num];
        HepMC::IO_GenEvent ascii_in(hepmcfile, std::ios::in);
        HepMC::GenEvent* evt = ascii_in.read_next_event();

        vector<PseudoJet> hadrons;

        while ( evt ) {

            PseudoJet Z_Boson_Pool_Init(0., 0., 0., 0.);
            PseudoJet Z_Boson_Component = Z_Boson_Pool_Init;
            vector<PseudoJet> Z_Boson_Vec(0);
            Z_Boson_Vec.clear();
            bool Wait_anti_particle = false;
            hadrons.clear();
            int Z_num = 0;

            for (HepMC::GenEvent::particle_iterator p = evt->particles_begin();
                    p != evt->particles_end(); ++p) {

                // get final state particles
                if ( isFinal(*p)) {
                    int btag = isBHadron(*p) ? 1 : 0;
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), (*p)->momentum().pz(), (*p)->momentum().e());
                    tmp.set_user_index(btag);
                    hadrons.push_back(tmp);
                }

                if (((*p)->pdg_id() == 11 || (*p)->pdg_id() == 13) && isFinal(*p)) {
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), (*p)->momentum().pz(), (*p)->momentum().e());
                    Z_Boson_Component += tmp;
                    Wait_anti_particle = true;
                }

                if (((*p)->pdg_id() == -11 || (*p)->pdg_id() == -13) && isFinal(*p) && Wait_anti_particle) {
                    PseudoJet tmp((*p)->momentum().px(),(*p)->momentum().py(), (*p)->momentum().pz(), (*p)->momentum().e());
                    Z_Boson_Component += tmp;
                    Z_Boson_Vec.push_back(Z_Boson_Component);
                    Wait_anti_particle = false;
                    Z_Boson_Component = Z_Boson_Pool_Init;
                }

            }

            cout << "--Event Info: " << (evt->weights())[0] / (evt->weights())[2] << endl;
            vector<PseudoJet> tmp_jets = JetAnalysor::SelectJet(hadrons, jet_def, select_akt);
            jets.insert(jets.end(), tmp_jets.begin(), tmp_jets.end());

            Selector select_Z = SelectorPtMin(60.);
            auto Z_Bosons_By_Select = select_Z(Z_Boson_Vec);
            Z_Bosons.insert(Z_Bosons.end(), Z_Bosons_By_Select.begin(), Z_Bosons_By_Select.end());
            if (tmp_jets.size() != 0 && Z_Bosons_By_Select.size() != 0) {
                for (unsigned jet_i = 0; jet_i < tmp_jets.size(); jet_i++) {
                    for (unsigned Z_i = 0; Z_i < Z_Bosons_By_Select.size(); Z_i++) {
                        double delta_phi_value = abs(tmp_jets[jet_i].phi() - Z_Bosons_By_Select[Z_i].phi());
                        delta_phi_value = delta_phi_value > pi ? 2 * pi - delta_phi_value : delta_phi_value;
                        deltaPhi.push_back(delta_phi_value);
                    }
                }
            }

            delete evt;
            ascii_in >> evt;
        }
    }

        std::ofstream jets_file;
        const char *outfile = "jetlist.dat";
        jets_file.open(outfile);
        jets_file << "#barcode" << ", " << "px" << ", " << "py" << ", " << "pz" << ", " <<  "pt" << ", " << "phi" << std::endl;
        for (unsigned i = 0; i < jets.size(); i++ ) {
            jets_file << i << ", " << jets[i].px() << ", " << jets[i].py() << ", " << jets[i].pz() << ", "
                << jets[i].pt() << ", " << jets[i].phi() << std::endl;
        }
        jets_file.close();
        cout << '\n' << "--Total jets: " << jets.size() << endl;
        cout << "--Jets Info: " << jet_def.description() << endl;

        Selector select_Z = SelectorPtMin(60.);
        auto Z_Bosons_By_Select = select_Z(Z_Bosons);
        std::ofstream Zs_file;
        const char *outZsfile = "Zlist.dat";
        Zs_file.open(outZsfile);
        Zs_file << "#barcode" << ", " << "px" << ", " << "py" << ", " << "pz" << ", " <<  "pt" << ", " << "phi" << std::endl;
        for (unsigned i = 0; i < Z_Bosons_By_Select.size(); i++) {
                Zs_file << i << ", " << Z_Bosons_By_Select[i].px() << ", " << Z_Bosons_By_Select[i].py() << ", "
                        << Z_Bosons_By_Select[i].pz() << ", " << Z_Bosons_By_Select[i].pt() << ", "
                        << Z_Bosons_By_Select[i].phi() << std::endl;
        }
        Zs_file.close();
        cout << "--Total Z bosons: " << Z_Bosons_By_Select.size() << endl;


        int N_Z_number = Z_Bosons_By_Select.size();
        std::ofstream deltaphi_file;
        const char *outDeltaPhifile = "deltaPhi.dat";
        deltaphi_file.open(outDeltaPhifile);
        deltaphi_file << "#Z Bosons number: " << N_Z_number << '\n';

        sort(deltaPhi.begin(), deltaPhi.end(), CompareDouble);
        for (unsigned i = 0; i < deltaPhi.size(); i++ ) {
            deltaphi_file << deltaPhi[i] << '\n';
        }
        deltaphi_file.close();

        std::ofstream delta_phi_dist_file;
        const char *out_delta_phi_dist_file = "deltaPhiDist.dat";
        delta_phi_dist_file.open(out_delta_phi_dist_file);
        auto delta_phi_dist = JetAnalysor::GetAzimuthalAngleDistData(deltaPhi, 0.05, N_Z_number);
        for(unsigned i = 0; i < delta_phi_dist.size(); i++) {
            delta_phi_dist_file << delta_phi_dist[i].delta_phi << ", " << delta_phi_dist[i].dist << endl;
        }

    return 0;
} //end main
