#ifndef PSANA_ENVOBJECTSTOREWRAPPER_H
#define PSANA_ENVOBJECTSTOREWRAPPER_H

#include <string>
#include <sstream>
#include <boost/python.hpp>
#include <PSEnv/Env.h>
#include <psddl_python/EnvGetter.h>

namespace Psana {
  using boost::python::api::object;

  // Need wrapper because EnvObjectStore is boost::noncopyable
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
