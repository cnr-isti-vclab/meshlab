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
#include <xercesc/util/NameIdPool.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/framework/psvi/XSAnnotation.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdlib.h>
#include <string.h>


XERCES_CPP_NAMESPACE_USE

// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
static void usage();

void process(char* const);
void processAttributes( XMLAttDefList& attList, bool margin = false );
void processDatatypeValidator( const DatatypeValidator*, bool margin = false
);
void processContentSpecNode( const ContentSpecNode* specNode, bool margin =
false );

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

// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    SEnumVal <XML file>\n\n"
            "This program parses a file, then shows how to enumerate the\n"
            "contents of the Schema Grammar. Essentially, shows how one can\n"
            "access the Schema information stored in internal data structures.\n"
         << XERCES_STD_QUALIFIER endl;
}

// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
    // cannot return out of catch-blocks lest exception-destruction
    // result in calls to destroyed memory handler!
    int errorCode = 0;
    // Initialize the XML4C system
    try
    {
         XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
         XERCES_STD_QUALIFIER cerr   << "Error during initialization! Message:\n"
                << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
         errorCode = 1;
    }
    if(errorCode) {
        XMLPlatformUtils::Terminate();
        return errorCode;
    } 

    // Check command line and extract arguments.
    // We only have one required parameter, which is the file to process
    if ((argC != 2) ||
        (*(argV[1]) == '-'))
    {
        usage();
        XMLPlatformUtils::Terminate();
        return 2;
    }

    try
    {
		process(argV[1]);
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorCode = 5;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << argV[1] << "'\n"
             << "Exception message is:  \n"
             << StrX(e.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
        errorCode = 3;
    }

    XMLPlatformUtils::Terminate();

	return errorCode;
}

