#include <Eigen/Core>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "kernel.hpp"
#include "testfunction.hpp"

int main(int argc, char *argv[]){
    double result;
    result = testfunction();
    printf("%f\n", result);
    return 0;
}