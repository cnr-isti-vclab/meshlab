/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PSVIWRITERHANDLER_HPP
#define PSVIWRITERHANDLER_HPP

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/framework/psvi/XSConstants.hpp>
#include <xercesc/framework/psvi/PSVIHandler.hpp>
#include <xercesc/framework/psvi/PSVIAttribute.hpp>
#include <xercesc/framework/psvi/PSVIAttributeList.hpp>
#include <xercesc/framework/psvi/PSVIElement.hpp>
#include <xercesc/framework/psvi/PSVIItem.hpp>
#include <xercesc/framework/psvi/XSAnnotation.hpp>
#include <xercesc/framework/psvi/XSAttributeDeclaration.hpp>
#include <xercesc/framework/psvi/XSAttributeGroupDefinition.hpp>
#include <xercesc/framework/psvi/XSAttributeUse.hpp>
#include <xercesc/framework/psvi/XSComplexTypeDefinition.hpp>
#include <xercesc/framework/psvi/XSElementDeclaration.hpp>
#include <xercesc/framework/psvi/XSFacet.hpp>
#include <xercesc/framework/psvi/XSIDCDefinition.hpp>
#include <xercesc/framework/psvi/XSModel.hpp>
#include <xercesc/framework/psvi/XSModelGroup.hpp>
#include <xercesc/framework/psvi/XSModelGroupDefinition.hpp>
#include <xercesc/framework/psvi/XSMultiValueFacet.hpp>
#include <xercesc/framework/psvi/XSNamedMap.hpp>
#include <xercesc/framework/psvi/XSNamespaceItem.hpp>
#include <xercesc/framework/psvi/XSNotationDeclaration.hpp>
#include <xercesc/framework/psvi/XSParticle.hpp>
#include <xercesc/framework/psvi/XSSimpleTypeDefinition.hpp>
#include <xercesc/framework/psvi/XSTypeDefinition.hpp>
#include <xercesc/framework/psvi/XSWildcard.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/XMLDocumentHandler.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/ValueStackOf.hpp>
#include <xercesc/util/ValueVectorOf.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/util/XMLResourceIdentifier.hpp>
#include <stdlib.h>
#include <string.h>

XERCES_CPP_NAMESPACE_USE


class AttrInfo {
public:
	AttrInfo(const XMLCh* pUri, const XMLCh* pName, const XMLCh* pType, const XMLCh* pValue) {
		uri = XMLString::replicate(pUri);
		name = XMLString::replicate(pName);
		type = XMLString::replicate(pType);
		value = XMLString::replicate(pValue);
	}

	~AttrInfo() {
		XMLString::release((XMLCh**)&uri);
		XMLString::release((XMLCh**)&name);
		XMLString::release((XMLCh**)&type);
		XMLString::release((XMLCh**)&value);
	}

	const XMLCh* getUri() const {
		return uri;
	}

	const XMLCh* getLocalName() const {
		return name;
	}

	const XMLCh* getType() const {
		return type;
	}

	const XMLCh* getValue() const {
		return value;
	}

private:
	const XMLCh* uri;
	const XMLCh* name;
	const XMLCh* type;
	const XMLCh* value;
};

class PSVIWriterHandlers : public PSVIHandler, public DefaultHandler, public XMLEntityResolver {
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    PSVIWriterHandlers(XMLFormatter* outputFormatter, XMLFormatter* errorFormatter = NULL);
    ~PSVIWriterHandlers();

    friend class PSVIAdvancedHandler;
    // -----------------------------------------------------------------------
    //  Convenience Utility
    // -----------------------------------------------------------------------
	void resetPSVIFormatter(XMLFormatter* outputFormatter);
    void resetDocument();

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ContentHandler interface
    // -----------------------------------------------------------------------
    void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs);
    void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
    void startDocument();
    void endDocument();
    void characters(const XMLCh* const chars, const XMLSize_t length);
    void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length);
    void comment(const XMLCh* const chars, const XMLSize_t length);
    void processingInstruction(const XMLCh* const target, const XMLCh* const data);
    void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);
    void endPrefixMapping(const XMLCh* const prefix);
    InputSource* resolveEntity(XMLResourceIdentifier* resourceIdentifier);
    InputSource* resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId);

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
	void warning(const SAXParseException& exc);
    void error(const SAXParseException& exc);
    void fatalError(const SAXParseException& exc);
    void resetErrors();

    // -----------------------------------------------------------------------
    //  Handlers for the PSVIHandler interface
    // -----------------------------------------------------------------------

	void handleAttributesPSVI( const XMLCh* const localName,
								const XMLCh* const uri,
								PSVIAttributeList* psviAttributes );
	void handleElementPSVI(	const XMLCh* const localName,
                                const XMLCh* const uri,
                                PSVIElement* elementInfo );
	void handlePartialElementPSVI( const XMLCh* const localName,
                                   const XMLCh* const uri,
                                   PSVIElement* elementInfo );
