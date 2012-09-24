#ifndef PSANA_EVENTWRAPPER_H
#define PSANA_EVENTWRAPPER_H

#include <boost/python.hpp>
#include <string>
#include <boost/shared_ptr.hpp>

#include <PSEvt/Event.h>

namespace psana_python {

class EventWrapper {
public:

  EventWrapper(const boost::shared_ptr<PSEvt::Event>& event) : _event(event) {}

  boost::python::object get(const std::string& key);
  boost::python::object getByType(const std::string& typeName, const std::string& detectorSourceName);
  void putBoolean(bool value, const std::string& key);
  void putList(const boost::python::list& list, const std::string& key);
  int run();
  boost::python::list keys();

private:

  boost::python::object getValue(const std::string& key, const PSEvt::EventKey& eventKey);

  boost::shared_ptr<PSEvt::Event> _event;

};

}

#endif // PSANA_EVENTWRAPPER_H
