#include "psana_python/PyWrapper.h"

#include "psana_python/GenericWrapper.h"
#include "MsgLogger/MsgLogger.h"
#include "psana/Exceptions.h"
#include "PSEvt/EventId.h"
#include <psana_python/PythonHelp.h>
#include <cstdio>
#include <cctype>

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  const char logger[] = "PyWrapper";

  struct PyRefDelete {
    void operator()(PyObject* obj) { Py_CLEAR(obj); }
  };
  typedef boost::shared_ptr<PyObject> PyObjPtr;

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

PyWrapper::PyWrapper(const std::string& name, PyObject* instance)
  : GenericWrapper(name)
  , m_moduleName(name)
  , m_instance(instance)
  , m_beginJob(0)
  , m_beginRun(0)
  , m_beginCalibCycle(0)
  , m_event(0)
  , m_endCalibCycle(0)
  , m_endRun(0)
  , m_endJob(0)
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
PyWrapper::~PyWrapper ()
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

/// Method which is called once at the beginning of the job
void 
PyWrapper::beginJob(Event& evt, Env& env)
{
  call(m_beginJob, evt, env);
}

/// Method which is called at the beginning of the run
void 
PyWrapper::beginRun(Event& evt, Env& env)
{
  call(m_beginRun, evt, env);
}

/// Method which is called at the beginning of the calibration cycle
void 
PyWrapper::beginCalibCycle(Event& evt, Env& env)
{
  call(m_beginCalibCycle, evt, env);
}

/// Method which is called with event data, this is the only required 
/// method, all other methods are optional
void 
PyWrapper::event(Event& evt, Env& env)
{
  call(m_event, evt, env);
}
  
/// Method which is called at the end of the calibration cycle
void 
PyWrapper::endCalibCycle(Event& evt, Env& env)
{
  call(m_endCalibCycle, evt, env);
}

/// Method which is called at the end of the run
void 
PyWrapper::endRun(Event& evt, Env& env)
{
  call(m_endRun, evt, env);
}

/// Method which is called once at the end of the job
void 
PyWrapper::endJob(Event& evt, Env& env)
{
  call(m_endJob, evt, env);
}

// call specific method
void
PyWrapper::call(PyObject* method, Event& evt, Env& env)
{
  if (not method) return;

#if 0
  PyObjPtr res(Psana::call(method, evt, env, name(), className()));
#else
  PyObjPtr res(Psana::call(method, evt, env, m_moduleName, m_moduleName)); // XXX
#endif
  if (not res) {
    PyErr_Print();
    throw ExceptionGenericPyError(ERR_LOC, "exception raised while calling Python method");
  }
}


} // namespace psana
