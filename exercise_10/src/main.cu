/**************************************************************************************************
 *
 *       Computer Engineering Group, Heidelberg University - Introduction to HPC - sheet X
 *
 *                                 Group : Naumann, Schindler, Klein
 *
 *                                  File : main.cu
 *
 *                               Purpose : Matrix Multiplication
 *
 *************************************************************************************************/

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chCommandLine.h>
#include <chTimer.hpp>

#include "mmult_cpu.h"

const static int DEFAULT_MATRIX_WIDTH  = 1024;
const static int DEFAULT_BLOCK_DIM     =   27;

//
// Function Prototypes
//
void printHelp(char * /*programName*/);

//
// matMul_Kernel
//
__global__ void
matMul_Kernel(int matrixSize, float* matrixA, float* matrixB, float* matrixC)
{
    int elementIdx = blockIdx.x * blockDim.x + threadIdx.x;
    int elementIdy = blockIdx.y * blockDim.y + threadIdx.y;
    
    int elementId = elementIdy * matrixSize + elementIdx;

    if (elementIdx < matrixSize && elementIdy < matrixSize) {
        matrixC[elementId] = 0; 
        for (int i = 0; i < matrixSize; i++) {
            matrixC[elementId] +=  matrixA[elementIdy * matrixSize + i]
                                * matrixB[matrixSize * i + elementIdx];
        }
    }
}

