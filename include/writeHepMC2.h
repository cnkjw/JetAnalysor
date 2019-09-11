//
// Created by JinWen on 2019/4/29.
//

#ifndef JETANALYSOR_WRITEHEPMC2_H
#define JETANALYSOR_WRITEHEPMC2_H

#include <iostream>
#include <vector>
#include <fstream>

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"

#include "Particle.h"

using namespace std;

namespace JetAnalysor {
    void WritePartonsToHepMc2(HepMC::IO_GenEvent &mIO_GenEvent, const int &mEvent_Id, 
        const double &mAlpahQCD, const double &mAlphaQED, const double &weight0, 
        const double &weight1, const double &weight2, const double &weight3, 
        const double &weight4, const vector<Particle> &mRetainPartons, 
        const vector<Particle> &mPartons);
}

void JetAnalysor::WritePartonsToHepMc2(HepMC::IO_GenEvent &mIO_GenEvent, 
        const int &mEvent_Id, const double &mAlpahQCD, const double &mAlphaQED, 
        const double &weight0, const double &weight1, const double &weight2, 
        const double &weight3, const double &weight4, 
        const vector<Particle> &mRetainPartons, const vector<Particle> &mPartons) {
    //

    // 1st create the event container
    HepMC::GenEvent* evt = new HepMC::GenEvent();

    // 2nd set event info
    evt->set_event_scale(0);
    evt->set_alphaQCD(mAlpahQCD);
    evt->set_alphaQED(mAlphaQED);
    evt->weights()["0"] = weight0;
    evt->weights()["1"] = weight1;
    evt->weights()["2"] = weight2;
    evt->weights()["3"] = weight3;
    evt->weights()["4"] = weight4;
    evt->use_units(HepMC::Units::GEV, HepMC::Units::MM);

    // 3rd create 1 vertex and add to event
    HepMC::GenVertex* vertex = new HepMC::GenVertex();
    evt->add_vertex(vertex);

    // 4th create retain partons and write to vertex
    // here record some particles and that not from outgoing but we need they
    for (unsigned i = 0; i < mRetainPartons.size(); i++) {
        HepMC::GenParticle* retain_particle = new HepMC::GenParticle(
            HepMC::FourVector(
                mRetainPartons[i].GetMomentum().x1(), 
                mRetainPartons[i].GetMomentum().x2(), 
                mRetainPartons[i].GetMomentum().x3(), 
                mRetainPartons[i].GetMomentum().x0()
            ), mRetainPartons[i].GetPDG_ID(), 1
        );
        vertex->add_particle_out(retain_particle);
        // delete retain_particle;
    }

    // 5th create outgoing partons and write to vertex
    for (unsigned i = 0; i < mPartons.size(); i++) {
        HepMC::GenParticle* outgoing_particle = new HepMC::GenParticle(
            HepMC::FourVector(
                mPartons[i].GetMomentum().x1(), 
                mPartons[i].GetMomentum().x2(), 
                mPartons[i].GetMomentum().x3(), 
                mPartons[i].GetMomentum().x0() 
            ), mPartons[i].GetPDG_ID(), 1
        );
        vertex->add_particle_in(outgoing_particle);
    }

    // 6th output event to file
    mIO_GenEvent << evt;
    
    // end clean event object
    delete evt;

}

#endif // JETANALYSOR_WRITEHEPMC2_H
