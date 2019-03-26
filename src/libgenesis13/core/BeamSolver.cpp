#include <complex>
#include "BeamSolver.h"
#include "Field.h"
#include "Beam.h"

using namespace std;

BeamSolver::BeamSolver(bool _onlyFundamental) : onlyFundamental(_onlyFundamental) {}

void BeamSolver::advance(double delz, Beam* beam, vector< Field*>* field,
                         Undulator* und) {
    // here the harmonics needs to be taken into account
    vector<int> nfld;
    vector<double> rtmp;
    vector<complex<double>> rpart;
    vector<double> rharm;
    double xks=1.0;  // default value in the case that no field is defined
    for (int i=0; i < field->size(); i++) {
        int harm=field->at(i)->getHarm();
        if ((harm==1) || !onlyFundamental) {
            xks=field->at(i)->xks/static_cast<double>
                (harm);    // fundamental field wavenumber used in ODE below
            nfld.push_back(i);
            rtmp.push_back(und->fc(harm)/field->at(
                               i)->xks);      // here the harmonics have to be taken care
            rpart.push_back(0);
            rharm.push_back(static_cast<double>(harm));
        }
    }
    double xku=und->getku();
    if (xku==0) {  // in the case of drifts - the beam stays in phase if it has the reference energy // this requires that the phase slippage is not applied
        xku=xks*0.5/und->getGammaRef()/und->getGammaRef();
    }
    double aw=und->getaw();
    double autophase=und->autophase();
    // Runge Kutta solver to advance particle
    for (int is=0; is<beam->beam.size(); is++) {
        for (int ip=0; ip<beam->beam.at(is).size(); ip++) {
            SimulationParams sim_params(rharm, rpart);
            sim_params.xku = xku;
            sim_params.xks = xks;
            sim_params.gamma=beam->beam.at(is).at(ip).gamma;
            sim_params.theta=beam->beam.at(is).at(ip).theta+autophase; // add autophase here
            double x =beam->beam.at(is).at(ip).x;
            double y =beam->beam.at(is).at(ip).y;
            double px=beam->beam.at(is).at(ip).px;
            double py=beam->beam.at(is).at(ip).py;
            double awloc=und->faw(x,
                                  y);                // get the transverse dependence of the undulator field
            sim_params.btpar=1+px*px+py*py+aw*aw*awloc*awloc;
            complex<double> cpart=0;
            double wx, wy;
            int idx;
            for (int ifld=0; ifld<nfld.size(); ifld++) {
                int islice=(is+field->at(nfld[ifld])->first) % field->at(nfld[ifld])->field.size();
                // check whether particle is on grid
                if (field->at(nfld[ifld])->getLLGridpoint(x, y, &wx, &wy,
                        &idx)) {
                    cpart=field->at(nfld[ifld])->field[islice].at(idx)*wx*wy;
                    idx++;
                    cpart+=field->at(nfld[ifld])->field[islice].at(idx)*(1-wx)*wy;
                    idx+=field->at(nfld[ifld])->ngrid-1;
                    cpart+=field->at(nfld[ifld])->field[islice].at(idx)*wx*(1-wy);
                    idx++;
                    cpart+=field->at(nfld[ifld])->field[islice].at(idx)*(1-wx)*(1-wy);
                    rpart[ifld]=rtmp[ifld]*awloc*conj(cpart);
                }
            }
            this->RungeKutta(delz, sim_params);
            beam->beam.at(is).at(ip).gamma=sim_params.gamma;
            beam->beam.at(is).at(ip).theta=sim_params.theta;
        }
    }
    return;
}

void BeamSolver::RungeKutta(const double delz, SimulationParams &sim_params) {
    // Runge Kutta Solver 4th order - taken from pushp from the old Fortran source
    // first step
    double k2gg=0.;
    double k2pp=0.;
    double k3gg=0;
    double k3pp=0;
    this->ODE(k2gg, k2pp, sim_params);
    // second step
    double stpz=0.5*delz;
    sim_params.gamma+=stpz*k2gg;
    sim_params.theta+=stpz*k2pp;
    k3gg=k2gg;
    k3pp=k2pp;
    k2gg=0;
    k2pp=0;
    this->ODE(k2gg, k2pp, sim_params);
    // third step
    sim_params.gamma+=stpz*(k2gg-k3gg);
    sim_params.theta+=stpz*(k2pp-k3pp);
    k3gg/=6;
    k3pp/=6;
    k2gg*=-0.5;
    k2pp*=-0.5;
    this->ODE(k2gg, k2pp, sim_params);
    // fourth step
    stpz=delz;
    sim_params.gamma+=stpz*k2gg;
    sim_params.theta+=stpz*k2pp;
    k3gg-=k2gg;
    k3pp-=k2pp;
    k2gg*=2;
    k2pp*=2;
    this->ODE(k2gg, k2pp, sim_params);
    sim_params.gamma+=stpz*(k3gg+k2gg/6.0);
    sim_params.theta+=stpz*(k3pp+k2pp/6.0);
    return;
}


void BeamSolver::ODE(double &k2gg, double &k2pp,
                     const SimulationParams &sim_params) {
    // differential equation for longitudinal motion
    double ztemp1=-2./sim_params.xks;
    complex<double> ctmp=0;
    for (int i=0; i<sim_params.rpart.size(); i++) {
        double angle = sim_params.rharm[i]*sim_params.theta;
        ctmp+=sim_params.rpart[i]*complex<double> (cos(angle), -sin(angle));
    }
    double btper0=sim_params.btpar+ztemp1*ctmp.real();   //perpendicular velocity
    double btpar0=sqrt(1.-btper0/
                       (sim_params.gamma*sim_params.gamma));     //parallel velocity
    k2pp+=sim_params.xks*(1.-1./btpar0)+sim_params.xku;             //dtheta/dz
    k2gg+=ctmp.imag()/btpar0/sim_params.gamma;         //dgamma/dz
    return;
}

void BeamSolver::initEField(double rmax, int ngrid, int nz, int nphi,
                            double lambda) {
    efield.init(rmax, ngrid, nz, nphi, lambda);
    return;
}


void BeamSolver::track(double dz, Beam* beam, Undulator* und, bool last) {
    tracker.track(dz, beam, und, last);
    return;
}

void BeamSolver::applyR56(Beam* beam, Undulator* und, double reflen) {
    tracker.applyR56(beam, und, reflen);
    return;
}


