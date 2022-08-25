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
 *
 * @author Andy Heninger, IBM
 */

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

#include <xercesc/dom/DOM.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/internal/MemoryManagerImpl.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

void clearFileInfoMemory();

#ifdef HAVE_PTHREAD
#include <pthread.h>
#include <unistd.h>
#include <errno.h>


//------------------------------------------------------------------------------
//
//   UNIX specific code for starting threads
//
//------------------------------------------------------------------------------

extern "C" {

typedef void (*ThreadFunc)(void *);
typedef void *(*pthreadfunc)(void *);

class ThreadFuncs           // This class isolates OS dependent threading
{                           //   functions from the rest of ThreadTest program.
public:
    static void Sleep(int millis);
    static void startThread(ThreadFunc, void *param);
};

void ThreadFuncs::Sleep(int millis)
{
   int seconds = millis/1000;
   if (seconds <= 0) seconds = 1;
#if defined(SOLARIS)
   // somehow the sleep hangs on Solaris
   // so ignore the call
#else
   ::sleep(seconds);
#endif
}


void ThreadFuncs::startThread(ThreadFunc func, void *param)
{
    int x;

    pthread_t tId;
    //thread_t tId;
#if defined(_HP_UX) && defined(XML_USE_DCE)
    x = pthread_create( &tId, pthread_attr_default,  (pthreadfunc)func,  param);
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    x = pthread_create( &tId, &attr,  (pthreadfunc)func,  param);
#endif
    if (x == -1)
    {
        fprintf(stderr, "Error starting thread.  Errno = %d\n", errno);
        clearFileInfoMemory();
        exit(-1);
    }

}

} // end of extern "C"

#else

//------------------------------------------------------------------------------
//
//   Windows specific code for starting threads
//
//------------------------------------------------------------------------------

#include <Windows.h>
#include <process.h>

typedef DWORD (WINAPI *ThreadFunc)(void *);

class ThreadFuncs           // This class isolates OS dependent threading
{                           //   functions from the rest of ThreadTest program.
public:
    static void Sleep(int millis) {::Sleep(millis);};
    static void startThread(ThreadFunc, void *param);
};

void ThreadFuncs::startThread(ThreadFunc func, void *param)
{
    HANDLE  tHandle;
    DWORD   threadID;

    tHandle = CreateThread(0,          // Security Attributes,
                           0x10000,    // Stack Size,
                           func,       // Starting Address.
                           param,      // Parmeters
                           0,          // Creation Flags,
                           &threadID); // Thread ID (Can not be null on 95/98)

    if (tHandle == 0)
    {
        fprintf(stderr, "Error starting thread.  Errno = %d\n", errno);
        clearFileInfoMemory();
        exit(-1);
    }

    // Set the priority of the working threads low, so that the UI of the running system will
    //   remain responsive.
    SetThreadPriority(tHandle, THREAD_PRIORITY_IDLE);
}

#endif


//------------------------------------------------------------------------------
//
//  struct InFileInfo   One of these structs will be set up for each file listed
//                      on the command line.  Once set, the data is unchanging
//                      and can safely be referenced by the test threads without
//                      use of synchronization.
//
//------------------------------------------------------------------------------
struct InFileInfo
{
    char    *fileName;
    XMLCh   *uFileName;      // When doing an in-memory parse, avoid transcoding file name
                             //    each time through.
    char    *fileContent;    // If doing an in-memory parse, this field points
                             //   to an allocated string containing the entire file
                             //   contents.  Otherwise it's 0.
    size_t  fileSize;        // The file length.  Only initialized when doing
                             //   an in-memory test.
    int     checkSum;        // The XML checksum.  Set up by the main thread for
                             //   each file before the worker threads are started.
};


//------------------------------------------------------------------------------
//
//  struct threadInfo  Holds information specific to an individual thread.
//                     One of these is set up for each thread in the test.
//                     The main program monitors the threads by looking
//                     at the status stored in these structs.
//
//------------------------------------------------------------------------------
struct ThreadInfo
{
    bool            fHeartBeat;   // Set true by the thread each time it finishes
                                  //   parsing a file.
    bool            fInProgress;  // Set to false by the thread when parse in progress
    unsigned int    fParses;      // Number of parses completed.
    int             fThreadNum;   // Identifying number for this thread.
    ThreadInfo() {
        fHeartBeat = false;
        fInProgress = false;
        fParses = 0;
        fThreadNum = -1;
    }
};


XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------
//
//  struct runInfo     Holds the info extracted from the command line.
//                     There is only one of these, and it is static, and
//                     unchanging once the command line has been parsed.
//                     During the test, the threads will access this info without
//                     any synchronization.
//
//------------------------------------------------------------------------------
const int MAXINFILES = 25;
struct RunInfo
{
    bool                            doGrammarCaching;
    bool                            quiet;
    bool                            verbose;
    bool                            stopNow;
    bool                            dom;
    bool                            sax;
    bool                            reuseParser;
    bool                            inMemory;
    bool                            dumpOnErr;
    bool                            doSchema;
    bool                            schemaFullChecking;
    bool                            doNamespaces;
    bool                            doInitialParse;
    bool                            doNamespacePrefixes;  // SAX2
    SAXParser::ValSchemes           valScheme;
    int                             numThreads;
    int                             totalTime;
    int                             numInputFiles;
    unsigned int                    numParses;
    InFileInfo                      files[MAXINFILES];
};

