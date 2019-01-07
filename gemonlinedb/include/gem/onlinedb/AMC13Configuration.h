#ifndef GEM_ONLINEDB_AMC13CONFIGURATION_H
#define GEM_ONLINEDB_AMC13CONFIGURATION_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>

#include <xercesc/dom/DOMDocument.hpp>

#include "gem/onlinedb/DataSet.h"
#include "gem/onlinedb/PartReference.h"
#include "gem/onlinedb/Run.h"

namespace gem {
    namespace onlinedb {

        // Forward declaration
        class AMCConfiguration;

        /**
         * @brief Describes the configuration of an AMC13.
         *
         * @note Due to the presence of configuration variables of non-integral
         *       types, this class doesn't realize the @c Configuration concept.
         *       Serialization is achieved by specializing the relevant template
         *       classes.
         *
         * @note Modifying objects of this class doesn't affect hardware.
         */
        class AMC13Configuration
        {
        private:
            int m_fedId;
            bool m_enableLocalTTC;
            std::string m_hostname;

            std::vector<std::shared_ptr<AMCConfiguration>> m_AMCConfigs;

        public:
            /**
             * @brief Compares two AMC configurations for equality.
             */
            bool operator== (const AMC13Configuration &other) const;

            ////////////////////////////////////////////////////////////////////

            /**
             * @brief Retrieves the id of the attached FED.
             */
            int getFEDId() const { return m_fedId; };

            /**
             * @brief Modifies the id of the attached FED.
             */
            void setFEDId(int id) { m_fedId = id; };

            /**
             * @brief Retrieves whether local TTC is enabled.
             */
            int isLocalTTCEnabled() const { return m_enableLocalTTC; };

            /**
             * @brief Enables or disables local TTC.
             */
            void setLocalTTCEnabled(bool enable) { m_enableLocalTTC = enable; };

            /**
             * @brief Retrieves the hostname.
             */
            std::string getHostname() const { return m_hostname; };

            /**
             * @brief Modifies the hostname.
             */
            void setHostname(const std::string &hostname) { m_hostname = hostname; };

            ////////////////////////////////////////////////////////////////////

            /**
             * @name Child AMC configuration
             * @{
             */
            /**
             * @brief Retrieves the configuration of the given AMC, if set.
             */
            const std::shared_ptr<AMCConfiguration> getAMCConfig(
                std::size_t vfat) const {
                return m_AMCConfigs.at(vfat); };

            /**
             * @brief Retrieves the configuration of the given AMC, if set.
             */
            std::shared_ptr<AMCConfiguration> getAMCConfig(
                std::size_t vfat) {
                return m_AMCConfigs.at(vfat); };

            /**
             * @brief Retrieves the configuration of all AMCs.
             */
            auto getAMCConfigs() const -> const decltype(m_AMCConfigs) & {
                return m_AMCConfigs; };

            /**
             * @brief Retrieves the configuration of all AMCs.
             */
            auto getAMCConfigs() -> decltype(m_AMCConfigs) & { return m_AMCConfigs; };

            /**
             * @brief Modifies the configuration of the given AMC.
             */
            void setAMCConfig(std::size_t amc,
                              const std::shared_ptr<AMCConfiguration> &config) {
                m_AMCConfigs.at(amc) = config; };

            /**
             * @brief Modifies the configuration of all AMCs.
             */
            void setAMCConfigs(const decltype(m_AMCConfigs) &configs) {
                m_AMCConfigs = configs; };

            /**
             * @}
             */
        };

        template<>
        class ConfigurationTraits<AMC13Configuration>
        {
        public:
            static std::string extTableName() { return "GEM_AMC13_CONFIGURATION"; };
            static std::string typeName() { return "GEM AMC13 Conf Lookup Table"; };
            static std::string kindOfPart() { return "GEM AMC13"; };
            using PartType = PartReferenceSN;
        };

        // Forward declaration
        template<class ConfigurationTypeT>
        class XMLSerializationData;

        // Forward declaration
        using DOMDocumentPtr = std::unique_ptr<xercesc::DOMDocument>;

        /**
         * @brief
         * Specializes @ref XMLSerializationData for @ref AMC13Configuration.
         *
         * @copydetails XMLSerializationData
         */
        template<>
        class XMLSerializationData<AMC13Configuration>
        {
        public:
            /**
             * @copydoc XMLSerializationData::ConfigurationType
             */
            using ConfigurationType = AMC13Configuration;

        private:
            Run m_run;
            std::vector<DataSet<ConfigurationType>> m_dataSets;

        public:
            /**
             * @copydoc XMLSerializationData::getRun
             */
            Run getRun() const { return m_run; };

            /**
             * @copydoc XMLSerializationData::setRun
             */
            void setRun(const Run &run) { m_run = run; };

            /**
             * @copydoc XMLSerializationData::getDataSets
             */
            std::vector<DataSet<ConfigurationType>> getDataSets() const {
                return m_dataSets; };

            /**
             * @copydoc XMLSerializationData::addDataSet
             */
            void addDataSet(const DataSet<ConfigurationType> &dataSet) {
                m_dataSets.push_back(dataSet); };

            /**
             * @copydoc XMLSerializationData::readDOM
             */
            void readDOM(const DOMDocumentPtr &dom);

            /**
             * @copydoc XMLSerializationData::makeDOM
             */
            DOMDocumentPtr makeDOM() const;
        };

    } /* namespace onlinedb */
} /* namespace gem */

#endif // GEM_ONLINEDB_AMC13CONFIGURATION_H
