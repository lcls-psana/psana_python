#ifndef PSANA_CREATEWRAPPERS_H
#define PSANA_CREATEWRAPPERS_H

#include <boost/python.hpp>

namespace Psana {
  using boost::python::api::object;
  extern void createWrappers();
  extern object EventWrapperClass;
  extern object EnvWrapperClass;
}

#endif // PSANA_CREATEWRAPPERS_H