//
//------------------------------------------------------------------------------
//
//  Global Data
//
//------------------------------------------------------------------------------
RunInfo         gRunInfo;
ThreadInfo      *gThreadInfo;

/** Grammar caching thread testing */
MemoryManager*  gpMemMgr = 0;
XMLGrammarPool* gp = 0;

#ifdef HELPER_ROUTINES
// Routines which maybe helpful for debugging
static void printString(const XMLCh *str)
{
    char *s = XMLString::transcode(str);
    printf("%s", s);
    delete s;
}

#define CHARS_PER_LINE           40
#define BYTES_PER_LINE           16

/*
 * DumpLine: Dump out a buffer (address and length) to stderr.
 */
static void DumpLine(char* address, int length) {
    int i, c, charCount=0;
    if (length % 4) length += 4;
    fprintf(stderr, "%8.8p: ", address);
    for (i=0; i < length/4; ++i) {
        fprintf(stderr, "%8.8X ", ((int*)address)[i]);
        charCount += 9;
    }
    for (i=charCount; i < CHARS_PER_LINE; ++i) {
        putc(' ', stderr);
    }
    fprintf(stderr, "| ");
    for (i=0; i < length; ++i) {
        c = address[i];
        c = (isprint(c) ? c : '.');
        fprintf(stderr, "%c", c);
    }
    fprintf(stderr, "\n");
}

/*
 * dump: dump out a buffer (address and length) to stderr by dumping out
 *       a line at a time (DumpLine), until the buffer is written out.
 */

static void dump(void* generalAddress, int length) {
    int curr = 0;
    char* address = (char*) generalAddress;
    while (&address[curr] < &address[length-BYTES_PER_LINE]) {
        DumpLine(&address[curr], BYTES_PER_LINE);
        curr += BYTES_PER_LINE;
    }
    if (curr < length) {
        DumpLine(&address[curr], length-curr);
    }
    fflush(stderr);
}
#endif

//------------------------------------------------------------------------------
//
//  class ThreadParser   Bundles together a SAX parser and the SAX handlers
//                       and contains the API that the rest of this test
//                       program uses for creating parsers and doing parsing.
//
//                       Multiple instances of this class can operate concurrently
//                       in different threads.
//
//-------------------------------------------------------------------------------

class ThreadParser
{
public:
    class SAXHandler;
    class SAX2Handler;
    SAXHandler*     fSAXHandler;
    SAX2Handler*    fSAX2Handler;
    ErrorHandler*   fDOMErrorHandler;

    //  This is the API used by the rest of the test program
    ThreadParser();
    ~ThreadParser();

    int parse(int fileNum);           // Parse the specified file.  fileNum is an index
                                      //   into the gRunInfo.files array.
                                      //  return the XML checksum, or
                                      //  0 if a parse error occurred.

    int getCheckSum() {
        return fCheckSum;
    }

    int reCheck();                    // Try to compute the checksum again.
                                      //  for DOM, re-walk the tree.
                                      //  for SAX, can't do, just return previous value.

    void domPrint();                  //   including any children.  Default (no param)
                                       //   version dumps the entire document.
    void  addToCheckSum(const XMLCh *chars, XMLSize_t len=(XMLSize_t)-1);

    //  These are the SAX call-back functions that this class implements. Can be used
    //  for SAX and SAX2.
    void characters(const XMLCh* const chars, const XMLSize_t length) {
        addToCheckSum(chars, length);
    }

    void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {
        addToCheckSum(chars, length);
    }

    void resetDocument() {
    }

    void warning(const SAXParseException& exc)     {
        fprintf(stderr, "*** Warning ");
        fflush(stderr);
        throw exc;
    }

    void error(const SAXParseException& exc)       {
        fprintf(stderr, "*** Error ");
        fflush(stderr);
        throw exc;
    }

    void fatalError(const SAXParseException& exc)  {
        fprintf(stderr, "***** Fatal error ");
        fflush(stderr);
        throw exc;
    }

    // Create a nested class that can inherit from HandlerBase
    // for SAX startElement callbacks.
    class SAXHandler :  public HandlerBase
    {
    public:
        ThreadParser* SAXInstance;

        void startElement(const XMLCh* const name, AttributeList& attributes);
    };

    // Create a nested class that can inherit from DefaultHandler
    // for SAX2 startElement callbacks.
    class SAX2Handler :  public DefaultHandler
    {
    public:
        ThreadParser* SAX2Instance;

        void startElement(const XMLCh* const uri,
                          const XMLCh* const localname,
                          const XMLCh* const qname,
                          const Attributes& attributes);
    };

private:
    int                                             fCheckSum;
    SAXParser*                                      fSAXParser;
    SAX2XMLReader*                                  fSAX2Parser;
    XercesDOMParser*                                fXercesDOMParser;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *    fDoc;


    ThreadParser(const ThreadParser &); // No copy constructor
    const ThreadParser & operator =(const ThreadParser &); // No assignment.

    void  domCheckSum(const DOMNode *);
};

