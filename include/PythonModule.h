#ifndef PSANA_PYTHONMODULE_H
#define PSANA_PYTHONMODULE_H
//--------------------------------------------------------------------------
// File and Version Information:
//      $Id$
//
// Description:
//      Class Exceptions.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------
#include <python/Python.h>
#include <boost/shared_ptr.hpp>

//----------------------
// Base Class Headers --
//----------------------
#include "psana/Module.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "pytools/make_pyshared.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//              ---------------------
//              -- Class Interface --
//              ---------------------

namespace psana_python {

/// @addtogroup psana_python

/**
 *  @ingroup psana_python
 *
 * @brief Base class for exceptions for psana package.
 *
 *  This software was developed for the LCLS project.  If you use all or
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */
class PythonModule : public psana::Module {
public:

  /// Codes that can be returned from event() method which will be
  /// translated into skip()/stop()/terminate(). This is pyana
  /// compatibility feature but it may be useful to support it
  /// in "native" psana. The numbers must be identical to what is
  /// defined in pyana python module.
  enum { Normal = 0, Skip = 1, Stop = 2, Terminate = 3 };

  PythonModule(const std::string& name, PyObject* instance);

  virtual ~PythonModule();

  // Standard module methods -- see psana/Module.h
  virtual void beginJob(PSEvt::Event& evt, PSEnv::Env& env) {
    call(m_methods[MethBeginJob].get(), false, evt, env);
  }

  virtual void beginRun(PSEvt::Event& evt, PSEnv::Env& env) {
    call(m_methods[MethBeginRun].get(), false, evt, env);
  }

  virtual void beginCalibCycle(PSEvt::Event& evt, PSEnv::Env& env) {
    call(m_methods[MethBeginScan].get(), false, evt, env);
  }

  virtual void event(PSEvt::Event& evt, PSEnv::Env& env) {
    call(m_methods[MethEvent].get(), false, evt, env);
  }

  virtual void endCalibCycle(PSEvt::Event& evt, PSEnv::Env& env) {
    call(m_methods[MethEndScan].get(), m_pyanaCompat, evt, env);
  }

  virtual void endRun(PSEvt::Event& evt, PSEnv::Env& env) {
    call(m_methods[MethEndRun].get(), m_pyanaCompat, evt, env);
  }

  virtual void endJob(PSEvt::Event& evt, PSEnv::Env& env) {
    call(m_methods[MethEndJob].get(), false, evt, env);
  }

  // need to expose few protected methods to allow python code access to them
  using Configurable::name;
  using Configurable::className;
  using Configurable::config;
  using Configurable::configStr;
  using Configurable::configSrc;
  using Configurable::configList;
  using psana::Module::skip;
  using psana::Module::stop;
  using psana::Module::terminate;

private:
  /**
   * class to ensure the GIL is locked and then restore it.
   * Intended to be used before/after calling the PythonModule functions.
   */
  class GILLocker {
    public:
      GILLocker() : m_gilState(PyGILState_Ensure()) {}
    
      ~GILLocker() {
        PyGILState_Release(m_gilState);
      }
    
    private:
      PyGILState_STATE m_gilState;
  };
  
  /**
   *   Method to call provided Python method with event and env args.
   *
   *   @param[in] psana_method  Python method for psana-style modules
   *   @param[in] psana_method  Python method for psana-style modules
   */
  void call(PyObject* method, bool pyana_optional_evt, PSEvt::Event& evt, PSEnv::Env& env);

  enum { MethBeginJob, MethBeginRun, MethBeginScan, MethEvent,
    MethEndScan, MethEndRun, MethEndJob, NumMethods };

  pytools::pyshared_ptr m_instance;      // Instance of loaded Python module
  bool m_pyanaCompat;        // True if env var PYANA_COMPAT is set.
                             // Enables various pyana-compatible hacks.
  pytools::pyshared_ptr m_methods[NumMethods];  // method objects

};

} // namespace psana_python

#endif // PSANA_PYTHONMODULE_H
