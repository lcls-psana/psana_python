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
  public:
    EventWrapper(Event& event) : _event(event) {}

    object get(const string& key);
    object getByType(const string& typeName, const string& detectorSourceName);

    void putBoolean(bool value, string key) {
      printf("put(key=%s, %s)\n", key.c_str(), value ? "true" : "false");
      const shared_ptr<bool> v(new bool(value));
      try {
        _event.put(v, key);
      } catch (...) {
        printf("problem with put(key=%s, %s)\n", key.c_str(), value ? "true" : "false");
      }
    }

    void putList(boost::python::list list, string key) {
      boost::python::ssize_t n = boost::python::len(list);
      printf("putList(key=%s): len(list)=%d\n", key.c_str(), n);
      const shared_ptr<boost::python::list> l = boost::make_shared<boost::python::list>(list);
      _event.put(l, key);
    }

    int run();

    boost::python::list keys();
  };

}

#endif // PSANA_EVENTWRAPPER_H
