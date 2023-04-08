#include "testfunction.hpp"
#include "kernel.hpp"
#include <Eigen/Core>
#include "testfunction.hpp"

double testfunction(){
    std::vector<Eigen::Vector3d> v1(10, Eigen::Vector3d{ 1.0, 1.0, 1.0 });
    std::vector<Eigen::Vector3d> v2(10, Eigen::Vector3d{ -1.0, 1.0, 1.0 });

    double x = dot(v1,v2);

    return x;
}