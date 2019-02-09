#include <memory>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gem/onlinedb/SystemTopology.h"
#include "gem/onlinedb/detail/XMLUtils.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SystemTopologyTest
#include <boost/test/unit_test.hpp>

/* Needed to make the linker happy. */
#include <xdaq/version.h>
config::PackageInfo xdaq::getPackageInfo()
{
    return config::PackageInfo("", "", "", "", "", "", "", "");
}

using namespace gem::onlinedb;
using namespace gem::onlinedb::detail::literals;

BOOST_FIXTURE_TEST_SUITE(SystemTopologyTest, detail::XercesGuard)

BOOST_AUTO_TEST_CASE(Populate)
{
    XERCES_CPP_NAMESPACE_USE

    try {
        // Load example document

        // Get an implementation
        auto impl = DOMImplementationRegistry::getDOMImplementation("LS"_xml);
        BOOST_REQUIRE(impl != nullptr);

        auto errorHandler = std::make_shared<detail::XercesAlwaysThrowErrorHandler>();

        XercesDOMParser parser;
        parser.setErrorHandler(errorHandler.get());
        parser.setExternalSchemaLocation(
            "https://github.com/cms-gem-daq-project/cmsgemos/tree/master/gemonlinedb/xml/schema/system-topology.xsd "
            "../schema/system-topology.xsd"); // Need to figure out a better way
        parser.setValidationScheme(XercesDOMParser::Val_Always);
        parser.setDoNamespaces(true);
        parser.setDoSchema(true);

        // Parse and validate
        parser.parse("xml/examples/system-topology.xml"_xml);
        DOMDocumentPtr document;
        document.reset(parser.adoptDocument());

        // Populate
        SystemTopology topo;
        topo.populate(document);

    } catch (DOMException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    } catch (XMLException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    } catch (SAXParseException &e) {
        auto column = e.getColumnNumber();
        auto line = e.getLineNumber();
        throw std::runtime_error(
            std::to_string(line) + ":" +
            std::to_string(column) + ": " +
            detail::transcode(e.getMessage()));
    } catch (SAXException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    }
}

BOOST_AUTO_TEST_SUITE_END()
