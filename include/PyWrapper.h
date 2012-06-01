#ifndef PSANA_PYWRAPPER_H
#define PSANA_PYWRAPPER_H

#include "psana_python/GenericWrapper.h"
#include "python/Python.h"
#include "PSEvt/Event.h"
#include "PSEnv/Env.h"

using PSEvt::Event;
using PSEnv::Env;
using psana::GenericWrapper;

namespace psana {

class PyWrapper : public GenericWrapper {
public:

  // Default constructor
  PyWrapper(const std::string& name, PyObject* instance) ;

  // Destructor
  virtual ~PyWrapper() ;

  /// Method which is called once at the beginning of the job
  virtual void beginJob(Event& evt, Env& env);
  
  /// Method which is called at the beginning of the run
  virtual void beginRun(Event& evt, Env& env);
  
  /// Method which is called at the beginning of the calibration cycle
  virtual void beginCalibCycle(Event& evt, Env& env);
  
  /// Method which is called with event data, this is the only required 
  /// method, all other methods are optional
  virtual void event(Event& evt, Env& env);
  
  /// Method which is called at the end of the calibration cycle
  virtual void endCalibCycle(Event& evt, Env& env);

  /// Method which is called at the end of the run
  virtual void endRun(Event& evt, Env& env);

  /// Method which is called once at the end of the job
  virtual void endJob(Event& evt, Env& env);

protected:

  // call specific method
  void call(PyObject* method, Event& evt, Env& env);

private:

  // fail if a pyana-style method is defined
  void checkMethodName(char* pyanaMethodName, char* psanaMethodName);

  const std::string m_moduleName;
  PyObject* m_instance;   // Instance of Python class
  PyObject* m_beginJob;
  PyObject* m_beginRun;
  PyObject* m_beginCalibCycle;
  PyObject* m_event;
  PyObject* m_endCalibCycle;
  PyObject* m_endRun;
  PyObject* m_endJob;
};

} // namespace psana

#endif // PSANA_PYWRAPPER_H
