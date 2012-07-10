//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class EpicsPvHeaderWrapper
//
// Author List:
//   Joseph S. Barrera III
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include <psana_python/EpicsPvHeaderWrapper.h>

namespace Psana {
  using boost::python::api::object;

  static std::string typeName(int type) {
    switch (type) {
      case Epics::DBR_STRING:
        return "DBR_STRING";
      case Epics::DBR_SHORT:
        return "DBR_SHORT";
      case Epics::DBR_FLOAT:
        return "DBR_FLOAT";
      case Epics::DBR_ENUM:
        return "DBR_ENUM";
      case Epics::DBR_CHAR:
        return "DBR_CHAR";
      case Epics::DBR_LONG:
        return "DBR_LONG";
      case Epics::DBR_DOUBLE:
        return "DBR_DOUBLE";
      case Epics::DBR_STS_STRING:
        return "DBR_STS_STRING";
      case Epics::DBR_STS_SHORT:
        return "DBR_STS_SHORT";
      case Epics::DBR_STS_FLOAT:
        return "DBR_STS_FLOAT";
      case Epics::DBR_STS_ENUM:
        return "DBR_STS_ENUM";
      case Epics::DBR_STS_CHAR:
        return "DBR_STS_CHAR";
      case Epics::DBR_STS_LONG:
        return "DBR_STS_LONG";
      case Epics::DBR_STS_DOUBLE:
        return "DBR_STS_DOUBLE";
      case Epics::DBR_TIME_STRING:
        return "DBR_TIME_STRING";
      case Epics::DBR_TIME_SHORT:
        return "DBR_TIME_SHORT";
      case Epics::DBR_TIME_FLOAT:
        return "DBR_TIME_FLOAT";
      case Epics::DBR_TIME_ENUM:
        return "DBR_TIME_ENUM";
      case Epics::DBR_TIME_CHAR:
        return "DBR_TIME_CHAR";
      case Epics::DBR_TIME_LONG:
        return "DBR_TIME_LONG";
      case Epics::DBR_TIME_DOUBLE:
        return "DBR_TIME_DOUBLE";
      case Epics::DBR_GR_STRING:
        return "DBR_GR_STRING";
      case Epics::DBR_GR_SHORT:
        return "DBR_GR_SHORT";
      case Epics::DBR_GR_FLOAT:
        return "DBR_GR_FLOAT";
      case Epics::DBR_GR_ENUM:
        return "DBR_GR_ENUM";
      case Epics::DBR_GR_CHAR:
        return "DBR_GR_CHAR";
      case Epics::DBR_GR_LONG:
        return "DBR_GR_LONG";
      case Epics::DBR_GR_DOUBLE:
        return "DBR_GR_DOUBLE";
      case Epics::DBR_CTRL_STRING:
        return "DBR_CTRL_STRING";
      case Epics::DBR_CTRL_SHORT:
        return "DBR_CTRL_SHORT";
      case Epics::DBR_CTRL_FLOAT:
        return "DBR_CTRL_FLOAT";
      case Epics::DBR_CTRL_ENUM:
        return "DBR_CTRL_ENUM";
      case Epics::DBR_CTRL_CHAR:
        return "DBR_CTRL_CHAR";
      case Epics::DBR_CTRL_LONG:
        return "DBR_CTRL_LONG";
      case Epics::DBR_CTRL_DOUBLE:
        return "DBR_CTRL_DOUBLE";
      default:
        char buf[64];
        sprintf(buf, "Epics::DBR_%d\n", type);
        return buf;
    }
  }

  object EpicsPvHeaderWrapper::value(int index) {
    if (not _header.get()) {
      fprintf(stderr, "value() called on empty object\n");
      return object();
    }
    const Psana::Epics::EpicsPvHeader* p = _header.get();
    int type = p->dbrType();
    switch (type) {
      case Epics::DBR_TIME_STRING:
        return object(((Epics::EpicsPvTimeString *) p)->value(index));
      case Epics::DBR_TIME_SHORT:
        return object(((Epics::EpicsPvTimeShort *) p)->value(index));
      case Epics::DBR_TIME_FLOAT:
        return object(((Epics::EpicsPvTimeFloat *) p)->value(index));
      case Epics::DBR_TIME_ENUM:
        return object(((Epics::EpicsPvTimeEnum *) p)->value(index));
      case Epics::DBR_TIME_CHAR:
        return object(((Epics::EpicsPvTimeChar *) p)->value(index));
      case Epics::DBR_TIME_LONG:
        return object(((Epics::EpicsPvTimeLong *) p)->value(index));
      case Epics::DBR_TIME_DOUBLE:
        return object(((Epics::EpicsPvTimeDouble *) p)->value(index));
      case Epics::DBR_CTRL_STRING:
        return object(((Epics::EpicsPvCtrlString *) p)->value(index));
      case Epics::DBR_CTRL_SHORT:
        return object(((Epics::EpicsPvCtrlShort *) p)->value(index));
      case Epics::DBR_CTRL_FLOAT:
        return object(((Epics::EpicsPvCtrlFloat *) p)->value(index));
      case Epics::DBR_CTRL_ENUM:
        return object(((Epics::EpicsPvCtrlEnum *) p)->value(index));
      case Epics::DBR_CTRL_CHAR:
        return object(((Epics::EpicsPvCtrlChar *) p)->value(index));
      case Epics::DBR_CTRL_LONG:
        return object(((Epics::EpicsPvCtrlLong *) p)->value(index));
      case Epics::DBR_CTRL_DOUBLE:
        return object(((Epics::EpicsPvCtrlDouble *) p)->value(index));
      default:
        fprintf(stderr, "value(%d) called on unsupported type %s\n", index, typeName(type).c_str());
        return object();
    }
  }