//
// Shared matMul_Kernel
//
__global__ void
shMatMul_Kernel(int matrixSize, float* matrixA, float* matrixB, float* matrixC)
{
    // shared memory //
    extern __shared__ float sh_Mem[];
    float *sh_MatrixA = &(sh_Mem[0]);
    // blockDim.x == blockDim.y is mandatory //
    float *sh_MatrixB = &(sh_Mem[blockDim.x * blockDim.y]);
    float *sh_MatrixC = &(sh_Mem[2*blockDim.x * blockDim.y]);
    
    // get indizes //
    int bbx = blockIdx.x, bby = blockIdx.y; 
    int tx = threadIdx.x, ty = threadIdx.y;
    int elementIdx = bbx * blockDim.x + tx;
    int elementIdy = bby * blockDim.y + ty;
    int id = ty * blockDim.x + tx; // thread id

    // actual indizes for elements which
    // should be loaded to shared memory
    // for matrix A and B
    int ax, ay, bx, by;
    
    // set output to zero first //
    sh_MatrixC[id] = 0;

    for (int i = 0; i < matrixSize; i += blockDim.x) {
        
        // each thread must load two matrix elements
        // into shared memory. ax and ay are the x and
        // y indizes of matrixA elements, which must be
        // loaded. bx and by analog for matrixB.
        ay = elementIdy;
        ax = i + tx;
        bx = elementIdx;
        by = i + ty;
        
        // this if else statements are mandatory if
        // matrixSize % blockDim.x != 0
        if (ax < matrixSize && ay < matrixSize)
            sh_MatrixA[id] = matrixA[ay * matrixSize + ax];
        else
            sh_MatrixA[id] = 0;
        if (bx < matrixSize && by < matrixSize)
            sh_MatrixB[id] = matrixB[by * matrixSize + bx];
        else 
            sh_MatrixB[id] = 0;

         __syncthreads();
        
         // add to result //
         for (int k = 0; k < blockDim.x; k++) {
             if (elementIdx < matrixSize && elementIdy < matrixSize) 
                 sh_MatrixC[id] += sh_MatrixA[ty * blockDim.y + k] 
                                * sh_MatrixB[ k * blockDim.y + tx];
         }
         __syncthreads();
    }
    
    // write to global memory //
    if (elementIdx < matrixSize && elementIdy < matrixSize) {
        matrixC[elementIdy * matrixSize + elementIdx] = sh_MatrixC[id];
    }
}
//
// Main
//
int
main(int argc, char * argv[])
{
    //
    // Show Help
    //
    bool showHelp = chCommandLineGetBool("h", argc, argv);
    if (!showHelp) {
        showHelp = chCommandLineGetBool("help", argc, argv);
    }

    if (showHelp) {
        printHelp(argv[0]);
        exit(0);
    }

    std::cout << "***" << std::endl
              << "*** Starting ..." << std::endl
              << "***" << std::endl;

    ChTimer memCpyH2DTimer, memCpyD2HTimer;
    ChTimer kernelTimer, cpuTimer;

    //
    // Allocate Memory
    //
    int matrixWidth = 0;
    chCommandLineGet<int>(&matrixWidth, "s", argc, argv);
    chCommandLineGet<int>(&matrixWidth, "size", argc, argv);
    matrixWidth = matrixWidth != 0 ? matrixWidth : DEFAULT_MATRIX_WIDTH;

    int matrixSize = matrixWidth * matrixWidth;

    //
    // Host Memory
    //
    bool pinnedMemory = chCommandLineGetBool("p", argc, argv);
    if (!pinnedMemory) {
        pinnedMemory = chCommandLineGetBool("pinned-memory",argc,argv);
    }

    float* h_matrixA = NULL;
    float* h_matrixB = NULL;
    float* h_matrixC = NULL;
    if (!pinnedMemory) {
        // Pageable
        h_matrixA = static_cast<float*>(malloc(
                        static_cast<size_t>(matrixSize * sizeof(*h_matrixA))));
        h_matrixB = static_cast<float*>(malloc(
                        static_cast<size_t>(matrixSize * sizeof(*h_matrixB))));
        h_matrixC = static_cast<float*>(calloc(
                        static_cast<size_t>(matrixSize), sizeof *h_matrixC));

    } else {
        // Pinned
        cudaMallocHost(&h_matrixA, static_cast<size_t>(matrixSize * sizeof(*h_matrixA)));
        cudaMallocHost(&h_matrixB, static_cast<size_t>(matrixSize * sizeof(*h_matrixB)));
        cudaMallocHost(&h_matrixC, static_cast<size_t>(matrixSize * sizeof(*h_matrixC)));
        memset ( h_matrixC, 0, matrixSize * sizeof(*h_matrixC) );
    }

    //
    // Device Memory
    //
    float* d_matrixA = NULL;
    float* d_matrixB = NULL;
    float* d_matrixC = NULL;
    cudaMalloc(&d_matrixA, static_cast<size_t>(matrixSize * sizeof(*d_matrixA)));
    cudaMalloc(&d_matrixB, static_cast<size_t>(matrixSize * sizeof(*d_matrixB)));
    cudaMalloc(&d_matrixC, static_cast<size_t>(matrixSize * sizeof(*d_matrixC)));

    //
    // Check Pointers
    //
    if (h_matrixA == NULL || h_matrixB == NULL || h_matrixC == NULL ||
        d_matrixA == NULL || d_matrixB == NULL || d_matrixC == NULL )
    {
        std::cout << "\033[31m***" << std::endl
                  << "*** Error - Allocation of Memory failed!!!" << std::endl
                  << "***\033[0m" << std::endl;
        exit(-1);
    }

    //
    // Init Matrices
    //
    for (int i = 0; i < matrixSize; i++) {
        int x = i % matrixWidth;
        int y = i / matrixWidth;
        h_matrixA[i] = static_cast<float>(x * y);
        h_matrixB[i] = static_cast<float>(x + y);
    }

    //
    // Copy Data to the Device
    //
    memCpyH2DTimer.start();

    cudaMemcpy(d_matrixA, h_matrixA, static_cast<size_t>(matrixSize * sizeof(*d_matrixA)), 
            cudaMemcpyHostToDevice);
    cudaMemcpy(d_matrixB, h_matrixB, static_cast<size_t>(matrixSize * sizeof(*d_matrixB)), 
            cudaMemcpyHostToDevice);

    memCpyH2DTimer.stop();

    //
    // Get Kernel Launch Parameters
    //
    int blockSize = 0,
        gridSize = 0;

    // Block Dimension / Threads per Block
    chCommandLineGet<int>(&blockSize,"t", argc, argv);
    chCommandLineGet<int>(&blockSize,"threads-per-block", argc, argv);
    blockSize = blockSize != 0 ? blockSize : DEFAULT_BLOCK_DIM;

    if (blockSize > 32) {
        std::cout << "\033[31m***" << std::endl
                  << "*** Error - The number of threads per block is too big" << std::endl
                  << "***\033[0m" << std::endl;
        exit(-1);
    }

    gridSize = ceil(static_cast<float>(matrixWidth) / static_cast<float>(blockSize));

    dim3 grid_dim = dim3(gridSize, gridSize, 1);
    dim3 block_dim = dim3(blockSize, blockSize, 1);

    std::cout << "***" << std::endl
              << "*** Grid Dim:  " << grid_dim.x << "x" << grid_dim.y << "x" << grid_dim.z 
                      << std::endl
              << "*** Block Dim: " << block_dim.x << "x" << block_dim.y << "x" << block_dim.z 
                      << std::endl
              << "***" << std::endl;

    //
    // Calculate Shared Memory Size
    //
    int sharedMemSize = (blockSize * blockSize * 3) * sizeof(float);

    // get shared memory maximum size
    cudaDeviceProp prop; 
    int devNr;
    cudaGetDevice(&devNr);
    cudaGetDeviceProperties(&prop, devNr);
    if (sharedMemSize > prop.sharedMemPerBlock 
            && chCommandLineGetBool("shared", argc, argv)) {
        std::cout << "\033[31m***" << std::endl
                  << "*** Error - MatrixSize is too big for shared memory" << std::endl
                  << "***\033[0m" << std::endl;
        exit(-1);
    }

    kernelTimer.start();

    //
    // Launch Kernel
    //
    if (!chCommandLineGetBool("shared", argc, argv)) {
        std::cout << "*** Execute MatMul_Kernel" << std::endl;
        matMul_Kernel <<< grid_dim, block_dim >>> (matrixWidth, d_matrixA, d_matrixB, d_matrixC);
    } else {
        std::cout << "*** Execute shMatMul_Kernel" << std::endl;
        shMatMul_Kernel<<<grid_dim, block_dim, sharedMemSize>>>(matrixWidth, d_matrixA, d_matrixB, d_matrixC);
    }

    //
    // Synchronize
    //
    cudaDeviceSynchronize();

    //
    // Check for Errors
    //
    cudaError_t cudaError = cudaGetLastError();
    if ( cudaError != cudaSuccess ) {
        std::cout << "\033[31m***" << std::endl
                  << "***ERROR*** " << cudaError << " - " << cudaGetErrorString(cudaError)
                    << std::endl
                  << "***\033[0m" << std::endl;

        return -1;
    }

    kernelTimer.stop();

    //
    // Copy Back Data
    //
    memCpyD2HTimer.start();

    cudaMemcpy(h_matrixC, d_matrixC, static_cast<size_t>(matrixSize * sizeof(*d_matrixC)), 
            cudaMemcpyDeviceToHost);

    memCpyD2HTimer.stop();

    //
    // Check Result
    //
    bool dontCheckResult = chCommandLineGetBool("c", argc, argv);
    if (!dontCheckResult) {
        dontCheckResult = chCommandLineGetBool("no-check", argc, argv);
    }

    if (!dontCheckResult) {
        std::cout << "*** Execute CPU Matrix Multiplication" << std::endl;
        float* h_matrixD = static_cast<float*>(
                calloc(static_cast<size_t>(matrixSize), sizeof(*h_matrixD)));
        cpuTimer.start();
        MatrixMulOnHostBlocked(h_matrixA, h_matrixB, h_matrixD, 
                static_cast<long>(matrixWidth), 32);
        cpuTimer.stop();
        
        std::cout << "*** Compare the Results" << std::endl;
        bool resultOk = MatrixCompare(h_matrixC, h_matrixD, 
                static_cast<long>(matrixWidth));

        if (!resultOk) {
            std::cout << "\033[31m***" << std::endl
                      << "*** Error - The two matrices are different!!!" << std::endl
                      << "***\033[0m" << std::endl;

            exit(-1);
        }

        free(h_matrixD);
    }

    //
    // Print Meassurement Results
    //
    double totTime = memCpyH2DTimer.getTime()
                   + memCpyD2HTimer.getTime()
                   + kernelTimer.getTime();

    std::cout << "***" << std::endl
              << "*** Results:" << std::endl
              << "***    matrix-size " << matrixSize << std::endl
              << "***    time-to-copy-to-device " << 1e3 * memCpyH2DTimer.getTime()
                << " ms" << std::endl
              << "***    copy-bandwidth-to-device " 
                << 1e-9 * memCpyH2DTimer.getBandwidth(2 * matrixSize * sizeof(*h_matrixA))
                << " GB/s" << std::endl
              << "***    time-to-copy-from-device " << 1e3 * memCpyD2HTimer.getTime()
                << " ms" << std::endl
              << "***    copy-bandwidth-from-device " 
                << 1e-9 * memCpyD2HTimer.getBandwidth(matrixSize * sizeof(*h_matrixA))
                << " GB/s" << std::endl
              << "***    time-for-matrix-multiplication " << 1e3 * kernelTimer.getTime()
                  << " ms" << std::endl;
              if (!dontCheckResult) {
                  std::cout << "***    time-for-cpu-matrix-multiplication " 
                                << 1e3 * cpuTimer.getTime()
                                << " ms" << std::endl
                            << "***    speedup-kernel " 
                                << cpuTimer.getTime()/kernelTimer.getTime()
                                << std::endl
                            << "***    speedup-overall "
                                << cpuTimer.getTime()/totTime << std::endl;
              }
              std::cout << "***" << std::endl;

    if (chCommandLineGetBool("print-matrix", argc, argv) 
       && matrixWidth <= 16) {
        printOutMatrix(h_matrixC, matrixWidth);
    }

    // Free Memory
    if (!pinnedMemory) {
        free(h_matrixA);
        free(h_matrixB);
        free(h_matrixC);
    } else {
        cudaFreeHost(h_matrixA);
        cudaFreeHost(h_matrixB);
        cudaFreeHost(h_matrixC);
    }
    cudaFree(d_matrixA);
    cudaFree(d_matrixB);
    cudaFree(d_matrixC);

    return 0;
}

