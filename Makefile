#
# GENESIS makefile
#
# libraries
#
LIB= -lm -lstdc++ -lmpi_cxx  
#
INCLUDE=-I./include
#
# cpp-macros
#
DMACRO = -DMPE -g
#
#  compilers
#
VPATH = src/Core src/IO src/Lattice src/Util src/Main src/Loading
CCOMPILER = h5pcc
#CCOMPILER = vtcxx -vt:cxx h5pcc -vt:inst manual -DVTRACE
#
#  executable name 
#
EXECUTABLE = gencore
#
# targets
#
OBJECTS = Sorting.o BesselJ.o Inverfc.o Hammerslay.o RandomU.o GaussHermite.o StringProcessing.o Track.o Setup.o AlterSetup.o Time.o Wake.o Parser.o Dump.o SponRad.o EField.o LoadBeam.o ImportBeam.o LoadField.o ImportField.o Profile.o ShotNoise.o QuietLoading.o Optics.o Lattice.o LatticeElements.o LatticeParser.o AlterLattice.o Gencore.o TrackBeam.o Control.o Field.o FieldSolver.o EFieldSolver.o Incoherent.o Collective.o Beam.o Undulator.o HDF5base.o readBeamHDF5.o writeBeamHDF5.o readFieldHDF5.o writeFieldHDF5.o SDDSBeam.o Output.o  GenMain.o 

genesis: $(OBJECTS) BeamSolver.o
	ar -cvq libgenesis13.a $(OBJECTS) BeamSolver.o
	mv libgenesis13.a ./lib
	$(CCOMPILER) src/Main/mainwrap.cpp -o $(EXECUTABLE) $(INCLUDE) $(LIB) -lgenesis13 -Llib

genesis-cuda: $(OBJECTS) CudaBeamSolver.o
	ar -cvq libgenesis13.a $(OBJECTS) CudaBeamSolver.o
	mv libgenesis13.a ./lib
	$(CCOMPILER) src/Main/mainwrap.cpp -o $(EXECUTABLE) $(INCLUDE) $(LIB) -g -DUSE_CUDA -lgenesis13 -lcuda -lcudart -Llib

CudaBeamSolver.o:
	nvcc -O2 -c $(DMACRO) -DUSE_CUDA $(INCLUDE) src/Core/CudaBeamSolver.cu \
		-gencode=arch=compute_52,code=sm_52 \
 		-gencode=arch=compute_60,code=sm_60 \
 		-gencode=arch=compute_61,code=sm_61 \
 		-gencode=arch=compute_61,code=compute_61 \
	   	-I/usr/include/hdf5/openmpi \
		-I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi \
		-I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/opal/mca/event/libevent2022/libevent\
	   	-I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/opal/mca/event/libevent2022/libevent/include\
	   	-I/usr/lib/x86_64-linux-gnu/openmpi/include\
	   	-Xcompiler="-pthread"

.cpp.o:
	$(CCOMPILER) -O2 -c $(DMACRO)  $(INCLUDE) $< -DUSE_CUDA

clean:
	rm -f src/Core/*~
	rm -f src/IO/*~
	rm -f src/Lattice/*~
	rm -f src/Loading/*~
	rm -f src/Main/*~
	rm -f src/Util/*~
	rm -f include/*~
	rm -f *.o
	rm -f lib/*.a

install:
	cp ./$(EXECUTABLE) ~/bin/genesis4

beta:
	cp ./$(EXECUTABLE) ~/bin/$(EXECUTABLE).beta

