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
  using std::list;
  using std::string;

  class EventWrapper {
  private:
    Event& _event;
  public:
    EventWrapper(Event& event) : _event(event) {}

    object get(const string& key) {
      printf("get(key=%s)\n", key.c_str());
      shared_ptr<string> s(_event.get(key));
      if (s.get()) {
        string& ss = *s;
        printf("get(%s) = %s\n", key.c_str(), ss.c_str());
        return object(s);
      }
      shared_ptr<bool> b(_event.get(key));
      if (b.get()) {
        bool bb = *b;
        printf("get(%s) = %s\n", key.c_str(), (bb ? "true" : "false"));
        return object(bb);
      }
      shared_ptr<boost::python::list> l(_event.get(key));
      if (l.get()) {
        boost::python::list ll = *l;
        printf("get(%s): is a list\n", key.c_str());
        return object(ll);
      }
      printf("WARNING: get(%s) found nothing of a known type\n", key.c_str());
      return object();
    }

    object getByType(const string& typeName, Source& detectorSource);
    object getByTypeId(int typeId, const string& detectorSourceName);

    void putBoolean(bool value, string key) {
      printf("put(key=%s, %s)\n", key.c_str(), value ? "true" : "false");
      const shared_ptr<bool> v(new bool(value));
      _event.put(v, key);
    }

    void putList(boost::python::list list, string key) {
      boost::python::ssize_t n = boost::python::len(list);
      printf("putList(key=%s): len(list)=%d\n", key.c_str(), n);
      const shared_ptr<boost::python::list> l = boost::make_shared<boost::python::list>(list);
      _event.put(l, key);
    }

    int run();

    list<string> getAllKeys();
  };

}

#endif // PSANA_EVENTWRAPPER_H