void
printHelp(char * programName)
{
    std::cout << "Help:" << std::endl
              << "  Usage: " << std::endl
              << "  " << programName << " [-p] [-s <matrix_size>] [-t <threads_per_block>]" 
                << std::endl
              << "                 [-g <blocks_per_grid] [-c] [--print-matrix]" 
                << std::endl
              << "" << std::endl
              << "  -p|--pinned-memory" << std::endl
              << "  Use pinned Memory instead of pageable memory" << std::endl
              << "" << std::endl
              << "  -s <matrix_size>|--size <matix_size>" << std::endl
              << "  The width of the Matrix" << std::endl
              << "" << std::endl
              << "  -t <threads_per_block>|--threads-per-block <threads_per_block>" 
                << std::endl
              << "  The number of threads per block" << std::endl
              << "" << std::endl
              << "  -c|--no-checking" << std::endl
              << "  Do not check the result of the matrix multiplication" << std::endl
              << "" << std::endl
              << "  --print-matrix" << std::endl
              << "  Print the output matrix (only recommended for small matrices)" << std::endl << std::endl
              << "Further Information:" << std::endl
              << "  sizeof(int)   = " << sizeof(int) << " bytes" << std::endl
              << "  sizeof(float) = " << sizeof(float) << " bytes" << std::endl
              << std::endl;
}
