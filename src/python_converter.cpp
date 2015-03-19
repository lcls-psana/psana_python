#include <psana_python/python_converter.h>

// Check APIs depricated in current version (NUMPY 1.7) are used
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define PY_ARRAY_UNIQUE_SYMBOL PSALG_NUMPY_NDARRAY_CONVERTER
#include <numpy/arrayobject.h>

#include <boost/preprocessor/seq/for_each_product.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <pytools/PyDataType.h>

#include <PSEvt/Event.h>
#include <psana_python/Event.h>

#include <PSEnv/Env.h>
#include <psana_python/Env.h>

#include <PSEvt/Source.h>
#include <psana_python/Source.h>


#include <string>
#include <iostream>
#include "MsgLogger/MsgLogger.h"


// set of ranks and types for which we instantiate converters
#define ND_RANKS (1)(2)(3)(4)(5)(6)
#define ND_TYPES (int8_t)(uint8_t)(int16_t)(uint16_t)(int32_t)(uint32_t)(int64_t)(uint64_t)(float)(double)
#define CONST_ND_TYPES (const int8_t)(const uint8_t)(const int16_t)(const uint16_t)(const int32_t)(const uint32_t)(const int64_t)(const uint64_t)(const float)(const double)
 

namespace psana_python {

  // String to identify debug statements produced by this code
  const char *pyConverterlogger = "Python_Converter";
  
  void createConverters() {
    
    // Initialise NUMPY 
    import_array();

    // These templates and BOOST preprocessor macros can be hard to understand what's
    // going on.  Here's a couple of examples of what they trying to do....
    //
    // Let's say we want converters for doubles NDarrays for 1-3 dimensions   
    // Without the BOOST macros above we would have to do this:
    // 
    // boost::python::to_python_converter<ndarray<double,1>,NDArrayToNumpy<double,1> >();
    // boost::python::to_python_converter<ndarray<double,2>,NDArrayToNumpy<double,2> >();
    // boost::python::to_python_converter<ndarray<double,3>,NDArrayToNumpy<double,3> >();
    //  
    // NumpyToNDArray<float,1>().from_python();
    // NumpyToNDArray<float,2>().from_python();
    // NumpyToNDArray<float,3>().from_python();
    // 
    // Rather than typing each out each coverter, we used the BOOST
    // preprosser macros to generate the code for us.
    
    
    // Preprocessor macro to define the call to create a NDArray to NUMPY converter. 
    #define REGISTER_NDARRAY_TO_NUMPY_CONVERTER(r,PRODUCT)			\
      psana_python::NDArrayToNumpy<BOOST_PP_SEQ_ELEM(0,PRODUCT),BOOST_PP_SEQ_ELEM(1,PRODUCT)>().register_ndarray_to_numpy_cvt();


    // Preprocessor macro to define the call to create a NUMPY to NDArray converter 
    #define REGISTER_NUMPY_TO_NDARRAY_CONVERTER(r,PRODUCT)			\
      psana_python::NumpyToNDArray<BOOST_PP_SEQ_ELEM(0,PRODUCT),BOOST_PP_SEQ_ELEM(1,PRODUCT)>().from_python();
  

    // BOOST preprocessor macro to create converters 1-6 dimensional NDArray, with
    // data type from int to double, both const and non const  
    BOOST_PP_SEQ_FOR_EACH_PRODUCT(REGISTER_NDARRAY_TO_NUMPY_CONVERTER,(ND_TYPES)(ND_RANKS))
    BOOST_PP_SEQ_FOR_EACH_PRODUCT(REGISTER_NDARRAY_TO_NUMPY_CONVERTER,(CONST_ND_TYPES)(ND_RANKS))
      
    BOOST_PP_SEQ_FOR_EACH_PRODUCT(REGISTER_NUMPY_TO_NDARRAY_CONVERTER,(ND_TYPES)(ND_RANKS))
    BOOST_PP_SEQ_FOR_EACH_PRODUCT(REGISTER_NUMPY_TO_NDARRAY_CONVERTER,(CONST_ND_TYPES)(ND_RANKS))


    // Register the STL list<ndtypes> to Numpy converters with BOOST
    #define REGISTER_STLLIST_TO_NUMPY_CONVERTER(r,data, ELEMENT)		\
      psana_python::StlListToNumpy< ELEMENT >().register_stllist_to_numpy_cvt();

    BOOST_PP_SEQ_FOR_EACH(REGISTER_STLLIST_TO_NUMPY_CONVERTER,BOOST_PP_EMPTY(),ND_TYPES)

    // Register Python-Event to Event converter  
    psana_python::PyEvtToEvt().from_python();
    
    // Register Python-Env to Env converter  
    psana_python::PyEnvToEnv().from_python();

    // Register Python-Source to Source converter  
    psana_python::PySourceToSource().from_python();
    
    return;
  }


