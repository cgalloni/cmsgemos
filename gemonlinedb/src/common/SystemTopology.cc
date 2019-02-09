#include "gem/onlinedb/SystemTopology.h"

#include <stdexcept>

#include "gem/onlinedb/detail/XMLUtils.h"

XERCES_CPP_NAMESPACE_USE

namespace gem {
    namespace onlinedb {
        namespace /* anonymous */ {
            /**
             * @brief Retrieves a part type from the attributes of a DOM element.
             */
            template<typename PartType>
            void populatePart(PartType &part, const DOMElement *el);

            template<>
            void populatePart<PartReferenceBarcode>(PartReferenceBarcode &part,
                                                    const DOMElement *el)
            {
                using namespace detail::literals;
                part.barcode = detail::transcode(el->getAttribute("barcode"_xml));
            }

            template<>
            void populatePart<PartReferenceSN>(PartReferenceSN &part,
                                               const DOMElement *el)
            {
                using namespace detail::literals;
                part.serialNumber =
                    detail::transcode(el->getAttribute("serial-number"_xml));
            }
        } // anonymous namespace

        void SystemTopology::populate(const DOMDocumentPtr &document)
        {
            auto systemEl = document->getDocumentElement();
            auto topologyEl = detail::findChildElement(systemEl, "gem:topology");

            m_roots.clear();
            for (auto amc13El = topologyEl->getFirstElementChild();
                 amc13El != nullptr;
                 amc13El = amc13El->getNextElementSibling()) {
                m_roots.emplace_back();
                m_roots.back().populate(amc13El);
            }
        }

        void SystemTopology::AMC13Node::populate(const DOMElement *el)
        {
            populatePart(reference, el);

            amc.clear();
            auto amcEl = el->getFirstElementChild();
            do {
                amc.emplace_back();
                amc.back().populate(amcEl);
            } while ((amcEl = amcEl->getNextElementSibling()));
        }

        void SystemTopology::AMCNode::populate(const DOMElement *el)
        {
            populatePart(reference, el);

            auto ohEl = el->getFirstElementChild();
            for (std::size_t i = 0; i < oh.size(); ++i) {
                if (detail::transcode(ohEl->getTagName()) == "gem:empty-slot") {
                    oh[i].reset();
                } else {
                    oh[i].reset(new OHv3Node);
                    oh[i]->populate(ohEl);
                }
                ohEl = ohEl->getNextElementSibling();
            }
        }

        void SystemTopology::OHv3Node::populate(const DOMElement *el)
        {
            populatePart(reference, el);

            auto vfatEl = el->getFirstElementChild();
            for (std::size_t i = 0; i < vfat.size(); ++i) {
                if (detail::transcode(vfatEl->getTagName()) == "gem:empty-slot") {
                    vfat[i].reset();
                } else {
                    vfat[i].reset(new VFAT3Node);
                    vfat[i]->populate(vfatEl);
                }
                vfatEl = vfatEl->getNextElementSibling();
            }
        }

        void SystemTopology::VFAT3Node::populate(const DOMElement *el)
        {
            populatePart(reference, el);
        }

    } /* namespace onlinedb */
} /* namespace gem */
