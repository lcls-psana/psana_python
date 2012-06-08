#ifndef PSANA_ENVWRAPPER_H
#define PSANA_ENVWRAPPER_H

#include <psana_python/EnvObjectStoreWrapper.h>
#include <string>
#include <boost/python.hpp>
#include <PSEnv/Env.h>

namespace Psana {
  using boost::python::api::object;
  using std::string;
  using PSEnv::Env;
  using PSEnv::EnvObjectStore;
  using PSEnv::EpicsStore;
  using PSEvt::Source;

  class EnvWrapper {
  private:
    Env& _env;
    const string& _name;
    const string& _className;
  public:
    EnvWrapper(Env& env, const string& name, const string& className) : _env(env), _name(name), _className(className) {}
    const string& jobName() const { return _env.jobName(); }
    const string& instrument() const { return _env.instrument(); }
    const string& experiment() const { return _env.experiment(); }
    const unsigned expNum() const { return _env.expNum(); }
    const string& calibDir() const { return _env.calibDir(); }
    EnvObjectStoreWrapper configStore() { return EnvObjectStoreWrapper(_env.configStore()); }
    EnvObjectStore& calibStore() { return _env.calibStore(); }
    EpicsStore& epicsStore() { return _env.epicsStore(); }
    RootHistoManager::RootHMgr& rhmgr() { return _env.rhmgr(); }
    PSHist::HManager& hmgr() { return _env.hmgr(); }
    Env& getEnv() { return _env; };
    void printAllKeys();
    void printConfigKeys();
    string configStr(const string& parameter, const string& _default);
    object get(const char* typeName, const char* detectorSourceName);
    object getConfig(int typeId, const char* detectorSourceName);
    void assert_psana() {}
    string configStr1(const string& parameter) { return configStr(parameter, ""); }
    object get1(const char* typeName) { return get(typeName, ""); }
    object getConfig1(int typeId) { return getConfig(typeId, "ProcInfo()"); }
    Source convertToSource(const string& value) { return (value == "" ? Source() : Source(value)); }
    string getTypeNameForId(int typeId) { return GenericGetter::getTypeNameForId(typeId); }
    bool subprocess() { return 0; } // XXX WITIDE
  };
}

#endif // PSANA_ENVWRAPPER_H