  namespace {

    // type traits for selected set of C++ types that we support as
    // elements of ndarrays
    template <typename T> struct Traits {};
  
    // ===> SHOULD BE ABLE TO REPLACE THIS WITH BOOST MACROS 
  
    // non-const data types
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
    
    
    // const data types
    template <> struct Traits<const int8_t> {
      static const char* typeName() { return "int8"; }
      static int numpyType() { return NPY_INT8; }
    };
    template <> struct Traits<const uint8_t> {
      static const char* typeName() { return "uint8"; }
      static int numpyType() { return NPY_UINT8; }
    };
    template <> struct Traits<const int16_t> {
      static const char* typeName() { return "int16"; }
      static int numpyType() { return NPY_INT16; }
    };
    template <> struct Traits<const uint16_t> {
      static const char* typeName() { return "uint16"; }
      static int numpyType() { return NPY_UINT16; }
    };
    template <> struct Traits<const int32_t> {
      static const char* typeName() { return "int32"; }
      static int numpyType() { return NPY_INT32; }
    };
    template <> struct Traits<const uint32_t> {
      static const char* typeName() { return "uint32"; }
      static int numpyType() { return NPY_UINT32; }
    };
    template <> struct Traits<const int64_t> {
      static const char* typeName() { return "int64"; }
      static int numpyType() { return NPY_INT64; }
    };
    template <> struct Traits<const uint64_t> {
      static const char* typeName() { return "uint64"; }
      static int numpyType() { return NPY_UINT64; }
    };
    template <> struct Traits<const float> {
      static const char* typeName() { return "float32"; }
      static int numpyType() { return NPY_FLOAT32; }
    };
    template <> struct Traits<const double> {
      static const char* typeName() { return "float64"; }
      static int numpyType() { return NPY_FLOAT64; }
    };
    

    // Returns true if strides correspond to C memory layout
    template <unsigned Rank>
    bool isCArray(const unsigned shape[], const int strides[]) {
      int stride = 1;
      for (int i = Rank; i > 0; -- i) {
	if (strides[i-1] != stride) return false;
	stride *= shape[i-1];
      }
      return true;
    }
    
  }
  
  
    
  // ***************************************************************************
  // ***************************************************************************
  //    NDARRAY TO NUMPY CONVERTER 
  // ***************************************************************************
  // ***************************************************************************
  
  namespace {
    // Special Python wrapper object for ndarray
    template <typename T, unsigned Rank>
    class NDarrayWrapper : 
      public pytools::PyDataType<NDarrayWrapper<T, Rank>, ndarray<T, Rank> > {  };
  }

