#ifndef PSANA_EVENTWRAPPER_H
#define PSANA_EVENTWRAPPER_H

#include <string>
#include <list>
#include <boost/python.hpp>
#include <PSEvt/Event.h>

namespace Psana {
  using boost::python::api::object;
  using std::list;
  using std::string;
  using PSEvt::Event;
  using PSEvt::Source;

  class EventWrapper {
  private:
    Event& _event;
  public:
    EventWrapper(Event& event) : _event(event) {}

    boost::shared_ptr<string> get(const string& key) {
      return boost::shared_ptr<string>(_event.get(key));
    }

    object getByType(const string& typeName, Source& detectorSource);
    object getByTypeId(int typeId, const string& detectorSourceName);
    int run();

    list<string> getAllKeys();
  };

}

#endif // PSANA_EVENTWRAPPER_H
