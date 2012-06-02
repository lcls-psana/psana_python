#ifndef PSANA_PYTHONMODULE_H
#define PSANA_PYTHONMODULE_H

#include <psana/Module.h>
#include <python/Python.h>

namespace psana {

  class PythonModule : public Module {
    // Instance of loaded Python module
    PyObject* m_instance;

    // Loaded Python methods
    PyObject* m_beginJob;
    PyObject* m_beginRun;
    PyObject* m_beginCalibCycle;
    PyObject* m_event;
    PyObject* m_endCalibCycle;
    PyObject* m_endRun;
    PyObject* m_endJob;

    // Method to call provided Python method with event and env args
    void call(PyObject* method, PSEvt::Event& event, PSEnv::Env& env);

  public:
    PythonModule(const std::string& name, PyObject* instance);
    ~PythonModule();

    // Standard module methods -- see psana/Module.h
    void beginJob(Event& evt, Env& env) { call(m_beginJob, evt, env); }
    void beginRun(Event& evt, Env& env) { call(m_beginRun, evt, env); }
    void beginCalibCycle(Event& evt, Env& env) { call(m_beginCalibCycle, evt, env); }
    void event(Event& evt, Env& env) { call(m_event, evt, env); }
    void endCalibCycle(Event& evt, Env& env) { call(m_endCalibCycle, evt, env); }
    void endRun(Event& evt, Env& env) { call(m_endRun, evt, env); }
    void endJob(Event& evt, Env& env) { call(m_endJob, evt, env); }
  };

} // namespace psana

#endif // PSANA_PYTHONMODULE_H
