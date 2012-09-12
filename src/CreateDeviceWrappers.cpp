#include <psana_python/CreateDeviceWrappers.h>

namespace psddl_python {
#undef DECL
#define DECL(ns) namespace ns { extern void createWrappers(); }
#include "WrapperList.txt"
}

namespace psana_python {
void createDeviceWrappers() {

#undef DECL
#define DECL(ns) psddl_python::ns::createWrappers();
#include "WrapperList.txt"

}
} // namespace Psana