//
//  ThreadParser constructor.  Invoked by the threads of the test program
//                              to create parsers.
//
ThreadParser::ThreadParser()
{
    fSAXParser       = 0;
    fSAX2Parser      = 0;
    fXercesDOMParser = 0;
    fSAXHandler      = 0;
    fSAX2Handler     = 0;
    fDOMErrorHandler = 0;
    fDoc             = 0;
    fCheckSum        = 0;

    if (gRunInfo.dom) {
        // Set up to use a DOM parser
        /** Grammar caching thread testing */
        if (gp) {
            fXercesDOMParser = new XercesDOMParser(0, XMLPlatformUtils::fgMemoryManager, gp);
            fXercesDOMParser->cacheGrammarFromParse(true);
            fXercesDOMParser->useCachedGrammarInParse(true);
        }
        else {
            fXercesDOMParser = new XercesDOMParser;
        }
        switch (gRunInfo.valScheme) {
            case SAXParser::Val_Never:
                fXercesDOMParser->setValidationScheme(XercesDOMParser::Val_Never);
                break;
            case SAXParser::Val_Auto:
                fXercesDOMParser->setValidationScheme(XercesDOMParser::Val_Auto);
                break;
            default: //SAXParser::Val_Always:
                fXercesDOMParser->setValidationScheme(XercesDOMParser::Val_Always);
                break;
        }
        fXercesDOMParser->setDoSchema(gRunInfo.doSchema);
        fXercesDOMParser->setHandleMultipleImports (true);
        fXercesDOMParser->setValidationSchemaFullChecking(gRunInfo.schemaFullChecking);
        fXercesDOMParser->setDoNamespaces(gRunInfo.doNamespaces);
        fDOMErrorHandler = (ErrorHandler*) new HandlerBase();
        fXercesDOMParser->setErrorHandler(fDOMErrorHandler);
    }

    else if (gRunInfo.sax) {
        // Set up to use a SAX1 parser.
        /** Grammar caching thread testing */
        if (gp) {
            fSAXParser = new SAXParser(0, XMLPlatformUtils::fgMemoryManager, gp);
            fSAXParser->cacheGrammarFromParse(true);
            fSAXParser->useCachedGrammarInParse(true);
        }
        else {
            fSAXParser = new SAXParser();
        }
        fSAXParser->setValidationScheme(gRunInfo.valScheme);
        fSAXParser->setDoSchema(gRunInfo.doSchema);
        fSAXParser->setHandleMultipleImports (true);
        fSAXParser->setValidationSchemaFullChecking(gRunInfo.schemaFullChecking);
        fSAXParser->setDoNamespaces(gRunInfo.doNamespaces);
        fSAXHandler = new ThreadParser::SAXHandler();
        fSAXHandler->SAXInstance = this;
        fSAXParser->setDocumentHandler(fSAXHandler);
        fSAXParser->setErrorHandler(fSAXHandler);
    }

    else {
        // Set up to use a SAX2 parser.
        /** Grammar caching thread testing */
        if (gp) {
            fSAX2Parser = XMLReaderFactory::createXMLReader(gpMemMgr, gp);
            fSAX2Parser->setFeature(XMLUni::fgXercesCacheGrammarFromParse,true);
            fSAX2Parser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse,true);
        }
        else {
            fSAX2Parser = XMLReaderFactory::createXMLReader();
        }

        fSAX2Parser->setFeature(XMLUni::fgSAX2CoreNameSpaces,(gRunInfo.doNamespaces));
        fSAX2Parser->setFeature(XMLUni::fgXercesSchema,(gRunInfo.doSchema));
        fSAX2Parser->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
        fSAX2Parser->setFeature(XMLUni::fgXercesSchemaFullChecking,(gRunInfo.schemaFullChecking));

        switch (gRunInfo.valScheme) {
            case SAXParser::Val_Never:
                fSAX2Parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
                break;
            case SAXParser::Val_Auto:
                fSAX2Parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
                fSAX2Parser->setFeature(XMLUni::fgXercesDynamic, true);
                break;
            default: //SAXParser::Val_Always:
                fSAX2Parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
                fSAX2Parser->setFeature(XMLUni::fgXercesDynamic, false);
                break;
        }

        fSAX2Parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes,(gRunInfo.doNamespacePrefixes));
        fSAX2Handler = new ThreadParser::SAX2Handler();
        fSAX2Handler->SAX2Instance = this;
        fSAX2Parser->setContentHandler(fSAX2Handler);
        fSAX2Parser->setErrorHandler(fSAX2Handler);
    }
}

ThreadParser::~ThreadParser()
{
     delete fSAXParser;
     delete fSAX2Parser;
     delete fXercesDOMParser;
     delete fSAXHandler;
     delete fSAX2Handler;
     delete fDOMErrorHandler;
}

