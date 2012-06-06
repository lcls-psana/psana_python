#ifndef PSANA_ENVWRAPPER_H
#define PSANA_ENVWRAPPER_H

#include <psana_python/EnvObjectStoreWrapper.h>
#include <string>
#include <boost/python.hpp>
#include <PSEnv/Env.h>

namespace Psana {
  using boost::python::api::object;
  using std::string;

  class EnvWrapper {
  private:
    PSEnv::Env& _env;
    const string& _name;
    const string& _className;
  public:
    EnvWrapper(PSEnv::Env& env, const string& name, const string& className) : _env(env), _name(name), _className(className) {}
    const string& jobName() const { return _env.jobName(); }
    const string& instrument() const { return _env.instrument(); }
    const string& experiment() const { return _env.experiment(); }
    const unsigned expNum() const { return _env.expNum(); }
    const string& calibDir() const { return _env.calibDir(); }
    EnvObjectStoreWrapper configStore() { return EnvObjectStoreWrapper(_env.configStore()); }
    PSEnv::EnvObjectStore& calibStore() { return _env.calibStore(); }
    PSEnv::EpicsStore& epicsStore() { return _env.epicsStore(); }
    RootHistoManager::RootHMgr& rhmgr() { return _env.rhmgr(); }
    PSHist::HManager& hmgr() { return _env.hmgr(); }
    PSEnv::Env& getEnv() { return _env; };
    void printAllKeys();
    void printConfigKeys();
#if 0
    const char* configStr(const string& parameter);
    string configStr2(const string& parameter, const char* _default);
    PSEvt::Source configSource(const string& _default);
#else
    string configStr(const string& parameter);
    string configStr2(const string& parameter, const string& _default);
    PSEvt::Source convertToSource(const string& value);
#endif
    object getConfigByType2(const char* typeName, const char* detectorSourceName);
    object getConfigByType1(const char* typeName);
    object getConfig2(int typeId, const char* detectorSourceName);
    object getConfig1(int typeId);
    static object getBoostPythonClass();
  };
}

#endif // PSANA_ENVWRAPPER_H
