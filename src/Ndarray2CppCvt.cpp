//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class Ndarray2CppCvt...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/Ndarray2CppCvt.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/make_shared.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MsgLogger/MsgLogger.h"
#include "ndarray/ndarray.h"
#include "psddl_python/psddl_python_numpy.h"
#include "PSEvt/DataProxy.h"
#include "pytools/make_pyshared.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  template <typename T, unsigned Rank>
  void makeAndSave(const boost::shared_ptr<T>& data, const unsigned shape[], const int strides[],
      PSEvt::ProxyDictI& proxyDict, const Pds::Src& source, const std::string& key, bool modifiable)
  {

    boost::shared_ptr<PSEvt::ProxyI> proxyPtr;
    const std::type_info* tinfo = 0;
    if (modifiable) {

      typedef ndarray<T, Rank> ArrayType;
      boost::shared_ptr<ArrayType> parray = boost::make_shared<ArrayType>(data, shape);
      parray->strides(strides);

      proxyPtr = boost::make_shared<PSEvt::DataProxy<ArrayType> >(parray);
      tinfo = &typeid(const ArrayType);

    } else {

      typedef ndarray<const T, Rank> ArrayType;
      boost::shared_ptr<ArrayType> parray = boost::make_shared<ArrayType>(data, shape);
      parray->strides(strides);

      proxyPtr = boost::make_shared<PSEvt::DataProxy<ArrayType> >(parray);
      tinfo = &typeid(const ArrayType);
    }
    // this may throw
    PSEvt::EventKey evKey = PSEvt::EventKey(tinfo, source, key);
    proxyDict.put(proxyPtr, evKey);
  }

  template <typename T>
  bool makeAndSave(int rank, pytools::pyshared_ptr shndarr, const unsigned shape[], const int strides[],
      PSEvt::ProxyDictI& proxyDict, const Pds::Src& source, const std::string& key, bool modifiable)
  {
    boost::shared_ptr<T> dataptr(shndarr, static_cast<T*>(PyArray_DATA(shndarr.get())));

    bool res = true;
    switch (rank) {
    case 1:
      makeAndSave<T, 1>(dataptr, shape, strides, proxyDict, source, key, modifiable);
      break;
    case 2:
      makeAndSave<T, 2>(dataptr, shape, strides, proxyDict, source, key, modifiable);
      break;
    case 3:
      makeAndSave<T, 3>(dataptr, shape, strides, proxyDict, source, key, modifiable);
      break;
    case 4:
      makeAndSave<T, 4>(dataptr, shape, strides, proxyDict, source, key, modifiable);
      break;
    case 5:
      makeAndSave<T, 5>(dataptr, shape, strides, proxyDict, source, key, modifiable);
      break;
    case 6:
      makeAndSave<T, 6>(dataptr, shape, strides, proxyDict, source, key, modifiable);
      break;
    default:
      res = false;
    }
    return res;
  }
}


//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace psana_python {

//----------------
// Constructors --
//----------------
Ndarray2CppCvt::Ndarray2CppCvt ()
  : Converter()
{
}

//--------------
// Destructor --
//--------------
Ndarray2CppCvt::~Ndarray2CppCvt ()
{
}

// Return type_infos of source C++ types.
std::vector<const std::type_info*>
Ndarray2CppCvt::from_cpp_types() const
{
  return std::vector<const std::type_info*>();
}

// Returns source Python types.
std::vector<PyTypeObject*>
Ndarray2CppCvt::from_py_types() const
{
  // we accept numpy.ndarray as input
  return std::vector<PyTypeObject*>(1, &PyArray_Type);
}

// Returns destination Python types.
std::vector<PyTypeObject*>
Ndarray2CppCvt::to_py_types() const
{
  return std::vector<PyTypeObject*>();
}

// Convert C++ object to Python
PyObject*
Ndarray2CppCvt::convert(PSEvt::ProxyDictI& proxyDict, const PSEvt::Source& source, const std::string& key) const
{
  return 0;
}

// Convert Python object to C++
bool
Ndarray2CppCvt::convert(PyObject* obj, PSEvt::ProxyDictI& proxyDict, const Pds::Src& source, const std::string& key) const
{
  // must be numpy array
  if (not PyArray_Check(obj)) return 0;

  // make shared pointer, do not steal
  pytools::pyshared_ptr shndarr = pytools::make_pyshared(obj, false);

  // dimensions
  const int rank = PyArray_NDIM(obj);
  const int itemsize = PyArray_ITEMSIZE(obj);
  unsigned shape[rank];
  int strides[rank];
  for (int i = 0; i != rank; ++ i) {
    shape[i] = PyArray_DIM(obj, i);
    strides[i] = PyArray_STRIDE(obj, i) / itemsize; // numpy strides are in bytes
  }

  bool modifiable = PyArray_CHKFLAGS(obj, NPY_WRITEABLE);

  bool result = true;
  switch (PyArray_TYPE(obj)) {
  case NPY_INT8:
    result = makeAndSave<int8_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_UINT8:
    result = makeAndSave<uint8_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_INT16:
    result = makeAndSave<int16_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_UINT16:
    result = makeAndSave<uint16_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_INT32:
    result = makeAndSave<int32_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_UINT32:
    result = makeAndSave<uint32_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_INT64:
    result = makeAndSave<int64_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_UINT64:
    result = makeAndSave<uint64_t>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_FLOAT32:
    result = makeAndSave<float>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  case NPY_FLOAT64:
    result = makeAndSave<double>(rank, shndarr, shape, strides, proxyDict, source, key, modifiable);
    break;
  default:
    result = false;
    break;
  }

  return result;
}

} // namespace psana_python
