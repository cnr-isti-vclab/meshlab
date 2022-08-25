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

/*
 * $Id$
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/framework/psvi/XSModel.hpp>
#include <xercesc/framework/psvi/XSElementDeclaration.hpp>
#include <xercesc/framework/psvi/XSTypeDefinition.hpp>
#include <xercesc/framework/psvi/XSSimpleTypeDefinition.hpp>
#include <xercesc/framework/psvi/XSComplexTypeDefinition.hpp>
#include <xercesc/framework/psvi/XSParticle.hpp>
#include <xercesc/framework/psvi/XSModelGroup.hpp>
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#include <fstream>
#else
#include <iostream.h>
#include <fstream.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

XERCES_CPP_NAMESPACE_USE

// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
static void usage();

void processElements(XSNamedMap<XSObject> *xsElements);
void processTypeDefinitions(XSNamedMap<XSObject> *xsTypeDefs);
void printBasic(XSObject *xsObject, const char *type);
void printCompositorTypeConnector(XSModelGroup::COMPOSITOR_TYPE type);
void processSimpleTypeDefinition(XSSimpleTypeDefinition * xsSimpleTypeDef);
void processComplexTypeDefinition(XSComplexTypeDefinition *xsComplexTypeDef);
void processParticle(XSParticle *xsParticle);

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
{
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
}

~StrX()
{
    XMLString::release(&fLocalForm);
}


// -----------------------------------------------------------------------
//  Getter methods
// -----------------------------------------------------------------------
const char* localForm() const
{
    return fLocalForm;
}

private :
// -----------------------------------------------------------------------
//  Private data members
//
//  fLocalForm
//      This is the local code page form of the string.
// -----------------------------------------------------------------------
char*   fLocalForm;
};

inline XERCES_STD_QUALIFIER ostream& operator<<(XERCES_STD_QUALIFIER ostream& target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}

class SCMPrintHandler : public DefaultHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    SCMPrintHandler();
    ~SCMPrintHandler();

    bool getSawErrors() const
    {
        return fSawErrors;
    }

	void warning(const SAXParseException& exc);
    void error(const SAXParseException& exc);
    void fatalError(const SAXParseException& exc);
    void resetErrors();


private:
    bool            fSawErrors;
};

SCMPrintHandler::SCMPrintHandler() :
    fSawErrors(false)
{
}

SCMPrintHandler::~SCMPrintHandler()
{
}

// ---------------------------------------------------------------------------
//  SCMPrintHandler: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void SCMPrintHandler::error(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nError at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void SCMPrintHandler::fatalError(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void SCMPrintHandler::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nWarning at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void SCMPrintHandler::resetErrors()
{
    fSawErrors = false;
}

// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
    "    SCMPrint [options] <XSD file | List file>\n\n"
    "This program parses XML Schema file(s), to show how one can\n"
    "access the Schema Content Model information.\n\n"
    "Options:\n"
	"    -f     Enable full schema constraint checking processing. Defaults to off.\n"
    "    -l     Indicate the input file is a List File that has a list of XSD files.\n"
    "           Default to off (Input file is a XSD file).\n"
	"    -?     Show this help.\n\n"
    << XERCES_STD_QUALIFIER endl;
}

// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
    // Check command line and extract arguments.
    if (argC < 2)
    {
        usage();
        return 1;
    }

    // cannot return out of catch-blocks lest exception-destruction
    // result in calls to destroyed memory handler!
    int errorCode = 0;
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
        XERCES_STD_QUALIFIER cerr   << "Error during initialization! Message:\n"
        << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
        errorCode = 2;
    }
    if(errorCode) {
        XMLPlatformUtils::Terminate();
        return errorCode;
    }

    bool							doList				= false;
    bool							schemaFullChecking	= false;
    const char*                     xsdFile             = 0;
    int argInd;

    for (argInd = 1; argInd < argC; argInd++)
    {
        // Break out on first parm not starting with a dash
        if (argV[argInd][0] != '-')
            break;

        // Watch for special case help request
        if (!strcmp(argV[argInd], "-?"))
        {
            usage();
            return 1;
        }
        else if (!strcmp(argV[argInd], "-l")
              ||  !strcmp(argV[argInd], "-L"))
        {
            doList = true;
        }
        else if (!strcmp(argV[argInd], "-f")
              ||  !strcmp(argV[argInd], "-F"))
        {
            schemaFullChecking = true;
        }
        else
        {
            XERCES_STD_QUALIFIER cerr << "Unknown option '" << argV[argInd]
                << "', ignoring it\n" << XERCES_STD_QUALIFIER endl;
        }
    }

    //
    //  There should be only one and only one parameter left, and that
    //  should be the file name.
    //
    if (argInd != argC - 1)
    {
        usage();
        return 1;
    }

    XMLGrammarPool *grammarPool = 0;
    SAX2XMLReader* parser = 0;
    try
    {
        grammarPool = new XMLGrammarPoolImpl(XMLPlatformUtils::fgMemoryManager);

        parser = XMLReaderFactory::createXMLReader(XMLPlatformUtils::fgMemoryManager, grammarPool);
        parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
        parser->setFeature(XMLUni::fgXercesSchema, true);
        parser->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
        parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
        parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);
        parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
        parser->setFeature(XMLUni::fgXercesDynamic, true);
        parser->setProperty(XMLUni::fgXercesScannerName, (void *)XMLUni::fgSGXMLScanner);

        SCMPrintHandler handler;
        parser->setErrorHandler(&handler);

        bool more = true;
        bool parsedOneSchemaOkay = false;
        XERCES_STD_QUALIFIER ifstream fin;

        // the input is a list file
        if (doList)
            fin.open(argV[argInd]);

        if (fin.fail()) {
            XERCES_STD_QUALIFIER cerr <<"Cannot open the list file: " << argV[argInd] << XERCES_STD_QUALIFIER endl;
            return 3;
        }

        while (more)
        {
            char fURI[1000];
            //initialize the array to zeros
            memset(fURI,0,sizeof(fURI));

            if (doList) {
                if (! fin.eof() ) {
                    fin.getline (fURI, sizeof(fURI));
                    if (!*fURI)
                        continue;
                    else {
                        xsdFile = fURI;
                        XERCES_STD_QUALIFIER cerr << "==Parsing== " << xsdFile << XERCES_STD_QUALIFIER endl;
                    }
                }
                else
                    break;
            }
            else {
                xsdFile = argV[argInd];
                more = false;
            }

            parser->loadGrammar(xsdFile, Grammar::SchemaGrammarType, true);
            if (handler.getSawErrors())
            {
                handler.resetErrors();
            }
            else
            {
                parsedOneSchemaOkay = true;
            }
        }

        if (parsedOneSchemaOkay)
        {
            XERCES_STD_QUALIFIER cout << "********** Printing out information from Schema **********" << "\n\n";
            bool updatedXSModel;
            XSModel *xsModel = grammarPool->getXSModel(updatedXSModel);
            if (xsModel)
            {
                StringList *namespaces = xsModel->getNamespaces();
                for (unsigned i = 0; i < namespaces->size(); i++) {

                    XERCES_STD_QUALIFIER cout << "Processing Namespace:   ";
                    const XMLCh *nameSpace = namespaces->elementAt(i);
                    if (nameSpace && *nameSpace)
                        XERCES_STD_QUALIFIER cout << StrX(nameSpace);
                    XERCES_STD_QUALIFIER cout << "\n============================================" << XERCES_STD_QUALIFIER endl << XERCES_STD_QUALIFIER endl;

                    processElements(xsModel->getComponentsByNamespace(XSConstants::ELEMENT_DECLARATION,
                                                                  nameSpace));
                    processTypeDefinitions(xsModel->getComponentsByNamespace(XSConstants::TYPE_DEFINITION,
                                                                         nameSpace));
                }
            }
            else
            {
                XERCES_STD_QUALIFIER cout << "No XSModel to print" << "\n\n";
            }
        }
        else
        {
            XERCES_STD_QUALIFIER cout << "Did not parse a schema document cleanly so not printing Schema for Schema XSModel information";
        }

        XERCES_STD_QUALIFIER cout << XERCES_STD_QUALIFIER endl;
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException during parsing: '" << xsdFile << "'\n" << XERCES_STD_QUALIFIER endl;
        errorCode = 6;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xsdFile << "'\n"
        << "Exception message is:  \n"
        << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }
    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xsdFile << "'\n" << XERCES_STD_QUALIFIER endl;
        errorCode = 5;
    }

    delete parser;
    delete grammarPool;
    XMLPlatformUtils::Terminate();

    return errorCode;
}

void printBasic(XSObject *xsObject, const char *type)
{
    XERCES_STD_QUALIFIER cout << "Name:\t\t\t";
    const XMLCh *nameSpace = xsObject->getNamespace();
    if (nameSpace && *nameSpace) {
        XERCES_STD_QUALIFIER cout << StrX(nameSpace) << ", ";
    }
    XERCES_STD_QUALIFIER cout << StrX(xsObject->getName()) << "\n";
    XERCES_STD_QUALIFIER cout << "Component Type:\t" << type << XERCES_STD_QUALIFIER endl;
}

void processElements(XSNamedMap<XSObject> *xsElements)
{
    if (!xsElements || xsElements->getLength() == 0) {
        XERCES_STD_QUALIFIER cout << "no elements\n\n"  << XERCES_STD_QUALIFIER endl;
        return;
    }
    for (XMLSize_t i=0; i < xsElements->getLength(); i++) {
        XSElementDeclaration *xsElement = (XSElementDeclaration *)xsElements->item(i);
        printBasic(xsElement, "Element");

        // Content Model
        XSTypeDefinition *xsTypeDef = xsElement->getTypeDefinition();
        XERCES_STD_QUALIFIER cout << "Content Model" << "\n";
        XERCES_STD_QUALIFIER cout << "\tType:\t";
        if (xsTypeDef->getTypeCategory() == XSTypeDefinition::SIMPLE_TYPE) {
            XERCES_STD_QUALIFIER cout << "Simple\n";
        } else {
            XERCES_STD_QUALIFIER cout << "Complex\n";
        }
        XERCES_STD_QUALIFIER cout << "\tName:\t"
            << StrX(xsTypeDef->getName()) << "\n";

        XERCES_STD_QUALIFIER cout << "\n--------------------------------------------" << XERCES_STD_QUALIFIER endl;
    }
}

void processSimpleTypeDefinition(XSSimpleTypeDefinition * xsSimpleTypeDef)
{
    XSTypeDefinition *xsBaseTypeDef = xsSimpleTypeDef->getBaseType();
    XERCES_STD_QUALIFIER cout << "Base:\t\t\t";
    XERCES_STD_QUALIFIER cout << StrX(xsBaseTypeDef->getName()) << XERCES_STD_QUALIFIER endl;

    int facets = xsSimpleTypeDef->getDefinedFacets();
    if (facets) {
        XERCES_STD_QUALIFIER cout << "Facets:\n";

        if (facets & XSSimpleTypeDefinition::FACET_LENGTH)
                XERCES_STD_QUALIFIER cout << "\tLength:\t\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_LENGTH)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_MINLENGTH)
                XERCES_STD_QUALIFIER cout << "\tMinLength:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_MINLENGTH)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_MAXLENGTH)
                XERCES_STD_QUALIFIER cout << "\tMaxLength:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_MAXLENGTH)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_PATTERN) {
            StringList *lexicalPatterns = xsSimpleTypeDef->getLexicalPattern();
            if (lexicalPatterns && lexicalPatterns->size()) {
                XERCES_STD_QUALIFIER cout << "\tPattern:\t\t";
                for (unsigned i = 0; i < lexicalPatterns->size(); i++) {
                    XERCES_STD_QUALIFIER cout << StrX(lexicalPatterns->elementAt(i));
                }
                XERCES_STD_QUALIFIER cout << XERCES_STD_QUALIFIER endl;
            }
        }
        if (facets & XSSimpleTypeDefinition::FACET_WHITESPACE)
                XERCES_STD_QUALIFIER cout << "\tWhitespace:\t\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_WHITESPACE)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_MAXINCLUSIVE)
                XERCES_STD_QUALIFIER cout << "\tMaxInclusive:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_MAXINCLUSIVE)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_MAXEXCLUSIVE)
                XERCES_STD_QUALIFIER cout << "\tMaxExclusive:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_MAXEXCLUSIVE)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_MINEXCLUSIVE)
                XERCES_STD_QUALIFIER cout << "\tMinExclusive:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_MINEXCLUSIVE)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_MININCLUSIVE)
                XERCES_STD_QUALIFIER cout << "\tMinInclusive:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_MININCLUSIVE)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_TOTALDIGITS)
                XERCES_STD_QUALIFIER cout << "\tTotalDigits:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_TOTALDIGITS)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_FRACTIONDIGITS)
                XERCES_STD_QUALIFIER cout << "\tFractionDigits:\t" << StrX(xsSimpleTypeDef->getLexicalFacetValue(XSSimpleTypeDefinition::FACET_FRACTIONDIGITS)) << XERCES_STD_QUALIFIER endl;
        if (facets & XSSimpleTypeDefinition::FACET_ENUMERATION) {
            StringList *lexicalEnums = xsSimpleTypeDef->getLexicalEnumeration();
            if (lexicalEnums && lexicalEnums->size()) {
                XERCES_STD_QUALIFIER cout << "\tEnumeration:\n";
                for (unsigned i = 0; i < lexicalEnums->size(); i++) {
                    XERCES_STD_QUALIFIER cout << "\t\t\t" << StrX(lexicalEnums->elementAt(i)) << "\n";
                }
                XERCES_STD_QUALIFIER cout << XERCES_STD_QUALIFIER endl;
            }
        }
    }
}

void printCompositorTypeConnector(XSModelGroup::COMPOSITOR_TYPE type)
{
    switch (type) {
        case XSModelGroup::COMPOSITOR_SEQUENCE :
            XERCES_STD_QUALIFIER cout << ",";
            break;
        case XSModelGroup::COMPOSITOR_CHOICE :
            XERCES_STD_QUALIFIER cout << "|";
            break;
        case XSModelGroup::COMPOSITOR_ALL :
            XERCES_STD_QUALIFIER cout << "*";
            break;
    }
}

void processParticle(XSParticle *xsParticle)
{
    if (!xsParticle) {
        XERCES_STD_QUALIFIER cout << "xsParticle is NULL";
        return;
    }
    XSParticle::TERM_TYPE termType = xsParticle->getTermType();
    if (termType == XSParticle::TERM_ELEMENT) {
        XSElementDeclaration *xsElement = xsParticle->getElementTerm();
        XERCES_STD_QUALIFIER cout << StrX(xsElement->getName());
    } else if (termType == XSParticle::TERM_MODELGROUP) {
        XERCES_STD_QUALIFIER cout << "(";

        XSModelGroup *xsModelGroup = xsParticle->getModelGroupTerm();
        XSModelGroup::COMPOSITOR_TYPE compositorType = xsModelGroup->getCompositor();
        XSParticleList *xsParticleList = xsModelGroup->getParticles();
        for (unsigned i = 0; i < xsParticleList->size()-1; i++) {
            processParticle(xsParticleList->elementAt(i));
            printCompositorTypeConnector(compositorType);
        }
        processParticle(xsParticleList->elementAt(xsParticleList->size()-1));

        XERCES_STD_QUALIFIER cout << ")";
    } else if (termType == XSParticle::TERM_WILDCARD) {
        XERCES_STD_QUALIFIER cout << "* (wildcard)";
    }
}

void processComplexTypeDefinition(XSComplexTypeDefinition *xsComplexTypeDef)
{
    XSTypeDefinition *xsBaseTypeDef = xsComplexTypeDef->getBaseType();
    if (xsBaseTypeDef) {
        XERCES_STD_QUALIFIER cout << "Base:\t\t\t";
        XERCES_STD_QUALIFIER cout << StrX(xsBaseTypeDef->getName()) << "\n";
    }

    XERCES_STD_QUALIFIER cout << "Content Model:\t";
    XSComplexTypeDefinition::CONTENT_TYPE contentType = xsComplexTypeDef->getContentType();
    if (contentType == XSComplexTypeDefinition::CONTENTTYPE_ELEMENT ||
        contentType == XSComplexTypeDefinition::CONTENTTYPE_MIXED) {
        processParticle(xsComplexTypeDef->getParticle());
        XERCES_STD_QUALIFIER cout << XERCES_STD_QUALIFIER endl;
    }
}

void processTypeDefinitions(XSNamedMap<XSObject> *xsTypeDefs)
{
    if (!xsTypeDefs) return;

    for (XMLSize_t i=0; i < xsTypeDefs->getLength(); i++) {
        XSTypeDefinition *xsTypeDef = (XSTypeDefinition *)xsTypeDefs->item(i);

        printBasic(xsTypeDef, "Type Definition");

        // Content Model
        XERCES_STD_QUALIFIER cout << "Category:\t";
        if (xsTypeDef->getTypeCategory() == XSTypeDefinition::SIMPLE_TYPE) {
            XERCES_STD_QUALIFIER cout << "\tSimple\n";
            processSimpleTypeDefinition((XSSimpleTypeDefinition *)xsTypeDef);
        } else {
            XERCES_STD_QUALIFIER cout << "\tComplex\n";
            processComplexTypeDefinition((XSComplexTypeDefinition *)xsTypeDef);
        }

        XERCES_STD_QUALIFIER cout << "\n--------------------------------------------" << XERCES_STD_QUALIFIER endl;
    }
}