//------------------------------------------------------------------------
//
//  parse   - This is the method that is invoked by the rest of
//            the test program to actually parse an XML file.
//
//------------------------------------------------------------------------
int ThreadParser::parse(int fileNum)
{
    MemBufInputSource *mbis = 0;
    InFileInfo        *fInfo = &gRunInfo.files[fileNum];
    bool              errors = false;

    fCheckSum = 0;

    if (gRunInfo.inMemory) {
        mbis = new  MemBufInputSource((const XMLByte *) fInfo->fileContent,
                                       fInfo->fileSize,
                                       fInfo->uFileName,
                                       false);
    }

    try
    {
        if (gRunInfo.dom) {
            // Do a DOM parse
            fXercesDOMParser->resetDocumentPool();
            if (gRunInfo.inMemory)
                fXercesDOMParser->parse(*mbis);
            else
                fXercesDOMParser->parse(fInfo->fileName);
            fDoc = fXercesDOMParser->getDocument();
            domCheckSum(fDoc);
        }
        else if (gRunInfo.sax) {
            // Do a SAX1 parse
            if (gRunInfo.inMemory)
                fSAXParser->parse(*mbis);
            else
                fSAXParser->parse(fInfo->fileName);
        }
        else {
            // Do a SAX2 parse
            if (gRunInfo.inMemory)
                fSAX2Parser->parse(*mbis);
            else
                fSAX2Parser->parse(fInfo->fileName);
        }
    }
    catch (const OutOfMemoryException&)
    {
	    fprintf(stderr, " during parsing: %s\n OutOfMemoryException.\n", fInfo->fileName);
	    errors = true;
    }
    catch (const XMLException& e)
    {
        char *exceptionMessage = XMLString::transcode(e.getMessage());
        fprintf(stderr, " during parsing: %s\n Exception message is: %s\n",
            fInfo->fileName, exceptionMessage);
        XMLString::release(&exceptionMessage);
        errors = true;
    }
    catch (const DOMException& toCatch)
    {
        fprintf(stderr, " during parsing: %s\n DOMException code is: %i\n",
            fInfo->fileName, toCatch.code);
        errors = true;
    }
    catch (const SAXParseException& e)
    {
        char *exceptionMessage = XMLString::transcode(e.getMessage());
        fprintf(stderr, " during parsing: %s\n Exception message is: %s\n",
            fInfo->fileName, exceptionMessage);
        XMLString::release(&exceptionMessage);
        errors = true;
    }
    catch (...)
    {
        fprintf(stderr, "Unexpected exception during parsing\n");
        errors = true;
    }

    delete mbis;
    if (errors) {
        fflush(stderr);
        return 0;  // if errors occurred, return zero as if checksum = 0;
    }
    return fCheckSum;
}


//
//  addToCheckSum - private function, used within ThreadParser in
//                  computing the checksum of the XML file.
//
//                  Unichar Strings to be added to the checksum
//                  can either be null terminated (omit len param, which
//                  will then default to -1), or provide an explicit
//                  length.
//
void ThreadParser::addToCheckSum(const XMLCh *chars, XMLSize_t len)
{
    if (len == (XMLSize_t)-1)
    {
        // Null terminated string.
        while (*chars != 0)
        {
            fCheckSum = fCheckSum*5 + *chars;
            chars++;
        }
    }
    else
    {
        // String with character count.
        XMLSize_t i;
        for (i=0; i<len; i++)
            fCheckSum = fCheckSum*5 + chars[i];
    }
}


//
// startElement - our SAX handler callback function for startElement.
//                Update the document checksum with the element name
//                and any attribute names and values.
//
 void ThreadParser::SAXHandler::startElement(const XMLCh *const name, AttributeList &attributes)
{
    SAXInstance->addToCheckSum(name);
    XMLSize_t n = attributes.getLength();
    XMLSize_t i;
    for (i=0; i<n; i++)
    {
        const XMLCh *attNam = attributes.getName(i);
        SAXInstance->addToCheckSum(attNam);
        const XMLCh *attVal = attributes.getValue(i);
        SAXInstance->addToCheckSum(attVal);
    }
}

//
// startElement - our SAX2 handler callback function for startElement.
//                Update the document checksum with the element name
//                and any attribute names and values.
//

void ThreadParser::SAX2Handler::startElement(const XMLCh *const /*uri*/,
                              const XMLCh *const localname,
                              const XMLCh *const /*qname*/,
                              const Attributes& attributes)
{
    SAX2Instance->addToCheckSum(localname);

    XMLSize_t n = attributes.getLength();
    XMLSize_t i;
    for (i=0; i<n; i++)
    {
        const XMLCh *attNam = attributes.getQName(i);
        SAX2Instance->addToCheckSum(attNam);
        const XMLCh *attVal = attributes.getValue(i);
        SAX2Instance->addToCheckSum(attVal);
    }
}

//
// domCheckSum  -  Compute the check sum for a DOM node.
//                 Works recursively - initially called with a document node.
//
void ThreadParser::domCheckSum(const DOMNode *node)
{
    const XMLCh        *s;
    DOMNode          *child;
    DOMNamedNodeMap  *attributes;

    switch (node->getNodeType() )
    {
    case DOMNode::ELEMENT_NODE:
        {
            s = node->getNodeName();   // the element name

            attributes = node->getAttributes();  // Element's attributes
            XMLSize_t numAttributes = attributes->getLength();
            XMLSize_t i;
            for (i=0; i<numAttributes; i++)
                domCheckSum(attributes->item(i));

            addToCheckSum(s);          // Content and Children
            for (child=node->getFirstChild(); child!=0; child=child->getNextSibling())
                domCheckSum(child);

            break;
        }

    case DOMNode::ATTRIBUTE_NODE:
        {
            s = node->getNodeName();  // The attribute name
            addToCheckSum(s);
            s = node->getNodeValue();  // The attribute value
            if (s != 0)
                addToCheckSum(s);
            break;
        }

    case DOMNode::TEXT_NODE:
    case DOMNode::CDATA_SECTION_NODE:
        {
            s = node->getNodeValue();
            addToCheckSum(s);
            break;
        }

    case DOMNode::ENTITY_REFERENCE_NODE:
    case DOMNode::DOCUMENT_NODE:
        {
            // For entity references and the document, nothing is dirctly
            //  added to the checksum, but we do want to process the chidren nodes.
            //
            for (child=node->getFirstChild(); child!=0; child=child->getNextSibling())
                domCheckSum(child);
            break;
        }
    case DOMNode::ENTITY_NODE:
    case DOMNode::PROCESSING_INSTRUCTION_NODE:
    case DOMNode::COMMENT_NODE:
    case DOMNode::DOCUMENT_TYPE_NODE:
    case DOMNode::DOCUMENT_FRAGMENT_NODE:
    case DOMNode::NOTATION_NODE:
        break;
    }
}


