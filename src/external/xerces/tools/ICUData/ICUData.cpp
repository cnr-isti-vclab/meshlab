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
//  This program is designed to parse a standard ICU .UCM file and spit out
//  a C++ code fragment that represents the tables required by the intrinsic
//  XML parser transcoders.
//
//  The file format is pretty simple and this program is not intended to be
//  industrial strength by any means. Its use by anyone but the author is
//  at the user's own risk.
//
//  The code looks for the min/max bytes per character to know what kind of
//  table to spit out, but for now only handles single char sets.
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    <ctype.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <iostream.h>
#include    <string.h>


// ---------------------------------------------------------------------------
//  Const data
// ---------------------------------------------------------------------------
static const unsigned int   gMaxInRecs = 1024;


// ---------------------------------------------------------------------------
//  Local data types
// ---------------------------------------------------------------------------
struct XlatRec
{
    unsigned short  uniVal;
    unsigned char   cpVal;
};


// ---------------------------------------------------------------------------
//  Local data
//
//  gInFile
//  gOutFile
//      These are the file stream for the input UCM file and the output file
//      that we write the C++ code to.
//
//  fLineNum
//      Used to track the current line number in the source file, for error
//      reporting.
//
//  gMainTable
//  gMainTableSz
//      This is the table that is filled in from the original source document.
//      We don't know how big it will be, but its not likely to be much more
//      than 300 entries or so (256 output code points with some multiply
//      mapped Unicode code points.) So we make it extra large and watch for
//      possible overflow.
//
//      The size value is bumped up as we load entries into it during the
//      parse of the file.
//
//  gMaxChar
//  gMinChar
//      The min/max chars that are used to represent a character. These are
//      read from the header of the input file.
//
//  gRepChar
//      The replacement character to be used. This is read from the header of
//      the input file.
// ---------------------------------------------------------------------------
static FILE*            gInFile;
static FILE*            gOutFile;
static unsigned int     fLineNum;
static XlatRec          gMainTable[gMaxInRecs];
static unsigned int     gMainTableSz = 0;
static unsigned int     gMaxChar;
static unsigned int     gMinChar;
static unsigned char    gRepChar = 1;


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static unsigned int getLine(        char* const     toFill
                            , const unsigned int    maxChars
                            , const bool            eofOk = false)
{
    while (true)
    {
        if (!fgets(toFill, maxChars, gInFile))
        {
            if (feof(gInFile))
            {
                if (eofOk)
                    return ~0UL;
                else
                    cout << "Unexpected end of input at line: " << fLineNum << endl;
            }
             else
            {
                cout << "Error processing input at line: " << fLineNum << endl;
                exit(1);
            }
        }
        fLineNum++;

        //
        //  If its not a comment, then break out
        //
        if (toFill[0] != '#')
            break;
    }

    //
    //  There could be a trailing comment on this line, so lets get rid
    //  of it. Search for a # char and put a null there.
    //
    char* endPtr = toFill;
    while (*endPtr && (*endPtr != '#'))
        endPtr++;
    if (*endPtr == '#')
        *endPtr = 0;

    // Strip trailing whitespace
    endPtr = toFill + (strlen(toFill) - 1);
    while (isspace(*endPtr))
        endPtr--;
    *(endPtr + 1) = 0;

    // And return the count of chars we got
    return strlen(toFill);
}


static unsigned int extractVal(char* const srcStr)
{
    char* srcPtr = srcStr;

    // Run forward to the first non-space
    while (isspace(*srcPtr))
        srcPtr++;

    if (!*srcPtr)
    {
        cout << "Invalid numeric value on line: " << fLineNum << endl;
        exit(1);
    }

    //
    //  If it starts with \, then its a hex value in the form \xXX. Else its
    //  just a decimal value.
    //
    unsigned int retVal;
    char* endPtr;
    if (*srcPtr == '\\')
    {
        // Skip the \\x and interpret as a hex value
        srcPtr += 2;
        retVal = (unsigned int)strtoul(srcPtr, &endPtr, 16);
    }
     else
    {
        retVal = (unsigned int)strtoul(srcPtr, &endPtr, 10);
    }

    // We should have translated up to the end of the string
    if (*endPtr)
    {
        cout << "Invalid numeric value on line: " << fLineNum << endl;
        exit(1);
    }

    return retVal;
}


