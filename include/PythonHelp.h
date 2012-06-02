#ifndef PSANA_PYTHONHELP_H
#define PSANA_PYTHONHELP_H

#include <python/Python.h>
#include <PSEvt/Event.h>
#include <PSEnv/Env.h>

namespace Psana {
  extern boost::shared_ptr<PyObject> call(PyObject* method,
                                          PSEvt::Event& evt,
                                          PSEnv::Env& env,
                                          const std::string& name,
                                          const std::string& className);
}

#endif // PSANA_PYTHONHELP_H
