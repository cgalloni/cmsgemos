/**
 * class: Calibration
 * description: Calibration application for GEM system
 *              structure borrowed from gemSupervisor
 * author: 
 * date:
 */

#include "gem/calib/Calibration.h"

#include <cstdlib>
#include <iomanip>

#include <map>
#include <set>
#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "gem/calib/CalibrationWeb.h"

#include "gem/utils/soap/GEMSOAPToolBox.h"
#include "gem/utils/exception/Exception.h"

typedef gem::base::utils::GEMInfoSpaceToolBox::UpdateType GEMUpdateType;

XDAQ_INSTANTIATOR_IMPL(gem::calib::Calibration);

gem::calib::Calibration::Calibration(xdaq::ApplicationStub* stub) :
  gem::base::GEMApplication(stub)
{
    CMSGEMOS_DEBUG("gem::calib::Calibration : Creating the CalibrationWeb interface");
    p_gemWebInterface = new gem::calib::CalibrationWeb(this);
    m_calType = NDEF;
    /*
    xgi::bind(this, &Calibration::stopAction, "stopAction");
    xgi::bind(this, &Calibration::resumeAction, "resumeAction");
    xgi::bind(this, &Calibration::pauseAction, "pauseAction");
    init();
    */
    xgi::bind(this, &Calibration::pauseAction, "pauseAction");
    xgi::bind(this, &Calibration::applyAction, "applyAction");
    xgi::bind(this, &Calibration::setCalType, "setCalType");
}

gem::calib::Calibration::~Calibration()
{
    CMSGEMOS_DEBUG("gem::calib::Calibration : Destructor called");
    // make sure to empty the v_supervisedApps  vector and free the pointers
}


// This is the callback used for handling xdata:Event objects
void gem::calib::Calibration::actionPerformed(xdata::Event& event)
{
    if (event.type() == "setDefaultValues" || event.type() == "urn:xdaq-event:setDefaultValues") {
      CMSGEMOS_DEBUG("gem::calib::Calibration::actionPerformed() setDefaultValues" <<
            "Default configuration values have been loaded from xml profile");
      //init();
    }
    
    // item is changed, update it
    if (event.type() == "ItemChangedEvent" || event.type() == "urn:xdata-event:ItemChangedEvent") {
      CMSGEMOS_DEBUG("gem::calib::Calibration:actionPerformed() ItemChangedEvent");
    }
    
    // update monitoring variables
     gem::base::GEMApplication::actionPerformed(event);
}

void gem::calib::Calibration::init()
{
  /*  v_daqmon.clear();
      v_daqmon.reserve(NAMC);
      for (int i = 1; i <= NAMC; ++i)
      {
      char t_board_name[20];
      sprintf(t_board_name, "gem-shelf%02d-amc%02d", m_shelfID.value_, i);
      CMSGEMOS_DEBUG("gem::daqmon::ShelfMonitor::init :  Domain name for the board " << std::dec << i << " : " << t_board_name);
      v_daqmon.push_back(new gem::daqmon::DaqMonitor(t_board_name, this->getApplicationLogger(), this, i));
      CMSGEMOS_DEBUG("gem::daqmon::ShelfMonitor::init : DaqMonitor pointer created");
  */
}


bool gem::calib::Calibration::isGEMApplication(const std::string& classname) const
{
    if (classname.find("gem::") != std::string::npos)
      return true;  // handle all GEM applications
    /*
      if (m_otherClassesToSupport.count(classname) != 0)
      return true;  // include from list
    */
    return false;
}

void gem::calib::Calibration::stopAction(xgi::Input* in, xgi::Output* out)
  throw (xgi::exception::Exception)
{
/*
  CMSGEMOS_INFO("ShelfMonitor::stopAction");
  out->getHTTPResponseHeader().addHeader("Content-Type", "application/json");
  //this->stopMonitoring();
  *out << " { \"mon_state\":\"STOPPED\"}" << std::endl;
*/
}

void gem::calib::Calibration::resumeAction(xgi::Input* in, xgi::Output* out)
  throw (xgi::exception::Exception)
{
/*
  CMSGEMOS_INFO("ShelfMonitor::startAction");
  out->getHTTPResponseHeader().addHeader("Content-Type", "application/json");
  //this->startMonitoring();
  *out << " { \"mon_state\":\"RUNNING\"}" << std::endl;
*/
}

void gem::calib::Calibration::pauseAction(xgi::Input* in, xgi::Output* out)
  throw (xgi::exception::Exception)
{
  
  CMSGEMOS_INFO("Calibration::pauseAction");
  out->getHTTPResponseHeader().addHeader("Content-Type", "application/json");
  //this->stopMonitoring();
  *out << " { \"mon_state\":\"PAUSED\"}" << std::endl;
  std::cout << "ciao! " << std::endl;
}

void gem::calib::Calibration::applyAction(xgi::Input* in, xgi::Output* out)
  throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);
    for (auto it: m_scanParams.find(m_calType)->second){
        it.second = cgi[it.first]->getIntegerValue();
        CMSGEMOS_INFO("Calibration::applyAction : " << it.first << " = " << it.second);
    }
}

void gem::calib::Calibration::setCalType(xgi::Input* in, xgi::Output* out)
  throw (xgi::exception::Exception)
{
    CMSGEMOS_INFO("Calibration::setCalType");
    out->getHTTPResponseHeader().addHeader("Content-Type", "text/html");
    cgicc::Cgicc cgi(in);
    m_calType = m_calTypeSelector.find(cgi["cal_type_select"]->getValue())->second;
    switch (m_calType) {
        case NDEF: 
            CMSGEMOS_DEBUG("Calibration::setCalType : Selected Cal Type: NDEF");
            break;
        default: 
            CMSGEMOS_DEBUG("Calibration::setCalType : Selected Cal Type: SCURVE");
            dynamic_cast<gem::calib::CalibrationWeb*>(p_gemWebInterface)->settingsInterface(m_calType, out); 
            break;
    }
}
