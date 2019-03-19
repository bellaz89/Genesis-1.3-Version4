#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mpi.h>
#include <libgenesis13/core/Field.h>
#include <libgenesis13/core/Beam.h>
#include <libgenesis13/core/Particle.h>
#include <libgenesis13/core/Gencore.h>
#include <libgenesis13/io/SDDSBeam.h>
#include <libgenesis13/lattice/AlterLattice.h>
#include <libgenesis13/lattice/Lattice.h>
#include <libgenesis13/loading/Profile.h>
#include <libgenesis13/loading/ImportBeam.h>
#include <libgenesis13/loading/ImportField.h>
#include <libgenesis13/loading/LoadField.h>
#include <libgenesis13/loading/LoadBeam.h>
#include "Time.h"
#include "SponRad.h"
#include "Wake.h"
#include "AlterSetup.h"
#include "Dump.h"
#include "Parser.h"
#include "Setup.h"
#include "Track.h"
#include "EField.h"

using namespace std;
const double vacimp = 376.73;
const double eev    = 510999.06; 
const double ce     = 4.8032045e-11;

string* meta_inputfile;
string* meta_latfile;

bool MPISingle;  // global variable to do mpic or not


double genmain (string mainstring, string latstring, bool streaming, bool supressOutput, bool split) {

        double ret=0;
    
        MPISingle=split;       


        int rank=MPI::COMM_WORLD.Get_rank(); // assign rank to node
        int size=MPI::COMM_WORLD.Get_size(); // get size of cluster
	if (MPISingle){
	  rank=0;
	  size=1;
        }


        time_t timer;
	if (rank==0){
          time(&timer);
          cout << "---------------------------------------------" << endl;
          cout << "GENESIS - Version " << VERSIONMAJOR <<"."<< VERSIONMINOR << "." << VERSIONREVISION ;
	  if (VERSIONBETA) {cout << " (beta)";}
	  cout << " has started..." << endl;			
	  cout << "Starting Time: " << ctime(&timer)<< endl;
          cout << "MPI-Comm Size: " << size << " nodes" << endl << endl;
        }

        //---------------------------------------------------------
        // Instance of beam and field class to carry the distribution

        vector<Field* > field;   // an vector of various field components (harmonics, vertical/horizonthal components)
        Beam * beam =new Beam;


        //----------------------------------------------------------
        // main loop extracting one element with arguments at a time
      
        Parser parser; 
        string element;
        map<string,string> argument;

        Setup* setup=new Setup;
	AlterLattice* alt=new AlterLattice;
        Lattice* lattice=new Lattice;
        Profile* profile=new Profile;
        Time* timewindow=new Time;

        if (streaming){
    	    meta_inputfile=new string ("streaming");
	} else {
    	    meta_inputfile=new string (mainstring);
	}

        parser.open(mainstring,rank,streaming);

        while(parser.parse(&element,&argument)){
           
          //----------------------------------------------
	  // setup & parsing the lattice file

          if (element.compare("&setup")==0){
            if (!setup->init(rank,&argument,lattice,latstring,streaming)){ break;}
	    meta_latfile=new string (setup->getLattice());
            continue;  
          }  

          //----------------------------------------------
	  // modifying run

          if (element.compare("&alter_setup")==0){
	    AlterSetup* altersetup= new AlterSetup;
            if (!altersetup->init(rank,&argument,setup,lattice,timewindow,beam,&field)){ break;}
	    delete altersetup;
            continue;  
          }  

          //----------------------------------------------
	  // modifying the lattice file

          if (element.compare("&lattice")==0){
            if (!alt->init(rank,size,&argument,lattice,setup)){ break;}
            continue;  
          }  

          //---------------------------------------------------
          // adding profile elements

          if ((element.compare("&profile_const")==0)||
              (element.compare("&profile_gauss")==0)||
              (element.compare("&profile_file")==0)||
              (element.compare("&profile_polynom")==0)||
              (element.compare("&profile_step")==0)){            
            if (!profile->init(rank,&argument,element)){ break; }
            continue;
	  }

          //----------------------------------------------------
          // defining the time window of simulation

	  if (element.compare("&time")==0){
            if (!timewindow->init(rank,size,&argument,setup)){ break;}
            continue;  
          }  

          //----------------------------------------------------
          // internal generation of the field

	  if (element.compare("&field")==0){
	    LoadField* loadfield=new LoadField;
            if (!loadfield->init(rank,size,&argument,&field,setup,timewindow,profile)){ break;}
	    delete loadfield;
            continue;  
          }  
	 
          //----------------------------------------------------
          // setup of space charge field

	  if (element.compare("&efield")==0){
   	    EField* efield=new EField;
            if (!efield->init(rank,size,&argument,beam,setup,timewindow)){ break;}
	    delete efield;
            continue;  
          }  

          //----------------------------------------------------
          // setup of spontaneous radiation

	  if (element.compare("&sponrad")==0){
            SponRad* sponrad=new SponRad;
            if (!sponrad->init(rank,size,&argument,beam)){ break;}
	    delete sponrad;
            continue;  
          }  

          //----------------------------------------------------
          // setup wakefield

	  if (element.compare("&wake")==0){
	    Wake* wake = new Wake;
            if (!wake->init(rank,size,&argument,timewindow, setup, beam)){ break;}
	    delete wake;
	    continue;  
          }  

          //----------------------------------------------------
          // internal generation of beam

	  if (element.compare("&beam")==0){
            LoadBeam* loadbeam=new LoadBeam;
            if (!loadbeam->init(rank,size,&argument,beam,setup,timewindow,profile,lattice)){ break;}
	    delete loadbeam;
            continue;  
          }  

          //----------------------------------------------------
          // external generation of beam with an sdds file

	  if (element.compare("&importdistribution")==0){
            SDDSBeam* sddsbeam=new SDDSBeam;
            if (!sddsbeam->init(rank,size,&argument,beam,setup,timewindow,lattice)){ break;}
	    delete sddsbeam;
            continue;  
          }  

          //----------------------------------------------------
          // tracking - the very core part of Genesis

	  if (element.compare("&track")==0){
            Track* track=new Track;
	    if (!track->init(rank,size,&argument,beam,&field,setup,lattice,alt,timewindow,supressOutput)){ break;}
            delete track;
            continue;  
          }  


          //----------------------------------------------------
          // write beam, field or undulator to file

	  if (element.compare("&sort")==0){
	    beam->sort();
            continue;  
          }  


          //----------------------------------------------------
          // write beam, field or undulator to file

	  if (element.compare("&write")==0){
            Dump* dump=new Dump;
	    if (!dump->init(rank,size,&argument,setup,beam,&field)){ break;}
            delete dump;
            continue;  
          }  


          //----------------------------------------------------
          // import beam from a particle dump

	  if (element.compare("&importbeam")==0){
            ImportBeam* import=new ImportBeam;
	    if (!import->init(rank,size,&argument,beam,setup,timewindow)){ break;}
            delete import;
            continue;  
          }  


          //----------------------------------------------------
          // import field from a field dump

	  if (element.compare("&importfield")==0){
            ImportField* import=new ImportField;
	    if (!import->init(rank,size,&argument,&field,setup,timewindow)){ break;}
            delete import;
            continue;  
          }  



          //-----------------------------------------------------
          // error because the element typ is not defined

          if (rank==0){
            cout << "*** Error: Unknow element in input file: " << element << endl; 
	  }
          break;
        } 



	//-------------------------
	// get some data back
        if (supressOutput && split){
	  ret=field[0]->power[field[0]->power.size()-1];
	  cout << "Power: " << ret << endl;
	}


 	if (rank==0) {
          time(&timer);
          cout << endl<< "Program is terminating..." << endl;
	  cout << "Ending Time: " << ctime(&timer);
          cout << "-------------------------------------" << endl;

        }



        return ret;

}
