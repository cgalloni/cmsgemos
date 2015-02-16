/*
 * This class provides interface for the Crate properties.
 */
#ifndef gem_base_utils_gemGLIBProperties_h
#define gem_base_utils_gemGLIBProperties_h

#include <string>
#include <vector>
#include <map>

#include "gemDeviceProperties.h"
#include "gemOHProperties.h"

namespace gem {
    namespace base {
        namespace utils {
            class gemGLIBProperties public gemDeviceProperties {
                friend class gem::base::utils::gemXMLparser;
                gemGLIBProperties(){}
                ~gemGLIBProperties(){}
                const std::vector<gem::base::utils::gemOHProperties>& getOHRefs(return subDevicesIDs_;)

                private:
                std::vector <gem::base::utils::gemOHProperties> subDevicesRefs_;
            }
        }
    }
}

#endif
