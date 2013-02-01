#include "psana_python/EventWrapper.h"

#include <cxxabi.h>
#include <string>
#include <boost/python.hpp>

#include "PSEvt/Event.h"
#include "PSEvt/EventId.h"
#include "PSEvt/Exceptions.h"
#include "psana_python/EventId.h"
#include "psana_python/EventKey.h"
#include "psddl_python/GetterMap.h"

using boost::shared_ptr;
using boost::python::object;
using psddl_python::GetterMap;
using psddl_python::Getter;

namespace {

std::string
getDemangledKeyTypeName(const PSEvt::EventKey& eventKey)
{
    int status;
    const char* mangledTypeName = eventKey.typeinfo()->name();
    const char* unmangledTypeName = abi::__cxa_demangle(mangledTypeName, 0, 0, &status);
    if (status == 0 && unmangledTypeName) {
      //printf("demangled %s -> %s\n", mangledTypeName, unmangledTypeName);
      return unmangledTypeName;
    }
    fprintf(stderr, "error: get('%s'): could not demangle type '%s'\n", eventKey.key().c_str(), mangledTypeName);
    return mangledTypeName;
  }

}

namespace psana_python {

void
EventWrapper::putBoolean(bool value, const std::string& key)
{
  //printf("put('%s', %s)\n", key.c_str(), value ? "true" : "false");
  const shared_ptr<bool> v(new bool(value));
  try {
    _event->put(v, key);
  } catch (PSEvt::ExceptionDuplicateKey e) {
    // XtcExplorer will often set the same key repeatedly.
    _event->remove<bool>(key);
    _event->put(v, key);
  }
}

void
EventWrapper::putList(const boost::python::list& list, const std::string& key)
{
  //boost::python::ssize_t n = boost::python::len(list);
  //printf("put('%s', list(len=%d))\n", key.c_str(), n);
  const shared_ptr<boost::python::list> l = boost::make_shared<boost::python::list>(list);
  _event->put(l, key);
}

object
EventWrapper::getValue(const std::string& key, const PSEvt::EventKey& eventKey)
{
  std::string typeName = getDemangledKeyTypeName(eventKey);
  if (typeName == "bool") {
    shared_ptr<bool> result(_event->get(key));
    bool value = *result;
    //printf("get('%s') -> %s\n", key.c_str(), value ? "true" : "false");
    return object(value);
  }
  if (typeName == "boost::python::list") {
    shared_ptr<boost::python::list> result(_event->get(key));
    boost::python::list l = *result;
    //boost::python::ssize_t len = boost::python::len(l);
    //printf("get('%s') -> list(len=%d)\n", key.c_str(), boost::python::len(l));
    return object(l);
  }

  fprintf(stderr, "**************************************** get('%s'): unknown type %s\n\n\n\n\n", key.c_str(), typeName.c_str());
#if 1
  exit(1);
#endif
  return object();
}

object
EventWrapper::get(const std::string& key)
{
  PSEvt::Event::GetResultProxy proxy(_event->get(key));
  std::list<PSEvt::EventKey> keys;
  proxy.m_dict->keys(keys, PSEvt::Source(PSEvt::Source::null));
  std::list<PSEvt::EventKey>::iterator it;
  for (it = keys.begin(); it != keys.end(); it++) {
    const PSEvt::EventKey& eventKey = *it;
    if (eventKey.key() == key) {
      return getValue(key, eventKey);
    }
  }
  // nothing found for key
  return object();
}

object
EventWrapper::getByType(const std::string& typeName, const std::string& detectorSourceName)
{
  if (typeName == "PSEvt::EventId") {
    // special case for EventId, this is a temporary hack until I redesign stuff
    const shared_ptr<PSEvt::EventId> eventId = _event->get();
    if (eventId) {
      PyObject* eid = EventId::PyObject_FromCpp(eventId);
      return object(boost::python::handle<PyObject>(eid));
    } else {
      return object();
    }
  }
  PSEvt::Source source = (detectorSourceName == "") ? PSEvt::Source() : PSEvt::Source(detectorSourceName);

  // find a getter for this name
  GetterMap& gmap = GetterMap::instance();
  boost::shared_ptr<Getter> getter = gmap.getGetter(typeName);
  if (getter) {
    boost::shared_ptr<void> vdata = _event->proxyDict()->get(&getter->typeinfo(), source, std::string(), 0);
    return getter->convert(vdata);
  }

  // try to find matching type from a range of types
  const GetterMap::NameList& names = gmap.getTemplate(typeName);
  for (GetterMap::NameList::const_iterator it = names.begin(); it != names.end(); ++ it) {
    boost::shared_ptr<Getter> getter = gmap.getGetter(*it);
    if (getter) {
      boost::shared_ptr<void> vdata = _event->proxyDict()->get(&getter->typeinfo(), source, std::string(), 0);
      if (vdata) {
        return getter->convert(vdata);
      }
    }
  }
  return object();
}

boost::python::list
EventWrapper::keys(const PSEvt::Source& src)
{
  const std::list<PSEvt::EventKey>& keys = _event->keys(src);

  boost::python::list pkeys;
  for (std::list<PSEvt::EventKey>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    PyObject* key = EventKey::PyObject_FromCpp(*it);
    pkeys.append(boost::python::handle<PyObject>(key));
  }
  return pkeys;
}

int
EventWrapper::run()
{
  const shared_ptr<PSEvt::EventId> eventId = _event->get();
  return eventId ? eventId->run() : -1;
}

}
