#include <cuda_runtime.h>
#include <cuComplex.h>

#include <Beam.h>
#include <Field.h>
#include <Undulator.h>
#include <vector>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

using namespace std;

#define BLOCK_DIM 32

//__device__ __constant__ int nfld_arr[]; //1D int texture obj
//__device__ __constant__ double rtmp_arr[]; //1D double texture obj
//__device__ __constant__ double rharm_arr[];//1D double texture obj
//__device__ __constant__ int harm_arr[];   // 1D double texture obj
//__device__ __constant__ int ngrid_arr[];  // 1D int texture obj
//__device__ __constant__ int first_arr[];  // 1D int texture obj
//__device__ __constant__ double dgrid_arr[];  // 1D double texture obj
//__device__ __constant__ double gridmax_arr[];// 1D double texture obj

surface<void, cudaSurfaceType2DLayered> field_surf; // 3D double2(complex) texture obj


BeamSolver::BeamSolver()
{
  onlyFundamental=false;
}

BeamSolver::~BeamSolver(){}


void BeamSolver::advance(double delz, Beam* beam, vector<Field*>* field, Undulator* und) {

    bool onlyFundamental = false; //!!!! change, stub
    vector<int> nfld;
    vector<double> rtmp;
    vector<double> rharm;
    int xks=1;  // default value in the case that no field is defined
    int xku=und->getku();

    int* harm_host;   // 1D double texture obj
    int* ngrid_host;  // 1D int texture obj
    int* first_host;  // 1D int texture obj
    double* dgrid_host;  // 1D double texture obj
    double* gridmax_host;// 1D double texture obj

    cudaChannelFormatDesc field_channel_desc = cudaCreateChannelDesc<cuDoubleComplex>();
    cudaArray* field_array_ref;
    cuDoubleComplex* field_array_data_host;
    cudaMemcpy3DParms field_memcpy_parms = {0};
    vector<SolverComputation> solver_computations;
    
    if (xku==0){   // in the case of drifts - the beam stays in phase if it has the reference energy // this requires that the phase slippage is not applied
        xku=xks*0.5/und->getGammaRef()/und->getGammaRef();
    }

    for (int i=0; i < field->size(); i++){
        int harm=field->at(i)->getHarm();
        if ((harm==1) || !onlyFundamental){
            xks=field->at(i)->xks/static_cast<double>(harm);    // fundamental field wavenumber used in ODE below
            nfld.push_back(i);
            rtmp.push_back(und->fc(harm)/field->at(i)->xks);      // here the harmonics have to be taken care
            rharm.push_back(static_cast<double>(harm));
        }
    }  

    double aw=und->getaw();
    double autophase=und->autophase();

    DeviceBeamSolver dev_beamsolver;
    dev_beamsolver.delz = delz;
    dev_beamsolver.xks = xks;
    dev_beamsolver.xku = xku;
    dev_beamsolver.numel = field->size();

    DeviceUndulator dev_undulator;
    dev_undulator.aw = und->aw[und->istepz];  
    dev_undulator.ax = und->ax[und->istepz];               
    dev_undulator.ay = und->ay[und->istepz];
    dev_undulator.kx = und->kx[und->istepz];
    dev_undulator.ky = und->ky[und->istepz];
    dev_undulator.gradx = und->gradx[und->istepz]; 
    dev_undulator.grady = und->grady[und->istepz];
    dev_undulator.autophase = autophase;

    DeviceField dev_field;
    dev_field.numel = field->size();
    dev_field.xsize = field->at(0)->field.size();
    dev_field.ysize = field->at(0)->field.at(0).size();

    gpuErrchk(cudaMalloc(&nfld_arr, nfld.size()*sizeof(int)));	
    gpuErrchk(cudaMalloc(&rtmp_arr, rtmp.size()*sizeof(double)));	
    gpuErrchk(cudaMalloc(&rharm_arr, rharm.size()*sizeof(double)));	
    
    gpuErrchk(cudaMemcpyToSymbol(nfld_arr, nfld.data(), sizeof(int)*nfld.size()));
    gpuErrchk(cudaMemcpyToSymbol(rtmp_arr, rtmp.data(), sizeof(double)*rtmp.size()));
    gpuErrchk(cudaMemcpyToSymbol(rharm_arr, rharm.data(), sizeof(double)*rharm.size()));

    gpuErrchk(cudaMallocHost(&harm_host, field->size()*sizeof(int)));	
    gpuErrchk(cudaMallocHost(&ngrid_host, field->size()*sizeof(int)));	
    gpuErrchk(cudaMallocHost(&first_host, field->size()*sizeof(int)));	
    gpuErrchk(cudaMallocHost(&dgrid_host, field->size()*sizeof(double)));	
    gpuErrchk(cudaMallocHost(&gridmax_host, field->size()*sizeof(double)));	
    
    field_array_data_host = (cuDoubleComplex*)malloc(sizeof(cuDoubleComplex)*dev_field.xsize*dev_field.ysize*dev_field.numel);

    for (int i = 0; i < field->size(); ++i) {
        Field* field_el = field->at(i);
        harm_host[i] = field_el->harm; 
        ngrid_host[i] = field_el->ngrid;   
        first_host[i] = field_el->first;   
        dgrid_host[i] = field_el->dgrid;  
        gridmax_host[i] = field_el->gridmax;

        for (int j = 0;  j < dev_field.xsize; j++) {
            
            memcpy((void*)(field_array_data_host+dev_field.ysize*(j+dev_field.xsize*i)), field_el->field[j].data(), sizeof(cuDoubleComplex)*dev_field.ysize);            
        }
    }
    
    cudaMalloc3DArray(&field_array_ref, &field_channel_desc, make_cudaExtent(sizeof(cuDoubleComplex)*dev_field.xsize, dev_field.ysize, dev_field.numel), cudaArrayLayered);
    field_memcpy_parms.srcPos = make_cudaPos(0,0,0);
    field_memcpy_parms.dstPos = make_cudaPos(0,0,0);
    field_memcpy_parms.srcPtr = make_cudaPitchedPtr(field_array_data_host, dev_field.xsize * sizeof(cuDoubleComplex), dev_field.ysize, dev_field.numel);
    field_memcpy_parms.dstArray = field_array_ref;
    field_memcpy_parms.extent = make_cudaExtent(dev_field.xsize, dev_field.ysize, dev_field.numel);
    field_memcpy_parms.kind = cudaMemcpyHostToDevice;
    cudaMemcpy3D(&field_memcpy_parms);
    cudaBindSurfaceToArray(field_surf, field_array_ref, field_channel_desc);

    cudaMemcpyToSymbol(harm_arr, harm_host, field->size()*sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(ngrid_arr, ngrid_host, field->size()*sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(first_arr, first_host, field->size()*sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(dgrid_arr, dgrid_host, field->size()*sizeof(double), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(gridmax_arr, gridmax_host, field->size()*sizeof(double), 0, cudaMemcpyHostToDevice);

    for (unsigned int i = 0; i < beam->beam.size(); ++i) {
        unsigned int group_len = beam->beam[i].size();
        unsigned int n_blocks = group_len / BLOCK_DIM;
        SolverComputation computation;
        if ((group_len % BLOCK_DIM) != 0) n_blocks++;
        cudaStreamCreate(&computation.stream);
        cudaMalloc(&computation.part_group, n_blocks*BLOCK_DIM*sizeof(Particle));     
        cudaMemcpyAsync(computation.part_group, beam->beam[i].data(), sizeof(Particle)*group_len, cudaMemcpyHostToDevice, computation.stream);
        dev_advance<<<n_blocks, BLOCK_DIM, 0, computation.stream>>>(dev_beamsolver, dev_undulator, dev_field, computation.part_group, group_len);
        cudaMemcpyAsync(beam->beam[i].data(), computation.part_group, sizeof(Particle)*group_len, cudaMemcpyDeviceToHost, computation.stream);
        solver_computations.push_back(computation);
    }

    for (unsigned int i = 0; i < beam->beam.size(); ++i) {
        SolverComputation computation = solver_computations[i];
        cudaStreamSynchronize(computation.stream);
        cudaFree(computation.part_group);
    }
    // No surface unbind procedures. MEH!
    cudaFreeArray(field_array_ref);
    free(field_array_data_host);
    cudaFreeHost(harm_host);
    cudaFreeHost(ngrid_host);
    cudaFreeHost(first_host);
    cudaFreeHost(dgrid_host);
    cudaFreeHost(gridmax_host);
}