  // NDArray to Numpy converter for BOOST
  template <typename T, unsigned Rank>
  PyObject* NDArrayToNumpy<T,Rank>::convert(ndarray<T, Rank> const& array) 
  {
    MsgLog(pyConverterlogger, debug, "Calling converter from ndarray<"
	   << Traits<T>::typeName() << "," << Rank << "> to python object"); 
        
    // item size
    const size_t itemsize = sizeof(T);
    
    // Convert itemsize to numpy type number
    // For now, use templated function from NdarrayCvt.h
    const int typenum = Traits<T>::numpyType();
    
    // Dimensions and strides 
    npy_intp dims[Rank], strides[Rank];
    
    // Copy dim and strides from ndarry to numpy
    // NB: Numpy strides are in bytes    
    for (unsigned i=0; i<Rank; i++) {
      dims[i] = array.shape()[i];
      strides[i] = array.strides()[i] * itemsize;
    }

    // Grab underlying ndarry data pointer.
    // Have to cast to void* for numpy creation
    // The const is needed incase we instatiate a const type ndarray
    // 
    // NB: IT IS BELIEVED THAT THIS WILL INCREMENT NDARRAY'S INTERNAL SMART
    // POINTER REFERENCE COUNT. SO THIS SHOULD BE MEMEORY SAFE.
    // BUT ONLY IF NDARRAY IS CREATED USING METHOD 2 OR 3 AS DOCUMENTED IN NDARRAY
    // MAN PAGES.
    // IF NDARRAY IS CREATED USING METHOD 1 (SEE NDARRAY WEB DOCS), THE USER IS RESPONSIBLE
    // FOR MEMORY MANAGEMENT. THUS THE ARRAY DATA COULD GET DELETED WITHOUT INFORMING PYTHON
    // 
    const void* data = array.data();


    // Set the numpy flags 
    int flags = 0;   //==> initialise to zero

    // Start by setting outgoing numpy array is writable
    flags |= NPY_ARRAY_WRITEABLE;  

    // Check NDArray is C array 
    // For now, use function from NdarrayCvt.h
    if (psana_python::isCArray<Rank>(array.shape(),array.strides())) {
      flags |= NPY_ARRAY_C_CONTIGUOUS;          
    }

    // Check NDArray is aligned
    if (reinterpret_cast<size_t>(array.data()) % itemsize == 0) {
      flags |= NPY_ARRAY_ALIGNED;
    }
    
    // Create the outgoing numpy array
    // Note the const_cast, as the PyArray_New only accepts non-const
    PyObject* outgoing_numpy_array = PyArray_New(&PyArray_Type,
						 Rank,
						 dims,
						 typenum,
						 strides,
						 const_cast<void*>(data),
						 itemsize,
						 flags,
						 0);

    // The next block of code needs some explanation....
    //
    // When constructed the outgoing numpy array, we pased the data
    // pointer, which is a pointer to the raw NDarray array data. The
    // alternative would be to copy all the data, which could be an
    // issue if the NDarry is huge (eg: pnCCD data, CSPad data).
    //
    // Given that we've passed the raw array pointer to numpy, we
    // need to tell numpy that it does not own the raw NDarray data
    // pointer. If we didn't do this, then when the outgoing numpy
    // array goes out of scope, python will delete the outgoing numpy
    // array, which in turn will delete the raw NDarray pointer
    // instead of calling NDArray's destructor.  When the NDArray
    // destructor gets call, it still believe it own the raw data
    // pointer, try to release it, and we'll have a crash (!).  
    // 
    // Numpy has a mechanism for dealing with data it does not
    // own. You set the base value to point to another python object
    // that does own So when the numpy array gets deleted, it will call
    // the owning python object and it will take care of the
    // memory. What we'll do here is create a PYTHON object that'll
    // call the NDArray destructor when it goes out of scope. 

    // POINT TO NOTE: There are three reference counts here: NUMPY,
    // PYTHON TRACKING OBJECT (See below), and BOOST reference count.
    // The line const void* data = array.data(); increments the BOOST
    // reference count ASSUMING that the original NDARRAY was created
    // using methods 2 or 3 (see NDARRAY web docs).  The PYTHON
    // TRACKING OBJECT reference count takes place in the line below
    // when the numpy_array_tracking_object is created, which wraps
    // the NDARRAY destructor, as mentioned above.
    // THE NUMPY reference count is the usual PYTHON reference count,
    // thus if there are multiple copies of the NUMPY array, its
    // reference count will be >0. 

    // Create the python object to keep track of outgoing numpy array
    PyObject* numpy_array_tracking_object =
      NDarrayWrapper<T,Rank>::PyObject_FromCpp(array);
    
    // Set the base pointer of outgoing_numpy_array to
    // numpy_array_tracking_object 
    // (have to do some casting as PyArray_SetBaseObject excepts a
    // PyArrayObject* 
    PyArrayObject* aoptr_outgoing_numpy_array =
      reinterpret_cast<PyArrayObject*>(outgoing_numpy_array);
    PyArray_SetBaseObject(aoptr_outgoing_numpy_array,
			  numpy_array_tracking_object);    
    

    // Now return the outgoing numpy array
    return outgoing_numpy_array;
  }


  // Function to register converter
  template <typename T, unsigned Rank>
  void NDArrayToNumpy<T,Rank>::register_ndarray_to_numpy_cvt()  
  {
    // Check if converter already registered for this type
    boost::python::type_info tinfo = boost::python::type_id<ndarray<T, Rank> >();
    boost::python::converter::registration const* reg = boost::python::converter::registry::query(tinfo);
    
    if (reg == NULL) {
      MsgLog(pyConverterlogger, debug,
	     "REGISTER NDARRAY<" << Traits<T>::typeName() << "," << Rank << ">"
	     << "TO NUMPY CONVERTER");      
      boost::python::to_python_converter<ndarray<T,Rank>,NDArrayToNumpy<T,Rank> >();

    } else if ( (*reg).m_to_python == NULL) {
      MsgLog(pyConverterlogger, debug,
	     "REGISTER NDARRAY<" << Traits<T>::typeName() << "," << Rank << ">"
	     << "TO NUMPY CONVERTER");      
      boost::python::to_python_converter<ndarray<T,Rank>,NDArrayToNumpy<T,Rank> >();

    } else {
      MsgLog(pyConverterlogger, debug,
	     "NDARRAY<" << Traits<T>::typeName() << "," << Rank << ">"
	     << "TO NUMPY CONVERTER ALREADY REGISTERED");
    } 
    
    return;
  }


