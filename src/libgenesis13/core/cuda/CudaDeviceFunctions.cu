//
// Cuda Beam solver functions 
//

#include<CudaDeviceFunctions.h>

__global__ void dev_advance(DeviceBeamSolver dev_beamsolver, DeviceUndulator dev_und, DeviceField dev_field, Particle* particles, unsigned int size){

    cuDoubleComplex* rpart = (cuDoubleComplex*)malloc(dev_beamsolver.numel*sizeof(cuDoubleComplex));
    __shared__ Particle local_parts[BLOCK_DIM];

    double awloc;
    double btpar;
    double wx, wy;
    unsigned int nfld;
    cuDoubleComplex cpart; 
    int islice, idx_part = 0;
    unsigned int idx = BLOCK_DIM*blockIdx.x+threadIdx.x;

    dev_move_particles(particles+BLOCK_DIM*blockIdx.x, local_parts);
    __syncthreads();

    if (idx < size) {

        Particle part = local_parts[threadIdx.x];
        awloc = dev_faw2(part.x, part.y, dev_und);
        btpar=1+part.px*part.px+part.py*part.py+dev_und.aw*dev_und.aw*awloc*awloc;	  
        for (unsigned int ifld=0; ifld<dev_beamsolver.numel; ifld++){
            islice = (idx + first_arr[ifld]) % dev_field.xsize;
            nfld = nfld_arr[ifld];
            if(dev_getLLGridpoint(part.x, part.y, wx, wy, idx_part, dev_field, nfld)){
                cpart = cuCxR(tex3DDoubleComplex(nfld, islice, idx_part), wx*wy);
                idx_part++;
                cuCAddAss(cpart, cuCxR(tex3DDoubleComplex(nfld, islice, idx_part),(1-wx)*wy));
                idx_part += ngrid_arr[nfld] - 1;
                cuCAddAss(cpart, cuCxR(tex3DDoubleComplex(nfld, islice, idx_part),wx*(1-wy)));
                idx_part++;
                cuCAddAss(cpart, cuCxR(tex3DDoubleComplex(nfld, islice, idx_part),(1-wx)*(1-wy)));
                cpart = cuConj(cpart);
                awloc *= rtmp_arr[ifld];
                cpart = cuCxR(cpart, awloc);
                rpart[ifld] = cpart;
            } else {
                rpart[ifld] = make_cuDoubleComplex(0, 0);
            }
        }
        dev_rungekutta(part.gamma, part.theta, btpar, rpart, dev_beamsolver); 
        local_parts[threadIdx.x] = part;
    }

    free(rpart);
    __syncthreads(); 
    dev_move_particles(local_parts,particles+BLOCK_DIM*blockIdx.x);
    __syncthreads(); 
}

__device__ cuDoubleComplex cuCxR (cuDoubleComplex x, double y)
{
    cuDoubleComplex ret = x;
    ret.x *= y;
    ret.y *= y;
    return ret;
}

__device__ void cuCAddAss (cuDoubleComplex &x, cuDoubleComplex y) { 
    x.x += y.x;
    x.y += y.y;    
}

__device__ cuDoubleComplex tex3DDoubleComplex(unsigned int i, unsigned int j, unsigned int k) {

    cuDoubleComplex ret;
    int4 v = surf2DLayeredread<int4>(field_surf, j, k, i);
    ret.x = __hiloint2double(v.y, v.x);
    ret.y = __hiloint2double(v.w, v.z);
    return ret;
}


__device__ void dev_move_particles(Particle* src, Particle* dst){

    double* src_array = (double*) src;
    double* dst_array = (double*) dst;

    for (unsigned int i = 0; i < 6; ++i) {
        dst_array[BLOCK_DIM*i+threadIdx.x] = src_array[BLOCK_DIM*i+threadIdx.x];
    }
}

__device__ double dev_faw2(double x, double y, DeviceUndulator& dev_und){  // square of the transverse dependence of the undulator field.
    double dx=x-dev_und.ax;
    double dy=y-dev_und.ay; 
    return (1+dev_und.kx*dx*dx+dev_und.ky*dy*dy+2*(dev_und.gradx*dx+dev_und.grady*dy));
}


__device__ bool dev_getLLGridpoint(double x, double y, double &wx, double &wy, int &idx, DeviceField& dev_field, unsigned int field_idx){

    double gridmax = gridmax_arr[field_idx];
    double dgrid   = dgrid_arr[field_idx];
    int ngrid      = ngrid_arr[field_idx];

    bool ret = (fabs(x) < gridmax) && (fabs(y) < gridmax);
    if (ret) {
        double tempwx = (x+gridmax)/dgrid;
        double tempwy = (y+gridmax)/dgrid;
        tempwx = 1+floor(tempwx)-tempwx; 
        tempwy = 1+floor(tempwy)-tempwy; 

        int ix = static_cast<int> (floor(tempwx));
        int iy = static_cast<int> (floor(tempwy));
        int tempidx = ix+iy*ngrid;

        wx  = tempwx;
        wy  = tempwy;
        idx = tempidx;
    }
    return ret;
}

__device__ void dev_run_ODE(double gamma, double theta, double& k2pp, double& k2gg, double btpar, cuDoubleComplex* rpart, DeviceBeamSolver dev_beamsolver){

    double real, imag;
    double ztemp1=-2./dev_beamsolver.xks;
    double btper0;
    double btpar0;
    cuDoubleComplex ctmp = make_cuDoubleComplex(0, 0);
    for (unsigned int i=0; i < dev_beamsolver.numel;i++){
        sincos(rharm_arr[i] * theta, &real, &imag);
        cuCAddAss(ctmp, cuCmul(make_cuDoubleComplex(real, -imag), rpart[i]));
    }
    btper0 = btpar+ztemp1*ctmp.x;
    btpar0 = sqrt(1.-btper0/(gamma*gamma));
    k2pp+= dev_beamsolver.xks*(1.-1./btpar0)+dev_beamsolver.xku;
    k2gg+= ctmp.y/btpar0/gamma;
}

__device__ void dev_rungekutta(double& gamma, double& theta, double btpar, cuDoubleComplex* rpart, DeviceBeamSolver dev_beamsolver) {

    double k2gg=0;
    double k2pp=0;
    double k3gg;
    double k3pp;
    double stpz=0.5 * dev_beamsolver.delz;
    dev_run_ODE(gamma,theta, k2pp, k2gg, btpar, rpart, dev_beamsolver);
    gamma+=stpz*k2gg;
    theta+=stpz*k2pp;
    k3gg=k2gg;
    k3pp=k2pp;
    k2gg=0;
    k2pp=0;
    dev_run_ODE(gamma,theta, k2pp, k2gg, btpar, rpart, dev_beamsolver);
    gamma+=stpz*(k2gg-k3gg);
    theta+=stpz*(k2pp-k3pp);
    k3gg/=6;
    k3pp/=6;
    k2gg*=-0.5;
    k2pp*=-0.5;
    dev_run_ODE(gamma,theta, k2pp, k2gg, btpar, rpart, dev_beamsolver);
    stpz=dev_beamsolver.delz;
    gamma+=stpz*k2gg;
    theta+=stpz*k2pp;
    k3gg-=k2gg;
    k3pp-=k2pp;
    k2gg*=2;
    k2pp*=2;
    dev_run_ODE(gamma, theta, k2pp, k2gg, btpar, rpart, dev_beamsolver);
    gamma+=stpz*(k3gg+k2gg/6.0);
    theta+=stpz*(k3pp+k2pp/6.0);
}


