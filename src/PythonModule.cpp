//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: PSAnaApp.cpp 3280 2012-05-01 16:41:53Z salnikov@SLAC.STANFORD.EDU $
//
// Description:
//	Class PSAnaApp
//
// Author List:
//  Andy Salnikov, Joseph S. Barrera III
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "psana_python/PythonModule.h"

//-----------------
// C/C++ Headers --
//-----------------
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <boost/python.hpp>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MsgLogger/MsgLogger.h"
#include "PSEvt/EventId.h"
#include "psana_python/Exceptions.h"
#include "psana_python/Env.h"
#include "psana_python/Event.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using std::string;
using boost::python::object;

namespace {

  const char logger[] = "PythonModule";

  // return string describing Python exception
  string pyExcStr()
  {
    PyObject *ptype;
    PyObject *pvalue;
    PyObject *ptraceback;

    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyObject* errstr = PyObject_Str(pvalue);
    string msg = PyString_AsString(errstr);

    Py_CLEAR(errstr);
    Py_CLEAR(ptype);
    Py_CLEAR(pvalue);
    Py_CLEAR(ptraceback);

    return msg;
  }

  // names for psana-style methods, this must correspond to method enums in class declaration
  const char* psana_methods[] = {
    "beginJob", "beginRun", "beginCalibCycle", "event", "endCalibCycle", "endRun", "endJob"
  };

  // names for pyana-style methods, this must correspond to method enums in class declaration
  const char* pyana_methods[] = {
    "beginjob", "beginrun", "begincalibcycle", "event", "endcalibcycle", "endrun", "endjob"
  };

