
#include "Control.h"
#include <sstream>
#include <utility>
#include <mpi.h>
#ifdef VTRACE
#include "vt_user.h"
#endif
#include <libgenesis13/io/HDF5Base.h>
#include <libgenesis13/io/WriteFieldHDF5.h>
#include <libgenesis13/io/WriteBeamHDF5.h>
#include <libgenesis13/io/Output.h>

#define PROCESS_TAG 1

using std::swap;
using std::fill;

bool Control::applyMarker(Beam* beam, vector<Field*>* field, Undulator* und) {
    bool sort=false;
    int marker=und->getMarker();
    // possible file names
    int istepz=und->getStep();
    stringstream sroot;
    sroot << "." << istepz;
    if ((marker & 1) != 0) {
        WriteFieldHDF5 dump;
        dump.write(root+sroot.str(), field);
    }
    if ((marker & 2) != 0) {
        WriteBeamHDF5 dump;
        dump.write(root+sroot.str(), beam);
    }
    if ((marker & 4) != 0) {
        sort=true;   // sorting is deferred after the particles have been pushed by Runge-Kutta
    }
    // bit value 8 is checked in und->advance()
    return sort;
}

void Control::output(Beam* beam, vector<Field*>* field, Undulator* und) {
    Output* out=new Output;
    string file=root.append(".out.h5");
    out->open(file, noffset, nslice);
    out->writeGlobal(und->getGammaRef(), reflen, sample, slen, one4one, timerun,
                     scanrun);
    out->writeLattice(beam, und);
    for (size_t i=0; i<field->size(); i++) {
        out->writeFieldBuffer(field->at(i));
    }
    out->writeBeamBuffer(beam);
    out->close();
    delete out;
    return;
}

bool Control::init(int inrank, int insize, const char* file, Beam* beam,
                   vector<Field*>* field, Undulator* und, bool inTime, bool inScan) {
    rank=inrank;
    size=insize;
    accushift=0;
    stringstream sroot(file);
    root=sroot.str();
    root.resize(root.size()-7);  // remove the extension ".h5"
    one4one=beam->one4one;
    reflen=beam->reflength;
    sample=beam->slicelength/reflen;
    timerun=inTime;
    scanrun=inScan;
    // cross check simulation size
    nslice=beam->beam.size();
    noffset=rank*nslice;
    int ntotal=size*nslice;  // all cores have the same amount of slices
    slen=ntotal*sample*reflen;
    if (rank==0) {
        if(scanrun) {
            cout << "Scan run with " << ntotal << " slices" << endl;
        } else {
            if(timerun) {
                cout << "Time-dependent run with " << ntotal << " slices";
                cout << " for a time window of " << slen*1e6 << " microns" << endl;
            } else {
                cout << "Steady-state run" << endl;
            }
        }
    }
    // initial diagnostic
    if (rank==0) {
        cout << "Initial analysis of electron beam and radiation field..."  << endl;
    }
    beam->initDiagnostics(und->outlength());
    beam->diagnostics(true, 0);
    beam->diagnosticsStart();
    for (size_t i=0; i<field->size(); i++) {
        field->at(i)->initDiagnostics(und->outlength());
        field->at(i)->diagnostics(true);  // initial values
    }
    return true;
}

void Control::applySlippage(double slippage, Field* field) {
#ifdef VTRACE
    VT_TRACER("Slippage");
#endif
    if (timerun==false) {
        return;
    }
    // update accumulated slippage
    accushift+=slippage;
    // following routine is applied if the required slippage is alrger than 80%
    // of the sampling size
    int direction=1;
    while(abs(accushift)>(sample*0.8)) {
        // check for anormal direction of slippage (backwards slippage)
        if (accushift<0) {
            direction=-1;
        }
        accushift-=sample*direction;
        // get adjacent node before and after in chain
        int rank_next=(rank==size-1) ? MPI_PROC_NULL : rank+1;
        int rank_prev=(rank==0) ? MPI_PROC_NULL :rank-1;
        // for inverse direction swap targets
        if (direction<0) {
            swap(rank_next, rank_prev);
        }
        // get slice which is transmitted
        int last=(field->first+field->field.size()-1)  %  field->field.size();
        // get first slice for inverse direction
        if (direction<0) {
            //  this actually first because it is sent backwards
            last=(last+1) % field->field.size();
        }
        // allocate receive buffer
        receive_buffer.resize(field->field[last].size());
        if (size>1) {
            MPI_Sendrecv(field->field[last].data(), field->field[last].size(),
                         MPI_DOUBLE_COMPLEX, rank_next, PROCESS_TAG,
                         receive_buffer.data(), field->field[last].size(),
                         MPI_DOUBLE_COMPLEX, rank_prev, PROCESS_TAG,
                         MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
            swap(field->field[last], receive_buffer);
        }
        // first node has emptz field slipped into the time window
        // last was the last slice to be transmitted to the succeding node
        // and then filled with the
        if (((rank==0) and (direction >0)) or ((rank==(size-1)) and (direction <0))) {
            fill(field->field[last].begin(), field->field[last].end(), complex<double>(0, 0));
        }
        // the field from the preceeding node, making it now the start of the field record.
        field->first=last;
        if (direction<0) {
            field->first=(last+1) % field->field.size();
        }
    }
}
