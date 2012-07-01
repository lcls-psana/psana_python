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
#include <psana_python/PythonModule.h>

//-----------------
// C/C++ Headers --
//-----------------
#include <cctype>
#include <cstdio>
#include <cstdlib>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include <MsgLogger/MsgLogger.h>
#include <PSEvt/EventId.h>
#include <psana/Exceptions.h>
#include <boost/python.hpp>
#include <psana_python/EnvWrapper.h>
#include <psana_python/EventWrapper.h>
#include <psana_python/CreateWrappers.h>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using std::string;
using boost::python::api::object;

namespace {

  const char logger[] = "PythonModule";

  struct PyRefDelete {
    void operator()(PyObject* obj) { Py_CLEAR(obj); }
  };
  typedef boost::shared_ptr<PyObject> PyObjPtr;

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
}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

namespace Psana {

//----------------
// Constructors --
//----------------
PythonModule::PythonModule(const string& name, PyObject* instance) : Module(name), m_instance(instance)
{
  // Currently, pyana compatibity is enabled unless the PYANA_COMPAT env var is 0.
  const char* pyana_compat_env = getenv("PYANA_COMPAT");
  const bool pyana_compat_disabled = pyana_compat_env && (string(pyana_compat_env) ==  "0");
  m_pyanaCompat = ! pyana_compat_disabled;

  m_beginJob = PyObject_GetAttrString(m_instance, "beginJob");
  m_beginRun = PyObject_GetAttrString(m_instance, "beginRun");
  m_beginCalibCycle = PyObject_GetAttrString(m_instance, "beginCalibCycle");
  m_event = PyObject_GetAttrString(m_instance, "event");
  m_endCalibCycle = PyObject_GetAttrString(m_instance, "endCalibCycle");
  m_endRun = PyObject_GetAttrString(m_instance, "endRun");
  m_endJob = PyObject_GetAttrString(m_instance, "endJob");

  m_beginjob = PyObject_GetAttrString(m_instance, "beginjob");
  m_beginrun = PyObject_GetAttrString(m_instance, "beginrun");
  m_begincalibcycle = PyObject_GetAttrString(m_instance, "begincalibcycle");
  m_endcalibcycle = PyObject_GetAttrString(m_instance, "endcalibcycle");
  m_endrun = PyObject_GetAttrString(m_instance, "endrun");
  m_endjob = PyObject_GetAttrString(m_instance, "endjob");

  if (! m_pyanaCompat) {
    if (m_beginjob ||
        m_beginrun ||
        m_begincalibcycle ||
        m_endcalibcycle ||
        m_endrun ||
        m_endjob) {
      fprintf(stderr, "Error: old pyana-style methods used (e.g. m_beginjob instead of m_beginJob)\n.");
      exit(1);
    }
  }

  Psana::createWrappers();
}

//--------------
// Destructor --
//--------------
PythonModule::~PythonModule ()
{
  Py_CLEAR(m_instance);
  Py_CLEAR(m_beginJob);
  Py_CLEAR(m_beginRun);
  Py_CLEAR(m_beginCalibCycle);
  Py_CLEAR(m_event);
  Py_CLEAR(m_endCalibCycle);
  Py_CLEAR(m_endRun);
  Py_CLEAR(m_endJob);

  // Temporary pyana (XtcExplorer) support
  Py_CLEAR(m_beginjob);
  Py_CLEAR(m_beginrun);
  Py_CLEAR(m_begincalibcycle);
  Py_CLEAR(m_endcalibcycle);
  Py_CLEAR(m_endrun);
  Py_CLEAR(m_endjob);
}

// The const char* arguments make it easier to figure out what's happening
// in a C++ stack trace when a segmentation violation occurs
static void ________CALL(const char* methodName, const char* name, const char* className, int nargs, PyObject* method, Event& evt, Env& env)
{
  PyObjPtr args(PyTuple_New(nargs), PyRefDelete());
  object evtWrapper;
  if (nargs > 1) {
    evtWrapper = object(EventWrapperClass(EventWrapper(evt)));
    PyTuple_SET_ITEM(args.get(), 0, evtWrapper.ptr());
  }
  object envWrapper(EnvWrapperClass(EnvWrapper(env, name, className)));
  PyTuple_SET_ITEM(args.get(), nargs - 1, envWrapper.ptr());
  PyObjPtr res(PyObject_Call(method, args.get(), NULL), PyRefDelete());
  if (not res) {
    PyErr_Print();
    exit(1);
  }
}

// call specific method
void
PythonModule::call(const char* methodName, PyObject* psana_method, PyObject* pyana_method, bool pyana_no_evt, Event& evt, Env& env)
{
  int nargs = (m_pyanaCompat && pyana_no_evt) ? 1 : 2;
  PyObject* method = m_pyanaCompat ? pyana_method : psana_method;
  if (method == NULL) {
    return;
  }
  PyObjPtr args(PyTuple_New(nargs), PyRefDelete());
  object evtWrapper;
  if (nargs > 1) {
    evtWrapper = object(EventWrapperClass(EventWrapper(evt)));
    PyTuple_SET_ITEM(args.get(), 0, evtWrapper.ptr());
  }
  ________CALL(methodName, name().c_str(), className().c_str(), nargs, method, evt, env);
}

// Load one user module. The name of the module has a format [Package.]Class[:name]
extern "C" PythonModule* moduleFactory(const string& name)
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

  // try to import module
  PyObjPtr mod(PyImport_ImportModule((char*)moduleName.c_str()), PyRefDelete());
  if (not mod) {
    throw ExceptionPyLoadError(ERR_LOC, "failed to import module " + moduleName + ": " + ::pyExcStr());
  }

  // there must be a class defined with this name
  PyObjPtr cls(PyObject_GetAttrString(mod.get(), (char*)className.c_str()), PyRefDelete());
  if (not cls) {
    throw ExceptionPyLoadError(ERR_LOC, "Python module " + moduleName + " does not define class " + className);
  }

  // make sure class is callable
  if (not PyCallable_Check(cls.get())) {
    throw ExceptionPyLoadError(ERR_LOC, "Python object " + moduleName + " cannot be instantiated (is not callable)");
  }

  // Create empty positional args list.
  PyObjPtr args(PyTuple_New(0), PyRefDelete());

  // Create keyword args list.
  PyObjPtr kwargs(PyDict_New(), PyRefDelete());
  ConfigSvc::ConfigSvc cfg;
  list<string> keys = cfg.getKeys(fullName);
  list<string>::iterator it;
  for (it = keys.begin(); it != keys.end(); it++) {
    const string& key = *it;
    const char* value = cfg.getStr(fullName, key).c_str();
    PyDict_SetItemString(kwargs.get(), key.c_str(), PyString_FromString(value));
  }

  // Construct the instance.
  PyObject* instance = PyObject_Call(cls.get(), args.get(), kwargs.get());
  if (not instance) {
    throw ExceptionPyLoadError(ERR_LOC, "error making an instance of class " + className + ": " + ::pyExcStr());
  }

  // Set m_className and m_fullName attributes.
  PyObject_SetAttr(instance, PyString_FromString("m_className"), PyString_FromString(className.c_str()));
  PyObject_SetAttr(instance, PyString_FromString("m_fullName"), PyString_FromString(fullName.c_str()));

  // check that instance has at least an event() method
  if (not PyObject_HasAttrString(instance, "event")) {
    Py_CLEAR(instance);
    throw ExceptionPyLoadError(ERR_LOC, "Python class " + className + " does not define event() method");
  }

  return new PythonModule(fullName, instance);
}

} // namespace psana