  // functor to check for non-zero pointers
  struct NonZero {
    bool operator()(const pytools::pyshared_ptr& p) const { return bool(p); }
  };

}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace psana_python {

//----------------
// Constructors --
//----------------
PythonModule::PythonModule(const string& name, PyObject* instance)
  : Module(name)
  , m_instance(pytools::make_pyshared(instance))
  , m_pyanaCompat(true)
{
  // Currently, pyana compatibity is enabled unless the PYANA_COMPAT env var is 0.
  const char* pyana_compat_env = std::getenv("PYANA_COMPAT");
  const bool pyana_compat_disabled = pyana_compat_env and std::strcmp(pyana_compat_env, "0") == 0;
  m_pyanaCompat = not pyana_compat_disabled;

  // check pyana-style methods first
  for (int i = 0; i != NumMethods; ++ i) {
    m_methods[i] = pytools::make_pyshared(PyObject_GetAttrString(m_instance.get(), pyana_methods[i]));
  }
  bool any = std::find_if(m_methods, m_methods+NumMethods, ::NonZero()) != (m_methods+NumMethods);

  // check that we are not getting pyana stuff if compatibility is disabled
  if (any and not m_pyanaCompat) {
    throw Exception(ERR_LOC, "Error: old pyana-style methods used (e.g. beginjob instead of beginJob)");
  }

  if (not m_pyanaCompat or not any) {
    // search for psana-style methods
    m_pyanaCompat = false;
    for (int i = 0; i != NumMethods; ++ i) {
        m_methods[i] = pytools::make_pyshared(PyObject_GetAttrString(m_instance.get(), psana_methods[i]));
    }
  }

  // check that at least one method is there
  any = std::find_if(m_methods, m_methods+NumMethods, ::NonZero()) != (m_methods+NumMethods);
  if (not any) {
    throw Exception(ERR_LOC, "Error: module " + name + " does not define any methods");
  }

}

//--------------
// Destructor --
//--------------
PythonModule::~PythonModule ()
{
}

void
PythonModule::call(PyObject* method, bool pyana_optional_evt, PSEvt::Event& evt, PSEnv::Env& env)
{
  if (not method) return;

  // in pyana mode some methods can take either (env) or (evt, env),
  // check number of arguments to guess how to call it
  int nargs = 2;
  if (pyana_optional_evt) {
    PyCodeObject* code = (PyCodeObject*)PyFunction_GetCode(PyMethod_Function(method));
    // co_argcount includes self argument
    nargs = code->co_argcount - 1;
  }

  pytools::pyshared_ptr args = pytools::make_pyshared(PyTuple_New(nargs));
  if (nargs > 1) {
    PyObject* pyevt = psana_python::Event::PyObject_FromCpp(evt.shared_from_this());
    PyTuple_SET_ITEM(args.get(), 0, pyevt);
  }
  PyObject* pyenv = psana_python::Env::PyObject_FromCpp(env.shared_from_this());
  PyTuple_SET_ITEM(args.get(), nargs - 1, pyenv);
  
  // call the method
  pytools::pyshared_ptr res = pytools::make_pyshared(PyObject_Call(method, args.get(), NULL));
  if (not res) {
    PyErr_Print();
    exit(1);
  }
}

// Load one user module. The name of the module has a format [Package.]Class[:name]
extern "C"
psana::Module*
moduleFactory(const string& name)
{
  // Make class name and module name. Use psana for package name if not given.
  // Full name should be package name . class name.
  string fullName = name;
  string moduleName = name;
  string::size_type p1 = moduleName.find(':');
  if (p1 != string::npos) {
    moduleName.erase(p1);
  }
  string className = moduleName;
  p1 = className.find('.');
  if (p1 == string::npos) {
    moduleName = "psana." + moduleName;
    fullName = "psana." + fullName;
  } else {
    className.erase(0, p1+1);
  }

  MsgLog(logger, debug, "names: module=" << moduleName << " class=" << className << " full=" << fullName);

  // Make sure python is initialized
  Py_Initialize();

  // Clear any lingering errors
  pyExcStr();

  // The whole shebang from this package needs to be initialized to expose the
  // wrapped stuff to Python interpreter. We are doing this via importing the module _psana.
  pytools::pyshared_ptr psanamod = pytools::make_pyshared(PyImport_ImportModule("_psana"));
  if (not psanamod) {
    string msg = "failed to import module _psana: " + ::pyExcStr();
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // try to import module
  pytools::pyshared_ptr mod = pytools::make_pyshared(PyImport_ImportModule((char*)moduleName.c_str()));
  if (not mod) {
    string msg = "failed to import module " + moduleName + ": " + ::pyExcStr();
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // there must be a class defined with this name
  pytools::pyshared_ptr cls = pytools::make_pyshared(PyObject_GetAttrString(mod.get(), (char*)className.c_str()));
  if (not cls) {
    string msg = "module " + moduleName + " does not define class " + className;
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // make sure class is callable
  if (not PyCallable_Check(cls.get())) {
    string msg = "class " + moduleName + " cannot be instantiated (is not callable)";
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // Create empty positional args list.
  pytools::pyshared_ptr args = pytools::make_pyshared(PyTuple_New(0));

  // Create keyword args list.
  pytools::pyshared_ptr kwargs = pytools::make_pyshared(PyDict_New());
  ConfigSvc::ConfigSvc cfg(psana::Context::get());
  std::list<std::string> keys = cfg.getKeys(fullName);
  std::list<std::string>::iterator it;
  for (it = keys.begin(); it != keys.end(); it++) {
    const std::string& key = *it;
    const char* value = cfg.getStr(fullName, key).c_str();
    PyDict_SetItemString(kwargs.get(), key.c_str(), PyString_FromString(value));
  }

  // Construct the instance.
  PyObject* instance = PyObject_Call(cls.get(), args.get(), kwargs.get());
  if (not instance) {
    std::string msg = "cannot create instance of class " + className + ": " + ::pyExcStr();
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  // Set m_className and m_fullName attributes.
  PyObject_SetAttr(instance, PyString_FromString("m_className"), PyString_FromString(className.c_str()));
  PyObject_SetAttr(instance, PyString_FromString("m_fullName"), PyString_FromString(fullName.c_str()));

  // check that instance has at least an event() method
  if (not PyObject_HasAttrString(instance, "event")) {
    Py_CLEAR(instance);
    std::string msg = "class " + className + " does not define event() method";
    MsgLog(logger, error, msg);
    throw ExceptionPyLoadError(ERR_LOC, msg);
  }

  return new PythonModule(fullName, instance);
}

} // namespace psana
