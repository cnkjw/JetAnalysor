//
// Created by JinWen on 2019/4/27.
//

#ifndef JETANALYSOR_UTILS_H
#define JETANALYSOR_UTILS_H

#include <fastjet/PseudoJet.hh>
#include <HepMC/IO_GenEvent.h>
#include <HepMC/GenEvent.h>

inline bool isUp (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 2;
}

inline bool isDown (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 1;
}

inline bool isCharm (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 4;
}

inline bool isStrange (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 3;
}

inline bool isTop (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 6;
}

inline bool isBottom (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 5;
}

inline bool isElectron (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 11;
}

inline bool isMuon (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 13;
}

inline bool isTau (const HepMC::GenParticle* p ){
    return abs(p->pdg_id()) == 15;
}

inline bool isLepton( const HepMC::GenParticle* p ) {
    return abs(p->pdg_id()) == 11 || abs(p->pdg_id()) == 13 || abs(p->pdg_id()) == 15;
}

inline bool isNeutrino(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) == 12 || abs(p->pdg_id()) == 14 || abs(p->pdg_id()) == 16;
}

inline bool isGluon( const HepMC::GenParticle* p ) {
    return p->pdg_id() == 21;
}

inline bool isPhoton( const HepMC::GenParticle* p ) {
    return p->pdg_id() == 22;
}

inline bool isZ( const HepMC::GenParticle* p ) {
    return p->pdg_id() == 23;
}

inline bool isW( const HepMC::GenParticle* p ) {
    return abs(p->pdg_id()) == 24;
}

inline bool isHiggs( const HepMC::GenParticle* p ) {
    return p->pdg_id() == 25;
}

//@TODO: Zprime,Wprime,Zprimeprime,H0,A0,H+

inline bool isChargedPion( const HepMC::GenParticle* p ) {
    return abs(p->pdg_id()) == 211;
}

inline bool isChargino1( const HepMC::GenParticle* p ) {
    return abs(p->pdg_id()) == 1000024;
}

inline bool isNeutralino2( const HepMC::GenParticle* p ) {
    return abs(p->pdg_id()) == 1000023;
}

inline bool isNeutralino1( const HepMC::GenParticle* p ) {
    return abs(p->pdg_id()) == 1000022;
}

//no way to access beams by status code (4 and 11 appear)
//but only beams have px = py = 0
inline bool isBeam( const HepMC::GenParticle* p ) {
    return p->status() == 4 || (p->momentum().py() == 0 && p->momentum().py() == 0);
}

// returns true if the GenParticle does not decay
inline bool isFinal( const HepMC::GenParticle* p ) {
    return !p->end_vertex() && p->status()==1;
}

//returns true if the GenParticle is about to decay (no more showering)
inline bool isLast( const HepMC::GenParticle* p ) {
    if ( p->end_vertex() ) {
        for ( HepMC::GenVertex::particle_iterator desc = p->end_vertex()->particles_begin(HepMC::descendants);
              desc != p->end_vertex()-> particles_end(HepMC::descendants);
              ++desc ) {

            //particle still showering if it decays to itself
            if ( p->pdg_id() == (*desc)->pdg_id() ) return false;
        }
        return true;
    }
    return false;
}

//returns true if particle not yet showered
inline bool isFirst(const HepMC::GenParticle* p){
    if ( p->production_vertex() ) {
        for ( HepMC::GenVertex::particle_iterator mother = p->production_vertex()->particles_begin(HepMC::parents);
              mother != p->production_vertex()-> particles_end(HepMC::parents);
              ++mother ) {

            //particle is showered if it has originated from itself
            if ( p->pdg_id() == (*mother)->pdg_id() ) return false;
        }
        return true;
    }
    return false;
}

//returns true if particle entry is from hard process i.e has 2 mothers
//wish there was a status code for this...
inline bool notFromShower( const HepMC::GenParticle* p ) {
    if ( p->production_vertex() ) {
        int nmoth=0;
        for ( HepMC::GenVertex::particle_iterator mother = p->production_vertex()->particles_begin(HepMC::parents);
              mother != p->production_vertex()-> particles_end(HepMC::parents);
              ++mother ) nmoth++;
        if (nmoth == 2) return true;
    }
    return false;
}

//returns true if GenParticle originates from a meson decay
inline bool fromMeson( const HepMC::GenParticle* p ) {
    if ( p->production_vertex() ) {
        for ( HepMC::GenVertex::particle_iterator mother = p->production_vertex()->particles_begin(HepMC::parents);
              mother != p->production_vertex()-> particles_end(HepMC::parents);
              ++mother ) {
            if ( abs((*mother)->pdg_id()) > 100 ) return true; //@TODO: this line is wrong!
        }
        return false;
    }
    return false;
}

