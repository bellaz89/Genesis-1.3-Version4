// This unit tests the original implementation of the inverse complementary
// error function
#include <gtest/gtest.h>
#include <random>
#include <cmath>
#include <libgenesis13/core/TrackBeam.h>

#define MAX_ERROR 1e-15
#define TEST_N_VALUES 1000

void matmul(double m[][4], double e[][4]) {
    double t[4][4];
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            t[i][j]=0;
            for (int k=0; k<4; k++) {
                t[i][j]+=e[i][k]*m[k][j];
            }
        }
    }
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            m[i][j]=t[i][j];
        }
    }
}

void apply_chicane(double m[][4], double angle, double lb, double ld, double lt) {
    double d1[4][4];
    double d2[4][4];
    double d3[4][4];
    double bp[4][4];
    double bn[4][4];
    double ep[4][4];
    double en[4][4];
    // construct the transfer matrix
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            m[i][j]=0;
            d1[i][j]=0;
            d2[i][j]=0;
            d3[i][j]=0;
            bp[i][j]=0;
            bn[i][j]=0;
            ep[i][j]=0;
            en[i][j]=0;
        }
        m[i][i]=1;
        d1[i][i]=1;
        d2[i][i]=1;
        d3[i][i]=1;
        bp[i][i]=1;
        bn[i][i]=1;
        ep[i][i]=1;
        en[i][i]=1;
    }
    d1[0][1]=ld/cos(angle);  // drift between dipoles
    d1[2][3]=ld/cos(angle);
    d2[0][1]=lt-4*lb-2*ld;   // drift in the middle
    d2[2][3]=lt-4*lb-2*ld;
    d3[0][1]=-lt;            // negative drift over total chicane to get a zero element
    d3[2][3]=-lt;
    double R=lb/sin(angle);
    double Lpath=R*angle;
    bp[2][3]=Lpath;  // positive deflection angle
    bp[0][0]=cos(angle);
    bp[0][1]=R*sin(angle);
    bp[1][0]=-sin(angle)/R;
    bp[1][1]=cos(angle);
    bn[2][3]=Lpath; // negative deflection angle
    bn[0][0]=cos(-angle);
    bn[0][1]=R*sin(-angle)*-1;
    bn[1][0]=-sin(-angle)/R*-1;
    bn[1][1]=cos(-angle);
    double efoc=tan(angle)/R;
    ep[1][0]=efoc;
    ep[3][2]=-efoc;
    en[1][0]=-efoc*-1;
    en[3][2]=efoc*-1;
    matmul(m, bp);
    matmul(m, ep);
    matmul(m, d1);
    matmul(m, en);
    matmul(m, bn);
    matmul(m, d2);
    matmul(m, bn);
    matmul(m, en);
    matmul(m, d1);
    matmul(m, ep);
    matmul(m, bp);
    matmul(m, d3); // transport backwards because the main tracking still has to do the drift
}

TEST(compat, chicane_transfer_matrix) {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.1, 1.55);
    double mold[4][4];
    Matrix4d mnew;
    for(unsigned int i=0; i<TEST_N_VALUES; i++) {
        double angle = distribution(generator);
        double lb = distribution(generator);
        double ld = distribution(generator);
        double lt = distribution(generator);
        apply_chicane(mold, angle, lb, ld, lt);
        TrackBeam::chicaneTransferMatrix(mnew, angle, lb, ld, lt);
        for(unsigned int j=0; j<4; j++)
            for(unsigned int k=0; k<4; k++) {
                ASSERT_NEAR(mold[j][k], mnew(j, k), MAX_ERROR);
            }
    }
}
