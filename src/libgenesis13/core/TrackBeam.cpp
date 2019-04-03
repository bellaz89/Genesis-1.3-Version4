#include "TrackBeam.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include "Beam.h"

void TrackBeam::track(double delz, Beam* beam, Undulator* und, bool lastStep=true) {
    // get undulator parameter for the given step
    double aw, dax, day, ku, kx, ky;
    double qf, dqx, dqy;
    double cx, cy;
    double angle, lb, ld, lt;
    double gamma0=und->getGammaRef();

    // function pointer to the operation to do on the beam
    void (*ApplyX)(double delz, double qf, double* x, double* px, double gammaz, double dx);
    void (*ApplyY)(double delz, double qf, double* x, double* px, double gammaz, double dx);

    und->getUndulatorParameters( &aw, &dax, &day, &ku, &kx, &ky);
    und->getQuadrupoleParameters(&qf, &dqx, &dqy);
    und->getCorrectorParameters(&cx, &cy);
    und->getChicaneParameters(&angle, &lb, &ld, &lt);
    double betpar0=sqrt(1-(1+aw*aw)/gamma0/gamma0);
    // effective focusing in x and y with the correct energy dependence
    double qquad=qf*gamma0;
    double qnatx=kx*aw*aw/gamma0/betpar0;  // kx has already the scaling with ku^2
    double qnaty=ky*aw*aw/gamma0/betpar0;  // same with ky
    double qx= qquad+qnatx;
    double qy=-qquad+qnaty;
    double xoff= qquad*dqx+qnatx*dax;
    double yoff=-qquad*dqx+qnatx*dax;
    // cout << "qnaty: " << qnaty/gamma0 << " Gamma0: " << gamma0 << endl;
    if (lastStep) {
        if ((cx!=0) || (cy!=0)) {
            applyCorrector(beam, cx*gamma0, cy*gamma0);
        }
    } else {
        if (angle!=0) {
            applyChicane(beam, angle, lb, ld, lt, gamma0);
        }
    }
    // handle the different cases (drift, focusing and defocusing) with function pointers to member functions
    if (qx==0) {
        ApplyX=&TrackBeam::applyDrift;
    } else {
        xoff=xoff/qx;
        if (qx>0) {
            ApplyX=&TrackBeam::applyFQuad;
        } else {
            ApplyX=&TrackBeam::applyDQuad;
        }
    }
    if (qy==0) {
        ApplyY=&TrackBeam::applyDrift;
    } else {
        yoff=yoff/qy;
        if (qy>0) {
            ApplyY=&TrackBeam::applyFQuad;
        } else {
            ApplyY=&TrackBeam::applyDQuad;
        }
    }
    for (size_t i=0; i<beam->beam.size(); i++) {
        for (size_t j=0; j<beam->beam.at(i).size(); j++) {
            Particle* p=&beam->beam.at(i).at(j);
            double gammaz=sqrt(p->gamma*p->gamma-1- aw*aw - p->px*p->px - p->py*p->py);
            (*ApplyX)(delz, qx, &(p->x), &(p->px), gammaz, xoff);
            (*ApplyY)(delz, qy, &(p->y), &(p->py), gammaz, yoff);
        }
    }
    return;
}

void TrackBeam::applyDrift(double delz, double qf, double* x, double* px,
                           double gammaz, double dx) {
    *x+=(*px)*delz/gammaz;
    return;
}

void TrackBeam::applyFQuad(double delz, double qf, double* x, double* px,
                           double gammaz, double dx) {
    double foc=sqrt(qf/gammaz);
    double omg=foc*delz;
    double a1=cos(omg);
    double a2=sin(omg)/foc;
    double a3=-a2*foc*foc;
    double xtmp=*x-dx;
    *x =a1*xtmp+a2*(*px)/gammaz+dx;
    *px=a3*xtmp*gammaz+a1*(*px);
    return;
}


void TrackBeam::applyDQuad(double delz, double qf, double* x, double* px,
                           double gammaz, double dx) {
    double foc=sqrt(-qf/gammaz);
    double omg=foc*delz;
    double a1=cosh(omg);
    double a2=sinh(omg)/foc;
    double a3=a2*foc*foc;
    double xtmp=*x-dx;
    *x =a1*xtmp+a2*(*px)/gammaz+dx;
    *px=a3*xtmp*gammaz+a1*(*px);
    return;
}

