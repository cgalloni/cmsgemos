#ifndef gem_base_GEMMonitor_h
#define gem_base_GEMMonitor_h

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <list>

#include "log4cplus/logger.h"

#include "xdaq/ApplicationStub.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/task/TimerListener.h"
#include "toolbox/task/TimerEvent.h"
#include "toolbox/lang/Class.h"
#include "toolbox/TimeVal.h"
#include "toolbox/TimeInterval.h"

#include "cgicc/HTMLClasses.h"

#include "gem/base/utils/GEMInfoSpaceToolBox.h"

namespace toolbox {
  namespace task {
    class Timer;
  }
}

namespace xdata {
  class InfoSpace;
}

namespace xgi {
  class Input;
  class Output;
}

namespace gem {
  namespace base {

    class GEMApplication;
    class GEMFSMApplication;

    namespace utils {
      class GEMInfoSpaceToolBox;
    }
    
    class GEMMonitor : public toolbox::task::TimerListener, public toolbox::lang::Class
      {
      public:
        
        /**
         * Constructor from generic xdaq::ApplicationStub
         * @param logger the logger object from the calling application
         * @param gemApp the pointer to the calling application
         */
        GEMMonitor(log4cplus::Logger& logger, xdaq::ApplicationStub* stub, int const& index);

        /**
         * Constructor from GEMApplication derived classes
         * @param logger the logger object from the calling application
         * @param gemApp the pointer to the calling application
         */
        GEMMonitor(log4cplus::Logger& logger, GEMApplication* gemApp, int const& index);

        /**
         * Constructor from GEMFSMApplication derived classes
         * Different update behaviour if the application derives from GEMFSMApplication
         * @param logger the logger object from the calling application
         * @param gemApp the pointer to the calling application
         */
        GEMMonitor(log4cplus::Logger& logger, GEMFSMApplication* gemFSMApp, int const& index);

        /**
         * Destructor 
         * 
         */
        virtual ~GEMMonitor();
        
        /**
         * Start the monitoring
         */
        void startMonitoring();

        /**
         * Stop the monitoring
         */
        void stopMonitoring();

        /**
         * Inherited from TimerListener, must be implemented
         * @param event
         */
        virtual void timeExpired(toolbox::task::TimerEvent& event);

        /**
         * Update method, pure virtual, must be implemented in specific monitor class
         * Should perform all actions to update any values stored in the monitor info space
         */
        virtual void updateMonitorables()=0;

        /**
         * Add an info space tool box to the monitor object
         * @param infoSpace is the info space tool box to monitor
         * 
         */
        void addInfoSpace(std::string const& name,
                          std::shared_ptr<gem::base::utils::GEMInfoSpaceToolBox> infoSpace,
                          toolbox::TimeInterval const& interval=toolbox::TimeInterval(30,0));

        /**
         * Add a set of monitorables into the specified info space tool box object
         * @param name is the name of the set of monitorables
         * @param infoSpace is the info space into which the monitorables will be created
         */
        void addMonitorableSet(std::string const& name, std::string const& infoSpaceName);

        /**
         * Add a monitorable to set of monitorables
         * @param setname is the name of the set of monitorables into which the monitorable will be added
         * @param infoSpaceName is the name of GEMInfoSpaceToolBox that manages this monitorable item
         * @param monpair is a pair of the name of the monitorable, and possibly the register to read
         * @param type is type of item that should be updated
         * @param format is the way the values will be displayed
         */
        void addMonitorable(std::string const& setname,
                            std::string const& infoSpaceName,
                            std::pair<std::string,std::string> const& monpair,
                            utils::GEMInfoSpaceToolBox::UpdateType type,
                            std::string const& format);
        
        /**
         * A way to get the formatted information from the items in the set
         * @param setname the name of the set for which to print the information
         * @returns a list of name, value, regname, docstring values for each item in the set
         */
        std::list<std::vector<std::string> > getFormattedItemSet(std::string const& setname);
        
        /**
         * Manages updating the items on web pages using json and ajax
         * @param setname the name of the set for which to print the information
         * @param out is the output xgi page
         */
        void jsonUpdateItemSet(   std::string const& setname, std::ostream *out);
        void jsonUpdateItemSets(  xgi::Output *out);
        void jsonUpdateInfoSpaces(xgi::Output *out);
        
        /**
         * Takes care of cleaning up the monitor after a reset
         * should empty all lists and maps of known items
         */
        virtual void reset();

        typedef struct {
          std::string name;
          std::string regname;
          std::shared_ptr<gem::base::utils::GEMInfoSpaceToolBox> infoSpace;
          utils::GEMInfoSpaceToolBox::UpdateType updatetype;
          std::string format;
        } GEMMonitorable;
        
      protected:
        std::unordered_map<std::string,
          std::pair<std::shared_ptr<gem::base::utils::GEMInfoSpaceToolBox>,
          toolbox::TimeInterval> > m_infoSpaceMap;
        
        std::unordered_map<std::string,
          std::list<std::string> > m_monitorableSetInfoSpaceMap;
        
        std::unordered_map<std::string,
          std::list<std::pair<std::string, GEMMonitorable> > > m_monitorableSetsMap;
        
        std::shared_ptr<GEMApplication> p_gemApp;
        // std::shared_ptr<GEMFSM>         p_gemFSM;
        
        log4cplus::Logger m_gemLogger;
        toolbox::task::Timer* m_timer;   // timer for general info space updates
        std::string m_timerName;
        toolbox::task::Timer* m_hwtimer; // time for hw updates
        std::string m_hwTimerName;
        
      };
  }
}
#endif