//
// Recompute the checksum.  Meaningful only for DOM, will tell us whether
//  a failure is transient, or whether the DOM data is permanently corrupted.
//
int ThreadParser::reCheck()
{
    if (gRunInfo.dom) {
        fCheckSum = 0;
        domCheckSum(fDoc);
    }
    return fCheckSum;
}

//
// domPrint  -  Dump the contents of a DOM node.
//              For debugging failures, when all else fails.
//                 Works recursively - initially called with a document node.
//
void ThreadParser::domPrint()
{
    printf("Begin DOMPrint ...\n");
    if (gRunInfo.dom)
    {
        try
        {
            XMLCh tempStr[100];
            XMLString::transcode("LS", tempStr, 99);
            DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
            DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
            DOMLSOutput       *theOutput     = ((DOMImplementationLS*)impl)->createLSOutput();
            XMLFormatTarget   *myFormTarget  = new StdOutFormatTarget();
            theOutput->setByteStream(myFormTarget);
            DOMNode           *doc           = fXercesDOMParser->getDocument();
            theSerializer->write(doc,theOutput);
            delete myFormTarget;
            theSerializer->release();
            theOutput->release();
        }
        catch (...)
        {
            // do nothing
        }
    }
    printf("End DOMPrint\n");
}

//----------------------------------------------------------------------
//
//   parseCommandLine   Read through the command line, and save all
//                      of the options in the gRunInfo struct.
//
//                      Display the usage message if the command line
//                      is no good.
//
//                      Probably ought to be a member function of RunInfo.
//
//----------------------------------------------------------------------

