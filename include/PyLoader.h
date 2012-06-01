#ifndef PSANA_PYLOADER_H
#define PSANA_PYLOADER_H

#include <string>
#include <psana_python/GenericWrapper.h>

namespace psana {

extern GenericWrapper* X_loadWrapper(const std::string& name);

} // namespace psana

#endif // PSANA_PYLOADER_H
