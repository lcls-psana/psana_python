#ifndef PSANA_PYTHON_PYTHON_CONVERTER_H
#define PSANA_PYTHON_PYTHON_CONVERTER_H

#include <boost/python.hpp>
#include <ndarray/ndarray.h>
#include <list>


namespace psana_python {
 
  // Function to create all the converters
  void createConverters();
  
  // Converter for NDArray to Numpy for BOOST
  template <typename T, unsigned Rank> struct NDArrayToNumpy 
  {
    // Converts NDArray to Numpy 
    static PyObject* convert(ndarray<T,Rank> const& array);

    // Registers NDArray to Numpy converter with BOOST
    static void register_ndarray_to_numpy_cvt();    
  };
  
  
  // Converter for Numpy to NDArray for BOOST
  template <typename T, unsigned Rank> struct NumpyToNDArray
  {
    // Registers Numpy to NDArray converter with BOOST
    NumpyToNDArray& from_python();

    // Check incoing PYTHON object can be converted to NDArray
    static void* convertible(PyObject* obj);
    
    // Converts Numpy to NDArray 
    typedef boost::python::converter::rvalue_from_python_stage1_data BoostData;
    static void  construct(PyObject* obj, BoostData* boostData);        
  };

  
  // Converter for STL list<ndtypes> to Numpy for BOOST
  // where ndtypes is defined by the macro ND_TYPES (see source file)
  template <typename T> struct StlListToNumpy
  {
    // Converts STL list<number> to Numpy for boost
    static PyObject* convert(std::list<T> const& list);

    // Register STL list<number> to Numpy converter with BOOST
    static void register_stllist_to_numpy_cvt();
  };
  
}

#endif // PSANA_PYTHON_PYTHON_CONVERTER_H
