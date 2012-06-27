#ifndef PSANA_EVENTWRAPPER_H
#define PSANA_EVENTWRAPPER_H

#include <string>
#include <list>
#include <boost/python.hpp>
#include <PSEvt/Event.h>

namespace Psana {
  using PSEvt::Event;
  using PSEvt::EventKey;
  using PSEvt::Source;
  using boost::python::api::object;
  using boost::shared_ptr;
  using std::string;

  class EventWrapper {
  private:
    Event& _event;
    object getValue(const string& key, const EventKey& eventKey);
  public:
    EventWrapper(Event& event) : _event(event) {}
    object get(const string& key);
    object getByType(const string& typeName, const string& detectorSourceName);
    void putBoolean(bool value, string key);
    void putList(boost::python::list list, string key);
    int run();
    boost::python::list keys();
  };

}

#endif // PSANA_EVENTWRAPPER_H
