/**************************************************************************
runLBT.cpp, Version: 1.0

Copyright (C) 2019
MIT License

Author: Kang Jin-Wen
Date: 2019-09-07
Description: Runing Linear Boltzmann Transport for init partons

Changelogs:
    2019-09-07 +0800 11:39 Create

**************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <HepMC/IO_GenEvent.h>

#include "utils.h"
#include "writeHepMC2.h"

#include "LBT.h"
#include "Particle.h"
#include "FourVector.h"

using namespace std;

vector<double> mJetNum, XNucleon, YNucleon;
vector<Particle> mPartons;

int main(int argc, char *argv[]) {

    if (argc == 1) {
        cout << "    " << "ERROR: Please supply HepMC files as argument..."
            << endl;
        return 1;
    }

    LBT *mLBT = new LBT();
    mLBT->Initialize("./LBT-Config.yaml");

    double mJetNumElement, mXNucleonElement, mYNucleonElement, mJetNumTotal = 0.;
    ifstream Geomtery("./LBT-tables/HydroProfile/geometry.dat");
    if (Geomtery.is_open()) {
        while (!Geomtery.eof()) {
            Geomtery >> mJetNumElement >> mXNucleonElement >> mYNucleonElement;
            mJetNumTotal += mJetNumElement;
            mJetNum.emplace_back(mJetNumElement);
            XNucleon.emplace_back(mXNucleonElement);
            YNucleon.emplace_back(mYNucleonElement);
        }
    } else {
        cout << "    " << "Can't open geometry.dat file." << endl;
        return 1;
    }

    double mRandomXY = mLBT->MyRandom();
    double R1 = 0., XXX, YYY;
    for (int i = 0; i < XNucleon.size(); i++) {
        R1 += mJetNum[i] / mJetNumTotal;
        if (mRandomXY < R1) {
            XXX = XNucleon[i];
            YYY = YNucleon[i];
            break;
        }
    }

    HepMC::IO_GenEvent out("FinalPartons.hepmc", std::ios::out);

    for (unsigned file_num = 1; file_num < argc; file_num++) {

        char *hepmc_file_name = argv[file_num];
        HepMC::IO_GenEvent ascii_in(hepmc_file_name, std::ios::in);
        HepMC::GenEvent* evt = ascii_in.read_next_event();

        double mEta, mP_T;
        Particle mJet;
        FourVector mMomentum, mPosition(0., XXX, YYY, 0.);

        while ( evt ) {

            for (HepMC::GenEvent::particle_iterator p = evt->particles_begin(); 
                    p != evt->particles_end(); ++p) {
                //
                if ( isFinal(*p) && ( isDown(*p) || isUp(*p) || isStrange(*p) || 
                    isCharm(*p) || isBottom(*p) || isGluon(*p) ) ) {
                    mMomentum.SetVector((*p)->momentum().e(), (*p)->momentum().px(), 
                        (*p)->momentum().py(), (*p)->momentum().pz());
                    mEta = 1.0 / 2.0 * log( (mMomentum.x0() + mMomentum.x3()) / (
                        mMomentum.x0() - mMomentum.x3()
                    ) );
                    mP_T = sqrt(pow(mMomentum.x1(), 2) + pow(mMomentum.x2(), 2));
                    
                    if (abs(mEta) <= 2.4 && mP_T >= 0.5) {
                        mJet.SetParticleInfo((*p)->pdg_id(), mMomentum, mPosition);
                        mJet.SetFormationTime(2.0 * mMomentum.x0() / (
                            pow(mMomentum.x1(), 2) + pow(mMomentum.x2(), 2)
                        ));
                        mJet.SetRadiationTime(mJet.GetFormationTime());
                        mPartons.emplace_back(mJet);
                    }
                }
            }
            //
            mLBT->SetEvent(mPartons);

            TimeInfo mTimeInfo = mLBT->GetTimeInfo();
            double mTime = mTimeInfo.Start;
            for (unsigned i = 0; i < mTimeInfo.StepNum; i++) {
                mTime += mTimeInfo.Step;
                mLBT->LinearBoltzmannTransport(mTime);
                if (mLBT->isReachTauEnd()) {
                    i = mTimeInfo.StepNum - 1;
                }
            }

            vector<Particle> mFinalPartons; 
            mFinalPartons = mLBT->GetFinalPartons();
            //Particle baoliu;
            
            JetAnalysor::WritePartonsToHepMc2(out, evt->event_number(), 
                evt->alphaQCD(), evt->alphaQED(), evt->weights()[0], 
                evt->weights()[1], evt->weights()[2], evt->weights()[3], 
                evt->weights()[4], mFinalPartons, 
                mFinalPartons);

            delete evt;
            ascii_in >> evt;
        }
    }
}