CC=g++
NVCC=nvcc
CC_FLAGS=-O3 -Wall -std=c++17 -g
NVCC_FLAGS= -std=c++17 -c -g
CC_LIBS= -lstdc++
NVCC_LIBS = -lcuda -lcudart -L/opt/cuda/lib64
INCDIRS=-I/usr/include/eigen3 -I/opt/cuda/include

GENCODE=-gencode arch=compute_50,code=sm_50 -gencode arch=compute_52,code=sm_52 -gencode arch=compute_60,code=sm_60 -gencode arch=compute_61,code=sm_61 -gencode arch=compute_70,code=sm_70 -gencode arch=compute_75,code=sm_75 -gencode arch=compute_80,code=sm_80 -gencode arch=compute_86,code=sm_86 -gencode arch=compute_89,code=sm_89 -gencode arch=compute_90,code=sm_90 -gencode arch=compute_90,code=compute_90

kernel_gpu.o: kernel_gpu.cu
		$(NVCC) -c $(NVCC_FLAGS) $(INCDIRS) kernel_gpu.cu -o kernel_gpu.o

kernel_cpu.o: kernel.cpp
		$(CC) -c $(CC_FLAGS) $(CC_LIBS) $(INCDIRS) kernel.cpp -o kernel_cpu.o

testfunction.o: testfunction.cpp
		$(CC) -c $(CC_FLAGS) $(CC_LIBS) $(INCDIRS) testfunction.cpp -o testfunction.o

test_kernel_cpu: kernel_cpu.o testfunction.o
		$(CC)    $(CC_FLAGS) $(CC_LIBS) $(INCDIRS) main.cpp testfunction.o kernel_cpu.o -o test_kernel_cpu

test_kernel_gpu: kernel_gpu.o testfunction.o
		$(CC)    $(CC_FLAGS) $(CC_LIBS) $(NVCC_LIBS) $(INCDIRS) main.cpp testfunction.o kernel_gpu.o -o test_kernel_gpu

cuSolve: cuSolverSp_LinearSolver.cpp
		$(NVCC) -ccbin g++ -I/home/ramen_newdals/Documents/ECE1755/project/cuda-samples/Common -m64 -Xcompiler -Wall --threads 0 --std=c++17 -o cuSolverSp_LinearSolver.o $(GENCODE) -c cuSolverSp_LinearSolver.cpp

clean:
		rm -rf main test_kernel_cpu test_kernel_gpu cuSolverSp_LinearSolver *.o