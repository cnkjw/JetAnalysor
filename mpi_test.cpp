#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

#include <mpi.h>

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
    
    // std::cout << "Hello world from process " << rank << " of " << size << " ";
    // for (int j = 0; j < send_count[rank]; j++)
    // {
    //     std::cout << argv[locale_file_index[j]] << ", ";
    // }
    // std::cout << '\n';

    for (int i = 0; i < send_count[rank]; i++) {
        
        // 待处理文件名
        const char *hepmc_in = argv[locale_file_index[i]];

        std::cout << "Rank: " << rank << ", File name: " 
            << hepmc_in << std::endl;

        // 输出文件名
        std::string output = static_cast<std::string>("./Output/") + 
            get_file_name(static_cast<std::string>(hepmc_in)) + 
            static_cast<std::string>("Rank-") + 
            std::to_string(rank) +
            static_cast<std::string>("-Prep.dat");
        std::ofstream out_file;
        out_file.open(output);

        int fact_event_number = 0;
        
        std::ifstream in_file(hepmc_in);
        if (!in_file.is_open()) {
            std::cout << "Can't open file." << std::endl;
            exit(EXIT_FAILURE);
        }
        while (!in_file.eof()) {
            std::cout << "Rank " << rank << ", read file " << hepmc_in << std::endl;
        }

        out_file.flush();


        out_file.close();
    }

    // MPI_Barrier(MPI_COMM_WORLD);

    delete []send_count;
    delete []global_file_index;
    delete []locale_file_index;
    delete []offset_array;

    MPI_Finalize();

    return 0;
    
    
}