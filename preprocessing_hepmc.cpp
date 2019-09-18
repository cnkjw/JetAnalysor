#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <HepMC/IO_GenEvent.h>
#include <HepMC/GenEvent.h>

std::string get_file_name(std::string filepath) {
    if (!filepath.empty()) {
        int location_point = filepath.find_last_of('.');
        int location_filename = filepath.find_last_of('/');
        std::string file_name = filepath.substr(location_filename + 1, 
            location_point - location_filename - 1);
        return file_name;
    } else {
        return "NULL";
    }
}

struct Particle {
    int pdg_code;
    double px;
    double py;
    double pz;
    double pe;
    double mass;
};

// returns true if the GenParticle does not decay
inline bool isFinal( const HepMC::GenParticle* p ) {
    return !p->end_vertex() && p->status()==1;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "    " << "ERROR: Please supply HepMC file as argument..." << std::endl;
        return 1;
    }

    for (int file_num = 1; file_num < argc; file_num++) {

        const char *hepmc_in = argv[file_num];
        HepMC::IO_GenEvent ascii_in(hepmc_in, std::ios::in);
        HepMC::GenEvent* evt = ascii_in.read_next_event();

        std::string output = static_cast<std::string>("./Output/") + 
            get_file_name(static_cast<std::string>(hepmc_in)) + 
            static_cast<std::string>("-Prep.dat");
        std::ofstream out_file;
        out_file.open(output.c_str());

        int fact_event_number = 0;

        while (evt) {

            std::vector<Particle> mParticles;
            std::vector<Particle> mZ_daughter;
            bool isGoodEvent = false;

            for (HepMC::GenEvent::particle_iterator p = evt->particles_begin();
                    p != evt->particles_end(); ++p) {
                if ( isFinal(*p) && ( abs((*p)->pdg_id()) == 11 || abs((*p)->pdg_id()) == 13 ) ) {
                    Particle tmp = {(*p)->pdg_id(), (*p)->momentum().px(), (*p)->momentum().py(), 
                        (*p)->momentum().pz(), (*p)->momentum().e(), (*p)->generated_mass()};
                    mZ_daughter.emplace_back(tmp);
                } else if ( isFinal(*p)) {
                    Particle tmp = {(*p)->pdg_id(), (*p)->momentum().px(), (*p)->momentum().py(), 
                        (*p)->momentum().pz(), (*p)->momentum().e(), (*p)->generated_mass()};
                    mParticles.emplace_back(tmp);
                } else {
                    continue;
                }
            }

            if (mZ_daughter.size() == 2) {
                if (sqrt(pow(mZ_daughter[0].px + mZ_daughter[1].px, 2) + 
                    pow(mZ_daughter[0].py + mZ_daughter[1].py, 2)) > 60.0) {
                    //
                    fact_event_number++;
                    isGoodEvent = true;
                } else {
                    continue;
                }
            }

            if (isGoodEvent) {
                int particles_number = mZ_daughter.size() + mParticles.size();
                out_file << fact_event_number << " " << particles_number << " "
                    << (evt->weights())[0] << " " << (evt->weights())[1] << " "
                    << (evt->weights())[2] << " " << (evt->weights())[3]
                    << std::endl;
                for (unsigned i = 0; i < mZ_daughter.size(); i++) {
                    out_file << mZ_daughter[i].pdg_code << " " 
                        << mZ_daughter[i].px << " " << mZ_daughter[i].py << " "
                        << mZ_daughter[i].pz << " " << mZ_daughter[i].pe << " "
                        << mZ_daughter[i].mass << std::endl;
                }
                for (unsigned i = 0; i < mParticles.size(); i++) {
                    out_file << mParticles[i].pdg_code << " " 
                        << mParticles[i].px << " " << mParticles[i].py << " "
                        << mParticles[i].pz << " " << mParticles[i].pe << " "
                        << mParticles[i].mass << std::endl;
                }
                out_file.flush();
            }
        }
        std::cout << "The file: " 
            << get_file_name(static_cast<std::string>(hepmc_in)) 
            << "preprocess successful!" << std::endl;
        out_file.close();
    }
}