void TrackBeam::applyCorrector(Beam* beam, double cx, double cy) {
    for (size_t i=0; i<beam->beam.size(); i++) {
        for (size_t j=0; j<beam->beam.at(i).size(); j++) {
            beam->beam.at(i).at(j).px+=cx;
            beam->beam.at(i).at(j).py+=cy;
        }
    }
    return;
}


void TrackBeam::applyChicane(Beam* beam, double angle, double lb, double ld,
                             double lt, double gamma0) {
    // the tracking is done my applying the transfer matrix for the chicane and
    // backtracking for a drift over the length of the chicane
    // the effect of the R56 is applied here to the particle phase.
    //Then the normal tracking should do the momentum dependent change in the
    // longitudinal position
    
    // The transfer matrix
    Eigen::Matrix4d tmatrix;
    chicaneTransferMatrix(tmatrix, angle, lb, ld, lt);

    for (size_t i=0; i<beam->beam.size(); i++) {
        for (size_t j=0; j<beam->beam.at(i).size(); j++) {
            Particle* p=&beam->beam.at(i).at(j);
            double gammaz=sqrt(p->gamma*p->gamma-1- p->px*p->px -
                               p->py*p->py); // = gamma*betaz=gamma*(1-(1+aw*aw)/gamma^2);
            double tmp=p->x;
            p->x =tmatrix(0, 0)*tmp        + tmatrix(0, 1)*p->px/gammaz;
            p->px=tmatrix(1, 0)*tmp*gammaz + tmatrix(1, 1)*p->px;
            tmp=p->y;
            p->y =tmatrix(2, 2)*tmp        + tmatrix(2, 3)*p->py/gammaz;
            p->py=tmatrix(3, 2)*tmp*gammaz + tmatrix(3, 3)*p->py;
        }
    }
    return;
}

void TrackBeam::chicaneTransferMatrix(Ref<Matrix4d> tmatrix, double angle, 
        double lb, double ld, double lt) {
    // the transfer matrix order is
    //  m -> bp -> ep -> d1 -> en -> bn -> d2 -> bn -> en-> d1 -> ep-> bp ->d3

    tmatrix = Eigen::Matrix4d::Identity();
    Eigen::Matrix4d d1 = Eigen::Matrix4d::Identity();
    Eigen::Matrix4d d2 = Eigen::Matrix4d::Identity();
    Eigen::Matrix4d d3 = Eigen::Matrix4d::Identity();
    Eigen::Matrix4d bpn = Eigen::Matrix4d::Identity();
    Eigen::Matrix4d epn = Eigen::Matrix4d::Identity();

    double cos_angle = cos(angle);
    double sin_angle = sin(angle);
    double R=lb/sin_angle;
    double Lpath=R*angle;
    double efoc=tan(angle)/R;
    d1(0, 1)=ld/cos_angle;  // drift between dipoles
    d1(2, 3)=ld/cos_angle;
    d2(0, 1)=lt-4*lb-2*ld;   // drift in the middle
    d2(2, 3)=lt-4*lb-2*ld;
    d3(0, 1)=-lt;            // negative drift over total chicane to get a zero element
    d3(2, 3)=-lt;
    bpn(2, 3)=Lpath;  // positive deflection angle
    bpn(0, 0)=cos_angle;
    bpn(0, 1)=R*sin_angle;
    bpn(1, 0)=-sin_angle/R;
    bpn(1, 1)=cos_angle;
    epn(1, 0)=efoc;
    epn(3, 2)=-efoc;

    tmatrix = d3*(bpn*(epn*(d1*(epn*(bpn*(d2*(bpn*(epn*(d1*(epn*bpn))))))))));  
}

void TrackBeam::applyR56(Beam* beam, Undulator* und, double lambda0) {
    double angle, lb, ld, lt;
    double gamma0=und->getGammaRef();
    und->getChicaneParameters(&angle, &lb, &ld, &lt);
    if (angle==0) {
        return;
    }
    double R56=(4*lb/sin(angle)*(1-angle/tan(angle))+2*ld*tan(angle)/cos(angle))*angle;
    //    cout << "R56: " << R56 << endl;
    R56=R56*2.*M_PI/lambda0/gamma0;
    for (size_t i=0; i<beam->beam.size(); i++) {
        for (size_t j=0; j<beam->beam.at(i).size(); j++) {
            beam->beam.at(i).at(j).theta+=R56*(beam->beam.at(i).at(j).gamma-gamma0);
        }
    }
    return;
}



