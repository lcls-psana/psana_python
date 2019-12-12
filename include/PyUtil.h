#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

/*
 * Temporary file as part of 2to3 transition
 *
 */

std::string PyString_AsString_Compatible(PyObject* obj) {
#ifdef IS_PY3K
  std::string result = "";
  if (PyUnicode_Check(obj)) {
    PyObject * temp_bytes = PyUnicode_AsEncodedString(obj, "UTF-8", "strict"); // Owned reference
    if (temp_bytes != NULL) {
      char* my_result = PyBytes_AS_STRING(temp_bytes); // Borrowed pointer
      my_result = strdup(my_result);
      Py_DECREF(temp_bytes);
      result += my_result;
    }
  }
  else if (PyBytes_Check(obj)) {
    char* my_result = PyBytes_AS_STRING(obj); // Borrowed pointer
    my_result = strdup(my_result);
    result += my_result;
  }
#else
  std::string result = PyString_AsString(obj);
#endif
  return result;
}