  // ***************************************************************************
  // ***************************************************************************
  //    END OF NDARRAY TO NUMPY CONVERTER
  // ***************************************************************************
  // ***************************************************************************
  
  
  
  
  
  
  // ***************************************************************************
  // ***************************************************************************
  //    START OF NUMPY TO NDARRAY CONVERTER
  // ***************************************************************************
  // ***************************************************************************

  // Numpy to NDArray convert for BOOST
  template <typename T, unsigned Rank>
  NumpyToNDArray<T,Rank>& NumpyToNDArray<T,Rank>::from_python()
  {
    // Check if converter was already registered
    boost::python::type_info tinfo = boost::python::type_id<ndarray<T, Rank> >();
    boost::python::converter::registration const* reg = boost::python::converter::registry::query(tinfo);

    // For debugging, printing out the contents of the reg pointer will be useful. 
    MsgLog(pyConverterlogger, debug,"reg:" << reg);
    MsgLog(pyConverterlogger, debug,"reg.m_to_python:" << (*reg).m_to_python);
    MsgLog(pyConverterlogger, debug,"reg.m_class_object:" << (*reg).m_class_object);
    MsgLog(pyConverterlogger, debug,"reg.lvalue_chain:" << (*reg).lvalue_chain);
    MsgLog(pyConverterlogger, debug,"reg.rvalue_chain:" << (*reg).rvalue_chain);
        
    if (reg == NULL) {
      boost::python::converter::registry::push_back(&NumpyToNDArray::convertible,
						    &NumpyToNDArray::construct,
						    boost::python::type_id< ndarray<T, Rank>  >() );    
      MsgLog(pyConverterlogger, debug,
	     "REGISTER BOOST PYTHON converter for NUMPY to NDARRAY"
	     << "<" << Traits<T>::typeName() << "," << Rank << ">");  

    } else if ((*reg).rvalue_chain == NULL && (*reg).lvalue_chain == NULL) {
      boost::python::converter::registry::push_back(&NumpyToNDArray::convertible,
						    &NumpyToNDArray::construct,
						    boost::python::type_id< ndarray<T, Rank>  >() );
      MsgLog(pyConverterlogger, debug,
	     "REGISTER BOOST PYTHON converter for NUMPY to NDARRAY"
	     << "<" << Traits<T>::typeName() << "," << Rank << ">");  
      // NB:When Numpy-->NDArray converter is missing, both rvalue_chain and lvalue_chain are NULL
      // NB: the rvalue and lvalue was only checked emperically. So could be incorrect...
      
    } else {
      MsgLog(pyConverterlogger, debug,
	     "BOOST PYTHON converter for NUMPY to NDARRAY"
	     << "<" << Traits<T>::typeName() << "," << Rank << "> ALREADY REGISTERED"); 	    
    } 
    
    return *this;
  }