void process(char* const xmlFile)
{
    //
    //  Create a Schema validator to be used for our validation work. Then create
    //  a SAX parser object and pass it our validator. Then, according to what
    //  we were told on the command line, set it to validate or not. He owns
    //  the validator, so we have to allocate it.
    //
    SAXParser parser;
    parser.setValidationScheme(SAXParser::Val_Always);
    parser.setDoNamespaces(true);
    parser.setDoSchema(true);

	parser.parse(xmlFile);

    if (parser.getErrorCount())
	{
        XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;
		return;
	}

	if (!parser.getValidator().handlesSchema())
	{
		XERCES_STD_QUALIFIER cout << "\n Non schema document, no output available\n" << XERCES_STD_QUALIFIER endl;
		return;
	}

	Grammar* rootGrammar = parser.getRootGrammar();
	if (!rootGrammar || rootGrammar->getGrammarType() != Grammar::SchemaGrammarType)
	{
		XERCES_STD_QUALIFIER cout << "\n Non schema grammar, no output available\n" << XERCES_STD_QUALIFIER endl;
		return;
	}

	//
	//  Now we will get an enumerator for the element pool from the validator
	//  and enumerate the elements, printing them as we go. For each element
	//  we get an enumerator for its attributes and print them also.
	//

	SchemaGrammar* grammar = (SchemaGrammar*) rootGrammar;
	RefHash3KeysIdPoolEnumerator<SchemaElementDecl> elemEnum = grammar->getElemEnumerator();

	if (!elemEnum.hasMoreElements())
	{
		XERCES_STD_QUALIFIER cout << "\nThe validator has no elements to display\n" << XERCES_STD_QUALIFIER endl;
		return;
	}

	while(elemEnum.hasMoreElements())
	{
		const SchemaElementDecl& curElem = elemEnum.nextElement();

		// Name
		XERCES_STD_QUALIFIER cout << "Name:\t\t\t" << StrX(curElem.getFullName()) << "\n";

		// Model Type
		XERCES_STD_QUALIFIER cout << "Model Type:\t\t";
		switch( curElem.getModelType() )
		{
		case SchemaElementDecl::Empty:          XERCES_STD_QUALIFIER cout << "Empty";         break;
		case SchemaElementDecl::Any:            XERCES_STD_QUALIFIER cout << "Any";           break;
		case SchemaElementDecl::Mixed_Simple:   XERCES_STD_QUALIFIER cout << "Mixed_Simple";  break;
		case SchemaElementDecl::Mixed_Complex:  XERCES_STD_QUALIFIER cout << "Mixed_Complex"; break;
		case SchemaElementDecl::Children:       XERCES_STD_QUALIFIER cout << "Children";      break;
		case SchemaElementDecl::Simple:         XERCES_STD_QUALIFIER cout << "Simple";        break;
        case SchemaElementDecl::ElementOnlyEmpty:    XERCES_STD_QUALIFIER cout << "ElementOnlyEmpty";    break;

		default:                                XERCES_STD_QUALIFIER cout << "Unknown";       break;
		}

		XERCES_STD_QUALIFIER cout << "\n";

		// Create Reason
		XERCES_STD_QUALIFIER cout << "Create Reason:\t";
		switch( curElem.getCreateReason() )
		{
		case XMLElementDecl::NoReason:          XERCES_STD_QUALIFIER cout << "Empty";            break;
		case XMLElementDecl::Declared:          XERCES_STD_QUALIFIER cout << "Declared";         break;
		case XMLElementDecl::AttList:           XERCES_STD_QUALIFIER cout << "AttList";          break;
		case XMLElementDecl::InContentModel:    XERCES_STD_QUALIFIER cout << "InContentModel";   break;
		case XMLElementDecl::AsRootElem:        XERCES_STD_QUALIFIER cout << "AsRootElem";       break;
		case XMLElementDecl::JustFaultIn:       XERCES_STD_QUALIFIER cout << "JustFaultIn";      break;

		default:                            XERCES_STD_QUALIFIER cout << "Unknown";  break;
		}

		XERCES_STD_QUALIFIER cout << "\n";

		// Content Spec Node
		processContentSpecNode( curElem.getContentSpec() );

		// Misc Flags
		int mflags = curElem.getMiscFlags();
		if( mflags !=0 )
		{
			XERCES_STD_QUALIFIER cout << "Misc. Flags:\t";
		}

        if ( mflags & SchemaSymbols::XSD_NILLABLE )
			XERCES_STD_QUALIFIER cout << "Nillable ";

		if ( mflags & SchemaSymbols::XSD_ABSTRACT )
			XERCES_STD_QUALIFIER cout << "Abstract ";

		if ( mflags & SchemaSymbols::XSD_FIXED )
			XERCES_STD_QUALIFIER cout << "Fixed ";

		if( mflags !=0 )
		{
			XERCES_STD_QUALIFIER cout << "\n";
		}

		// Substitution Name
		SchemaElementDecl* subsGroup = curElem.getSubstitutionGroupElem();
		if( subsGroup )
		{
			const XMLCh* uriText = parser.getURIText(subsGroup->getURI());
			XERCES_STD_QUALIFIER cout << "Substitution Name:\t" << StrX(uriText)
			     << "," << StrX(subsGroup->getBaseName()) << "\n";
		}

		// Content Model
		const XMLCh* fmtCntModel = curElem.getFormattedContentModel();
		if( fmtCntModel != NULL )
		{
			XERCES_STD_QUALIFIER cout << "Content Model:\t" << StrX(fmtCntModel) << "\n";
		}

		const ComplexTypeInfo* ctype = curElem.getComplexTypeInfo();
		if( ctype != NULL)
		{
			XERCES_STD_QUALIFIER cout << "ComplexType:\n";
			XERCES_STD_QUALIFIER cout << "\tTypeName:\t" << StrX(ctype->getTypeName()) << "\n";

			ContentSpecNode* cSpecNode = ctype->getContentSpec();
			processContentSpecNode(cSpecNode, true );
		}

		// Datatype
		DatatypeValidator* dtValidator = curElem.getDatatypeValidator();
		processDatatypeValidator( dtValidator );

		// Get an enumerator for this guy's attributes if any
		if ( curElem.hasAttDefs() )
		{
			processAttributes( curElem.getAttDefList() );
		}

		XERCES_STD_QUALIFIER cout << "--------------------------------------------";
		XERCES_STD_QUALIFIER cout << XERCES_STD_QUALIFIER endl;

    }

    return;
}