inline bool iso1Meson(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) == 111 ||
           abs(p->pdg_id()) == 211 ||
           abs(p->pdg_id()) == 113 ||
           abs(p->pdg_id()) == 213 ||
           abs(p->pdg_id()) == 115 ||
           abs(p->pdg_id()) == 215 ||
           abs(p->pdg_id()) == 117 ||
           abs(p->pdg_id()) == 217 ||
           abs(p->pdg_id()) == 119 ||
           abs(p->pdg_id()) == 219 ||
           abs(p->pdg_id()) == 9000111 ||
           abs(p->pdg_id()) == 9000211 ||
           abs(p->pdg_id()) == 100111 ||
           abs(p->pdg_id()) == 100211 ||
           abs(p->pdg_id()) == 10111 ||
           abs(p->pdg_id()) == 10211 ||
           abs(p->pdg_id()) == 9010111 ||
           abs(p->pdg_id()) == 9010211 ||
           abs(p->pdg_id()) == 10113 ||
           abs(p->pdg_id()) == 10213 ||
           abs(p->pdg_id()) == 20113 ||
           abs(p->pdg_id()) == 20213 ||
           abs(p->pdg_id()) == 9000113 ||
           abs(p->pdg_id()) == 9000213 ||
           abs(p->pdg_id()) == 9010113 ||
           abs(p->pdg_id()) == 9010213 ||
           abs(p->pdg_id()) == 100113 ||
           abs(p->pdg_id()) == 100213 ||
           abs(p->pdg_id()) == 9010113 ||
           abs(p->pdg_id()) == 9010213 ||
           abs(p->pdg_id()) == 9020113 ||
           abs(p->pdg_id()) == 9020213 ||
           abs(p->pdg_id()) == 30113 ||
           abs(p->pdg_id()) == 30213 ||
           abs(p->pdg_id()) == 9030113 ||
           abs(p->pdg_id()) == 9030213 ||
           abs(p->pdg_id()) == 9040113 ||
           abs(p->pdg_id()) == 9040213 ||
           abs(p->pdg_id()) == 10115 ||
           abs(p->pdg_id()) == 10215 ||
           abs(p->pdg_id()) == 9000115 ||
           abs(p->pdg_id()) == 9000215 ||
           abs(p->pdg_id()) == 9010115 ||
           abs(p->pdg_id()) == 9010215 ||
           abs(p->pdg_id()) == 9000117 ||
           abs(p->pdg_id()) == 9000217 ||
           abs(p->pdg_id()) == 9010117 ||
           abs(p->pdg_id()) == 9010217;
}

inline bool iso0Meson(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) == 221 ||
           abs(p->pdg_id()) == 331 ||
           abs(p->pdg_id()) == 223 ||
           abs(p->pdg_id()) == 333 ||
           abs(p->pdg_id()) == 225 ||
           abs(p->pdg_id()) == 335 ||
           abs(p->pdg_id()) == 227 ||
           abs(p->pdg_id()) == 337 ||
           abs(p->pdg_id()) == 229 ||
           abs(p->pdg_id()) == 339 ||
           abs(p->pdg_id()) == 339 ||
           abs(p->pdg_id()) == 100221 ||
           abs(p->pdg_id()) == 10221 ||
           abs(p->pdg_id()) == 100331 ||
           abs(p->pdg_id()) == 10331 ||
           abs(p->pdg_id()) == 10223 ||
           abs(p->pdg_id()) == 20223 ||
           abs(p->pdg_id()) == 10333 ||
           abs(p->pdg_id()) == 20333 ||
           abs(p->pdg_id()) == 100223 ||
           abs(p->pdg_id()) == 30223 ||
           abs(p->pdg_id()) == 100333 ||
           abs(p->pdg_id()) == 10225 ||
           abs(p->pdg_id()) == 10335 ||
           abs(p->pdg_id()) == 9000221 ||
           abs(p->pdg_id()) == 9010221 ||
           abs(p->pdg_id()) == 9020221 ||
           abs(p->pdg_id()) == 9030221 ||
           abs(p->pdg_id()) == 9040221 ||
           abs(p->pdg_id()) == 9050221 ||
           abs(p->pdg_id()) == 9060221 ||
           abs(p->pdg_id()) == 9070221 ||
           abs(p->pdg_id()) == 9080221 ||
           abs(p->pdg_id()) == 9000223 ||
           abs(p->pdg_id()) == 9010223 ||
           abs(p->pdg_id()) == 9000225 ||
           abs(p->pdg_id()) == 9010225 ||
           abs(p->pdg_id()) == 9020225 ||
           abs(p->pdg_id()) == 9030225 ||
           abs(p->pdg_id()) == 9040225 ||
           abs(p->pdg_id()) == 9050225 ||
           abs(p->pdg_id()) == 9060225 ||
           abs(p->pdg_id()) == 9070225 ||
           abs(p->pdg_id()) == 9080225 ||
           abs(p->pdg_id()) == 9090225 ||
           abs(p->pdg_id()) == 9000229 ||
           abs(p->pdg_id()) == 9010229;
}