  // Check object can be converted
  template <typename T, unsigned Rank> 
  void* NumpyToNDArray<T,Rank>::convertible(PyObject* obj) 
  {
    MsgLog(pyConverterlogger, debug,"CHECKING PYTHON OBJECT IS A NUMPY ARRAY");
    MsgLog(pyConverterlogger, debug,
	   "Value from PyArray_Check " << PyArray_Check(obj) );

    if ( !PyArray_Check(obj) ) {
      MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS NOT A NUMPY ARRAY");
      return NULL;
     }

    PyArrayObject* arrayPtr = reinterpret_cast<PyArrayObject*>(obj);
    const int rank = PyArray_NDIM(arrayPtr);
    if (rank != Rank) {
      MsgLog(pyConverterlogger, debug,
	     "INCORRECT NUMBER OF DIMENSIONS. Expected:" << Rank << " Got:" << rank);
      return NULL;
    }

    if (Traits<T>::numpyType() != PyArray_TYPE(arrayPtr)) {
      MsgLog(pyConverterlogger, debug,
	     "INCORRECT TYPE.  Expected " << Traits<T>::typeName()
	     << " Got:" << PyArray_TYPE(arrayPtr));
      return NULL;
    }

    MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS A NUMPY ARRAY");
    MsgLog(pyConverterlogger, debug,"Leaving convertible");
    return obj;
  }





  
  template <typename T, unsigned Rank>
  void NumpyToNDArray<T,Rank>::construct(PyObject* obj, BoostData* boostData) 
  {
    // Reminder that BoostData is a typedef defined in the header file
    //  --->  typedef boost::python::converter::rvalue_from_python_stage1_data BoostData;
    PyArrayObject* arrayPtr = reinterpret_cast<PyArrayObject*>(obj);

    const int itemsize = PyArray_ITEMSIZE(arrayPtr);

    // ==> Make a boost shared pointer to the underlying Numpy array 
    // ==> Do this for memory safety    
    T* array = reinterpret_cast<T*>(PyArray_DATA(arrayPtr));

    // ndarray<T,Rank>::shape_t shape[Rank]; // --> store array shape as C integers
    unsigned shape[Rank]; // --> store array shape as C integers
    int strides[Rank];     // --> store array strides as C integers
    for (unsigned i=0; i<Rank; i++) {
      shape[i] = PyArray_DIM(arrayPtr,i);
      // ==> NUMPY strides are in bytes
      strides[i] = PyArray_STRIDE(arrayPtr,i)/itemsize;
    }

    // Now that we have created out outgoing NDAraay, tell BOOST about
    // it, so it can pass it onto the calling C++ function
         
    // Get pointer to the converter's allocated memory block for the
    // outgoing NDArray
    // --> first create typedef storagetype for convenience
    typedef boost::python::converter::rvalue_from_python_storage<ndarray<T,Rank> > storagetype;
    // --> Now grab the pointer that BOOST has allocated to store the
    // --> NDArray
    void* storage = reinterpret_cast<storagetype*> (boostData)->storage.bytes;

    // --> Now set data's convertible attribute to outgoing NDArray
    MsgLog(pyConverterlogger, debug,"Creating outgoing ndarray");
    boostData->convertible = new(storage) ndarray<T,Rank>(array,shape);

    // NB: NDARRAY is created with a raw pointer.  That means NDARRAY
    // is not responsible for the memory, which is good in that the
    // calling C/C++ function cannot delete the data underneath NUMPY.
    // BUT if the NUMPY array goes out of scope in PYTHON, and the
    // calling C/C++ function still retains a reference to it, PYTHON
    // will delete the array data, and the C/C++ will be crash when
    // the C/C++ dereferences it.

    // Thus these converters should only be used for C/C++ functions 
    // that have no object persistance; ie:- PSALG functions are fine. 
    
    // For future thought.  If the NDArray constructor and destructor
    // could increment and decrement the PYTHON reference count, respectively,
    // via some intermediate object, this would allow robust memory-safe BOOST
    // converters with persistant objects. 

    
    // Set the strides of the outgoing NDArray
    ndarray<T,Rank>* outgoingArray = reinterpret_cast<ndarray<T,Rank>*>(boostData->convertible);
    outgoingArray->strides(strides);

    MsgLog(pyConverterlogger, debug,
	   "Address of orignal Numpy data " << array);
    return;
  }
  // ***************************************************************************
  // ***************************************************************************
  //    END OF NDARRAY TO NUMPY CONVERTER
  // ***************************************************************************
  // ***************************************************************************
  
  
  
  
  
  
  // ***************************************************************************
  // ***************************************************************************
  //    START OF STL-LIST<NUMBERS> TO NUMPY CONVERTER
  // ***************************************************************************
  // ***************************************************************************
  
  // stl::list<ndtypes> to Numpy converter for BOOST
  // where ndtypes is defined by macro ND_TYPES defined at top of this header
  template <typename T>
  PyObject* StlListToNumpy<T>::convert( std::list<T> const& list ) 
  {
    MsgLog(pyConverterlogger, debug,
	   "Calling converter from stl::list<" 
	   <<  Traits<T>::typeName() << "> to python object");  
            
    // Convert itemsize to numpy type number    
    const int typenum = Traits<T>::numpyType();
    
    // Numpy array shape is set to be 1D and same size as list
    npy_intp dims[1];
    dims[0] = list.size();
            
    // Create a 1D numpy array that has same size as the list            
    PyObject* outgoing_numpy_array = PyArray_SimpleNew(1,dims,typenum);
    
    // Recast outgoing_numpy_array as PyArrayObject* to get access to data
    PyArrayObject* aoptr_outgoing_numpy_array =
      reinterpret_cast<PyArrayObject*>(outgoing_numpy_array);
      
    // Get pointer to underlying numpy array
    T* arrayData = reinterpret_cast<T*>(PyArray_DATA(aoptr_outgoing_numpy_array));
          
    // Deep copy contents of list to outgoing_numpy_array 
    // ...also avoids problems associated with lifetime of numpy and stl list
    typename std::list<T>::const_iterator listIter;
    for(listIter = list.begin(); listIter != list.end(); listIter++,arrayData++) 
      {
	MsgLog(pyConverterlogger, debug, *listIter);
	*arrayData = *listIter;
      }

    
    // Now return the outgoing numpy array
    return outgoing_numpy_array;
  }
  


