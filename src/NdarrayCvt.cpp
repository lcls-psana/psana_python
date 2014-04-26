//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class NdarrayCvt...
//
// Author List:
//      Andy Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/NdarrayCvt.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <stdexcept>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/preprocessor/seq/for_each_product.hpp>
#include <boost/preprocessor/seq/elem.hpp>
//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MsgLogger/MsgLogger.h"
#include "ndarray/ndarray.h"
#include "psddl_python/ConverterMap.h"
#include "psddl_python/psddl_python_numpy.h"
#include "pytools/PyDataType.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

// set of ranks and types for which we instantiate converters
// if you extend ND_TYPES macro also add specialization of Traits struct below
#define ND_RANKS (1)(2)(3)(4)(5)(6)
#define ND_TYPES (int8_t)(uint8_t)(int16_t)(uint16_t)(int32_t)(uint32_t)(int64_t)(uint64_t)(float)(double)

namespace {

  const char logger[] = "psana_python.NdarrayCvt";


  // type traits for selected set of C++ types that we support as
  // elements of ndarrays
  template <typename T> struct Traits {};
  template <> struct Traits<int8_t> {
    static const char* typeName() { return "int8"; }
    static int numpyType() { return NPY_INT8; }
  };
  template <> struct Traits<uint8_t> {
    static const char* typeName() { return "uint8"; }
    static int numpyType() { return NPY_UINT8; }
  };
  template <> struct Traits<int16_t> {
    static const char* typeName() { return "int16"; }
    static int numpyType() { return NPY_INT16; }
  };
  template <> struct Traits<uint16_t> {
    static const char* typeName() { return "uint16"; }
    static int numpyType() { return NPY_UINT16; }
  };
  template <> struct Traits<int32_t> {
    static const char* typeName() { return "int32"; }
    static int numpyType() { return NPY_INT32; }
  };
  template <> struct Traits<uint32_t> {
    static const char* typeName() { return "uint32"; }
    static int numpyType() { return NPY_UINT32; }
  };
  template <> struct Traits<int64_t> {
    static const char* typeName() { return "int64"; }
    static int numpyType() { return NPY_INT64; }
  };
  template <> struct Traits<uint64_t> {
    static const char* typeName() { return "uint64"; }
    static int numpyType() { return NPY_UINT64; }
  };
  template <> struct Traits<float> {
    static const char* typeName() { return "float32"; }
    static int numpyType() { return NPY_FLOAT32; }
  };
  template <> struct Traits<double> {
    static const char* typeName() { return "float64"; }
    static int numpyType() { return NPY_FLOAT64; }
  };

  // Type for Python objects that will hold C++ ndarray objects
  template <typename T, unsigned Rank>
  class NdarrayWrapper : public pytools::PyDataType<NdarrayWrapper<T, Rank>, ndarray<const T, Rank> > {
  public:

    typedef pytools::PyDataType<NdarrayWrapper, ndarray<const T, Rank> > BaseType;

    /// Initialize Python type and register it in a module
    static void initType( PyObject* module ) {

      static char typedoc[] = "Special Python type which wraps C++ ndarray. "
          "The instances of this type are not used directly, but the type itself is used as "
          "an argument for Event.get() method.";

      static std::string name = std::string("ndarray_") + Traits<T>::typeName() + "_" + boost::lexical_cast<std::string>(Rank);

      PyTypeObject* type = BaseType::typeObject() ;
      type->tp_doc = typedoc;

      BaseType::initType(name.c_str(), module, "psana");
    }

    // Dump object info to a stream
    void print(std::ostream& out) const {
      out << this->m_obj;
    }

  };

  // REturns true if strides correspond to C memmory layout
  template <unsigned Rank>
  bool isCArray(const unsigned shape[], const int strides[]) {
    int stride = 1;
    for (int i = Rank; i > 0; -- i) {
      if (strides[i-1] != stride) return false;
      stride *= shape[i-1];
    }
    return true;
  }

  template<typename T, unsigned Rank>
  std::string BothConstAndNonConstNdarrayMsg(const PSEvt::Source &source, 
                                             const std::string &key) {
    std::ostringstream msg;
    msg << "Both const and non-const element type found for ndarray<"
          << Traits<T>::typeName() << "," << Rank << "> for event key with source=" 
          << source << " and key=" << key 
          << " convert is ambiguous. Use key string to distingish.";
    return msg.str();
  }

}



//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace psana_python {

//----------------
// Constructors --
//----------------
template <typename T, unsigned Rank>
NdarrayCvt<T, Rank>::NdarrayCvt (PyObject* module)
  : Converter()
{
}

//--------------
// Destructor --
//--------------
template <typename T, unsigned Rank>
NdarrayCvt<T, Rank>::~NdarrayCvt ()
{
}

