#ifndef PSANA_PYTHONMODULE_H
#define PSANA_PYTHONMODULE_H

#include <psana/Module.h>
#include <python/Python.h>

namespace Psana {

  class PythonModule : public Module {
    // Instance of loaded Python module
    PyObject* m_instance;

    // True if env var PYANA_COMPAT is set.
    // Enables various pyana-compatible hacks.
    bool m_pyanaCompat;

    // Loaded Python methods
    PyObject* m_beginJob;
    PyObject* m_beginRun;
    PyObject* m_beginCalibCycle;
    PyObject* m_event;
    PyObject* m_endCalibCycle;
    PyObject* m_endRun;
    PyObject* m_endJob;

    // Loaded Python methods (only if m_pyanaCompat is true)
    PyObject* m_beginjob;
    PyObject* m_beginrun;
    PyObject* m_begincalibcycle;
    PyObject* m_endcalibcycle;
    PyObject* m_endrun;
    PyObject* m_endjob;

    // Method to call provided Python method with event and env args
    void call(PyObject* psana_method, PyObject* pyana_method, bool pyana_no_evt, Event& evt, Env& env);

  public:
    PythonModule(const std::string& name, PyObject* instance);
    ~PythonModule();

    // Standard module methods -- see psana/Module.h
    void beginJob(Event& evt, Env& env) { call(m_beginJob, m_beginjob, false, evt, env); }
    void beginRun(Event& evt, Env& env) { call(m_beginRun, m_beginrun, false, evt, env); }
    void beginCalibCycle(Event& evt, Env& env) { call(m_beginCalibCycle, m_begincalibcycle, false, evt, env); }
    void event(Event& evt, Env& env) { call(m_event, m_event, false, evt, env); }
    void endJob(Event& evt, Env& env) { call(m_endJob, m_endjob, true, evt, env); }
    void endCalibCycle(Event& evt, Env& env) { call(m_endCalibCycle, m_endcalibcycle, true, evt, env); }
    void endRun(Event& evt, Env& env) { call(m_endRun, m_endrun, true, evt, env); }
  };

} // namespace psana

#endif // PSANA_PYTHONMODULE_H
