#ifndef GEM_ONLINEDB_XMLSERIALIZATIONDATA_H
#define GEM_ONLINEDB_XMLSERIALIZATIONDATA_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMXPathResult.hpp>

#include "gem/onlinedb/ConfigurationTraits.h"
#include "gem/onlinedb/DataSet.h"
#include "gem/onlinedb/PartReference.h"
#include "gem/onlinedb/Run.h"
#include "gem/onlinedb/detail/RegisterData.h"

namespace gem {
    namespace onlinedb {

        /**
         * @brief Convenience alias.
         */
        using DOMDocumentPtr = std::unique_ptr<xercesc::DOMDocument>;

        /**
         * @brief Represents the contents of XML files.
         *
         * This class is used to represent the contents of XML files as C++
         * objects before they are serialized and after they are read. It is
         * used as an indermediate step when (un)serializing configuration data.
         *
         * @tparam ConfigurationTypeT The configuration class serialized in the
         *                            XML file.
         */
        template<class ConfigurationTypeT>
        class XMLSerializationData
        {
        public:
            /**
             * @brief Convenience typedef.
             */
            using ConfigurationType = ConfigurationTypeT;

        private:
            Run m_run;
            std::vector<DataSet<ConfigurationType>> m_dataSets;

        public:
            /**
             * @brief Returns the run information from the file.
             */
            Run getRun() const { return m_run; }

            /**
             * @brief Sets the run for which to generate XML data.
             */
            void setRun(const Run &run) { m_run = run; }

            /**
             * @brief Gets the list of parsed datasets.
             */
            std::vector<DataSet<ConfigurationType>> getDataSets() const
            {
                return m_dataSets;
            }

            /**
             * @brief Adds a dataset to be serialized
             */
            void addDataSet(const DataSet<ConfigurationType> &dataSet)
            {
                m_dataSets.push_back(dataSet);
            }

            /**
             * @brief Reads data from a DOM document.
             *
             * @warning No validation is performed as part of this function.
             *          Feeding a document that doesn't conform to the schema
             *          results in undefined behaviour.
             */
            void readDOM(const DOMDocumentPtr &dom);

            /**
             * @brief Generates a DOM document representing data previously set
             *        using @ref setRun and @ref addDataSet.
             */
            DOMDocumentPtr makeDOM() const;
        };

        // Utility functions for readDOM
        namespace detail {

            /**
             * @brief Extracts run information from XML data.
             */
            Run getRun(const DOMDocumentPtr &dom);

            /**
             * @brief Queries the list of data sets present in @c dom
             */
            xercesc::DOMXPathResult *queryDataSets(const DOMDocumentPtr &dom);

            /**
             * @brief Reads the comment for the data set currently represented
             *        by @c result.
             */
            std::string readDataSetComment(const DOMDocumentPtr &document,
                                           const xercesc::DOMXPathResult *result);

            /**
             * @brief Reads the version for the data set currently represented
             *        by @c result.
             */
            std::string readDataSetVersion(const DOMDocumentPtr &document,
                                           const xercesc::DOMXPathResult *result);

            /**
             * @brief Reads the part reference for the data set currently
             *        represented by @c result.
             */
            template<class PartReferenceType>
            PartReferenceType readPartReference(
                const DOMDocumentPtr &document,
                const xercesc::DOMXPathResult *result);

            /**
             * @brief Reads DATA elements for the data set currently
             *        represented by @c result.
             */
            std::vector<detail::RegisterData> readRegisterData(
                const DOMDocumentPtr &document,
                const xercesc::DOMXPathResult *result);

        } /* namespace detail */

        template<class ConfigurationType>
        void XMLSerializationData<ConfigurationType>::readDOM(
            const DOMDocumentPtr &dom)
        {
            using Traits = ConfigurationTraits<ConfigurationType>;
            using PartType = typename Traits::PartType;

            // Get run info
            m_run = detail::getRun(dom);

            // Iterate over DATA_SETs
            auto dataSetsResult = detail::queryDataSets(dom);
            auto count = dataSetsResult->getSnapshotLength();
            for (XMLSize_t i = 0; i < count; ++i) {
                dataSetsResult->snapshotItem(i);

                m_dataSets.emplace_back();
                auto &dataSet = m_dataSets.back();

                dataSet.setComment(detail::readDataSetComment(dom, dataSetsResult));
                dataSet.setVersion(detail::readDataSetVersion(dom, dataSetsResult));
                dataSet.setPart(
                    detail::readPartReference<PartType>(dom, dataSetsResult));

                auto allData = detail::readRegisterData(dom, dataSetsResult);
                for (const detail::RegisterData &data : allData) {
                    auto config = ConfigurationType();
                    config.readRegisterData(data);
                    dataSet.addData(config);
                }
            }
            delete dataSetsResult;
        }

        // Utility functions for makeDOM
        namespace detail {
            /**
             * @brief Creates a DOM document to record data into.
             * @param extTableName The name of the database table to record data in.
             * @param comment A free-form comment.
             * @param run The run the data will be associated to.
             * @throw exception::SoftwareProblem If some Xerces object cannot be created.
             */
            DOMDocumentPtr makeDOM(const std::string &extTableName,
                                   const std::string &comment,
                                   const Run &run);

            /**
             * @brief Creates a DATA_SET node inside a DOM document.
             * @param document A DOM document created by @ref makeDOM.
             * @param comment A free-form comment.
             * @param version The version of the data set.
             */
            xercesc::DOMElement *createDataSetElement(DOMDocumentPtr &document,
                                                      const std::string &comment,
                                                      const std::string &version);

            /**
             * @brief Appends a PART element to the given parent.
             * @param kindOfPart The kind of part configured.
             * @param partRef A way of identifying the part.
             */
            void createPartElement(xercesc::DOMElement *parent,
                                   const std::string &kindOfPart,
                                   const PartReferenceBarcode &partRef);

            /**
             * @brief Appends a PART element to the given parent.
             * @param kindOfPart The kind of part configured.
             * @param partRef A way of identifying the part.
             */
            void createPartElement(xercesc::DOMElement *parent,
                                   const std::string &kindOfPart,
                                   const PartReferenceSN &partRef);

            /**
             * @brief Appends a DATA element to the given parent.
             * @param content The contents of the DATA element.
             */
            void createDataElement(xercesc::DOMElement *parent,
                                   const RegisterData &content);
        } /* namespace detail */

        template<class ConfigurationType>
        DOMDocumentPtr XMLSerializationData<ConfigurationType>::makeDOM() const
        {
            using Info = ConfigurationTraits<ConfigurationType>;

            // Create the general DOM structure
            auto dom = detail::makeDOM(Info::extTableName(), Info::typeName(), m_run);

            for (const auto &dataSet : m_dataSets) {
                // Add a DATA_SET node
                auto dataSetNode = detail::createDataSetElement(dom,
                                                                dataSet.getComment(),
                                                                dataSet.getVersion());
                // Create the PART element
                detail::createPartElement(dataSetNode,
                                          Info::kindOfPart(),
                                          dataSet.getPart());
                // Add DATA elements
                for (const auto &entry : dataSet.getData()) {
                    detail::createDataElement(dataSetNode,
                                              entry.getRegisterData());
                }
            }
            return dom;
        }

    } /* namespace onlinedb */
} /* namespace gem */

#endif // GEM_ONLINEDB_XMLSERIALIZATIONDATA_H