//---------------------------------------------------------------------
//  Prints the Attribute's properties
//---------------------------------------------------------------------
void processAttributes( XMLAttDefList& attList, bool margin )
{
    if ( attList.isEmpty() )
    {
        return;
    }

    if ( margin )
    {
        XERCES_STD_QUALIFIER cout << "\t";
    }

    XERCES_STD_QUALIFIER cout << "Attributes:\n";
    for (unsigned int i=0; i<attList.getAttDefCount(); i++)
    {
        // Name
        SchemaAttDef& curAttDef = (SchemaAttDef&)attList.getAttDef(i);
        XERCES_STD_QUALIFIER cout << "\tName:\t\t\t" << StrX(curAttDef.getFullName()) << "\n";

        // Type
        XERCES_STD_QUALIFIER cout << "\tType:\t\t\t";
		XERCES_STD_QUALIFIER cout << StrX(XMLAttDef::getAttTypeString(curAttDef.getType()));
        XERCES_STD_QUALIFIER cout << "\n";

        // Default Type
        XERCES_STD_QUALIFIER cout << "\tDefault Type:\t";
		XERCES_STD_QUALIFIER cout << StrX(XMLAttDef::getDefAttTypeString(curAttDef.getDefaultType()));
        XERCES_STD_QUALIFIER cout << "\n";

        // Value
        if( curAttDef.getValue() )
        {
            XERCES_STD_QUALIFIER cout << "\tValue:\t\t\t";
            XERCES_STD_QUALIFIER cout << StrX(curAttDef.getValue());
            XERCES_STD_QUALIFIER cout << "\n";
        }

        // Enum. values
        if( curAttDef.getEnumeration() )
        {
            XERCES_STD_QUALIFIER cout << "\tEnumeration:\t";
            XERCES_STD_QUALIFIER cout << StrX(curAttDef.getEnumeration());
            XERCES_STD_QUALIFIER cout << "\n";
        }

         const DatatypeValidator* dv = curAttDef.getDatatypeValidator();
         processDatatypeValidator( dv, true );

        XERCES_STD_QUALIFIER cout << "\n";
    }
}

void processDatatypeValidator( const DatatypeValidator* dtValidator, bool margin )
{
    if( !dtValidator )
    {
        return;
    }

    if( margin )
    {
        XERCES_STD_QUALIFIER cout << "\t";
    }

    XERCES_STD_QUALIFIER cout << "Base Datatype:\t\t";
    switch( dtValidator->getType() )
    {
    case DatatypeValidator::String:         XERCES_STD_QUALIFIER cout << "string";      break;
    case DatatypeValidator::AnyURI:         XERCES_STD_QUALIFIER cout << "AnyURI";      break;
    case DatatypeValidator::QName:          XERCES_STD_QUALIFIER cout << "QName";       break;
    case DatatypeValidator::Name:           XERCES_STD_QUALIFIER cout << "Name";        break;
    case DatatypeValidator::NCName:         XERCES_STD_QUALIFIER cout << "NCName";      break;
    case DatatypeValidator::Boolean:        XERCES_STD_QUALIFIER cout << "Boolean";     break;
    case DatatypeValidator::Float:          XERCES_STD_QUALIFIER cout << "Float";       break;
    case DatatypeValidator::Double:         XERCES_STD_QUALIFIER cout << "Double";      break;
    case DatatypeValidator::Decimal:        XERCES_STD_QUALIFIER cout << "Decimal";     break;
    case DatatypeValidator::HexBinary:      XERCES_STD_QUALIFIER cout << "HexBinary";   break;
    case DatatypeValidator::Base64Binary:   XERCES_STD_QUALIFIER cout << "Base64Binary";break;
    case DatatypeValidator::Duration:       XERCES_STD_QUALIFIER cout << "Duration";    break;
    case DatatypeValidator::DateTime:       XERCES_STD_QUALIFIER cout << "DateTime";    break;
    case DatatypeValidator::Date:           XERCES_STD_QUALIFIER cout << "Date";        break;
    case DatatypeValidator::Time:           XERCES_STD_QUALIFIER cout << "Time";        break;
    case DatatypeValidator::MonthDay:       XERCES_STD_QUALIFIER cout << "MonthDay";    break;
    case DatatypeValidator::YearMonth:      XERCES_STD_QUALIFIER cout << "YearMonth";   break;
    case DatatypeValidator::Year:           XERCES_STD_QUALIFIER cout << "Year";        break;
    case DatatypeValidator::Month:          XERCES_STD_QUALIFIER cout << "Month";       break;
    case DatatypeValidator::Day:            XERCES_STD_QUALIFIER cout << "Day";         break;
    case DatatypeValidator::ID:             XERCES_STD_QUALIFIER cout << "ID";          break;
    case DatatypeValidator::IDREF:          XERCES_STD_QUALIFIER cout << "IDREF";       break;
    case DatatypeValidator::ENTITY:         XERCES_STD_QUALIFIER cout << "ENTITY";      break;
    case DatatypeValidator::NOTATION:       XERCES_STD_QUALIFIER cout << "NOTATION";    break;
    case DatatypeValidator::List:           XERCES_STD_QUALIFIER cout << "List";        break;
    case DatatypeValidator::Union:          XERCES_STD_QUALIFIER cout << "Union";       break;
    case DatatypeValidator::AnySimpleType:  XERCES_STD_QUALIFIER cout << "AnySimpleType"; break;
    case DatatypeValidator::UnKnown:        XERCES_STD_QUALIFIER cout << "UNKNOWN";     break;
    }

    XERCES_STD_QUALIFIER cout << "\n";

    // Facets
	RefHashTableOf<KVStringPair>* facets = dtValidator->getFacets();
    if( facets && facets->getCount()>0)
    {
        XMLSize_t i;
        // Element's properties
        XERCES_STD_QUALIFIER cout << "Facets:\t\t\n";
        // use a list to print them sorted, or the list could be different on 64-bit machines
        RefVectorOf<XMLCh> sortedList(facets->getCount(), false);
        RefHashTableOfEnumerator<KVStringPair> enumFacets(facets);
        while( enumFacets.hasMoreElements() )
        {
            const KVStringPair& curPair = enumFacets.nextElement();
            const XMLCh* key=curPair.getKey();
            XMLSize_t len=sortedList.size();
            for(i=0;i<len;i++)
                if(XMLString::compareString(key, sortedList.elementAt(i))<0)
                {
                    sortedList.insertElementAt((XMLCh*)key,i);
                    break;
                }
            if(i==len)
                sortedList.addElement((XMLCh*)key);
        }

        XMLSize_t len=sortedList.size();
        for(i=0;i<len;i++)
        {
            const XMLCh* key = sortedList.elementAt(i);
            XERCES_STD_QUALIFIER cout << "\t" << StrX( key )    << "="
                         << StrX( facets->get(key)->getValue() )  << "\n";
        }
    }

	// Enumerations
	RefVectorOf<XMLCh>* enums = (RefVectorOf<XMLCh>*) dtValidator->getEnumString();
	if (enums)
	{
		XERCES_STD_QUALIFIER cout << "Enumeration:\t\t\n";

		XMLSize_t enumLength = enums->size();
		for ( XMLSize_t i = 0; i < enumLength; i++)
        {
            XERCES_STD_QUALIFIER cout << "\t" << StrX( enums->elementAt(i)) << "\n";
        }

	}
}