/// Return type_info of the corresponding C++ type.
template <typename T, unsigned Rank>
std::vector<const std::type_info*>
NdarrayCvt<T, Rank>::from_cpp_types() const
{
  const std::type_info *types[2] = {&typeid(ndarray<T, Rank>), 
                                    &typeid(ndarray<const T, Rank>)};
  return std::vector<const std::type_info*>(types,types+2);
}

/// Returns source Python types.
template <typename T, unsigned Rank>
std::vector<PyTypeObject*>
NdarrayCvt<T, Rank>::from_py_types() const
{
  // we accept numpy.ndarray as input
  return std::vector<PyTypeObject*>();
}

/// Returns destination Python types.
template <typename T, unsigned Rank>
std::vector<PyTypeObject*>
NdarrayCvt<T, Rank>::to_py_types() const
{
  std::vector<PyTypeObject*> res;
  res.push_back(NdarrayWrapper<T, Rank>::typeObject());
  res.push_back(&PyArray_Type);
  return res;
}


/// Convert C++ object to Python
template <typename T, unsigned Rank>
PyObject*
NdarrayCvt<T, Rank>::convert(PSEvt::ProxyDictI& proxyDict, const PSEvt::Source& source, const std::string& key) const
{
  typedef ndarray<T, Rank> ArrType;
  typedef ndarray<const T, Rank> ConstArrType;

  // item size
  const size_t itemsize = sizeof(T);

  // NumPy type number
  const int typenum = Traits<T>::numpyType();

  // dimensions and strides, numpy strides are in bytes
  npy_intp dims[Rank], strides[Rank];

  void* data = 0;
  int flags = 0;
  PyObject* base = 0;

  if (boost::shared_ptr<void> vdata = proxyDict.get(&typeid(ArrType), source, key, 0)) {
    if (proxyDict.get(&typeid(ConstArrType), source, key, 0)) {
      throw std::runtime_error(BothConstAndNonConstNdarrayMsg<T,Rank>(source, key));
    }
    const ArrType& arr = *boost::static_pointer_cast<ArrType>(vdata);

    std::copy(arr.shape(), arr.shape()+Rank, dims);
    for (unsigned i = 0; i != Rank; ++ i) {
      strides[i] = arr.strides()[i] * itemsize;
    }

    // set all flags
    flags |= NPY_WRITEABLE;
    if (::isCArray<Rank>(arr.shape(), arr.strides())) {
      flags |= NPY_C_CONTIGUOUS;
    }
    if (reinterpret_cast<size_t>(arr.data()) % itemsize == 0) {
      flags |= NPY_ALIGNED;
    }

    data = (void*)(arr.data());
    base = NdarrayWrapper<T, Rank>::PyObject_FromCpp(ConstArrType(arr));

  } else if (boost::shared_ptr<void> vdata = proxyDict.get(&typeid(ConstArrType), source, key, 0)) {
    if (proxyDict.get(&typeid(ArrType), source, key, 0)) {
      throw std::runtime_error(BothConstAndNonConstNdarrayMsg<T,Rank>(source, key));
    }

    const ConstArrType& arr = *boost::static_pointer_cast<ConstArrType>(vdata);

    std::copy(arr.shape(), arr.shape()+Rank, dims);
    for (unsigned i = 0; i != Rank; ++ i) {
      strides[i] = arr.strides()[i] * itemsize;
    }

    // set all flags
    if (::isCArray<Rank>(arr.shape(), arr.strides())) {
      flags |= NPY_C_CONTIGUOUS;
    }
    if (reinterpret_cast<size_t>(arr.data()) % itemsize == 0) {
      flags |= NPY_ALIGNED;
    }

    data = (void*)(arr.data());
    base = NdarrayWrapper<T, Rank>::PyObject_FromCpp(arr);

  } else {

    return 0;

  }

  // now make an instance of numpy.ndarray
  PyObject* array = PyArray_New(&PyArray_Type, Rank, dims, typenum, strides,
                                data, itemsize, flags, 0);

  // array does not own its data, create an instance which handles lifetime of the array
  PyArrayObject* oarray = (PyArrayObject*)array;
  oarray->base = base;

  return array;
}

/*
 *  Method that registers converters for all supported types, this will also
 *  create all necessary data types.
 */
void initNdarrayCvt(psddl_python::ConverterMap& cmap, PyObject* module)
{
#define INST_CVT(r, PRODUCT) \
  ::NdarrayWrapper<BOOST_PP_SEQ_ELEM(0, PRODUCT), BOOST_PP_SEQ_ELEM(1, PRODUCT)>::initType(module); \
  cmap.addConverter(boost::make_shared<NdarrayCvt<BOOST_PP_SEQ_ELEM(0, PRODUCT), BOOST_PP_SEQ_ELEM(1, PRODUCT)> >(module));

  BOOST_PP_SEQ_FOR_EACH_PRODUCT(INST_CVT, (ND_TYPES)(ND_RANKS))
}

} // namespace psana_python
