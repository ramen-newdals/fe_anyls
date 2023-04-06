CC=g++
NVCC=nvcc
CC_FLAGS=-O3 -Wall -std=c++17 -g
NVCC_FLAGS= -std=c++17 -c
CC_LIBS= -lstdc++
NVCC_LIBS = -lcuda -lcudart -L/opt/cuda/lib64
INCDIRS=-I/usr/include/eigen3 -I/opt/cuda/include

kernel_gpu.o: kernel_gpu.cu
		$(NVCC) $(NVCC_FLAGS) $(INCDIRS) kernel_gpu.cu

kernel.o: kernel.cpp
		$(CC) -c $(CC_FLAGS) $(CC_LIBS) $(INCDIRS) kernel.cpp -o kernel.o

test.o: test.cpp
		$(CC) -c $(CXXFLAGS) $(LIBS) $(INCDIRS) test.cpp -o test.o

all: kernel.o test.o
		$(CC) -o main main.cpp kernel.o test.o $(LIBDIRS) $(INCDIRS) $(LIBS) $(CXXFLAGS)

gpu: kernel_gpu.o test.o
		$(CC) main.cpp test.o kernel_gpu.o $(INCDIRS) $(CC_LIBS) $(NVCC_LIBS) -o main

clean:
		rm -rf main *.o