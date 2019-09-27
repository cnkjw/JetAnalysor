#include <iostream>
#include <string>
#include <fstream>

#include <mpi.h>
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
    double energy;
    double mass;
};

// returns true if the GenParticle does not decay
inline bool isFinal( const HepMC::GenParticle* p ) {
    return !p->end_vertex() && p->status()==1;
}

int main(int argc, char *argv[]) 
{
    
    if (argc == 1) 
    {
        std::cout << "    " << "ERROR: Please supply HepMC file as arguments..."
            << std::endl;
        exit(EXIT_FAILURE);
    }

    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if ((argc - 1) < size) {
        std::cout << "Assigned process number greater than files number, please check it!"
            << std::endl;
        exit(EXIT_FAILURE);
    }
    const int data_size_general = (argc - 1) / size;
    const int data_size_last = (argc - 1) % size;

    // 申请动态数组使用的内存块 
    int* send_count = new int[size]();
    int* global_file_index = new int[argc - 1]();
    int* locale_file_index = new int[argc - 1]();
    int* offset_array = new int[size]();

    // int *send_count = 0, *global_file_index = 0, *locale_file_index = 0;
    // int *offset_array = 0;
    // // 申请动态数组使用的内存块 
    // send_count = (int *)malloc(sizeof(int) * size);
    // global_file_index = (int *)malloc(sizeof(int) * (argc - 1));
    // locale_file_index = (int *)malloc(sizeof(int) * (argc - 1));
    // offset_array = (int *)malloc(sizeof(int) * size);
    // if (send_count == 0 || global_file_index == 0 || locale_file_index == 0 
    //     || offset_array == 0)
    // {   // 申请内存失败，终止程序运行，返回操作系统
    //     std::cout << "Created array by 'malloc' failure! " << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    for (int i = 1; i < argc; i++)
    {
        global_file_index[i - 1] = i;
    }

    for (int i = 0; i < size; i++)
    {
        send_count[i] = data_size_general;
    }

    for (int i = 0; i < data_size_last; i++)
    {
        send_count[i % size] += 1;
    }

    offset_array[0] = 0;
    for (int i = 1; i < size; i++)
    {
        offset_array[i] = offset_array[i - 1] + send_count[i - 1];
    }
    
    MPI_Scatterv(global_file_index, send_count, offset_array, MPI_INT, locale_file_index, send_count[rank], MPI_INT, 0, MPI_COMM_WORLD);  // 分发数据
    
    std::cout << "Hello world from process " << rank << " of " << size << " ";
    for (int j = 0; j < send_count[rank]; j++)
    {
        std::cout << argv[locale_file_index[j]] << ", ";
    }
    std::cout << '\n';

    for (int i = 0; i < send_count[rank]; i++) {
    // for (int i = 0; i < 1; i++) {
        
        // 待处理文件名
        const char *hepmc_in = argv[locale_file_index[i]];
        // const char *hepmc_in = argv[1];

        std::cout << "Rank: " << rank << ", File name: " 
            << hepmc_in << std::endl;

        // HepMC::IO_GenEvent ascii_in(hepmc_in, std::ios::in);
        // HepMC::GenEvent* evt = ascii_in.read_next_event();

        // 输出文件名
        std::string output = static_cast<std::string>("./Output/") + 
            get_file_name(static_cast<std::string>(hepmc_in)) + 
            static_cast<std::string>("Rank-") + 
            std::to_string(rank) +
            static_cast<std::string>("-Prep.dat");
        std::ofstream out_file;
        out_file.open(output);

        HepMC::IO_GenEvent ascii_in(hepmc_in, std::ios::in);
        // std::cout << "Rank: " << rank << " open file, status 1!" << std::endl;
        HepMC::GenEvent* evt = ascii_in.read_next_event();

        // // MPI_Barrier(MPI_COMM_WORLD);
        // std::cout << "Rank: " << rank << " open file, status 2!" << std::endl;

        int fact_event_number = 0;
        while (evt) {

            std::vector<Particle> mParticles;
            std::vector<Particle> mZ_daughter;
            bool isGoodEvent = false;

            // std::cout << "Number: " << evt->event_number() << std::endl;

            for (HepMC::GenEvent::particle_iterator p = evt->particles_begin();
                    p != evt->particles_end(); ++p) {
                
                // std::cout << "Found 1 " << std::endl;
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
                    delete evt;
                    ascii_in >> evt;
                    continue;
                }
            }

            if (isGoodEvent) {
                // std::cout << "Yes is Good: " << rank << std::endl;
                int particles_number = mZ_daughter.size() + mParticles.size();
                out_file << fact_event_number << " " << particles_number << " "
                    << (evt->weights())[0] << " " << (evt->weights())[1] << " "
                    << (evt->weights())[2] << " " << (evt->weights())[3]
                    << std::endl;
                for (unsigned i = 0; i < mZ_daughter.size(); i++) {
                    out_file << mZ_daughter[i].pdg_code << " " 
                        << mZ_daughter[i].px << " " << mZ_daughter[i].py << " "
                        << mZ_daughter[i].pz << " " << mZ_daughter[i].energy << " "
                        << mZ_daughter[i].mass << std::endl;
                }
                for (unsigned i = 0; i < mParticles.size(); i++) {
                    out_file << mParticles[i].pdg_code << " " 
                        << mParticles[i].px << " " << mParticles[i].py << " "
                        << mParticles[i].pz << " " << mParticles[i].energy << " "
                        << mParticles[i].mass << std::endl;
                }
                out_file.flush();
            }

            delete evt;
            ascii_in >> evt;
        }
        out_file.close();
    }

    std::cout << " finished!" << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);

    delete []send_count;
    delete []global_file_index;
    delete []locale_file_index;
    delete []offset_array;

    MPI_Finalize();

    return 0;
    
    
}