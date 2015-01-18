/*********************************************************************************
 * FILENAME         cuda_device_prop.cu
 *
 * DESCRIPTION      this program prints information about the used
 *                  graphic card to standard output
 *
 * AUTHORS          Christoph Klein
 *
 * LAST CHANGE      2 DEZ 2014
 *
 ********************************************************************************/

#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
    
    cudaDeviceProp prop;
    int dev,count;
    cudaGetDeviceCount(&count);
    cudaGetDevice(&dev);
    cudaGetDeviceProperties(&prop, dev); 

    printf("*** DEVICE PROPERTIES\n***\n");
    printf("***    DEVICE-COUNT %i\n", count);
    printf("***    ACTUAL-DEVICE %i\n", dev);
    printf("***    NAME %s\n", prop.name);
    printf("***    TOTAL-GLOBAL-MEM %.1f GB\n", prop.totalGlobalMem/1e9);
    printf("***    SHARED-MEM-PER-BLOCK %.1f kB\n", prop.sharedMemPerBlock/1e3);
    printf("***    MAX-THREADS-PER-BLOCK %i\n", prop.maxThreadsPerBlock);
    printf("***    MAX-THREADS-PER-MULTIPROCESSOR %i\n", prop.maxThreadsPerMultiProcessor);
    printf("***    MAX-THREADS-DIM (%i, %i, %i)\n"
            ,prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
    printf("***    MAX-GRID-DIM (%i, %i, %i)\n",
            prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
    printf("***    CLOCK-RATE %.2f GHz\n", prop.clockRate/1e6);
    printf("***    MULTI-PROC-COUNT %i\n", prop.multiProcessorCount);
    printf("***    MEMORY-CLOCK-RATE %.2f GHz\n", prop.memoryClockRate/1e6);
    printf("***    MEMORY-BUS-WIDTH %i bits\n", prop.memoryBusWidth);
    printf("***    L2-CACHE-SIZE %.1f kB\n", prop.l2CacheSize/1e3);
    printf("***    WARP-SIZE %i\n", prop.warpSize);
    printf("***    32BIT-REGISTERS-PER-BLOCK %i\n", prop.regsPerBlock);
    printf("***    ASYNCH-ENGINE-COUNT %i\n", prop.asyncEngineCount);
    printf("***    COMPUTE-CAPABILITY %i.%i\n", prop.major, prop.minor);
    printf("***\n");

}