void parseCommandLine(int argc, char **argv)
{
    gRunInfo.doGrammarCaching = false;
    gRunInfo.quiet = false;               // Set up defaults for run.
    gRunInfo.verbose = false;
    gRunInfo.stopNow = false;
    gRunInfo.dom = false;
    gRunInfo.sax = true;
    gRunInfo.reuseParser = false;
    gRunInfo.inMemory = false;
    gRunInfo.dumpOnErr = false;
    gRunInfo.doSchema = false;
    gRunInfo.schemaFullChecking = false;
    gRunInfo.doNamespaces = false;
    gRunInfo.doInitialParse = false;
    gRunInfo.doNamespacePrefixes = false;

    gRunInfo.valScheme = SAXParser::Val_Auto;
    gRunInfo.numThreads = 2;
    gRunInfo.totalTime = 0;
    gRunInfo.numInputFiles = 0;
    gRunInfo.numParses = 0;

    try             // Use exceptions for command line syntax errors.
    {
        int argnum = 1;
        while (argnum < argc) {
            if (strcmp(argv[argnum], "-quiet") == 0)
                gRunInfo.quiet = true;
            else if (strcmp(argv[argnum], "-verbose") == 0)
                gRunInfo.verbose = true;
            else if (strncmp(argv[argnum], "-v=", 3) == 0) {
                const char* const parm = &argv[argnum][3];
                if (!strcmp(parm, "never"))
                    gRunInfo.valScheme = SAXParser::Val_Never;
                else if (!strcmp(parm, "auto"))
                    gRunInfo.valScheme = SAXParser::Val_Auto;
                else if (!strcmp(parm, "always"))
                    gRunInfo.valScheme = SAXParser::Val_Always;
                else {
                    fprintf(stderr, "Unrecognized -v option \"%s\"\n", parm);
                    throw 1;
                }
            }
            else if (strcmp(argv[argnum], "-v") == 0) {
                fprintf(stderr, "Please note the -v option has been changed to -v=[always | never | auto]\n");
                fprintf(stderr, "ThreadTest will continue with -v=always\n");
                gRunInfo.valScheme = SAXParser::Val_Always;
            }
            else if (strcmp(argv[argnum], "-s") == 0)
                gRunInfo.doSchema = true;
            else if (strcmp(argv[argnum], "-f") == 0)
                gRunInfo.schemaFullChecking = true;
            else if (strcmp(argv[argnum], "-n") == 0)
                gRunInfo.doNamespaces = true;
            else if (strcmp(argv[argnum], "-p") == 0)
                gRunInfo.doNamespacePrefixes = true;
            else if (!strncmp(argv[argnum], "-parser=", 8)) {
                const char* const parm = &argv[argnum][8];
                if (!strcmp(parm, "dom")) {
                    gRunInfo.dom = true;
                    gRunInfo.sax = false;
                }
                else if (!strcmp(parm, "sax")) {
                    gRunInfo.dom = false;
                    gRunInfo.sax = true;
                }
                else if (!strcmp(parm, "sax2")) {
                    gRunInfo.dom = false;
                    gRunInfo.sax = false;
                }
                else {
                    fprintf(stderr, "Unrecognized -parser option \"%s\"\n", parm);
                    throw 1;
                }
            }
            else if (strcmp(argv[argnum], "-init") == 0)
                gRunInfo.doInitialParse = true;
            else if (strcmp(argv[argnum], "-reuse") == 0)
                gRunInfo.reuseParser = true;
            else if (strcmp(argv[argnum], "-dump") == 0)
                gRunInfo.dumpOnErr = true;
            else if (strcmp(argv[argnum], "-mem") == 0)
                gRunInfo.inMemory = true;
            else if (strcmp(argv[argnum], "-threads") == 0) {
                ++argnum;
                if (argnum >= argc) {
                    fprintf(stderr, "Invalid -threads option (missing # of threads)\n");
                    throw 1;
                }
                gRunInfo.numThreads = atoi(argv[argnum]);
                if (gRunInfo.numThreads < 0) {
                    fprintf(stderr, "Invalid -threads option (negative # of threads)\n");
                    throw 1;
                }
            }
            else if (strcmp(argv[argnum], "-time") == 0) {
                ++argnum;
                if (argnum >= argc) {
                    fprintf(stderr, "Invalid -time option (missing time value)\n");
                    throw 1;
                }
                gRunInfo.totalTime = atoi(argv[argnum]);
                if (gRunInfo.totalTime < 1) {
                    fprintf(stderr, "Invalid -time option (time value < 1)\n");
                    throw 1;
                }
            }
            else if (strcmp(argv[argnum], "-gc") == 0)
                gRunInfo.doGrammarCaching = true;
            else if (strcmp(argv[argnum], "-parses") == 0) {
                ++argnum;
                if (argnum >= argc) {
                    fprintf(stderr, "Invalid -parses option (missing # of parses)\n");
                    throw 1;
                }
                int temp = atoi(argv[argnum]);
                if (temp < 0) {
                    fprintf(stderr, "Invalid -parses option (negative # of parses)\n");
                    throw 1;
                }
                gRunInfo.numParses = temp;
            }
            else  if (argv[argnum][0] == '-') {
                fprintf(stderr, "Unrecognized command line option.  Scanning \"%s\"\n",
                    argv[argnum]);
                throw 1;
            }
            else {
                gRunInfo.numInputFiles++;
                if (gRunInfo.numInputFiles >= MAXINFILES) {
                    fprintf(stderr, "Too many input files.  Limit is %d\n", MAXINFILES);
                    throw 1;
                }
                gRunInfo.files[gRunInfo.numInputFiles-1].fileName = argv[argnum];
            }
            argnum++;
        }

        // We've made it through the command line.
        // Verify that at least one input file to be parsed was specified.
        if (gRunInfo.numInputFiles == 0) {
            fprintf(stderr, "No input XML file specified on command line.\n");
            throw 1;
        };

        if (gRunInfo.numParses && gRunInfo.totalTime) {
            fprintf(stderr, "Both -parses nnn and -time nnn were specified. Ignoring -time nnn.\n");
        }
    }
    catch (int)
    {
        fprintf(stderr, "usage:  ThreadTest [-v] [-threads nnn] [-time nnn] [-quiet] [-verbose] xmlfile...\n"
            "     -v=xxx         Validation scheme [always | never | auto].  Default is AUTO.\n"
            "     -n             Enable namespace processing. Defaults to off.\n"
            "     -s             Enable schema processing. Defaults to off.\n"
            "     -f             Enable full schema constraint checking. Defaults to off.\n"
            "     -parser=xxx    Parser Type [dom | sax | sax2].  Default is SAX (SAX1).\n"
            "     -p             Enable namespace prefixes. Defaults to off.\n"
            "                    (Only used with -parser=sax2, ignored otherwise.)\n"
            "     -quiet         Suppress periodic status display.\n"
            "     -verbose       Display extra messages.\n"
            "     -reuse         Retain and reuse parser.  Default creates new for each parse.\n"
            "     -threads nnn   Number of threads.  Default is 2.\n"
            "     -time nnn      Total time to run, in seconds.  Default is forever.\n"
            "     -parses nnn    Run for nnn parses instead of time.  Default is to use time\n"
            "     -dump          Dump DOM tree on error.\n"
            "     -mem           Read files into memory once only, and parse them from there.\n"
            "     -gc            Enable grammar caching (i.e. grammar cached and used in subsequent parses). Defaults to off.\n"
            "     -init          Perform an initial parse of the file(s) before starting up the individual threads.\n\n"
            );
        exit(1);
    }
}


//---------------------------------------------------------------------------
//
//   ReadFilesIntoMemory   For use when parsing from memory rather than
//                          reading the files each time, here is the code that
//                          reads the files into local memory buffers.
//
//                          This function is only called once, from the main
//                          thread, before all of the worker threads are started.
//
//---------------------------------------------------------------------------
void ReadFilesIntoMemory()
{
    int     fileNum;
    FILE    *fileF;
    size_t  t;

    if (gRunInfo.inMemory)
    {
        for (fileNum = 0; fileNum <gRunInfo.numInputFiles; fileNum++)
        {
            InFileInfo *fInfo = &gRunInfo.files[fileNum];
            fInfo->uFileName = XMLString::transcode(fInfo->fileName);
            fileF = fopen( fInfo->fileName, "rb" );
            if (fileF == 0) {
                fprintf(stderr, "Can not open file \"%s\".\n", fInfo->fileName);
                clearFileInfoMemory();
                exit(-1);
            }
            fseek(fileF, 0, SEEK_END);
            fInfo->fileSize = ftell(fileF);
            fseek(fileF, 0, SEEK_SET);
            fInfo->fileContent = new char[fInfo->fileSize + 1];
            t = fread(fInfo->fileContent, 1, fInfo->fileSize, fileF);
            if (t != fInfo->fileSize) {
                fprintf(stderr, "Error reading file \"%s\".\n", fInfo->fileName);
                clearFileInfoMemory();
                exit(-1);
            }
            fclose(fileF);
            fInfo->fileContent[fInfo->fileSize] = 0;
        }
    }
}