private:
    // -----------------------------------------------------------------------
    //  Private methods
    // -----------------------------------------------------------------------

    void processAttributes(PSVIAttributeList* psviAttributes, const RefVectorOf<AttrInfo>* attributesInfo);
    void processNamespaceAttributes(PSVIAttributeList* psviAttributes, const RefVectorOf<AttrInfo>* attributes);
    void processAttributePSVI(PSVIAttribute* attribute);
    void processInScopeNamespaces();
    void processActualValue(PSVIItem*);
    void formDateTime(XSValue*);

    void processSchemaInformation(XSModel* model);
    void processNamespaceItem(XSNamespaceItem* namespaceItem);
    void processSchemaComponents(XSNamespaceItem* namespaceItem);
    void processSchemaDocuments(XSNamespaceItem* namespaceItem);
    void processSchemaAnnotations(XSAnnotationList* annotations);
    void processSchemaErrorCode(StringList* errors);

    void processTypeDefinition(XSTypeDefinition* type);
    void processComplexTypeDefinition(XSComplexTypeDefinition* complexType);
    void processSimpleTypeDefinition(XSSimpleTypeDefinition* simpleType);
    void processModelGroupDefinition(XSModelGroupDefinition* modelGroup);
    void processAttributeGroupDefinition(XSAttributeGroupDefinition* attributeGroup);

    void processElementDeclaration(XSElementDeclaration* element);
    void processAttributeDeclaration(XSAttributeDeclaration* attribute);
    void processNotationDeclaration(XSNotationDeclaration* notation);

    void processAnnotations(XSAnnotationList* annotations);
    void processAttributeUses(XSAttributeUseList* attributeUses);
    void processFacets(XSFacetList* facets, XSMultiValueFacetList* multiFacets);
    void processFundamentalFacets(XSSimpleTypeDefinition* facets);
    void processMemberTypeDefinitions(XSSimpleTypeDefinitionList* memberTypes);

    void processAnnotation(XSAnnotation* annotation);
    void processDOMElement(const XMLCh* const encloseName, DOMElement* rootElem, const XMLCh* const elementName);
    void processDOMAttributes(DOMNamedNodeMap* attrs);
    void processWildcard(XSWildcard* wildcard);
    void processModelGroup(XSModelGroup* modelGroup);
    void processParticle(XSParticle* particle);

    void processAttributeWildcard(XSWildcard* wildcard);
    void processScope(XSComplexTypeDefinition* enclosingCTD, short scope);
    void processValueConstraint(XSConstants::VALUE_CONSTRAINT ConstraintType, const XMLCh* constraintValue);

    void processIdentityConstraintDefinition(XSNamedMap<XSIDCDefinition>* identityConstraint);
    void processFields(StringList* fields);
    void processXPath(const XMLCh* xpath);

    void processChildren();
    void processChildrenEnd();

    void processTypeDefinitionOrRef(const XMLCh* enclose, XSTypeDefinition* type);
	void processSimpleTypeDefinitionOrRef(XSSimpleTypeDefinition* type);
    void processAttributeDeclarationOrRef(XSAttributeDeclaration* attrDecl);
    void processElementDeclarationOrRef(XSElementDeclaration* elemDecl);
	void processTypeDefinitionRef(const XMLCh* enclose, XSTypeDefinition* type);
    void processAttributeDeclarationRef(const XMLCh* enclose, XSAttributeDeclaration* attrDecl);
    void processElementDeclarationRef(const XMLCh* enclose, XSElementDeclaration* elemDecl);
    void sendReference(const XMLCh* elementName, XSObject* obj);

    void sendElementEmpty(const XMLCh* const elementName);
	void sendElementValueInt(const XMLCh* const elementName, int value);
    void sendElementValue(const XMLCh* const elementName, const XMLCh* const value);
    void sendElementValueList(const XMLCh* const elementName, const StringList* const values);

	void sendIndentedElement(const XMLCh* const elementName);
    void sendIndentedElementWithID(const XMLCh* const elementName, XSObject* obj);	//adds the ID to the attribute list before sending
    void sendUnindentedElement(const XMLCh* const elementName);

    void writeOpen(const XMLCh* const elementName);
	void writeOpen(const XMLCh* const elementName, const StringList* const attrs);
	void writeClose(const XMLCh* const elementName);
	void writeValue(const XMLCh* const elementName, const XMLCh* const value);
	void writeValue(const XMLCh* const elementName, const StringList* const values);
	void writeEmpty(const XMLCh* const elementName, const StringList* const attrs);
	void writeEmpty(const XMLCh* const elementName);
    void writeString(const XMLCh* const string);

    const XMLCh* translateScope(XSConstants::SCOPE scope);
    const XMLCh* translateValueConstraint(XSConstants::VALUE_CONSTRAINT constraintKind);
    const XMLCh* translateBlockOrFinal(short val);
    const XMLCh* translateDerivationMethod(XSConstants::DERIVATION_TYPE derivation);
    const XMLCh* translateProcessContents(XSWildcard::PROCESS_CONTENTS processContents);
    const XMLCh* translateCompositor(XSModelGroup::COMPOSITOR_TYPE compositor);
    const XMLCh* translateValidity(PSVIItem::VALIDITY_STATE validity);
    const XMLCh* translateValidationAttempted(PSVIItem::ASSESSMENT_TYPE validation);
    const XMLCh* translateIdConstraintCategory(XSIDCDefinition::IC_CATEGORY category);
    const XMLCh* translateComplexContentType(XSComplexTypeDefinition::CONTENT_TYPE contentType);
    const XMLCh* translateSimpleTypeVariety(XSSimpleTypeDefinition::VARIETY variety);
    const XMLCh* translateOrderedFacet(XSSimpleTypeDefinition::ORDERING ordered);
    const XMLCh* translateFacet(XSSimpleTypeDefinition::FACET facetKind);
    const XMLCh* translateComponentType(XSConstants::COMPONENT_TYPE type);
    const XMLCh* translateBool(bool flag);

    XMLCh* createID(XSObject* obj);
    const XMLCh* getIdName(XSObject* obj);
    void incIndent();
    void decIndent();