static void loadTable()
{
    //
    //  Just loop, reading lines at a time, until we either find the start
    //  of the character table or hit the end of the file. Along the way, we
    //  should see a few header values that we store away.
    //
    const unsigned int  tmpBufSz = 2048;
    char                tmpBuf[tmpBufSz - 1];
    while (getLine(tmpBuf, tmpBufSz))
    {
        //
        //  Check for one of the special values we are intersted int. If
        //  its CHARMAP, then we fall out of this loop.
        //
        if (!strcmp(tmpBuf, "CHARMAP"))
            break;

        if (!strncmp(tmpBuf, "<mb_cur_max>", 12))
        {
            gMaxChar = extractVal(&tmpBuf[12]);
        }
         else if (!strncmp(tmpBuf, "<mb_cur_min>", 12))
        {
            gMinChar = extractVal(&tmpBuf[12]);
        }
         else if (!strncmp(tmpBuf, "<subchar>", 9))
        {
            gRepChar = (char)extractVal(&tmpBuf[9]);
        }
    }

    //
    //  Ok, now we just run till we hit the "END CHARMAP" line. Each entry
    //  will be in the form:
    //
    //      <UXXXX>     \xXX
    //
    //  Where X is a hex number.
    //
    char* endPtr;
    while (getLine(tmpBuf, tmpBufSz))
    {
        // Watch for the end of table
        if (!strcmp(tmpBuf, "END CHARMAP"))
            break;

        // The absolute minium it could be is 12 chars
        if (strlen(tmpBuf) < 12)
        {
            cout << "Line " << fLineNum << " is too short to hold a valid entry"
                 << endl;
            exit(1);
        }

        // Make sure the first token meets the criteria
        if ((tmpBuf[0] != '<')
        ||  (tmpBuf[1] != 'U')
        ||  (tmpBuf[6] != '>'))
        {
            cout << "Line " << fLineNum << " has a badly formed Unicode value"
                 << endl;
            exit(1);
        }

        //
        //  Looks reasonable so lets try to convert it. We can play tricks
        //  with this buffer, so put a null over the > char.
        //
        tmpBuf[6] = 0;
        const unsigned int uniVal = strtoul(&tmpBuf[2], &endPtr, 16);
        if (*endPtr)
        {
            cout << "Invalid Unicode value on line " << fLineNum << endl;
            exit(1);
        }

        //
        //  Ok, lets search over to the second token. We have to find a \\
        //  character.
        //
        char* srcPtr = &tmpBuf[7];
        while (*srcPtr && (*srcPtr != '\\'))
            srcPtr++;

        // If we never found it, its in error
        if (!*srcPtr)
        {
            cout << "Never found second token on line " << fLineNum << endl;
            exit(1);
        }

        // Try to translate it
        srcPtr += 2;
        const unsigned int cpVal = strtoul(srcPtr, &endPtr, 16);
        if (*endPtr)
        {
            cout << "Invalid code page value on line " << fLineNum << endl;
            exit(1);
        }

        // Make sure that the values are within range
        if (uniVal > 0xFFFF)
        {
            cout << "Unicode value is too big on line " << fLineNum << endl;
            exit(1);
        }

        if (cpVal > 0xFF)
        {
            cout << "Code page value is too big on line " << fLineNum << endl;
            exit(1);
        }

        // Looks reasonable, so add a new entry to the global table
        gMainTable[gMainTableSz].uniVal = (unsigned short)uniVal;
        gMainTable[gMainTableSz].cpVal = (unsigned char)cpVal;
        gMainTableSz++;
    }
}



int compFuncTo(const void* p1, const void* p2)
{
    const XlatRec* rec1 = (const XlatRec*)p1;
    const XlatRec* rec2 = (const XlatRec*)p2;

    return (int)rec1->uniVal - (int)rec2->uniVal;
}


int compFuncFrom(const void* p1, const void* p2)
{
    const XlatRec* rec1 = (const XlatRec*)p1;
    const XlatRec* rec2 = (const XlatRec*)p2;

    //
    //  Since there can be multiple Unicode chars that map to a single
    //  code page char, we have to handle the situationw here they are
    //  equal specially. If the code page vals are equal, then the one
    //  with the smaller Unicode code point is considered smaller.
    //
    if (rec1->cpVal == rec2->cpVal)
        return (int)rec1->uniVal - (int)rec2->uniVal;

    // Else use the code page value for sorting
    return (int)rec1->cpVal - (int)rec2->cpVal;
}


