#ifndef PSANA_PYWRAPPER_H
#define PSANA_PYWRAPPER_H 1

#include "EvtGetter.h"
#include "EnvGetter.h"

#include <string>
#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <numpy/arrayobject.h>


namespace Psana {
  extern void createWrappers();
  extern boost::shared_ptr<PyObject> call(PyObject* method, PSEvt::Event& evt, PSEnv::Env& env, const std::string& name, const std::string& className);
}

#endif // PSANA_PYWRAPPER_H
