#ifndef PSANA_GENERICWRAPPER_H
#define PSANA_GENERICWRAPPER_H

#include "PSEvt/Event.h"
#include "PSEnv/Env.h"
#include <string>

using PSEvt::Event;
using PSEnv::Env;

namespace psana {

  class GenericWrapper {
public:
  GenericWrapper(const std::string& name) {}
  virtual ~GenericWrapper() {}
  virtual void beginJob(Event& evt, Env& env) = 0;
  virtual void beginRun(Event& evt, Env& env) = 0;
  virtual void beginCalibCycle(Event& evt, Env& env) = 0;
  virtual void event(Event& evt, Env& env) = 0;
  virtual void endCalibCycle(Event& evt, Env& env) = 0;
  virtual void endRun(Event& evt, Env& env) = 0;
  virtual void endJob(Event& evt, Env& env) = 0;
};

} // namespace GenericWrapper

#endif // PSANA_GENERICWRAPPER_H
