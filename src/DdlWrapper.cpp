#include <psddl_pyana/DdlWrapper.h>

using boost::python::numeric::array;

namespace Psana {
  static bool did_init = false;

  static void init() {
    if (! did_init) {
      // Required initialization of numpy array support
      _import_array();
      array::set_module_and_type("numpy", "ndarray");
      did_init = true;
    }
  }

  PyObject* ndConvert(const unsigned ndim, const unsigned* shape, int ptype, void* data) {
    init();
    npy_intp dims[ndim];
    for (unsigned i = 0; i < ndim; i++) {
      dims[i] = shape[i];
    }
    return PyArray_SimpleNewFromData(ndim, dims, ptype, data);
  }
} // namespace Psana
