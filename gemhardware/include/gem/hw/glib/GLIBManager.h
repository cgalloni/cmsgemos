/** @file GLIBManager.h */

#ifndef GEM_HW_GLIB_GLIBMANAGER_H
#define GEM_HW_GLIB_GLIBMANAGER_H

#include <array>

#include "gem/base/GEMFSMApplication.h"
//#include "gem/hw/glib/GLIBSettings.h"

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

      typedef std::shared_ptr<HwGLIB>  glib_shared_ptr;
      typedef std::shared_ptr<gem::base::utils::GEMInfoSpaceToolBox> is_toolbox_ptr;

      class GLIBManager : public gem::base::GEMFSMApplication
        {

          friend class GLIBManagerWeb;
          //friend class GLIBMonitor;

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
          //virtual void noAction()         throw (gem::hw::glib::exception::Exception) override;

          virtual void failAction(toolbox::Event::Reference e)
            throw (toolbox::fsm::exception::Exception);

          virtual void resetAction(toolbox::Event::Reference e)
            throw (toolbox::fsm::exception::Exception);
          
         
          xoap::MessageReference calibParamPrint(xoap::MessageReference msg) ;//CG
	  /* bool is_initialized_, is_configured_, is_running_, is_paused_, is_resumed_; */ ///< FIXME REMOVE UNUSED

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

          toolbox::task::WorkLoop *p_amc_wl; ///< paralelize the calls to different AMCs
          toolbox::BSem m_amc_wl_semaphore[MAX_AMCS_PER_CRATE]; // do we need a semaphore for the workloop or each of them?

          class GLIBInfo {

          public:
            GLIBInfo();
            void registerFields(xdata::Bag<GLIBManager::GLIBInfo>* bag);

            //monitoring information
            xdata::Boolean present;
            xdata::Integer crateID;
            xdata::Integer slotID;
            xdata::String  cardName; ///< FIXME USAGE cardname shoule be gem-shelfXX-amcYY from crateID and slotID
            xdata::String  birdName; ///< FIXME REMOVE TEMPORARY

            //configuration parameters
            /* xdata::String controlHubAddress; */ ///< FIXME REMOVE OBSOLETE
            /* xdata::String deviceIPAddress; */   ///< FIXME REMOVE OBSOLETE
            /* xdata::String ipBusProtocol; */     ///< FIXME REMOVE OBSOLETE
            /* xdata::String addressTable; */      ///< FIXME REMOVE OBSOLETE

            // list of GTX links to enable in the DAQ
            xdata::String            gtxLinkEnableList;
            xdata::UnsignedInteger32 gtxLinkEnableMask;

            /* xdata::UnsignedInteger32 controlHubPort; */ ///< FIXME REMOVE OBSOLETE
            /* xdata::UnsignedInteger32 ipBusPort; */      ///< FIXME REMOVE OBSOLETE

            //registers to set
            xdata::Integer sbitSource;
            xdata::Boolean enableZS;

            inline std::string toString() {
              std::stringstream os;
              os << "present:"  << present.toString()  << std::endl
                 << "crateID:"  << crateID.toString()  << std::endl
                 << "slotID:"   << slotID.toString()   << std::endl
                 << "cardName:" << cardName.toString() << std::endl
                 << "birdName:" << birdName.toString() << std::endl

                 /* << "controlHubAddress:" << controlHubAddress.toString() << std::endl */ ///< FIXME REMOVE OBSOLETE
                 /* << "deviceIPAddress:"   << deviceIPAddress.toString()   << std::endl */ ///< FIXME REMOVE OBSOLETE
                 /* << "ipBusProtocol:"     << ipBusProtocol.toString()     << std::endl */ ///< FIXME REMOVE OBSOLETE
                 /* << "addressTable:"      << addressTable.toString()      << std::endl */ ///< FIXME REMOVE OBSOLETE
                 /* << "controlHubPort:"    << controlHubPort.value_        << std::endl */ ///< FIXME REMOVE OBSOLETE
                 /* << "ipBusPort:"         << ipBusPort.value_             << std::endl */ ///< FIXME REMOVE OBSOLETE

                 << "gtxLinkEnableList:" << gtxLinkEnableList.toString() << std::endl
                 << "gtxLinkEnableMask:" << std::hex << gtxLinkEnableMask.value_ << std::dec << std::endl

                 << "sbitSource:0x"      << std::hex << sbitSource.value_ << std::dec << std::endl
                 << "enableZS:0x"        << std::hex << enableZS.value_   << std::dec << std::endl
                 << std::endl;
              return os.str();
            };
          };

          mutable gem::utils::Lock m_deviceLock;  // [MAX_AMCS_PER_CRATE];

          std::array<glib_shared_ptr, MAX_AMCS_PER_CRATE>              m_glibs;
          std::array<std::shared_ptr<GLIBMonitor>, MAX_AMCS_PER_CRATE> m_glibMonitors;
          std::array<is_toolbox_ptr, MAX_AMCS_PER_CRATE>               is_glibs;

          xdata::Vector<xdata::Bag<GLIBInfo> > m_glibInfo;  // [MAX_AMCS_PER_CRATE];
          xdata::String                        m_amcSlots;
          xdata::String                        m_connectionFile;
          xdata::Boolean                       m_uhalPhaseShift; // FIXME OBSOLETE
          xdata::Boolean                       m_bc0LockPhaseShift;
          xdata::Boolean                       m_relockPhase;
          
          uint32_t m_lastLatency, m_lastVT1, m_lastVT2;
          
        public:

          gem::calib::calType m_calType;

          std::map<gem::calib::calType , std::map<std::string,  uint32_t >> m_calParams{
              {gem::calib::calType::GBTPHASE  ,{{"nSamples",100},{"phaseMin", 0},{"phaseMax", 14},{"stepSize", 1},}},
                  {gem::calib::calType::LATENCY,{
                          {"nSamples"  , 100},
                              {"trigType"  , 0},
                                  {"l1aTime"   , 250},
                                      {"mspl"      , 4},
                                          {"scanMin"   , 0},
                                              {"scanMax"   , 255},
                                                  {"vfatChMin" , 0},
                                                      {"vfatChMax" , 127},
                                                      
                                                         
                                                              {"signalSourceType"       , 0},
                                                                  {"pulseDelay" , 40},
                                                                      }},
                      {gem::calib::calType::SCURVE,{
                              {"nSamples"  , 100},
                                  {"trigType"  , 0}, // TODO: TTC local should be only possible one
                                      {"l1aTime"   , 250},
                                          {"pulseDelay", 40},
                                              {"latency"   , 33},
                                                  {"vfatChMin" , 0},
                                                      {"vfatChMax" , 127},
                                                          {"mspl"      , 4},
                                                              }},
                          {gem::calib::calType::SBITARMDACSCAN  ,{
                                  {"comparatorType",0},
                                      {"perChannelType",0},
                                          {"vfatChMin" , 0},
                                              {"vfatChMax" , 127},
                                                  {"stepSize", 1},
                                                      }},
                              {gem::calib::calType::ARMDACSCAN  ,{
                                      {"nSamples"  , 1000},
                                          {"trigType"  , 0},
                                              {"vfatChMin" , 0},
                                                  {"vfatChMax" , 127},
                                                      }},
                                  {gem::calib::calType::TRIMDAC  , {
                                          {"nSamples"   , 100},
                                              {"trigType"   , 0}, // TODO: TTC local should be only possible one
                                                  {"nSamples"   , 100},
                                                      {"l1aTime"    , 250},
                                                          {"pulseDelay" , 40},
                                                              {"latency"    , 33},
                                                                  {"mspl"       , 4},
                                                                      //  {"trimValues" , "-63,0,63"}, TODO: to be load as strings and converted 
                   
                                                                      }},
                                      {gem::calib::calType::DACSCANV3  ,{
                                              {"adcType",0},
                                                  }},
                                          {gem::calib::calType::CALIBRATEARMDAC,{
                                                  {"nSamples"  , 100},
                                                      {"trigType"  , 0}, // TODO: TTC local should be only possible one
                                                          {"l1aTime"   , 250},
                                                              {"pulseDelay", 40},
                                                                  {"latency"   , 33},
                                                                      // {"armDacPoins","17,20,23,25,30,40,50,60,70,80,100,125,150,175"},TODO: to be load as strings and converted 
                   
                                                                      }},

                            
                                              };
          
          std::map<gem::calib::calType , std::map<std::string, std::string >> m_calStringParams {
              
              {gem::calib::calType::CALIBRATEARMDAC, {{"armDacPoins","17,20,23,25,30,40,50,60,70,80,100,125,150,175"}, }},
                  {gem::calib::calType::TRIMDAC  , {{"trimValues" , "-63,0,63"},  }},
                      
                      };
         
          std::map<gem::calib::dacScanType, gem::calib::Calibration::dacFeature> m_dacScanTypeParams{
              {gem::calib::dacScanType::CFG_CAL_DAC,{"CFG_CAL_DAC", 0, 255, false}},
                  {gem::calib::dacScanType::CFG_BIAS_PRE_I_BIT, {"CFG_BIAS_PRE_I_BIT", 0, 255, false}},
                      {gem::calib::dacScanType::CFG_BIAS_PRE_I_BLCC,{"CFG_BIAS_PRE_I_BLCC", 0, 63, false}},
                          {gem::calib::dacScanType::CFG_BIAS_PRE_I_BSF,{"CFG_BIAS_PRE_I_BSF", 0, 63, false}},
                              {gem::calib::dacScanType::CFG_BIAS_SH_I_BFCAS,{"CFG_BIAS_SH_I_BFCAS", 0, 255, false}},
                                  {gem::calib::dacScanType::CFG_BIAS_SH_I_BDIFF,{"CFG_BIAS_SH_I_BDIFF", 0, 255, false}},
                                      {gem::calib::dacScanType::CFG_BIAS_SD_I_BDIFF,{"CFG_BIAS_SD_I_BDIFF", 0, 255, false}},
                                          {gem::calib::dacScanType::CFG_BIAS_SD_I_BFCAS,{"CFG_BIAS_SD_I_BFCAS", 0, 255, false}},
                                              {gem::calib::dacScanType::CFG_BIAS_SD_I_BSF,{"CFG_BIAS_SD_I_BSF", 0, 63, false}},
                                                  {gem::calib::dacScanType::CFG_BIAS_CFD_DAC_1,{"CFG_BIAS_CFD_DAC", 0, 63, false}},
                                                      {gem::calib::dacScanType::CFG_BIAS_CFD_DAC_2,{"CFG_BIAS_CFD_DAC", 0, 63, false}},
                                                          {gem::calib::dacScanType::CFG_HYST,{"CFG_HYST", 0, 63, false}},
                                                              {gem::calib::dacScanType::CFG_THR_ARM_DAC,{"CFG_THR_ARM_DAC", 0, 255, false}},
                                                                  {gem::calib::dacScanType::CFG_THR_ZCC_DAC,{"CFG_THR_ZCC_DAC", 0, 255, false}},
                                                                      {gem::calib::dacScanType::CFG_BIAS_PRE_VREF,{"CFG_BIAS_PRE_VREF", 0, 255, false}},
                                                                          {gem::calib::dacScanType::CFG_VREF_ADC,{"CFG_VREF_ADC", 0, 3, false}}
            };
          
          
           
        };  // class GLIBManager
    }  // namespace gem::hw::glib
  }  // namespace gem::hw
}  // namespace gem

#endif  // GEM_HW_GLIB_GLIBMANAGER_H