void clearFileInfoMemory()
{
    int     fileNum;

    if (gRunInfo.inMemory)
    {
        for (fileNum = 0; fileNum <gRunInfo.numInputFiles; fileNum++)
        {
            InFileInfo *fInfo = &gRunInfo.files[fileNum];
            XMLString::release(&fInfo->uFileName);
            delete [] fInfo->fileContent;
        }
    }
}



//----------------------------------------------------------------------
//
//  threadMain   The main function for each of the swarm of test threads.
//               Run in an infinite loop, parsing each of the documents
//               given on the command line in turn.
//
//----------------------------------------------------------------------

#ifdef HAVE_PTHREAD
extern "C" {
void threadMain (void *param)
#else
unsigned long WINAPI threadMain (void *param)
#endif
{
    ThreadInfo   *thInfo = (ThreadInfo *)param;
    ThreadParser *thParser = 0;

    if (gRunInfo.verbose)
        printf("Thread #%d: starting\n", thInfo->fThreadNum);

    int docNum = gRunInfo.numInputFiles;

    //
    // Each time through this loop, one file will be parsed and its checksum
    // computed and compared with the precomputed value for that file.
    //
    while (gRunInfo.stopNow == false) {
        if (gRunInfo.numParses == 0 || thInfo->fParses < gRunInfo.numParses) {
            thInfo->fInProgress = true;

            if (thParser == 0)
                thParser = new ThreadParser;

            docNum++;

            if (docNum >= gRunInfo.numInputFiles)
                docNum = 0;

            InFileInfo *fInfo = &gRunInfo.files[docNum];

            if (gRunInfo.verbose )
                printf("Thread #%d: parse %d starting file %s\n", thInfo->fThreadNum, thInfo->fParses, fInfo->fileName);

            int checkSum = 0;

            checkSum = thParser->parse(docNum);

            // For the case where we skip the preparse we will have nothing to
            // compare the first parse's results to ... so if this looks like first
            // parse move the checkSum back into the gRunInfo data for this file.

            if (gRunInfo.files[docNum].checkSum == 0) {
                gRunInfo.files[docNum].checkSum = checkSum;
            }
            else if (checkSum != gRunInfo.files[docNum].checkSum) {
                if (checkSum == 0) {
                    // parse returns 0 if there was an error so do this to get the real
                    // checksum value
                    checkSum = thParser->getCheckSum();
                }
                fprintf(stderr, "\nThread %d: Parse Check sum error on file  \"%s\" for parse # %d.  Expected %x,  got %x\n",
                    thInfo->fThreadNum, fInfo->fileName, thInfo->fParses, fInfo->checkSum, checkSum);

	            double totalParsesCompleted = 0;
                for (int threadNum=0; threadNum < gRunInfo.numThreads; threadNum++) {
                    totalParsesCompleted += gThreadInfo[threadNum].fParses;
                }
                fprintf(stderr, "Total number of parses completed is %f.\n", totalParsesCompleted);

                // Revisit - let the loop continue to run?
                int secondTryCheckSum = thParser->reCheck();
                fprintf(stderr, "   Retry checksum is %x\n", secondTryCheckSum);
                if (gRunInfo.dumpOnErr && gRunInfo.dom) {
                    thParser->domPrint();
                }
                fflush(stdout);
                fflush(stderr);
                clearFileInfoMemory();
                exit(-1);
            }

            if (gRunInfo.reuseParser == false) {
                delete thParser;
                thParser = 0;
            }

            thInfo->fHeartBeat = true;
            thInfo->fParses++;
            thInfo->fInProgress = false;
        }
        else {
            ThreadFuncs::Sleep(1000);
        }
    }
    delete thParser;
#ifdef HAVE_PTHREAD
	return;
}
#else
    return 0;
#endif
}


//----------------------------------------------------------------------
//
//   main
//
//----------------------------------------------------------------------

