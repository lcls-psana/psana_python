#ifndef PSANA_EPICSPVHEADERWRAPPER_H
#define PSANA_EPICSPVHEADERWRAPPER_H

#include <string>
#include <boost/python.hpp>
#include <psddl_psana/epics.ddl.h>

namespace Psana {
  class EpicsPvHeaderWrapper {
  private:
    boost::shared_ptr<Psana::Epics::EpicsPvHeader> _header;
  public:
    EpicsPvHeaderWrapper(boost::shared_ptr<Psana::Epics::EpicsPvHeader> header) : _header(header) {}
    int pvId() { return _header->pvId(); }
    int dbrType() { return _header->dbrType(); }
    int numElements() { return _header->numElements(); }
    void print() { _header->print(); }
    int isCtrl() { return _header->isCtrl(); }
    int isTime() { return _header->isTime(); }
    int status() { return _header->status(); }
    int severity() { return _header->severity(); }
    boost::python::api::object value(int index);
    boost::python::api::object value0() { return value(0); }
    int16_t precision();
    std::string units();
    double upper_disp_limit();
    double lower_disp_limit();
    double upper_alarm_limit();
    double upper_warning_limit();
    double lower_warning_limit();
    double lower_alarm_limit();
    double upper_ctrl_limit();
    double lower_ctrl_limit();
  };
}

#endif // PSANA_EPICSPVHEADERWRAPPER_H
