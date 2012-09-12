#ifndef PSANA_ENVWRAPPER_H
#define PSANA_ENVWRAPPER_H

#include <string>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include <psana_python/EnvObjectStoreWrapper.h>
#include <PSEnv/Env.h>
#include <ConfigSvc/ConfigSvc.h>

namespace psana_python {

class EnvWrapper {
public:

  EnvWrapper(const boost::shared_ptr<PSEnv::Env>& env, const string& name, const string& className)
    : _env(env), _name(name), _className(className) {}

  const std::string& jobName() const { return _env->jobName(); }
  const std::string& instrument() const { return _env->instrument(); }
  const std::string& experiment() const { return _env->experiment(); }
  const unsigned expNum() const { return _env->expNum(); }
  const std::string& calibDir() const { return _env->calibDir(); }
  EnvObjectStoreWrapper configStore() { return EnvObjectStoreWrapper(_env->configStore().shared_from_this()); }
  boost::python::object getConfig(const std::string& typeName, const std::string& sourceName) { return configStore().get(typeName, sourceName); }
  PSEnv::EnvObjectStore& calibStore() { return _env->calibStore(); }
  PSEnv::EpicsStore& epicsStore() { return _env->epicsStore(); }
  RootHistoManager::RootHMgr& rhmgr() { return _env->rhmgr(); }
  PSHist::HManager& hmgr() { return _env->hmgr(); }
  PSEnv::Env& getEnv() { return *_env; };
  void assert_psana() {}
  bool subprocess() { return 0; } // XXX What is this?
  boost::python::list keys();
  std::string configStr(const std::string& parameter, const std::string& _default);

private:

  boost::shared_ptr<PSEnv::Env> _env;
  const string _name;
  const string _className;
};

}

#endif // PSANA_ENVWRAPPER_H
