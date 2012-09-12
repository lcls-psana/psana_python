#ifndef PSANA_CREATEWRAPPERS_H
#define PSANA_CREATEWRAPPERS_H

#include <boost/python.hpp>

namespace psana_python {

extern void createWrappers();

extern boost::python::object EventWrapperClass;
extern boost::python::object EnvWrapperClass;

}

#endif // PSANA_CREATEWRAPPERS_H
