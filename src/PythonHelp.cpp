#if 0
////////////////////////////////////////////////////////////////////////////////
//
// XXX TO DO:
//
// Python wrappers should use attributes instead of functions
// e.g. ConfigV1.pvControls[i] instead of ConfigV1.pvControls()[i]
//
//
////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class PythonHelp
//
// Author List:
//   Joseph S. Barrera III
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include <psana_python/PythonHelp.h>

//-----------------
// C/C++ Headers --
//-----------------
#include <boost/python.hpp>
#include <psddl_python/vector_indexing_suite_nocopy.hpp>
#include <boost/utility.hpp>
#include <numpy/arrayobject.h>
#include <string>
#include <set>
#include <cxxabi.h>
#include <python/Python.h>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include <MsgLogger/MsgLogger.h>
#include <PSEnv/Env.h>
#include <PSEnv/EpicsStore.h>
#include <PSEvt/Event.h>
#include <PSEvt/EventId.h>
#include <ConfigSvc/ConfigSvc.h>
#include <psddl_python/GenericGetter.h>
#include <psddl_python/EvtGetter.h>
#include <psddl_python/EvtGetMethod.h>
#include <psana_python/EnvWrapper.h>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using boost::python::api::object;
using boost::python::class_;
using boost::python::copy_const_reference;
using boost::python::return_by_value;
using boost::python::init;
using boost::python::no_init;
using boost::python::numeric::array;
using boost::python::reference_existing_object;
using boost::python::return_value_policy;
using boost::python::vector_indexing_suite_nocopy;

using std::map;
using std::set;
using std::string;
using std::vector;
using std::list;

using PSEnv::Env;
using PSEnv::EnvObjectStore;
using PSEnv::EpicsStore;
using PSEvt::Event;
using PSEvt::EventKey;
using PSEvt::Source;
using Pds::Src;

typedef boost::shared_ptr<PyObject> PyObjPtr;





namespace Psana {

  struct PyRefDelete {
    void operator()(PyObject* obj) { Py_CLEAR(obj); }
  };
#if 0
  // XXX static?
  object Event_Class;
  object EnvWrapper_Class;


  object getEnvWrapper(Env& env, const string& name, const string& className) {
    EnvWrapper _envWrapper(env, name, className);
    object envWrapper(EnvWrapper_Class(_envWrapper));
    return envWrapper;
  }

  object getEvtWrapper(Event& evt) {
    return object(Event_Class(evt));
  }

  // call specified method
  boost::shared_ptr<PyObject> call(PyObject* method, Event* evt, Env* env, const string& name, const string& className)
  {
    int nargs = (evt == NULL ? 1 : 2);
    PyObjPtr args(PyTuple_New(nargs), PyRefDelete());
    object evtWrapper;
    if (evt) {
      evtWrapper = Psana::getEvtWrapper(*evt);
      PyTuple_SET_ITEM(args.get(), 0, evtWrapper.ptr());
    }
    object envWrapper = Psana::getEnvWrapper(*env, name, className);
    PyTuple_SET_ITEM(args.get(), nargs - 1, envWrapper.ptr());
    PyObjPtr res(PyObject_Call(method, args.get(), NULL), PyRefDelete());
    return res;
  }
#endif
} // namespace Psana
#endif