  // Register STL list<number> to Numpy converter with BOOST
  template <typename T>
  void StlListToNumpy<T>::register_stllist_to_numpy_cvt() 
  {
    // Check if converter already registered for this type
    boost::python::type_info tinfo = boost::python::type_id< std::list<T> >();
    boost::python::converter::registration const* reg = boost::python::converter::registry::query(tinfo);
    
    if (reg == NULL) {
      MsgLog(pyConverterlogger, debug,
	     "REGISTER STL LIST<" << Traits<T>::typeName() << ">"
	     << "TO NUMPY CONVERTER");
      boost::python::to_python_converter< std::list<T>, StlListToNumpy<T> >();

    } else if ( (*reg).m_to_python == NULL) {
      MsgLog(pyConverterlogger, debug,
	     "REGISTER STL LIST<" << Traits<T>::typeName() << ">"
	     << "TO NUMPY CONVERTER");
      boost::python::to_python_converter< std::list<T>, StlListToNumpy<T> >();

    } else {
      MsgLog(pyConverterlogger, debug,
	     "STL LIST<" << Traits<T>::typeName() << ">"
	     << "TO NUMPY CONVERTER ALREADY REGISTERED");
    } 
    
    return;
  }

// ***************************************************************************
// ***************************************************************************
//    END OF STL-LIST<NUMBERS> TO NDARRAY CONVERTER
// ***************************************************************************
// ***************************************************************************





  // ***************************************************************************
  // ***************************************************************************
  //    START OF PYTHON-EVENT TO CPP-EVENT CONVERTER
  // ***************************************************************************
  // ***************************************************************************

  // Python-Event to Event convert for BOOST
  PyEvtToEvt& PyEvtToEvt::from_python()
  {
    // Check if converter was already registered
    boost::python::type_info tinfo = boost::python::type_id< boost::shared_ptr<PSEvt::Event> >();
    boost::python::converter::registration const* reg = boost::python::converter::registry::query(tinfo);

    // For debugging, printing out the contents of the reg pointer will be useful. 
    MsgLog(pyConverterlogger, debug,"reg:" << reg);
    if (reg == NULL) {
      boost::python::converter::registry::push_back(&PyEvtToEvt::convertible,
						    &PyEvtToEvt::construct,
						    boost::python::type_id<boost::shared_ptr<PSEvt::Event> >());
      
      MsgLog(pyConverterlogger, debug,"REGISTER BOOST PYTHON converter for Event");  

    } else if ((*reg).rvalue_chain == NULL && (*reg).lvalue_chain == NULL) {
      boost::python::converter::registry::push_back(&PyEvtToEvt::convertible,
						    &PyEvtToEvt::construct,
						    boost::python::type_id<boost::shared_ptr<PSEvt::Event> >());
      MsgLog(pyConverterlogger, debug,"REGISTER BOOST PYTHON converter for Event");  
      // NB:When Python-Event-->Event converter is missing, both rvalue_chain and lvalue_chain are NULL
      // NB: the rvalue and lvalue was only checked emperically. So could be incorrect...
      
    } else {
      MsgLog(pyConverterlogger, debug,"BOOST PYTHON converter for Event ALREADY REGISTERED"); 	    
    } 
    
    return *this;
  }


