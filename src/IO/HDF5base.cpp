#include "HDF5base.h"

HDF5Base::HDF5Base(){}
HDF5Base::~HDF5Base(){}

//-------------------------------
// write of a single node in parallal HDF5




void HDF5Base::writeDouble1DExist(hsize_t datsize, double *data, hid_t gid, string dataset)
{

  int dataset_rank=1;
  hsize_t dims[1] = {datsize}; // dataset size must be the same for all nodes

  hid_t did=H5Dopen(gid,dataset.c_str(),H5P_DEFAULT);

  hid_t filespace=H5Dget_space(did);
  hid_t memspace=H5Screate_simple(dataset_rank,dims,NULL);


  // set up transfer and write
  hid_t pid =  H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(pid,H5FD_MPIO_INDEPENDENT);    
  H5Dwrite(did,H5T_NATIVE_DOUBLE,memspace,filespace,pid,data);
 
  // close all HDF5 stuff except for the file id fid
  H5Dclose(did);
  H5Sclose(memspace);
  H5Sclose(filespace);
  H5Pclose(pid);


  return;
}





void HDF5Base::writeChar1D(hsize_t reclen, hsize_t datsize, const char *data, hid_t gid, string name)
{

  int dataset_rank=1;
  hsize_t dims[1] = {datsize}; // dataset size must be the same for all nodes
  hid_t filespace = H5Screate_simple(dataset_rank,dims,NULL);

  hid_t did=H5Dcreate(gid,name.c_str(),H5T_NATIVE_CHAR,filespace,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);  
  H5Sclose(filespace);

 

  hsize_t count[1] = {reclen};     // length of record entry 
  hsize_t offset[1] = {0};   // offset of record entry
  hid_t pid =  H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(pid,H5FD_MPIO_COLLECTIVE);    

  hid_t memspace=H5Screate_simple(dataset_rank,count,NULL);
  if (reclen==0) {H5Sselect_none(memspace);}

  
  filespace= H5Dget_space(did);
  H5Sselect_hyperslab(filespace,H5S_SELECT_SET,offset,NULL,count,NULL);
  if (reclen==0){ H5Sselect_none(filespace);  }

  if (reclen==0) {
      H5Dwrite(did,H5T_NATIVE_CHAR,memspace,filespace,pid,NULL);
  } else { 
      H5Dwrite(did,H5T_NATIVE_CHAR,memspace,filespace,pid,data);
  }
  

  H5Dclose(did);
  H5Pclose(pid);
  H5Sclose(memspace);
  H5Sclose(filespace);

  return;
}


void HDF5Base::writeDouble1D(hsize_t reclen, hsize_t datsize, double *data, hid_t gid, string name)
{

  int dataset_rank=1;
  hsize_t dims[1] = {datsize}; // dataset size must be the same for all nodes
  hid_t filespace = H5Screate_simple(dataset_rank,dims,NULL);

  hid_t did=H5Dcreate(gid,name.c_str(),H5T_NATIVE_DOUBLE,filespace,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);  
  H5Sclose(filespace);

 

  hsize_t count[1] = {reclen};     // length of record entry 
  hsize_t offset[1] = {0};   // offset of record entry
  hid_t pid =  H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(pid,H5FD_MPIO_COLLECTIVE);    

  hid_t memspace=H5Screate_simple(dataset_rank,count,NULL);
  if (reclen==0) {H5Sselect_none(memspace);}

  
  filespace= H5Dget_space(did);
  H5Sselect_hyperslab(filespace,H5S_SELECT_SET,offset,NULL,count,NULL);
  if (reclen==0){ H5Sselect_none(filespace);  }

  if (reclen==0) {
      H5Dwrite(did,H5T_NATIVE_DOUBLE,memspace,filespace,pid,NULL);
  } else { 
      H5Dwrite(did,H5T_NATIVE_DOUBLE,memspace,filespace,pid,data);
  }
  

  H5Dclose(did);
  H5Pclose(pid);
  H5Sclose(memspace);
  H5Sclose(filespace);

  return;
}