inline bool strangeMeson(const HepMC::GenParticle* p){
    return  abs(p->pdg_id()) == 130 ||
            abs(p->pdg_id()) == 310 ||
            abs(p->pdg_id()) == 311 ||
            abs(p->pdg_id()) == 321 ||
            abs(p->pdg_id()) == 313 ||
            abs(p->pdg_id()) == 323 ||
            abs(p->pdg_id()) == 315 ||
            abs(p->pdg_id()) == 325 ||
            abs(p->pdg_id()) == 317 ||
            abs(p->pdg_id()) == 327 ||
            abs(p->pdg_id()) == 319 ||
            abs(p->pdg_id()) == 329 ||
            abs(p->pdg_id()) == 9000311 ||
            abs(p->pdg_id()) == 9000321 ||
            abs(p->pdg_id()) == 10311 ||
            abs(p->pdg_id()) == 10321 ||
            abs(p->pdg_id()) == 100311 ||
            abs(p->pdg_id()) == 100321 ||
            abs(p->pdg_id()) == 9010311 ||
            abs(p->pdg_id()) == 9010321 ||
            abs(p->pdg_id()) == 9020311 ||
            abs(p->pdg_id()) == 9020321 ||
            abs(p->pdg_id()) == 10313 ||
            abs(p->pdg_id()) == 10323 ||
            abs(p->pdg_id()) == 20313 ||
            abs(p->pdg_id()) == 20323 ||
            abs(p->pdg_id()) == 100313 ||
            abs(p->pdg_id()) == 100323 ||
            abs(p->pdg_id()) == 9000313 ||
            abs(p->pdg_id()) == 9000323 ||
            abs(p->pdg_id()) == 30313 ||
            abs(p->pdg_id()) == 30323 ||
            abs(p->pdg_id()) == 9000315 ||
            abs(p->pdg_id()) == 9000325 ||
            abs(p->pdg_id()) == 10315 ||
            abs(p->pdg_id()) == 10325 ||
            abs(p->pdg_id()) == 20315 ||
            abs(p->pdg_id()) == 20325 ||
            abs(p->pdg_id()) == 9010315 ||
            abs(p->pdg_id()) == 9010325 ||
            abs(p->pdg_id()) == 9020315 ||
            abs(p->pdg_id()) == 9020325 ||
            abs(p->pdg_id()) == 9010317 ||
            abs(p->pdg_id()) == 9010327 ||
            abs(p->pdg_id()) == 9010319 ||
            abs(p->pdg_id()) == 9010329;
}

inline bool charmedMeson(const HepMC::GenParticle* p){
    return  abs(p->pdg_id()) == 411 ||
            abs(p->pdg_id()) == 421 ||
            abs(p->pdg_id()) == 413 ||
            abs(p->pdg_id()) == 423 ||
            abs(p->pdg_id()) == 415 ||
            abs(p->pdg_id()) == 425 ||
            abs(p->pdg_id()) == 431 ||
            abs(p->pdg_id()) == 433 ||
            abs(p->pdg_id()) == 435 ||
            abs(p->pdg_id()) == 10411 ||
            abs(p->pdg_id()) == 10421 ||
            abs(p->pdg_id()) == 10413 ||
            abs(p->pdg_id()) == 10423 ||
            abs(p->pdg_id()) == 20413 ||
            abs(p->pdg_id()) == 20423 ||
            abs(p->pdg_id()) == 10431 ||
            abs(p->pdg_id()) == 10433 ||
            abs(p->pdg_id()) == 20433;
}

inline bool bottomMeson(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) == 511 ||
           abs(p->pdg_id()) == 521 ||
           abs(p->pdg_id()) == 513 ||
           abs(p->pdg_id()) == 523 ||
           abs(p->pdg_id()) == 515 ||
           abs(p->pdg_id()) == 525 ||
           abs(p->pdg_id()) == 531 ||
           abs(p->pdg_id()) == 533 ||
           abs(p->pdg_id()) == 535 ||
           abs(p->pdg_id()) == 541 ||
           abs(p->pdg_id()) == 543 ||
           abs(p->pdg_id()) == 545 ||
           abs(p->pdg_id()) == 10511 ||
           abs(p->pdg_id()) == 10521 ||
           abs(p->pdg_id()) == 10513 ||
           abs(p->pdg_id()) == 10523 ||
           abs(p->pdg_id()) == 20513 ||
           abs(p->pdg_id()) == 20523 ||
           abs(p->pdg_id()) == 10531 ||
           abs(p->pdg_id()) == 10533 ||
           abs(p->pdg_id()) == 20533 ||
           abs(p->pdg_id()) == 10541 ||
           abs(p->pdg_id()) == 10543 ||
           abs(p->pdg_id()) == 20543;
}