  // Check object can be converted
  void* PyEvtToEvt::convertible(PyObject* obj) 
  {
    MsgLog(pyConverterlogger, debug,"CHECKING PYTHON OBJECT IS A PYTHON-EVENT");
    MsgLog(pyConverterlogger, debug,"Pyobject type " << obj->ob_type->tp_name );

    if (!psana_python::Event::Object_TypeCheck(obj)) {
      MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS NOT A PSANA EVENT");
      return NULL;
     }

    MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS A PSANA EVENT");
    MsgLog(pyConverterlogger, debug,"Leaving convertible");
    return obj;
  }





  
  void PyEvtToEvt::construct(PyObject* obj, BoostData* boostData) 
  {
    // Reminder that BoostData is a typedef defined in the header file
    //  --->  typedef boost::python::converter::rvalue_from_python_stage1_data BoostData;

    // --> Set boostData's convertible attribute to point to the original PSANA Event object
    MsgLog(pyConverterlogger, debug,"Pointing back to original PSANA Event Object");

    // NB: WE ARE GIVING BOOST A REFERENCE TO A SMART POINTER. WE
    // ASSUME THAT BOOST PASSES THE SMART POINTER BY VALUE WHEN
    // CALLING THE C++ FUNCTION, THUS INCREMENTING THE SHARED POINTER
    // REFERENCE COUNT
    // (PYTHON CANNOT DELETE THE SHARED POINTER WHILE THIS HAPPENS)

    MsgLog(pyConverterlogger, debug,
	   " WE ARE GIVING BOOST A REFERENCE TO A SMART POINTER")
    psana_python::Event* py_this = static_cast<psana_python::Event*>(obj);
    boostData->convertible = static_cast<void*> (&(py_this->m_obj));

    return;
  }
  // ***************************************************************************
  // ***************************************************************************
  //    END OF PYTHON-EVENT TO CPP-EVENT CONVERTER
  // ***************************************************************************
  // ***************************************************************************




  
  // ***************************************************************************
  // ***************************************************************************
  //    START OF PYTHON-ENV TO CPP-ENV CONVERTER
  // ***************************************************************************
  // ***************************************************************************

  // Python-Env to Env convert for BOOST
  PyEnvToEnv& PyEnvToEnv::from_python()
  {
    // Check if converter was already registered
    boost::python::type_info tinfo = boost::python::type_id< boost::shared_ptr<PSEnv::Env> >();
    boost::python::converter::registration const* reg = boost::python::converter::registry::query(tinfo);

    // For debugging, printing out the contents of the reg pointer will be useful. 
    MsgLog(pyConverterlogger, debug,"reg:" << reg);
    if (reg == NULL) {
      boost::python::converter::registry::push_back(&PyEnvToEnv::convertible,
						    &PyEnvToEnv::construct,
						    boost::python::type_id<boost::shared_ptr<PSEnv::Env> >());
      
      MsgLog(pyConverterlogger, debug,"REGISTER BOOST PYTHON converter for Env");  

    } else if ((*reg).rvalue_chain == NULL && (*reg).lvalue_chain == NULL) {
      boost::python::converter::registry::push_back(&PyEnvToEnv::convertible,
						    &PyEnvToEnv::construct,
						    boost::python::type_id<boost::shared_ptr<PSEnv::Env> >());
      MsgLog(pyConverterlogger, debug,"REGISTER BOOST PYTHON converter for Env");  
      // NB:When Python-Env-->Env converter is missing, both rvalue_chain and lvalue_chain are NULL
      // NB: the rvalue and lvalue was only checked emperically. So could be incorrect...
      
    } else {
      MsgLog(pyConverterlogger, debug,"BOOST PYTHON converter for Env ALREADY REGISTERED"); 	    
    } 
    
    return *this;
  }


  // Check object can be converted
  void* PyEnvToEnv::convertible(PyObject* obj) 
  {
    MsgLog(pyConverterlogger, debug,"CHECKING PYTHON OBJECT IS A PYTHON-ENV");
    MsgLog(pyConverterlogger, debug,"Pyobject type " << obj->ob_type->tp_name );

    if (!psana_python::Env::Object_TypeCheck(obj)) {
      MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS NOT A PSANA ENV");
      return NULL;
     }

    MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS A PSANA ENV");
    MsgLog(pyConverterlogger, debug,"Leaving convertible");
    return obj;
  }





  
  void PyEnvToEnv::construct(PyObject* obj, BoostData* boostData) 
  {
    // Reminder that BoostData is a typedef defined in the header file
    //  --->  typedef boost::python::converter::rvalue_from_python_stage1_data BoostData;

    // --> Set boostData's convertible attribute to point to the original PSANA Env object
    MsgLog(pyConverterlogger, debug,"Pointing back to original PSANA Env Object");


    // NB: WE ARE GIVING BOOST A REFERENCE TO A SMART POINTER. WE
    // ASSUME THAT BOOST PASSES THE SMART POINTER BY VALUE WHEN
    // CALLING THE C++ FUNCTION, THUS INCREMENTING THE SHARED POINTER
    // REFERENCE COUNT
    // (PYTHON CANNOT DELETE THE SHARED POINTER WHILE THIS HAPPENS)
    MsgLog(pyConverterlogger, debug,
	   " WE ARE GIVING BOOST A REFERENCE TO A SMART POINTER")      
    psana_python::Env* py_this = static_cast<psana_python::Env*>(obj);
    boostData->convertible = static_cast<void*> (&(py_this->m_obj));

    return;
  }
  // ***************************************************************************
  // ***************************************************************************
  //    END OF PYTHON-ENV TO CPP-ENV CONVERTER
  // ***************************************************************************
  // ***************************************************************************





