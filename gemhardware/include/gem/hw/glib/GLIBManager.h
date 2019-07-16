/** @file GLIBManager.h */

#ifndef GEM_HW_GLIB_GLIBMANAGER_H
#define GEM_HW_GLIB_GLIBMANAGER_H

#include <array>

#include "gem/base/GEMFSMApplication.h"
// #include "gem/hw/glib/GLIBSettings.h"

#include "gem/hw/glib/exception/Exception.h"

#include "gem/utils/soap/GEMSOAPToolBox.h"
#include "gem/utils/exception/Exception.h"

#include "gem/calib/GEMCalibEnums.h"
#include "gem/calib/Calibration.h"
#include "xdata/Bag.h"
#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/String.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/Vector.h"


namespace gem {
  namespace hw {
    namespace glib {

      class HwGLIB;
      class GLIBManagerWeb;
      class GLIBMonitor;

      using glib_shared_ptr = std::shared_ptr<HwGLIB>;
      using is_toolbox_ptr  = std::shared_ptr<gem::base::utils::GEMInfoSpaceToolBox>;

      class GLIBManager : public gem::base::GEMFSMApplication
        {

          friend class GLIBManagerWeb;
          // friend class GLIBMonitor;

        public:
          XDAQ_INSTANTIATOR();

          GLIBManager(xdaq::ApplicationStub* s);

          virtual ~GLIBManager();

        protected:
          /* virtual void init() override; */
          void init();

          virtual void actionPerformed(xdata::Event& event) override;

          // state transitions FIXME: remove exception specifiers
          virtual void initializeAction() throw (gem::hw::glib::exception::Exception) override;
          virtual void configureAction()  throw (gem::hw::glib::exception::Exception) override;
          virtual void startAction()      throw (gem::hw::glib::exception::Exception) override;
          virtual void pauseAction()      throw (gem::hw::glib::exception::Exception) override;
          virtual void resumeAction()     throw (gem::hw::glib::exception::Exception) override;
          virtual void stopAction()       throw (gem::hw::glib::exception::Exception) override;
          virtual void haltAction()       throw (gem::hw::glib::exception::Exception) override;
          virtual void resetAction()      throw (gem::hw::glib::exception::Exception) override;
          // virtual void noAction()         throw (gem::hw::glib::exception::Exception) override;

          void failAction(toolbox::Event::Reference e)
            throw (toolbox::fsm::exception::Exception);

          virtual void resetAction(toolbox::Event::Reference e)
              throw (toolbox::fsm::exception::Exception) override;

        protected:
          /**
           * OBSOLETE not present in generic AMC FW
           */
          std::vector<uint32_t> dumpGLIBFIFO(int const& glib);

          /**
           * OBSOLETE not present in generic AMC FW
           */
          void dumpGLIBFIFO(xgi::Input* in, xgi::Output* out);

        private:
          void     createGLIBInfoSpaceItems(is_toolbox_ptr is_glib, glib_shared_ptr glib);
          uint16_t m_amcEnableMask;

          toolbox::task::WorkLoop *p_amc_wl;                     ///< paralelize the calls to different AMCs
          toolbox::BSem m_amc_wl_semaphore[MAX_AMCS_PER_CRATE];  ///< do we need a semaphore for the workloop or each of them?

          class GLIBInfo {

          public:
            GLIBInfo();
            void registerFields(xdata::Bag<GLIBManager::GLIBInfo>* bag);

            // monitoring information
            xdata::Boolean present;  ///< FIXME BAD USAGE
            xdata::Integer crateID;  ///< Specifies the crate to which the OptoHybrid is connected
            xdata::Integer slotID;   ///< Specifies the AMC slot to which the OptoHybrid is connected

            // list of GTX links to enable in the DAQ
            xdata::String            gtxLinkEnableList;
            xdata::UnsignedInteger32 gtxLinkEnableMask;

            // registers to set
            xdata::Integer sbitSource;
            xdata::Boolean enableZS;

            inline std::string toString() {
              std::stringstream os;
              os << "present:"  << present.toString()  << std::endl
                 << "crateID:"  << crateID.toString()  << std::endl
                 << "slotID:"   << slotID.toString()   << std::endl

                 << "gtxLinkEnableList:" << gtxLinkEnableList.toString() << std::endl
                 << "gtxLinkEnableMask:" << std::hex << gtxLinkEnableMask.value_ << std::dec << std::endl

                 << "sbitSource:0x"      << std::hex << sbitSource.value_ << std::dec << std::endl
                 << "enableZS:0x"        << std::hex << enableZS.value_   << std::dec << std::endl
                 << std::endl;
              return os.str();
            };
          };

          mutable gem::utils::Lock m_deviceLock;  ///< [MAX_AMCS_PER_CRATE];


          std::array<glib_shared_ptr, MAX_AMCS_PER_CRATE> m_glibs;                      ///< HwGenericAMC pointers to be managed
          std::array<std::shared_ptr<GLIBMonitor>, MAX_AMCS_PER_CRATE> m_glibMonitors;  ///< AMCMonito pointers to be managed
          std::array<is_toolbox_ptr, MAX_AMCS_PER_CRATE> is_glibs;                      ///< AMC InfoSpace pointers to be managed


          xdata::Vector<xdata::Bag<GLIBInfo> > m_glibInfo;  ///< [MAX_AMCS_PER_CRATE];
          xdata::String  m_amcSlots;           ///< 
          xdata::String  m_connectionFile;     ///< 
          xdata::Boolean m_doPhaseShift;       ///< Whether or not to do a phase shifting procedure during configuration
          xdata::Boolean m_bc0LockPhaseShift;  ///< Use BC0 to find the best phase during the phase shifting procedure
          xdata::Boolean m_relockPhase;        ///< Relock the phase during phase shifting

	  uint32_t m_lastLatency;         ///< Special variable for latency scan mode
          uint32_t m_lastVT1, m_lastVT2;  ///< Special variable for threshold scan mode 

 
        };  // class GLIBManager
    }  // namespace gem::hw::glib
  }  // namespace gem::hw
}  // namespace gem

#endif  // GEM_HW_GLIB_GLIBMANAGER_H