void HDF5Base::writeInt1D(hsize_t reclen, hsize_t  datsize, int *data, hid_t gid, string name)
{

  int dataset_rank=1;
  hsize_t dims[1] = {datsize}; // dataset size must be the same for all nodes
  hid_t filespace = H5Screate_simple(dataset_rank,dims,NULL);

  hid_t did=H5Dcreate(gid,name.c_str(),H5T_NATIVE_INT,filespace,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);  
  H5Sclose(filespace);

 

  hsize_t count[1] = {reclen};     // length of record entry 
  hsize_t offset[1] = {0};   // offset of record entry
  hid_t pid =  H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(pid,H5FD_MPIO_COLLECTIVE);    

  hid_t memspace=H5Screate_simple(dataset_rank,count,NULL);
  if (reclen==0) {H5Sselect_none(memspace);}

  
  filespace= H5Dget_space(did);
  H5Sselect_hyperslab(filespace,H5S_SELECT_SET,offset,NULL,count,NULL);
  if (reclen==0){ H5Sselect_none(filespace);  }

  if (reclen==0) {
      H5Dwrite(did,H5T_NATIVE_INT,memspace,filespace,pid,NULL);
  } else { 
      H5Dwrite(did,H5T_NATIVE_INT,memspace,filespace,pid,data);
  }
  

  H5Dclose(did);
  H5Pclose(pid);
  H5Sclose(memspace);
  H5Sclose(filespace);

  return;
}



//----------------------
// generating expandable dataset

void HDF5Base::createExpDataset(hid_t fid, char *name, hsize_t nz, hsize_t ns)
{
  hsize_t dims[2] = {nz, ns};
  hsize_t maxdims[2]={H5S_UNLIMITED, H5S_UNLIMITED};
  hid_t dataspace = H5Screate_simple(2,dims,maxdims);
  hid_t cparms=H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunk_dims[2]={10,10};
  herr_t status=H5Pset_chunk( cparms, 2, chunk_dims);
  hid_t dataset=H5Dcreate(fid,name,H5T_NATIVE_DOUBLE,dataspace,H5P_DEFAULT,cparms,H5P_DEFAULT);

  H5Dclose(dataset);
  H5Sclose(dataspace);
  
  return;

}

void HDF5Base::expandDataset(hid_t fid, vector<double> *rec, int pos, hsize_t recsize, hsize_t slice, char *name)
{

  double *data= new double[recsize];
  
  int stride=rec->size()/recsize;
  for(int i=0;i<recsize;i++){
    data[i]=rec->at(i*stride+pos);
    //    cout << data[i] << endl;
  }
  //  return;

  hid_t did=H5Dopen(fid,name,H5P_DEFAULT);
  hsize_t size[2] = {recsize, slice};  
  herr_t status=H5Dset_extent(did,size); 

  hid_t filespace=H5Dget_space(did);  
  hsize_t offset[2] = {0,slice-1};
  hsize_t dims[2]={recsize,1};
  hid_t dataspace=H5Screate_simple(2,dims,dims);  

  status=H5Sselect_hyperslab(filespace,H5S_SELECT_SET,offset,NULL,dims,NULL);
  status=H5Dwrite(did,H5T_NATIVE_DOUBLE,dataspace,filespace,H5P_DEFAULT,data);
  
  status=H5Sclose(dataspace);
  status=H5Sclose(filespace);
  status=H5Dclose(did);

  delete [] data;
  return;
}


//------------------------
// writing procedures

