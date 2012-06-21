#ifndef PSANA_ENVWRAPPER_H
#define PSANA_ENVWRAPPER_H

#include <psana_python/EnvObjectStoreWrapper.h>
#include <string>
#include <boost/python.hpp>
#include <PSEnv/Env.h>
#include <ConfigSvc/ConfigSvc.h>

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
    void assert_psana() {}
    bool subprocess() { return 0; } // XXX What is this?

    boost::python::list keys() {
      boost::python::list l;
      ConfigSvc::ConfigSvc cfg;
      list<string> keys = cfg.getKeys(_name);
      list<string>::iterator it;
      for (it = keys.begin(); it != keys.end(); it++) {
        string& key = *it;
        l.append(key);
      }
      return l;
    }

    string configStr(const string& parameter, const string& _default) {
      ConfigSvc::ConfigSvc cfg;
      try {
        return cfg.getStr(_name, parameter);
      } catch (const ConfigSvc::ExceptionMissing& ex) {
        return cfg.getStr(_className, parameter, _default);
      }
    }
  };
}

#endif // PSANA_ENVWRAPPER_H