  int16_t EpicsPvHeaderWrapper::precision() {
    if (not _header.get()) {
      fprintf(stderr, "precision() called on empty object\n");
    }
    const Psana::Epics::EpicsPvHeader* p = _header.get();
    int type = p->dbrType();
    switch (type) {
      case Epics::DBR_CTRL_FLOAT:
        return ((Epics::EpicsPvCtrlFloat *) p)->dbr().precision();
      case Epics::DBR_CTRL_DOUBLE:
        return ((Epics::EpicsPvCtrlDouble *) p)->dbr().precision();
      default:
        fprintf(stderr, "precision() called on unsupported type %s\n", typeName(type).c_str());
        return 0;
    }
  }

  std::string EpicsPvHeaderWrapper::units() {
    if (not _header.get()) {
      fprintf(stderr, "units() called on empty object\n");
    }
    const Psana::Epics::EpicsPvHeader* p = _header.get();
    int type = p->dbrType();
    switch (type) {
      case Epics::DBR_CTRL_SHORT:
        return ((Epics::EpicsPvCtrlShort *) p)->dbr().units();
      case Epics::DBR_CTRL_FLOAT:
        return ((Epics::EpicsPvCtrlFloat *) p)->dbr().units();
      case Epics::DBR_CTRL_CHAR:
        return ((Epics::EpicsPvCtrlChar *) p)->dbr().units();
      case Epics::DBR_CTRL_LONG:
        return ((Epics::EpicsPvCtrlLong *) p)->dbr().units();
      case Epics::DBR_CTRL_DOUBLE:
        return ((Epics::EpicsPvCtrlDouble *) p)->dbr().units();
      default:
        fprintf(stderr, "units() called on unsupported type %s\n", typeName(type).c_str());
        return "";
    }
  }

  static std::string to_string(int limit) {
    char buf[1024];
    sprintf(buf, "%d", limit);
    return std::string(buf);
  }
  
  static std::string to_string(double limit) {
    char buf[1024];
    sprintf(buf, "%g", limit);
    return std::string(buf);
  }
  
#define DECL_LIMIT_METHOD(method_name)                                \
  std::string EpicsPvHeaderWrapper:: method_name () {                 \
    if (not _header.get()) {                                          \
      fprintf(stderr, #method_name "() called on empty object\n");    \
      return "???";                                                   \
    }                                                                 \
    const Psana::Epics::EpicsPvHeader* p = _header.get();             \
    int type = p->dbrType();                                          \
    switch (type) {                                                   \
      case Epics::DBR_CTRL_SHORT:  return to_string(((Epics::EpicsPvCtrlShort *)  p)->dbr(). method_name ()); \
      case Epics::DBR_CTRL_FLOAT:  return to_string(((Epics::EpicsPvCtrlFloat *)  p)->dbr(). method_name ()); \
      case Epics::DBR_CTRL_CHAR:   return to_string(((Epics::EpicsPvCtrlChar *)   p)->dbr(). method_name ()); \
      case Epics::DBR_CTRL_LONG:   return to_string(((Epics::EpicsPvCtrlLong *)   p)->dbr(). method_name ()); \
      case Epics::DBR_CTRL_DOUBLE: return to_string(((Epics::EpicsPvCtrlDouble *) p)->dbr(). method_name ()); \
      default:                                                          \
        fprintf(stderr, #method_name "() called on unsupported type %s\n", typeName(type).c_str()); \
        return "???";                                                   \
    }                                                                   \
  }

DECL_LIMIT_METHOD(upper_disp_limit)
DECL_LIMIT_METHOD(lower_disp_limit)
DECL_LIMIT_METHOD(upper_alarm_limit)
DECL_LIMIT_METHOD(upper_warning_limit)
DECL_LIMIT_METHOD(lower_warning_limit)
DECL_LIMIT_METHOD(lower_alarm_limit)
DECL_LIMIT_METHOD(upper_ctrl_limit)
DECL_LIMIT_METHOD(lower_ctrl_limit)
}