static void formatSBTables()
{
    // For now, only handle single byte char sets
    if ((gMinChar != 1) || (gMaxChar != 1))
    {
        cout << "formatSBTables can only handle single byte encodings"
             << endl;
        exit(1);
    }

    //
    //  First, we want to sort the table by the code page value field. This
    //  is the order required for the 'from' table to convert from the code
    //  page to the internal Unicode format.
    //
    qsort(gMainTable, gMainTableSz, sizeof(gMainTable[0]), compFuncFrom);

    //
    //  Now spit out the header for the table. This is the same for all
    //  of them, since they are static to the file and can just all have
    //  the same name.
    //
    fprintf
    (
        gOutFile
        , "static const XMLCh gFromTable[256] =\n{\n    "
    );

    //
    //  Now for each unique entry in the cp value field, we want to put out
    //  the Unicode value for that entry. Since we sorted them such that
    //  dups have the one with the smaller Unicode value in the lower index,
    //  we always hit the desired value first, and then can just skip over
    //  a duplicate.
    //
    unsigned int curValue = 0;
    unsigned int index;
    for (index = 0; index < gMainTableSz; index++)
    {
        if (curValue)
        {
            if (!(curValue % 8))
                fprintf(gOutFile, "\n  , ");
            else
                fprintf(gOutFile, ", ");
        }

        if (curValue == gMainTable[index].cpVal)
        {
            fprintf(gOutFile, "0x%04X", (unsigned int)gMainTable[index].uniVal);

            // If there is a dump, then skip it
            if (index < gMainTableSz)
            {
                if (gMainTable[index + 1].cpVal == curValue)
                    index++;
            }
        }
         else if (curValue < gMainTable[index].cpVal)
        {
            fprintf(gOutFile, "0xFFFF");
        }
         else
        {
            // Screwed up
            cout << "Current value got above target value\n" << endl;
            exit(1);
        }
        curValue++;

        // If the current value goes over 256, we are in trouble
        if (curValue > 256)
        {
            cout << "The code page value cannot be > 256 in SB mode\n" << endl;
            exit(1);
        }
    }

    // And print the trailer for this table
    fprintf(gOutFile, "\n};\n\n");


    //
    //  Now lets sort by the Unicode value field. This sort is used for
    //  the 'to' table. The Unicode value is found by binary search and
    //  used to map to the right output encoding value.
    //
    qsort(gMainTable, gMainTableSz, sizeof(gMainTable[0]), compFuncTo);

    // Output the table ehader for this one
    fprintf
    (
        gOutFile
        , "static const XMLTransService::TransRec gToTable[] =\n{\n    "
    );

    for (index = 0; index < gMainTableSz; index++)
    {
        if (index)
        {
            if (!(index % 4))
                fprintf(gOutFile, "\n  , ");
            else
                fprintf(gOutFile, ", ");
        }

        fprintf
        (
            gOutFile
            , "{ 0x%04X, 0x%02X }"
            , (unsigned int)gMainTable[index].uniVal
            , (unsigned int)gMainTable[index].cpVal
        );
    }

    // Print the trailer for this table
    fprintf(gOutFile, "\n};\n");

    // And print out the table size constant
    fprintf(gOutFile, "static const unsigned int gToTableSz = %d;\n", gMainTableSz);
}

static void showUsage()
{
    cout << "ICUData inputUCMfile outputfile\n" << endl;
}



// ---------------------------------------------------------------------------
//  The parameters are:
//
//  argV[1] = The source UCM file
//  argV[2] = The path to the output file
// ---------------------------------------------------------------------------
int main(int argC, char** argV)
{
    // We have to have 3 parameters
    if (argC != 3)
    {
        showUsage();
        return 1;
    }

    // Try to open the first file for input
    gInFile = fopen(argV[1], "rt");
    if (!gInFile)
    {
        cout << "Could not find input file: " << argV[1] << endl;
        return 1;
    }

    // Try to open the second file for output (truncated)
    gOutFile = fopen(argV[2], "wt+");
    if (!gOutFile)
    {
        cout << "Could not create output file: " << argV[1] << endl;
        return 1;
    }

    //
    //  This will parse the file and load the table. It will also look for
    //  a couple of key fields in the file header and store that data into
    //  globals.
    //
    loadTable();

    // If we didn't get any table entries, then give up
    if (!gMainTableSz)
    {
        cout << "No translation table entries were found in the file" << endl;
        return 1;
    }

    //
    //  Ok, we got the data loaded. Now lets output the tables. This method
    //  spit out both tables to the output file, in a format ready to be
    //  incorporated directly into the source code.
    //
    formatSBTables();

    // Close our files
    fclose(gInFile);
    fclose(gOutFile);

    return 0;
}