void processContentSpecNode( const ContentSpecNode* cSpecNode, bool margin )
{
    if( !cSpecNode )
    {
        return;
    }

    if( margin )
    {
        XERCES_STD_QUALIFIER cout << "\t";
    }

    XERCES_STD_QUALIFIER cout << "ContentType:\t";
    switch( cSpecNode->getType() )
    {
        case ContentSpecNode::Leaf:                XERCES_STD_QUALIFIER cout << "Leaf";           break;
        case ContentSpecNode::ZeroOrOne:           XERCES_STD_QUALIFIER cout << "ZeroOrOne";      break;
        case ContentSpecNode::ZeroOrMore:          XERCES_STD_QUALIFIER cout << "ZeroOrMore";     break;
        case ContentSpecNode::OneOrMore:           XERCES_STD_QUALIFIER cout << "OneOrMore";      break;
        case ContentSpecNode::ModelGroupChoice:    
        case ContentSpecNode::Choice:              XERCES_STD_QUALIFIER cout << "Choice";         break;
        case ContentSpecNode::ModelGroupSequence:  
        case ContentSpecNode::Sequence:            XERCES_STD_QUALIFIER cout << "Sequence";       break;
        case ContentSpecNode::All:                 XERCES_STD_QUALIFIER cout << "All";            break;
        case ContentSpecNode::Loop:                XERCES_STD_QUALIFIER cout << "Loop";           break;
        case ContentSpecNode::Any:                 XERCES_STD_QUALIFIER cout << "Any";            break;
        case ContentSpecNode::Any_Other:           XERCES_STD_QUALIFIER cout << "Any_Other";      break;
        case ContentSpecNode::Any_NS:              XERCES_STD_QUALIFIER cout << "Any_NS";         break;
        case ContentSpecNode::Any_Lax:             XERCES_STD_QUALIFIER cout << "Any_Lax";        break;
        case ContentSpecNode::Any_Other_Lax:       XERCES_STD_QUALIFIER cout << "Any_Other_Lax";  break;
        case ContentSpecNode::Any_NS_Lax:          XERCES_STD_QUALIFIER cout << "Any_NS_Lax";     break;
        case ContentSpecNode::Any_Skip:            XERCES_STD_QUALIFIER cout << "Any_Skip";       break;
        case ContentSpecNode::Any_Other_Skip:      XERCES_STD_QUALIFIER cout << "Any_Other_Skip"; break;
        case ContentSpecNode::Any_NS_Skip:         XERCES_STD_QUALIFIER cout << "Any_NS_Skip";    break;
        case ContentSpecNode::Any_NS_Choice:       XERCES_STD_QUALIFIER cout << "Any_NS_Choice";  break;
        case ContentSpecNode::UnknownType:         XERCES_STD_QUALIFIER cout << "UnknownType";    break;
    }
    XERCES_STD_QUALIFIER cout << "\n";
}