  // ***************************************************************************
  // ***************************************************************************
  //    START OF PYTHON-SOURCE TO CPP-SOURCE CONVERTER
  // ***************************************************************************
  // ***************************************************************************

  // Python-Source to Source convert for BOOST
  PySourceToSource& PySourceToSource::from_python()
  {
    // Check if converter was already registered
    boost::python::type_info tinfo = boost::python::type_id<PSEvt::Source>();
    boost::python::converter::registration const* reg = boost::python::converter::registry::query(tinfo);

    // For debugging, printing out the contents of the reg pointer will be useful. 
    MsgLog(pyConverterlogger, debug,"reg:" << reg);
    if (reg == NULL) {
      boost::python::converter::registry::push_back(&PySourceToSource::convertible,
						    &PySourceToSource::construct,
						    boost::python::type_id<PSEvt::Source>());
						          
      MsgLog(pyConverterlogger, debug,"REGISTER BOOST PYTHON converter for Source");  

    } else if ((*reg).rvalue_chain == NULL && (*reg).lvalue_chain == NULL) {
      boost::python::converter::registry::push_back(&PySourceToSource::convertible,
						    &PySourceToSource::construct,
      						    boost::python::type_id<PSEvt::Source>());
      MsgLog(pyConverterlogger, debug,"REGISTER BOOST PYTHON converter for Source");  
      // NB:When Python-Source-->Source converter is missing, both rvalue_chain and lvalue_chain are NULL
      // NB: the rvalue and lvalue was only checked emperically. So could be incorrect...
      
    } else {
      MsgLog(pyConverterlogger, debug,"BOOST PYTHON converter for Source ALREADY REGISTERED"); 	    
    } 
    
    return *this;
  }


  // Check object can be converted
  void* PySourceToSource::convertible(PyObject* obj) 
  {
    MsgLog(pyConverterlogger, debug,"CHECKING PYTHON OBJECT IS A PYTHON-SOURCE");

    if (!psana_python::Source::Object_TypeCheck(obj) ) {
      MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS NOT A PSANA SOURCE");
      return NULL;
    }

    MsgLog(pyConverterlogger, debug,"PYTHON OBJECT IS A PSANA SOURCE");
    MsgLog(pyConverterlogger, debug,"Leaving convertible");
    return obj;
  }





  
  void PySourceToSource::construct(PyObject* obj, BoostData* boostData) 
  {
    // Reminder that BoostData is a typedef defined in the header file
    //  --->  typedef boost::python::converter::rvalue_from_python_stage1_data BoostData;

    // --> Set boostData's convertible attribute to point to the original PSANA Source object
    MsgLog(pyConverterlogger, debug,"Pointing back to original PSANA Source Object");

    psana_python::Source* py_this = static_cast<psana_python::Source*>(obj);
    
    
    // Get pointer to the converter's allocated memory block for the
    // outgoing CPP-SOURCE
    // --> first create typedef storagetype for convenience
    typedef boost::python::converter::rvalue_from_python_storage<PSEvt::Source> storagetype;
    // --> Now grab the pointer that BOOST has allocated to store the
    // --> CPP-SOURCE
    void* storage = reinterpret_cast<storagetype*> (boostData)->storage.bytes;

    // --> Now set data's convertible attribute to outgoing CPP-SOURCE
    MsgLog(pyConverterlogger, debug,"Creating outgoing CPP-SOURCE");
    boostData->convertible = new(storage) PSEvt::Source(py_this->m_obj);

    return;
  }
  // ***************************************************************************
  // ***************************************************************************
  //    END OF PYTHON-SOURCE TO CPP-SOURCE CONVERTER
  // ***************************************************************************
  // ***************************************************************************





}  // end of psana_python namespace

