#include "kernel.hpp"
#include <Eigen/Core>
#include <vector>

// C++ Version
double dot(const std::vector<Eigen::Vector3d> & v1, const std::vector<Eigen::Vector3d> & v2){
    double x=0;
    for (int i=0; i<v1.size(); ++i){
        x += v1[i].dot(v2[i]);
    }
    return x;
}