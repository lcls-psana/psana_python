#ifndef PSANA_ENVOBJECTSTOREWRAPPER_H
#define PSANA_ENVOBJECTSTOREWRAPPER_H

#include <string>
#include <boost/python.hpp>
#include <PSEnv/Env.h>

namespace Psana {
  using boost::python::api::object;

  class EnvObjectStoreWrapper {
  private:
    PSEnv::EnvObjectStore& _store;
  public:
    EnvObjectStoreWrapper(PSEnv::EnvObjectStore& store) : _store(store) {}
    object get(const string& typeName, const string& sourceName);
    boost::python::list keys();
  };
}

#endif // PSANA_ENVOBJECTSTOREWRAPPER_H