int main (int argc, char **argv)
{


    parseCommandLine(argc, argv);

    //
    // Initialize the XML system.
    //
    try
    {
         XMLPlatformUtils::Initialize();
    }
    catch (...)
    {
        fprintf(stderr, "Exception from XMLPlatfromUtils::Initialize.\n");
        return 1;
    }


    /** Grammar caching thread testing */
    // Initialize memory manger and grammar pool
    // set doInitialParse to true so that the first parse will cache the
    // grammar and it'll be used in subsequent parses

    if (gRunInfo.doSchema == true && gRunInfo.doNamespaces == true && gRunInfo.doGrammarCaching == true) {
        gpMemMgr = new MemoryManagerImpl();
        gp = new XMLGrammarPoolImpl(gpMemMgr);
        gRunInfo.doInitialParse = true;
    }

    //
    // If we will be parsing from memory, read each of the input files
    //  into memory now.
    //
    ReadFilesIntoMemory();

    // Initialize checksums to zero so we can check first parse and if
    // zero then we need to move first parse's checksum into array. This
    // is for the cse where we skip the initial parse.
    for (int n = 0; n < gRunInfo.numInputFiles; n++)
    {
        gRunInfo.files[n].checkSum = 0;
    }

    if (gRunInfo.doInitialParse)
    {
    //
    // While we are still single threaded, parse each of the documents
    // once, to check for errors, and to note the checksum.
    // Blow off the rest of the test if there are errors.
    //
        ThreadParser *mainParser = new ThreadParser;
        int     n;
        bool    errors = false;
        int     cksum;

        for (n = 0; n < gRunInfo.numInputFiles; n++)
        {
            char *fileName = gRunInfo.files[n].fileName;
            if (gRunInfo.verbose)
                printf("%s checksum is ", fileName);

            cksum = mainParser->parse(n);

            if (cksum == 0) {
                fprintf(stderr, "An error occurred while initially parsing %s\n",
                    fileName);
                errors = true;
            };

            gRunInfo.files[n].checkSum = cksum;
            if (gRunInfo.verbose )
                printf("%x\n", cksum);
            if (gRunInfo.dumpOnErr && errors && gRunInfo.dom) {
                mainParser->domPrint();
            }

        }
        delete mainParser;

        if (errors) {
            fprintf(stderr, "Quitting due to error incurred during initial parse\n");
            clearFileInfoMemory();
            return 1;
        }
    }

    //
    //  Fire off the requested number of parallel threads
    //

    if (gRunInfo.numThreads == 0) {
        clearFileInfoMemory();
        exit(0);
    }

    gThreadInfo = new ThreadInfo[gRunInfo.numThreads];

    int threadNum;
    for (threadNum=0; threadNum < gRunInfo.numThreads; threadNum++)
    {
        gThreadInfo[threadNum].fThreadNum = threadNum;
        ThreadFuncs::startThread(threadMain, &gThreadInfo[threadNum]);
    }

    if (gRunInfo.numParses)
    {
        bool notDone;
        while (true)
        {
            ThreadFuncs::Sleep(1000);
            notDone = false;

            for (threadNum = 0; threadNum < gRunInfo.numThreads; threadNum++) {
                if (gThreadInfo[threadNum].fParses < gRunInfo.numParses)
                    notDone = true;
            }
            if (notDone == false) {
                break;
            }
        }
    }
    else
    {
        //
        //  Loop, watching the heartbeat of the worker threads.
        //    Each second, display "+" when all threads have completed a parse
        //                 display "." if some thread hasn't since previous "+"
        //

        unsigned long startTime = XMLPlatformUtils::getCurrentMillis();
        int elapsedSeconds = 0;
        while (gRunInfo.totalTime == 0 || gRunInfo.totalTime > elapsedSeconds) {
            ThreadFuncs::Sleep(1000);
            if (gRunInfo.quiet == false && gRunInfo.verbose == false) {
                char c = '+';
                for (threadNum=0; threadNum < gRunInfo.numThreads; threadNum++) {
                    if (gThreadInfo[threadNum].fHeartBeat == false) {
                        c = '.';
                        break;
                    }
                }
                fputc(c, stdout);
                fflush(stdout);
                if (c == '+')
                    for (threadNum=0; threadNum < gRunInfo.numThreads; threadNum++)
                        gThreadInfo[threadNum].fHeartBeat = false;
            }
            elapsedSeconds = (XMLPlatformUtils::getCurrentMillis() - startTime) / 1000;
        }
    }

    //
    //  Time's up, we are done.  (We only get here if this was a timed run)
    //  Tally up the total number of parses completed by each of the threads.
    //
    gRunInfo.stopNow = true;      // set flag, which will cause worker threads to stop.

    //
    //  Make sure all threads are done before terminate
    //
    for (threadNum=0; threadNum < gRunInfo.numThreads; threadNum++) {
        while (gThreadInfo[threadNum].fInProgress == true) {
            ThreadFuncs::Sleep(1000);
        }
        if (gRunInfo.verbose)
            printf("Thread #%d: is finished.\n", threadNum);
    }

    //
    //  We are done!   Count the number of parse and terminate the program
    //
    double totalParsesCompleted = 0;
    for (threadNum=0; threadNum < gRunInfo.numThreads; threadNum++)
    {
        totalParsesCompleted += gThreadInfo[threadNum].fParses;
        // printf("%f   ", totalParsesCompleted);
    }

    if (gRunInfo.quiet == false) {
        if (gRunInfo.numParses) {
            printf("\n%8.0f total parses were completed.\n", totalParsesCompleted);
        }
        else {
            double parsesPerMinute = totalParsesCompleted / (double(gRunInfo.totalTime) / double(60));
            printf("\n%8.2f parses per minute.\n", parsesPerMinute);
        }
    }

    // delete grammar pool and memory manager
    if (gp) {
        delete gp;
        delete gpMemMgr;
    }

    XMLPlatformUtils::Terminate();

    clearFileInfoMemory();

    delete [] gThreadInfo;

    printf("Test Run Successfully\n");

    return 0;
}