inline bool ccMeson(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) == 441 ||
           abs(p->pdg_id()) == 443 ||
           abs(p->pdg_id()) == 445 ||
           abs(p->pdg_id()) == 10441 ||
           abs(p->pdg_id()) == 100441 ||
           abs(p->pdg_id()) == 10443 ||
           abs(p->pdg_id()) == 100443 ||
           abs(p->pdg_id()) == 20443 ||
           abs(p->pdg_id()) == 30443 ||
           abs(p->pdg_id()) == 9000443 ||
           abs(p->pdg_id()) == 9010443 ||
           abs(p->pdg_id()) == 9020443 ||
           abs(p->pdg_id()) == 100445;
}

inline bool bbMeson(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) == 551 ||
           abs(p->pdg_id()) == 553 ||
           abs(p->pdg_id()) == 555 ||
           abs(p->pdg_id()) == 557 ||
           abs(p->pdg_id()) == 10551 ||
           abs(p->pdg_id()) == 100551 ||
           abs(p->pdg_id()) == 110551 ||
           abs(p->pdg_id()) == 200551 ||
           abs(p->pdg_id()) == 210551 ||
           abs(p->pdg_id()) == 10553 ||
           abs(p->pdg_id()) == 20553 ||
           abs(p->pdg_id()) == 30553 ||
           abs(p->pdg_id()) == 100553 ||
           abs(p->pdg_id()) == 110553 ||
           abs(p->pdg_id()) == 120553 ||
           abs(p->pdg_id()) == 130553 ||
           abs(p->pdg_id()) == 200553 ||
           abs(p->pdg_id()) == 210553 ||
           abs(p->pdg_id()) == 220553 ||
           abs(p->pdg_id()) == 300553 ||
           abs(p->pdg_id()) == 9000553 ||
           abs(p->pdg_id()) == 9010553 ||
           abs(p->pdg_id()) == 10555 ||
           abs(p->pdg_id()) == 20555 ||
           abs(p->pdg_id()) == 100555 ||
           abs(p->pdg_id()) == 110555 ||
           abs(p->pdg_id()) == 120555 ||
           abs(p->pdg_id()) == 200555 ||
           abs(p->pdg_id()) == 100557;
}

inline bool isMeson(const HepMC::GenParticle* p){
    return iso1Meson(p) ||
           iso0Meson(p) ||
           strangeMeson(p) ||
           charmedMeson(p) ||
           bottomMeson(p) ||
           ccMeson(p) ||
           bbMeson(p);
}

inline bool lightBaryon(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) == 2212 ||
           abs(p->pdg_id()) == 2112 ||
           abs(p->pdg_id()) == 2224 ||
           abs(p->pdg_id()) == 2214 ||
           abs(p->pdg_id()) == 2114 ||
           abs(p->pdg_id()) == 1114;
}

inline bool strangeBaryon(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) >= 3000 && abs(p->pdg_id()) < 4000;
}

inline bool charmedBaryon(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) >= 4000 && abs(p->pdg_id()) < 5000;
}

inline bool bottomBaryon(const HepMC::GenParticle* p){
    return abs(p->pdg_id()) >= 5000 && abs(p->pdg_id()) < 6000;
}

inline bool isBaryon(const HepMC::GenParticle* p){
    return lightBaryon(p) ||
           charmedBaryon(p) ||
           strangeBaryon(p) ||
           bottomBaryon(p);
}

inline bool isHadron(const HepMC::GenParticle* p){
    return isMeson(p) || isBaryon(p);
}

inline bool isBHadron(const HepMC::GenParticle* p){
    return bottomMeson(p) || bbMeson(p) || bottomBaryon(p);
}

inline bool isVisible(const HepMC::GenParticle* p){
    return isElectron(p) || isMuon(p) || isPhoton(p) || isHadron(p);
}

inline bool bTagged(fastjet::PseudoJet jet ){
    for (int i=0; i < jet.constituents().size(); i++){
        if ( jet.constituents()[i].user_index() == 1) return true;
    }
    return false;
}

#endif //JETANALYSOR_UTILS_H
