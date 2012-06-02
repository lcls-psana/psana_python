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
#include <cstdio>
#include <cctype>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include <MsgLogger/MsgLogger.h>
#include <PSEvt/EventId.h>
#include <psana/Exceptions.h>
#include <psana_python/PythonHelp.h>
#include <psana_python/CreateWrappers.h>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

using std::string;

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

namespace psana {

//----------------
// Constructors --
//----------------
static PyObject* getMethodByName(PyObject* instance, char* name) {
  PyObject* method = PyObject_GetAttrString(instance, name);
  if (method == NULL) {
    const int size = strlen(name) + 1;
    char lname[size];
    for (int i = 0; i < size; i++) {
      lname[i] = tolower(name[i]);
    }
    method = PyObject_GetAttrString(instance, lname);
  }
  return method;
}

PythonModule::PythonModule(const string& name, PyObject* instance) : Module(name), m_instance(instance)
{
  m_beginJob = getMethodByName(m_instance, "beginJob");
  m_beginRun = getMethodByName(m_instance, "beginRun");
  m_beginCalibCycle = getMethodByName(m_instance, "beginCalibCycle");
  m_event = getMethodByName(m_instance, "event");
  m_endCalibCycle = getMethodByName(m_instance, "endCalibCycle");
  m_endRun = getMethodByName(m_instance, "endRun");
  m_endJob = getMethodByName(m_instance, "endJob");

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
}

// call specific method
void
PythonModule::call(PyObject* method, Event& evt, Env& env)
{
  if (not method) return;

  PyObjPtr res(Psana::call(method, evt, env, name(), className()));
  if (not res) {
    PyErr_Print();
    throw ExceptionGenericPyError(ERR_LOC, "exception raised while calling Python method");
  }
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