void HDF5Base::writeDataDouble(hid_t fid, const char *name, const double *data, int size)
{
  hsize_t dims[1];
  dims[0]=size;
  hid_t dataspace_id=H5Screate_simple(1,dims,NULL);
  hid_t dataset_id=H5Dcreate(fid,name,H5T_NATIVE_DOUBLE,dataspace_id,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
  H5Dwrite(dataset_id,H5T_NATIVE_DOUBLE,H5S_ALL,H5S_ALL,H5P_DEFAULT,data);
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
}

void HDF5Base::writeDataChar(hid_t fid, const char *name, const char *data, int size)
{
  hsize_t dims[1];
  dims[0]=size;
  hid_t dataspace_id=H5Screate_simple(1,dims,NULL);
  hid_t dataset_id=H5Dcreate(fid,name,H5T_NATIVE_CHAR,dataspace_id,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
  H5Dwrite(dataset_id,H5T_NATIVE_CHAR,H5S_ALL,H5S_ALL,H5P_DEFAULT,data);
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
}



void HDF5Base::writeDataInt(hid_t fid, const char *name, const int *data, int size)
{
  hsize_t dims[1];
  dims[0]=size;
  hid_t dataspace_id=H5Screate_simple(1,dims,NULL);
  hid_t dataset_id=H5Dcreate(fid,name,H5T_NATIVE_INT,dataspace_id,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
  H5Dwrite(dataset_id,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,data);
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
}

//--------------------- 
// reading procedures


void HDF5Base::readDouble1D(hid_t fid, const char *name, double *data, hsize_t datsize, hsize_t start)
{



  int dataset_rank=1;

  hsize_t count[1] = {datsize};     // length of record entry 
  hsize_t offset[1] = {start};   // offset of record entry


  hid_t did=H5Dopen(fid,name,H5P_DEFAULT);

  hid_t filespace=H5Dget_space(did);
  H5Sselect_hyperslab(filespace,H5S_SELECT_SET,offset,NULL,count,NULL);
  hid_t memspace=H5Screate_simple(dataset_rank,count,NULL);


  hid_t pid =  H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(pid,H5FD_MPIO_COLLECTIVE);    


  H5Dread(did,H5T_NATIVE_DOUBLE,memspace,filespace,pid,data);

 
  // close all HDF5 stuff except for the file id fid
  H5Dclose(did);
  H5Sclose(memspace);
  H5Sclose(filespace);
  H5Pclose(pid);

  return;
}


void HDF5Base::readDataDouble(hid_t fid, char *name, double *data, int size)
{

  hsize_t dims[1];
  dims[0]=size;
  hid_t dataspace_id=H5Screate_simple(1,dims,NULL);
  hid_t dataset_id=H5Dopen(fid,name,H5P_DEFAULT);
  hid_t plist_id=H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(plist_id,H5FD_MPIO_INDEPENDENT);

  H5Dread(dataset_id,H5T_NATIVE_DOUBLE,H5S_ALL,H5S_ALL,plist_id,data);
  H5Dclose(dataset_id);     
  H5Sclose(dataspace_id);
  H5Pclose(plist_id);
  return;
}

void HDF5Base::readDataChar(hid_t fid, char *name, char *data, int size)
{

  hsize_t dims[1];
  dims[0]=size;
  hid_t dataspace_id=H5Screate_simple(1,dims,NULL);
  hid_t dataset_id=H5Dopen(fid,name,H5P_DEFAULT);
  hid_t plist_id=H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(plist_id,H5FD_MPIO_INDEPENDENT);

  H5Dread(dataset_id,H5T_NATIVE_CHAR,H5S_ALL,H5S_ALL,plist_id,data);
  H5Dclose(dataset_id);     
  H5Sclose(dataspace_id);
  H5Pclose(plist_id);

  return;
}

void HDF5Base::readDataInt(hid_t fid, char *name, int *data, int size)
{
  hsize_t dims[1];
  dims[0]=size;
  hid_t dataspace_id=H5Screate_simple(1,dims,NULL);
  hid_t dataset_id=H5Dopen(fid,name,H5P_DEFAULT);
  hid_t plist_id=H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(plist_id,H5FD_MPIO_INDEPENDENT);

  H5Dread(dataset_id,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,plist_id,data);
  H5Dclose(dataset_id);     
  H5Sclose(dataspace_id);
  H5Pclose(plist_id);
  return;
}


int HDF5Base::getDatasetSize(hid_t fid, char *name)
{

  hsize_t dims[1],maxdims[1];
  hid_t  dsid=H5Dopen(fid,name,H5P_DEFAULT);
  hid_t spaceid=H5Dget_space(dsid);
  H5Sget_simple_extent_dims(spaceid,dims,maxdims);
  H5Dclose(dsid);
  return dims[0];

}


//------------------------
// simple read functions
bool HDF5Base::simpleReadDouble1D(const string &path, vector<double> *data){
  vector<string> ele;
  stringstream ss(path);
  string file;
  string group;
  char delim='/';             // does not compile it I use double quotation marks
  if (getline(ss,file,delim)){
     if (!getline(ss,group)){
       return false;
     }
  } else {
    return false;
  }
  

  hid_t fid=H5Fopen(file.c_str(),H5F_ACC_RDONLY,H5P_DEFAULT);
  int nsize=this->getDatasetSize(fid,(char *)group.c_str());
  
  data->resize(nsize);
  this->readDataDouble(fid, (char *)group.c_str(), &data->at(0), nsize);
  H5Fclose(fid);
 
  return true;
}


//------------------------------
// utility functions

bool HDF5Base::checkForLink(hid_t fid, string name){

  htri_t result=H5Lexists(fid,name.c_str(),H5P_DEFAULT);
  if (result==true){
    return true;
  } 
  return false;
}

 
