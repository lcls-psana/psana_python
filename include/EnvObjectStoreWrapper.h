#ifndef PSANA_ENVOBJECTSTOREWRAPPER_H
#define PSANA_ENVOBJECTSTOREWRAPPER_H

#include <boost/python.hpp>
#include <string>
#include <boost/shared_ptr.hpp>

#include <PSEnv/EnvObjectStore.h>

namespace psana_python {

class EnvObjectStoreWrapper {
public:

  EnvObjectStoreWrapper(const boost::shared_ptr<PSEnv::EnvObjectStore>& store) : _store(store) {}

  boost::python::object get(const std::string& typeName, const std::string& sourceName);
  boost::python::list keys();

private:
  boost::shared_ptr<PSEnv::EnvObjectStore> _store;
};
}

#endif // PSANA_ENVOBJECTSTOREWRAPPER_H
