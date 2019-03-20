#ifndef __CUDA_BEAM_SOLVER__
#define __CUDA_BEAM_SOLVER__

#include<vector>
#include<cuda_runtime.h>
#include<Particle.h>

using namespace std;

// support structures

typedef struct {
    double delz;
    double xks;
    double xku;
    unsigned int numel;
    cudaSurfaceObject_t nfld;
    cudaSurfaceObject_t rtmp;
    cudaSurfaceObject_t rharm;
} DeviceBeamSolver;

typedef struct {
    double aw;
    double ax;
    double ay;
    double kx;
    double ky;
    double gradx;
    double grady;
    double autophase;
} DeviceUndulator;

typedef struct {
    unsigned int xsize, ysize;
    unsigned int numel;
    cudaSurfaceObject_t harm_host;
    cudaSurfaceObject_t ngrid_host;
    cudaSurfaceObject_t first_host;
    cudaSurfaceObject_t dgrid_host;
    cudaSurfaceObject_t gridmax_host;
} DeviceField;

typedef struct {
    Particle* part_group;
    cudaStream_t stream;
} SolverComputation;


// Core functions

void dev_advance(DeviceBeamSolver dev_beamsolver, DeviceUndulator dev_und,
                 DeviceField dev_field, Particle* particles, unsigned int size) {
    __device__ double dev_faw2(double x, double y, DeviceUndulator& dev_und);
    __device__ bool dev_getLLGridpoint(double x, double y, double &wx, double &wy,
                                       int &idx, DeviceField& dev_field,
                                       unsigned int field_idx);
    __device__ void dev_run_ODE(double gamma, double theta, double& k2pp, double& k2gg,
                                double btpar, cuDoubleComplex* rpart,
                                DeviceBeamSolver dev_beamsolver);
    __device__ void dev_rungekutta(double& gamma, double& theta, double btpar,
                                   cuDoubleComplex* rpart,
                                   DeviceBeamSolver dev_beamsolver);
    // Support functions
    __device__ cuDoubleComplex cuComplexScalarMult (cuDoubleComplex x, double y);
    __device__ void cuCAddAccumulate (cuDoubleComplex &x, cuDoubleComplex y);
    __device__ void dev_move_particles(Particle* src, Particle* dst);
    __device__ double surf1DRead(cudaSurfaceObject_t obj, int idx,
                                 boundaryMode = cudaBoundaryModeTrap);
    __device__ cuDoubleComplex surf3DComplex(cudaSurfaceObject_t, int i, int j, int k,
            boundaryMode = cudaBoundaryModeTrap);
    template<class T> void load_cuda_array(vector<T> &elems, cudaSurfaceObject_t &obj,
                                           cudaArray &arr) {
        cudaResourceDesc desc;
        cudaChannelFormatDesc channelDesc1 = cudaCreateChannelDesc<T>();
        cudaMallocArray(&arr, &channelDesc1, elems.size()*sizeof(T));
        cudaMemcpyToArray(arr, 0, 0, elems.data(), elems.size()*sizeof(T));
        desc.resType = cudaResourceTypeArray;
        desc.res.array.array = arr;
        cudaCreateSurfaceObject(&obj, &desc);
    }
    void load_cuda_array(vector<double> elems &, cudaSurfaceObject_t &obj,
                         cudaArray &arr) {
        cudaResourceDesc desc;
        cudaChannelFormatDesc channelDesc1 = cudaCreateChannelDesc<uint2>();
        cudaMallocArray(&arr, &channelDesc1, elems.size()*sizeof(double));
        cudaMemcpyToArray(arr, 0, 0, elems.data(), elems.size()*sizeof(double));
        desc.resType = cudaResourceTypeArray;
        desc.res.array.array = arr;
        cudaCreateSurfaceObject(&obj, &desc);
    }
#endif