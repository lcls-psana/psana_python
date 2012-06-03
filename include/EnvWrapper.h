#ifndef PSANA_ENVWRAPPER_H
#define PSANA_ENVWRAPPER_H

#include <psana_python/EnvObjectStoreWrapper.h>
#include <string>
#include <boost/python.hpp>
#include <PSEnv/Env.h>

namespace Psana {
  using boost::python::api::object;

  class EnvWrapper {
  private:
    PSEnv::Env& _env;
    const std::string& _name;
    const std::string& _className;
  public:
    EnvWrapper(PSEnv::Env& env, const std::string& name, const std::string& className) : _env(env), _name(name), _className(className) {}
    const std::string& jobName() const { return _env.jobName(); }
    const std::string& instrument() const { return _env.instrument(); }
    const std::string& experiment() const { return _env.experiment(); }
    const unsigned expNum() const { return _env.expNum(); }
    const std::string& calibDir() const { return _env.calibDir(); }
    EnvObjectStoreWrapper configStore() { return EnvObjectStoreWrapper(_env.configStore()); }
    PSEnv::EnvObjectStore& calibStore() { return _env.calibStore(); }
    PSEnv::EpicsStore& epicsStore() { return _env.epicsStore(); }
    RootHistoManager::RootHMgr& rhmgr() { return _env.rhmgr(); }
    PSHist::HManager& hmgr() { return _env.hmgr(); }
    PSEnv::Env& getEnv() { return _env; };
    void printAllKeys();
    void printConfigKeys();
    const char* configStr(const std::string& parameter);
    std::string configStr2(const std::string& parameter, const char* _default);
    PSEvt::Source configSource(const std::string& _default);
    object getConfigByType2(const char* typeName, const char* detectorSourceName);
    object getConfigByType1(const char* typeName);
    object getConfig2(int typeId, const char* detectorSourceName);
    object getConfig1(int typeId);
  };
}

#endif // PSANA_ENVWRAPPER_H
