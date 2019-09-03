//
// Created by JinWen on 2019/4/30.
//

#ifndef JETANALYSOR_CALCDELTAPHIDIST_H
#define JETANALYSOR_CALCDELTAPHIDIST_H

#include <vector>
#include <math.h>

#include <fastjet/PseudoJet.hh>

using namespace std;

namespace JetAnalysor {

    struct AzimuthalAngleNum {
        double delta_phi;
        int number;
    };

    struct AzimuthalAngleDist {
        double delta_phi;
        double dist;
    };

    vector<AzimuthalAngleDist> GetAzimuthalAngleDistData(vector<double> &delta_phi_vec, double half_d_delta_phi, int N_z);

    vector<AzimuthalAngleDist> GetAzimuthalAngleDistData(vector<double> &delta_phi_vec, double half_d_delta_phi, int N_z) {

        const int vec_size = floor( fastjet::pi / (2 * half_d_delta_phi) ) + 1;
        vector<AzimuthalAngleNum> azimuthal_angle_num(vec_size);
        vector<AzimuthalAngleDist> azimuthal_angle_dist(vec_size);

        for (int i = 0; i < 2 * azimuthal_angle_num.size(); i = i + 2) {
            azimuthal_angle_num[i/2].number = 0;
            azimuthal_angle_num[i/2].delta_phi = (i + 1) * half_d_delta_phi;
        }
        for (const auto _ : delta_phi_vec) {
            for (int i = 0; i < 2 * azimuthal_angle_num.size(); i = i + 2) {
                if ( (_ >= i * half_d_delta_phi) && (_ < (i + 2) * half_d_delta_phi) ) {
                    azimuthal_angle_num[i/2].number += 1;
                }
            }
        }

        for (int i = 0; i < azimuthal_angle_num.size(); i++) {

            if ( (i + 1) != azimuthal_angle_num.size()) {
                azimuthal_angle_dist[i].delta_phi = azimuthal_angle_num[i].delta_phi;
                azimuthal_angle_dist[i].dist = azimuthal_angle_num[i].number / (2 * half_d_delta_phi * N_z);
            } else {
                azimuthal_angle_dist[i].delta_phi = fastjet::pi;
                double last_width = (fastjet::pi - azimuthal_angle_num[i].delta_phi + half_d_delta_phi);
                azimuthal_angle_dist[i].dist = azimuthal_angle_num[i].number / (last_width * N_z);
            }
        }
        return azimuthal_angle_dist;

    }


}

#endif //JETANALYSOR_CALCDELTAPHIDIST_H