protected:
	XMLFormatter* fFormatter;
	XMLFormatter* fErrorFormatter;

	StringList* fAttrList;
    XMLCh* fTempResult;
    XMLCh* fIndentChars;
    XMLCh* fBaseUri;

    unsigned int fIndent;
    unsigned int fIndentCap;
    unsigned int fAnonNum;

	RefHashTableOf<XMLCh>* fIdMap;
    RefVectorOf<XSObject>* fDefinedIds;
    RefArrayVectorOf<XMLCh>* fIdNames;
    RefArrayVectorOf<XMLCh>* fObjectLocations;

	RefHashTableOf<XMLCh>* fPrefixMap;
    RefArrayVectorOf<XMLCh>* fNamespaces;

	ValueVectorOf<XMLSize_t>* fNSAttributes;  //REVISIT  dont need if NSAttrs in different object
	ValueStackOf<bool>* fElementChildren;

	RefVectorOf<AttrInfo>* fAttributesInfo;
};

class PSVIAdvancedHandler: public XMLDocumentHandler {
public:
    PSVIAdvancedHandler(PSVIWriterHandlers* writerHandler) : XMLDocumentHandler(), fWriterHandler(writerHandler) {}
    ~PSVIAdvancedHandler() {}
    void docCharacters(const XMLCh* const, const XMLSize_t, const bool) {}
    void docComment(const XMLCh* const) {}
    void docPI(const XMLCh* const, const XMLCh* const) {}
    void endDocument() {}
    void endElement(const XMLElementDecl&, const unsigned int, const bool, const XMLCh* const) {}
    void endEntityReference(const   XMLEntityDecl&) {}

    void ignorableWhitespace(const XMLCh* const, const XMLSize_t, const bool) {}

    void resetDocument() {}
    void startDocument() {}
    void startElement(const XMLElementDecl&, const unsigned int, const XMLCh* const, const RefVectorOf<XMLAttr>&
                     ,const XMLSize_t, const bool, const bool) {}
    void startEntityReference(const XMLEntityDecl&) {};

    void XMLDecl(const XMLCh* const versionStr, const XMLCh* const encodingStr, const XMLCh* const standaloneStr, const XMLCh* const autoEncodingStr);
private:
    PSVIWriterHandlers* fWriterHandler;
};

#endif
