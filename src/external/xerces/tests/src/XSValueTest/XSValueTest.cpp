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
#include "XSValueTest.hpp"

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLUni.hpp>

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif

#include <stdio.h>
#include <math.h>

#include <xercesc/framework/psvi/XSValue.hpp>
#include <xercesc/util/PlatformUtils.hpp>

static const char* null_string=0;

static const bool  EXP_RET_VALID_TRUE  = true;
static const bool  EXP_RET_VALUE_TRUE  = true;
static const bool  EXP_RET_CANREP_TRUE = true;

static const bool  EXP_RET_VALID_FALSE  = false;
static const bool  EXP_RET_VALUE_FALSE  = false;
static const bool  EXP_RET_CANREP_FALSE = false;

static const XSValue::Status DONT_CARE = XSValue::st_UnknownType;
static bool  errSeen = false;

// This code is deliberately exeeding the minimum and maxium limits of
// the various integer types.  Don't warn about it since it's
// expected.
#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wc++11-compat"
#endif

static const XMLCh* getDataTypeString(const XSValue::DataType dt)
{
    switch(dt)
    {
    case XSValue::dt_string:
        return SchemaSymbols::fgDT_STRING;
    case XSValue::dt_boolean:
        return SchemaSymbols::fgDT_BOOLEAN;
    case XSValue::dt_decimal:
        return SchemaSymbols::fgDT_DECIMAL;
    case XSValue::dt_float:
        return SchemaSymbols::fgDT_FLOAT;
    case XSValue::dt_double:
        return SchemaSymbols::fgDT_DOUBLE;
    case XSValue::dt_duration:
        return SchemaSymbols::fgDT_DURATION;
    case XSValue::dt_dateTime:
        return SchemaSymbols::fgDT_DATETIME;
    case XSValue::dt_time:
        return SchemaSymbols::fgDT_TIME;
    case XSValue::dt_date:
        return SchemaSymbols::fgDT_DATE;
    case XSValue::dt_gYearMonth:
        return SchemaSymbols::fgDT_YEARMONTH;
    case XSValue::dt_gYear:
        return SchemaSymbols::fgDT_YEAR;
    case XSValue::dt_gMonthDay:
        return SchemaSymbols::fgDT_MONTHDAY;
    case XSValue::dt_gDay:
        return SchemaSymbols::fgDT_DAY;
    case XSValue::dt_gMonth:
        return SchemaSymbols::fgDT_MONTH;
    case XSValue::dt_hexBinary:
        return SchemaSymbols::fgDT_HEXBINARY;
    case XSValue::dt_base64Binary:
        return SchemaSymbols::fgDT_BASE64BINARY;
    case XSValue::dt_anyURI:
        return SchemaSymbols::fgDT_ANYURI;
    case XSValue::dt_QName:
        return SchemaSymbols::fgDT_QNAME;
    case XSValue::dt_NOTATION:
        return XMLUni::fgNotationString;
    case XSValue::dt_normalizedString:
        return SchemaSymbols::fgDT_NORMALIZEDSTRING;
    case XSValue::dt_token:
        return SchemaSymbols::fgDT_TOKEN;
    case XSValue::dt_language:
        return SchemaSymbols::fgDT_LANGUAGE;
    case XSValue::dt_NMTOKEN:
        return XMLUni::fgNmTokenString;
    case XSValue::dt_NMTOKENS:
        return XMLUni::fgNmTokensString;
    case XSValue::dt_Name:
        return SchemaSymbols::fgDT_NAME;
    case XSValue::dt_NCName:
        return SchemaSymbols::fgDT_NCNAME;
    case XSValue::dt_ID:
        return XMLUni::fgIDString;
    case XSValue::dt_IDREF:
        return XMLUni::fgIDRefString;
    case XSValue::dt_IDREFS:
        return XMLUni::fgIDRefsString;
    case XSValue::dt_ENTITY:
        return XMLUni::fgEntityString;
    case XSValue::dt_ENTITIES:
        return XMLUni::fgEntitiesString;
    case XSValue::dt_integer:
        return SchemaSymbols::fgDT_INTEGER;
    case XSValue::dt_nonPositiveInteger:
        return SchemaSymbols::fgDT_NONPOSITIVEINTEGER;
    case XSValue::dt_negativeInteger:
        return SchemaSymbols::fgDT_NEGATIVEINTEGER;
    case XSValue::dt_long:
        return SchemaSymbols::fgDT_LONG;
    case XSValue::dt_int:
        return SchemaSymbols::fgDT_INT;
    case XSValue::dt_short:
        return SchemaSymbols::fgDT_SHORT;
    case XSValue::dt_byte:
        return SchemaSymbols::fgDT_BYTE;
    case XSValue::dt_nonNegativeInteger:
        return SchemaSymbols::fgDT_NONNEGATIVEINTEGER;
    case XSValue::dt_unsignedLong:
        return SchemaSymbols::fgDT_ULONG;
    case XSValue::dt_unsignedInt:
        return SchemaSymbols::fgDT_UINT;
    case XSValue::dt_unsignedShort:
        return SchemaSymbols::fgDT_USHORT;
    case XSValue::dt_unsignedByte:
        return SchemaSymbols::fgDT_UBYTE;
    case XSValue::dt_positiveInteger:
        return SchemaSymbols::fgDT_POSITIVEINTEGER;
    default:
        return 0;
    }
}

static bool compareActualValue( const XSValue::DataType             datatype
                              , const XSValue::XSValue_Data         actValue
                              , const XSValue::XSValue_Data         expValue)
{
    switch (datatype) {
        case XSValue::dt_boolean:
            if (actValue.fValue.f_bool == expValue.fValue.f_bool)
                return true;
            printf("ACTVALUE_TEST Unexpected XSValue for dt_boolean, got %d expected %d\n",
                    actValue.fValue.f_bool, expValue.fValue.f_bool);
            return false;

        case XSValue::dt_decimal:
            if (fabs(actValue.fValue.f_double - expValue.fValue.f_double) < fabs(actValue.fValue.f_double)/1000)
                return true;
            printf("ACTVALUE_TEST Unexpected XSValue for datatype %s, got %f expected %f\n", StrX(getDataTypeString(datatype)).localForm(),
                actValue.fValue.f_double, expValue.fValue.f_double);
            return false;
        case XSValue::dt_double:
            if (actValue.fValue.f_doubleType.f_doubleEnum == XSValue::DoubleFloatType_Normal) {
                if (fabs(actValue.fValue.f_double - expValue.fValue.f_double) < fabs(actValue.fValue.f_double)/1000)
                    return true;
                printf("ACTVALUE_TEST Unexpected XSValue for datatype %s, got %f expected %f\n", StrX(getDataTypeString(datatype)).localForm(),
                        actValue.fValue.f_double, expValue.fValue.f_double);
                return false;
            }
            else {
                if (actValue.fValue.f_doubleType.f_doubleEnum == expValue.fValue.f_doubleType.f_doubleEnum)
                    return true;
                printf("ACTVALUE_TEST Unexpected XSValue enum for datatype %s, got %d expected %d\n", StrX(getDataTypeString(datatype)).localForm(),
                        actValue.fValue.f_doubleType.f_doubleEnum, expValue.fValue.f_doubleType.f_doubleEnum);
                return false;
            }
        case XSValue::dt_float:
            if (actValue.fValue.f_floatType.f_floatEnum == XSValue::DoubleFloatType_Normal) {
                if (fabs(actValue.fValue.f_float - expValue.fValue.f_float) < fabs(actValue.fValue.f_float)/1000)
                    return true;
                printf("ACTVALUE_TEST Unexpected XSValue for datatype %s, got %f expected %f\n", StrX(getDataTypeString(datatype)).localForm(),
                        actValue.fValue.f_float, expValue.fValue.f_float);
                return false;
            }
            else {
                if (actValue.fValue.f_floatType.f_floatEnum == expValue.fValue.f_floatType.f_floatEnum)
                    return true;
                printf("ACTVALUE_TEST Unexpected XSValue enum for datatype %s, got %d expected %d\n", StrX(getDataTypeString(datatype)).localForm(),
                        actValue.fValue.f_floatType.f_floatEnum, expValue.fValue.f_floatType.f_floatEnum);
                return false;
            }
        case XSValue::dt_duration:
        case XSValue::dt_dateTime:
        case XSValue::dt_time:
        case XSValue::dt_date:
        case XSValue::dt_gYearMonth:
        case XSValue::dt_gYear:
        case XSValue::dt_gMonthDay:
        case XSValue::dt_gDay:
        case XSValue::dt_gMonth:
            if (actValue.fValue.f_datetime.f_year   == expValue.fValue.f_datetime.f_year   &&
                actValue.fValue.f_datetime.f_month  == expValue.fValue.f_datetime.f_month  &&
                actValue.fValue.f_datetime.f_day    == expValue.fValue.f_datetime.f_day    &&
                actValue.fValue.f_datetime.f_hour   == expValue.fValue.f_datetime.f_hour   &&
                actValue.fValue.f_datetime.f_min    == expValue.fValue.f_datetime.f_min    &&
                actValue.fValue.f_datetime.f_second == expValue.fValue.f_datetime.f_second &&
                (fabs(actValue.fValue.f_datetime.f_milisec  - expValue.fValue.f_datetime.f_milisec) < 0.01))
                return true;
            printf("ACTVALUE_TEST Unexpected %s XSValue\n", StrX(getDataTypeString(datatype)).localForm());
            printf(" Actual year = %d, month = %d, day = %d, hour = %d, min = %d, second = %d, milisec = %f\n",
                actValue.fValue.f_datetime.f_year, actValue.fValue.f_datetime.f_month, actValue.fValue.f_datetime.f_day,
                actValue.fValue.f_datetime.f_hour, actValue.fValue.f_datetime.f_min, actValue.fValue.f_datetime.f_second, actValue.fValue.f_datetime.f_milisec);
            printf(" Expect year = %d, month = %d, day = %d, hour = %d, min = %d, second = %d, milisec = %f\n",
                expValue.fValue.f_datetime.f_year, expValue.fValue.f_datetime.f_month, expValue.fValue.f_datetime.f_day,
                expValue.fValue.f_datetime.f_hour, expValue.fValue.f_datetime.f_min, expValue.fValue.f_datetime.f_second, expValue.fValue.f_datetime.f_milisec);
            return false;

        case XSValue::dt_hexBinary:
            // in the tests in this file the hexBinary data is always 2 long...
            if (actValue.fValue.f_byteVal[0] == expValue.fValue.f_byteVal[0] &&
                actValue.fValue.f_byteVal[1] == expValue.fValue.f_byteVal[1])
                return true;
            printf("ACTVALUE_TEST Unexpected hexBinary value\n");
            printf(" Actual Value = %x:%x\n",actValue.fValue.f_byteVal[0],actValue.fValue.f_byteVal[1]);
            printf(" Expect Value = %x:%x\n",expValue.fValue.f_byteVal[0],expValue.fValue.f_byteVal[1]);
            return false;

        case XSValue::dt_base64Binary:
            // in the tests in this file the base64Binary data is always 9 long (XMLByte[9])
            // however, a zero byte is used to indicate when the smaller data stream is empty
            {
                for (unsigned int i=0; i<9; i++)
                {
                    if (!expValue.fValue.f_byteVal[i])
                        return true;
                    if (actValue.fValue.f_byteVal[i] != expValue.fValue.f_byteVal[i])
                    {
                        printf("ACTVALUE_TEST Unexpected base64Binary value for byte %d\n", i);
                        printf(" Actual Value = %x\n",actValue.fValue.f_byteVal[i]);
                        printf(" Expect Value = %x\n",expValue.fValue.f_byteVal[i]);
                        return false;
                    }
                }
                return true;
            }

        case XSValue::dt_string:
        case XSValue::dt_anyURI:
        case XSValue::dt_QName:
        case XSValue::dt_NOTATION:
        case XSValue::dt_normalizedString:
        case XSValue::dt_token:
        case XSValue::dt_language:
        case XSValue::dt_NMTOKEN:
        case XSValue::dt_NMTOKENS:
        case XSValue::dt_Name:
        case XSValue::dt_NCName:
        case XSValue::dt_ID:
        case XSValue::dt_IDREF:
        case XSValue::dt_IDREFS:
        case XSValue::dt_ENTITY:
        case XSValue::dt_ENTITIES:
            printf("ACTVALUE_TEST no Actual Value for datatype %s\n", StrX(getDataTypeString(datatype)).localForm());
            return false;

        case XSValue::dt_integer:
        case XSValue::dt_nonPositiveInteger:
        case XSValue::dt_negativeInteger:
        case XSValue::dt_long:
            if (actValue.fValue.f_long == expValue.fValue.f_long)
                return true;
            printf("ACTVALUE_TEST Unexpected %s XSValue, got %ld expected %ld\n", StrX(getDataTypeString(datatype)).localForm(),
                    actValue.fValue.f_long, expValue.fValue.f_long);
            return false;

        case XSValue::dt_int:
            if (actValue.fValue.f_int == expValue.fValue.f_int)
                return true;
            printf("ACTVALUE_TEST Unexpected dt_int XSValue, got %d expected %d\n",
                    actValue.fValue.f_int, expValue.fValue.f_int);
            return false;
        case XSValue::dt_short:
            if (actValue.fValue.f_short == expValue.fValue.f_short)
                return true;
            printf("ACTVALUE_TEST Unexpected dt_short XSValue, got %d expected %d\n",
                    actValue.fValue.f_short, expValue.fValue.f_short);
            return false;
        case XSValue::dt_byte:
            if (actValue.fValue.f_char == expValue.fValue.f_char)
                return true;
            printf("ACTVALUE_TEST Unexpected dt_byte XSValue, got %d expected %d\n",
                    actValue.fValue.f_char, expValue.fValue.f_char);
            return false;
        case XSValue::dt_nonNegativeInteger:
        case XSValue::dt_unsignedLong:
        case XSValue::dt_positiveInteger:
            if (actValue.fValue.f_ulong == expValue.fValue.f_ulong)
                return true;
            printf("ACTVALUE_TEST Unexpected %s XSValue, got %lu expected %lu\n", StrX(getDataTypeString(datatype)).localForm(),
                    actValue.fValue.f_ulong, expValue.fValue.f_ulong);
            return false;
        case XSValue::dt_unsignedInt:
            if (actValue.fValue.f_uint == expValue.fValue.f_uint)
                return true;
            printf("ACTVALUE_TEST Unexpected dt_unsignedIntXSValue, got %d expected %d\n",
                    actValue.fValue.f_uint, expValue.fValue.f_uint);
            return false;
        case XSValue::dt_unsignedShort:
            if (actValue.fValue.f_ushort == expValue.fValue.f_ushort)
                return true;
            printf("ACTVALUE_TEST Unexpected dt_unsignedShort XSValue, got %d expected %d\n",
                    actValue.fValue.f_ushort, expValue.fValue.f_ushort);
            return false;
        case XSValue::dt_unsignedByte:
            if (actValue.fValue.f_uchar == expValue.fValue.f_uchar)
                return true;
            printf("ACTVALUE_TEST Unexpected dt_unsignedByte XSValue, got %d expected %d\n",
                    actValue.fValue.f_uchar, expValue.fValue.f_uchar);
            return false;
        default:
            printf("ACTVALUE_TEST Unexpected datatype\n");
            return false;
    }
}


static const char* getStatusString(const XSValue::Status status)
{
    switch (status)
    {
    case XSValue::st_Init:
        return "st_Init";
        break;
    case XSValue::st_NoContent:
        return "st_NoContent";
        break;
    case XSValue::st_NoCanRep:
        return "st_NoCanRep";
        break;
    case XSValue::st_NoActVal:
        return "st_NoActVal";
        break;
    case XSValue::st_NotSupported:
        return "st_NotSupported";
        break;
    case XSValue::st_CantCreateRegEx:
        return "st_CantCreateRegEx";
        break;
    case XSValue::st_FOCA0002:
        return "st_FOCA0002";
        break;
    case XSValue::st_FOCA0001:
        return "st_FOCA0001";
        break;
    case XSValue::st_FOCA0003:
        return "st_FOCA0003";
        break;
    case XSValue::st_FODT0003:
        return "st_FODT0003";
        break;
    case XSValue::st_UnknownType:
        return "st_UnknownType";
        break;
    default:
        return "st_UnknownType";
        break;
    }

}

/**
 * This is to test methods for XSValue
 */


#ifdef _DEBUG
void VALIDATE_TEST( const char*                  const  data
                  , const XSValue::DataType             datatype
                  ,       bool                          expRetValid
                  , const XSValue::Status               expStatus
                  )
{
    XSValue::Status myStatus = XSValue::st_Init;

    bool actRetValid = XSValue::validate(
                                          StrX(data).unicodeForm()
                                        , datatype
                                        , myStatus
                                        , XSValue::ver_10
                                        , XMLPlatformUtils::fgMemoryManager);

    if (actRetValid != expRetValid)
    {
        printf("VALIDATE_TEST Validation Fail: data=<%s>, datatype=<%s>, expRetVal=<%d>\n",
                data, StrX(getDataTypeString(datatype)).localForm(), expRetValid);
        errSeen = true;
    }
    else
    {
        if (!expRetValid             &&
             expStatus != DONT_CARE  &&
             expStatus != myStatus )
        {
            printf("VALIDATE_TEST Context Diff, data=<%s> datatype=<%s>, expStatus=<%s>, actStatus=<%s>\n",
                    data, StrX(getDataTypeString(datatype)).localForm(), getStatusString(expStatus), getStatusString(myStatus));
            errSeen = true;
        }
    }
}
#else
#define VALIDATE_TEST(data, datatype, expRetValid, expStatus)                        \
{                                                                                    \
    XSValue::Status myStatus = XSValue::st_Init;                                     \
    bool actRetValid = XSValue::validate(                                            \
                                          StrX(data).unicodeForm()                   \
                                        , datatype                                   \
                                        , myStatus                                   \
                                        , XSValue::ver_10                            \
                                        , XMLPlatformUtils::fgMemoryManager);        \
    if (actRetValid != expRetValid) {                                                \
        printf("VALIDATE_TEST Validation Fail:                                       \
                at line <%d>, data=<%s>, datatype=<%s>, expRetVal=<%d>\n"            \
              , __LINE__, data, StrX(getDataTypeString(datatype)).localForm()        \
              , expRetValid);                                                        \
        errSeen = true;                                                              \
    }                                                                                \
    else {                                                                           \
        if (!expRetValid             &&                                              \
             expStatus != DONT_CARE  &&                                              \
             expStatus != myStatus ) {                                               \
             printf("VALIDATE_TEST Context Diff,                                     \
                   at line <%d>, data=<%s> datatype=<%s>,                            \
                   expStatus=<%s>, actStatus=<%s>\n"                                 \
                  , __LINE__, data, StrX(getDataTypeString(datatype)).localForm(),   \
                  getStatusString(expStatus), getStatusString(myStatus));            \
             errSeen = true;                                                         \
        }                                                                            \
    }                                                                                \
}
#endif

#ifdef _DEBUG
void ACTVALUE_TEST(  const char*                  const  data
                   , const XSValue::DataType             datatype
                   ,       bool                          toValidate
                   ,       bool                          expRetValue
                   , const XSValue::Status               expStatus
                   , const XSValue::XSValue_Data         expValue
                    )
{
    XSValue::Status myStatus = XSValue::st_Init;

    XSValue* actRetValue = XSValue::getActualValue(
                                                   StrX(data).unicodeForm()
                                                 , datatype
                                                 , myStatus
                                                 , XSValue::ver_10
                                                 , toValidate
                                                 , XMLPlatformUtils::fgMemoryManager);
    if (actRetValue)
    {
        if (!expRetValue)
        {
            printf("ACTVALUE_TEST XSValue returned: data=<%s>, datatype=<%s>\n",
                    data, StrX(getDataTypeString(datatype)).localForm());
            errSeen = true;
        }
        else if (!compareActualValue(datatype, actRetValue->fData, expValue))
        {
            errSeen = true;
        }
        delete actRetValue;
    }
    else
    {
        if (expRetValue)
        {
            printf("ACTVALUE_TEST No XSValue returned, data=<%s>, datatype=<%s>\n" ,
                    data, StrX(getDataTypeString(datatype)).localForm());
            errSeen = true;
        }
        else
        {
            if (expStatus != DONT_CARE  &&
                expStatus != myStatus    )
            {
                printf("ACTVALUE_TEST Context Diff, data=<%s>, datatype=<%s>, expStatus=<%s>, actStatus=<%s>\n" ,
                       data, StrX(getDataTypeString(datatype)).localForm(), getStatusString(expStatus), getStatusString(myStatus));
                errSeen = true;
            }
        }
    }
}
#else
#define ACTVALUE_TEST(data, datatype, toValidate, expRetValue, expStatus, expValue)    \
{                                                                                      \
    XSValue::Status myStatus = XSValue::st_Init;                                       \
    XSValue* actRetValue = XSValue::getActualValue(                                    \
                                                   StrX(data).unicodeForm()            \
                                                 , datatype                            \
                                                 , myStatus                            \
                                                 , XSValue::ver_10                     \
                                                 , toValidate                          \
                                                 , XMLPlatformUtils::fgMemoryManager); \
    if (actRetValue) {                                                                 \
       if (!expRetValue) {                                                             \
            printf("ACTVALUE_TEST XSValue returned,                                    \
                    at line <%d> data=<%s>, datatype=<%s>\n"                           \
                  ,__LINE__, data, StrX(getDataTypeString(datatype)).localForm());     \
            errSeen = true;                                                            \
       }                                                                               \
       else if (!compareActualValue(datatype, actRetValue->fData, expValue)) {         \
            errSeen = true;                                                            \
       }                                                                               \
       delete actRetValue;                                                             \
    }                                                                                  \
    else {                                                                             \
        if (expRetValue) {                                                             \
            printf("ACTVALUE_TEST No XSValue returned,                                 \
                    at line <%d> data=<%s>, datatype=<%s>\n"                           \
                  , __LINE__, data, StrX(getDataTypeString(datatype)).localForm());    \
            errSeen = true;                                                            \
         }                                                                             \
         else {                                                                        \
             if (expStatus != DONT_CARE &&                                             \
                 expStatus != myStatus) {                                              \
                 printf("ACTVALUE_TEST Context Diff,                                   \
                         at line <%d> data=<%s>, datatype=<%s>,                        \
                         expStatus=<%s>, actStatus=<%s>\n"                             \
                        , __LINE__, data, StrX(getDataTypeString(datatype)).localForm() \
                        , getStatusString(expStatus), getStatusString(myStatus));      \
                 errSeen = true;                                                       \
             }                                                                         \
        }                                                                              \
    }                                                                                  \
}
#endif

#ifdef _DEBUG
void CANREP_TEST(const char*                  const  data
               , const XSValue::DataType             datatype
               ,       bool                          toValidate
               ,       bool                          expRetCanRep
               , const char*                  const  toCompare
               , const XSValue::Status               expStatus
                )
{
    XSValue::Status myStatus = XSValue::st_Init;

    XMLCh* actRetCanRep = XSValue::getCanonicalRepresentation(
                                                            StrX(data).unicodeForm()
                                                          , datatype
                                                          , myStatus
                                                          , XSValue::ver_10
                                                          , toValidate
                                                          , XMLPlatformUtils::fgMemoryManager);
    if (actRetCanRep)
    {
        if (!expRetCanRep)
        {
            printf("CANREP_TEST CanRep returned, data=<%s>, datatype=<%s>\n" ,
                    data, StrX(getDataTypeString(datatype)).localForm());
            XMLPlatformUtils::fgMemoryManager->deallocate(actRetCanRep);
            errSeen = true;
        }
        else
        {
            char* actRetCanRep_inchar = XMLString::transcode(actRetCanRep);

            if (!XMLString::equals(actRetCanRep_inchar, toCompare))
            {
                printf("CANREP_TEST CanRep Diff , data=<%s>, datatype=<%s>, actCanRep=<%s>, toCompare=<%s>\n" ,
                    data, StrX(getDataTypeString(datatype)).localForm(), actRetCanRep_inchar, toCompare);
                errSeen = true;
            }

            XMLPlatformUtils::fgMemoryManager->deallocate(actRetCanRep);
            XMLString::release(&actRetCanRep_inchar);
        }
    }
    else
    {
        if (expRetCanRep)
        {
            printf("CANREP_TEST No CanRep returned, data=<%s>, datatype=<%s>\n" ,
                    data, StrX(getDataTypeString(datatype)).localForm());
            errSeen = true;
        }
        else
        {
            if (expStatus != DONT_CARE  &&
                expStatus != myStatus    )
            {
                printf("CANREP_TEST Context Diff, data=<%s>, datatype=<%s>\n expStatus=<%s>, actStatus=<%s>\n" ,
                    data, StrX(getDataTypeString(datatype)).localForm(), getStatusString(expStatus), getStatusString(myStatus));
                errSeen = true;
            }
        }
    }
}
#else
#define CANREP_TEST(data, datatype, toValidate, expRetCanRep, toCompare, expStatus)   \
{                                                                                     \
    XSValue::Status myStatus = XSValue::st_Init;                                      \
    XMLCh* actRetCanRep = XSValue::getCanonicalRepresentation(                        \
                                                  StrX(data).unicodeForm()            \
                                                , datatype                            \
                                                , myStatus                            \
                                                , XSValue::ver_10                     \
                                                , toValidate                          \
                                                , XMLPlatformUtils::fgMemoryManager); \
    if (actRetCanRep) {                                                               \
        if (!expRetCanRep) {                                                          \
            printf("CANREP_TEST CanRep returned:                                      \
                    at line <%d> data=<%s>, datatype=<%s>\n"                          \
                  , __LINE__, data, StrX(getDataTypeString(datatype)).localForm());   \
            XMLPlatformUtils::fgMemoryManager->deallocate(actRetCanRep);              \
            errSeen = true;                                                           \
        }                                                                             \
        else {                                                                        \
            char* actRetCanRep_inchar = XMLString::transcode(actRetCanRep);           \
            if (!XMLString::equals(actRetCanRep_inchar, toCompare)) {                 \
                printf("CANREP_TEST CanRep Diff                                       \
                  , at line <%d> data=<%s>, datatype=<%s>,                            \
                    actCanRep=<%s>, toCompare=<%s>\n"                                 \
                  , __LINE__, data, StrX(getDataTypeString(datatype)).localForm()     \
                  , actRetCanRep_inchar, toCompare);                                  \
                errSeen = true;                                                       \
            }                                                                         \
            XMLPlatformUtils::fgMemoryManager->deallocate(actRetCanRep);              \
            XMLString::release(&actRetCanRep_inchar);                                 \
        }                                                                             \
    }                                                                                 \
    else {                                                                            \
        if (expRetCanRep){                                                            \
            printf("CANREP_TEST No CanRep returned,                                   \
                    at line <%d> data=<%s>, datatype=<%s>\n"                          \
                  , __LINE__, data, StrX(getDataTypeString(datatype)).localForm());   \
            errSeen = true;                                                           \
        }                                                                             \
        else {                                                                        \
        if (expStatus != myStatus) {                                                  \
                printf("CANREP_TEST Context Diff,                                     \
                        at line <%d> data=<%s>, datatype=<%s>\n                       \
                        expStatus=<%s>, actStatus=<%s>\n"                             \
                      , __LINE__, data, StrX(getDataTypeString(datatype)).localForm() \
                      , getStatusString(expStatus), getStatusString(myStatus));       \
                errSeen = true;                                                       \
            }                                                                         \
        }                                                                             \
    }                                                                                 \
}
#endif

#ifdef _DEBUG
void DATATYPE_TEST( const XMLCh*             const  dt_String
                  , const XSValue::DataType         expDataType
                  )
{
    XSValue::DataType actDataType = XSValue::getDataType(dt_String);

    if (actDataType != expDataType)
    {
        char* dt_str = XMLString::transcode(dt_String);
        printf("DATATYPE_TEST Fails: data=<%s>, actDataType=<%d>, expDataType=<%d>\n",
                dt_str, actDataType, expDataType);
        XMLString::release(&dt_str);
        errSeen = true;
    }
}
#else
#define DATATYPE_TEST(dt_String, expDataType)                                          \
{                                                                                      \
    XSValue::DataType actDataType = XSValue::getDataType(dt_String);                   \
    if (actDataType != expDataType)                                                    \
    {                                                                                  \
        char* dt_str = XMLString::transcode(dt_String);                                \
        printf("DATATYPE_TEST Fails: data=<%s>, actDataType=<%d>, expDataType=<%d>\n", \
                dt_str, actDataType, expDataType);                                     \
        XMLString::release(&dt_str);                                                   \
        errSeen = true;                                                                \
    }                                                                                  \
}
#endif

void  testNoActVal(const char*                  const  data
                 , const XSValue::DataType             datatype
                 , const XSValue::Status               expStatus)
{
    XSValue::Status ret_Status = XSValue::st_Init;
    XSValue* actVal = XSValue::getActualValue(StrX(data).unicodeForm(), datatype, ret_Status);

    if (actVal)
    {
        printf("testNoActVal fails, data=<%s>\n", data);
        delete actVal;
        errSeen=true;
        return;
    }

    if (ret_Status != expStatus)
    {
        printf("testNoActVal fails, data=<%s> retStatus=<%s> expStatus=<%s>\n",
                data
              , getStatusString(ret_Status)
              , getStatusString(expStatus));
        errSeen=true;
    }

}

void  testNoCanRep(const char*                  const  data
                 , const XSValue::DataType             datatype
                 , const XSValue::Status               expStatus)
{
    XSValue::Status ret_Status = XSValue::st_Init;
    XMLCh* canRep = XSValue::getCanonicalRepresentation(StrX(data).unicodeForm(), datatype, ret_Status);

    if (canRep)
    {
        printf("testNoCanRep fails, data=<%s>\n", data);
        delete canRep;
        errSeen=true;
        return;
    }

    if (ret_Status != expStatus)
    {
        printf("testNoCanRep fails, data=<%s> retStatus=<%s> expStatus=<%s>\n",
                data
              , getStatusString(ret_Status)
              , getStatusString(expStatus));
        errSeen=true;
    }
}

/***
 *   Test cases
 ***/
void test_dt_decimal()
{
    const XSValue::DataType dt = XSValue::dt_decimal;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="  1234.567  \n";
    const char lex_v_ran64_v_1[]="18446744073709551615.999";
    const char lex_v_ran64_v_2[]="999.18446744073709551615";
    const char lex_v_ran64_iv_1[]="18446744073709551616.999";
    const char lex_v_ran64_iv_2[]="999.18446744073709551616";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_1[]="4294967295.999";
    const char lex_v_ran32_v_2[]="999.4294967295";
#endif
//    const char lex_v_ran32_iv_1[]="4294967296.999";
//    const char lex_v_ran32_iv_2[]="999.4294967296";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234.56.789";

    XSValue::XSValue_Data act_v_ran_v_1;   act_v_ran_v_1.fValue.f_double = (double)1234.567;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_1; act_v_ran64_v_1.fValue.f_double = (double)18446744073709551615.999;
    XSValue::XSValue_Data act_v_ran64_v_2; act_v_ran64_v_2.fValue.f_double = (double)999.18446744073709551615;
    //XSValue::XSValue_Data act_v_ran64_iv_1;="18446744073709551616.999";
    //XSValue::XSValue_Data act_v_ran64_iv_2;="999.18446744073709551616";
#endif

    XSValue::XSValue_Data act_v_ran32_v_1; act_v_ran32_v_1.fValue.f_double = (double)4294967295.999;
    XSValue::XSValue_Data act_v_ran32_v_2; act_v_ran32_v_2.fValue.f_double = (double)999.4294967295;
    //XSValue::XSValue_Data act_v_ran32_iv_1;="4294967296.999";
    //XSValue::XSValue_Data act_v_ran32_iv_2;="999.4294967296";

/***
 * 3.2.3.2 Canonical representation
 *
 * The canonical representation for decimal is defined by prohibiting certain options from the Lexical
 * representation (3.2.3.1). Specifically,
 * 1. the preceding optional "+" sign is prohibited.
 * 2. The decimal point is required.
 * 3. Leading and trailing zeroes are prohibited subject to the following:
 *       there must be at least one digit to the right and
 *       to the left of the decimal point which may be a zero.
 ***/

    const char data_rawstr_1[]="  -123.45  \n";
    const char data_canrep_1[]="-123.45";
    const char data_rawstr_2[]="+123.45";
    const char data_canrep_2[]="123.45";
    const char data_rawstr_3[]="12345";
    const char data_canrep_3[]="12345.0";
    const char data_rawstr_4[]="000123.45";
    const char data_canrep_4[]="123.45";
    const char data_rawstr_5[]="123.45000";
    const char data_canrep_5[]="123.45";
    const char data_rawstr_6[]="00.12345";
    const char data_canrep_6[]="0.12345";
    const char data_rawstr_7[]="123.00";
    const char data_canrep_7[]="123.0";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid       n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_1 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_1 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_1);
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        //ACTVALUE_TEST(lex_v_ran64_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0001);
        //ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0001);
#else
        ACTVALUE_TEST(lex_v_ran32_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        //ACTVALUE_TEST(lex_v_ran32_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0001);
        //ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0001);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    DONT_CARE);
        CANREP_TEST(data_rawstr_3,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_3,    DONT_CARE);
        CANREP_TEST(data_rawstr_4,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_4,    DONT_CARE);
        CANREP_TEST(data_rawstr_5,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_5,    DONT_CARE);
        CANREP_TEST(data_rawstr_6,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_6,    DONT_CARE);
        CANREP_TEST(data_rawstr_7,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_7,    DONT_CARE);

        CANREP_TEST(lex_v_ran64_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_1,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_1, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_1, DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2, DONT_CARE);
        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,                XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,                XSValue::st_FOCA0002);

    }

}

/***
  * FLT_EPSILON 1.192092896e-07F
  * FLT_MIN 1.175494351e-38F
  * FLT_MAX 3.402823466e+38F
***/

void test_dt_float()
{

    const XSValue::DataType dt = XSValue::dt_float;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="  1234.e+10  \n";
    const char lex_v_ran_v_1[]="+3.402823466e+38";
    const char lex_v_ran_v_2[]="-3.402823466e+38";
    const char lex_v_ran_v_3[]="+1.175494351e-38";
    const char lex_v_ran_v_4[]="-1.175494351e-38";

    XSValue::XSValue_Data act_v_ran_v_0; act_v_ran_v_0.fValue.f_float = (float)1234.e+10;
    XSValue::XSValue_Data act_v_ran_v_1; act_v_ran_v_1.fValue.f_float = (float)+3.402823466e+38;
    XSValue::XSValue_Data act_v_ran_v_2; act_v_ran_v_2.fValue.f_float = (float)-3.402823466e+38;
    XSValue::XSValue_Data act_v_ran_v_3; act_v_ran_v_3.fValue.f_float = (float)+1.175494351e-38;
    XSValue::XSValue_Data act_v_ran_v_4; act_v_ran_v_4.fValue.f_float = (float)-1.175494351e-38;

    const char lex_v_ran_iv_1[]="+3.402823466e+39";
    const char lex_v_ran_iv_2[]="-3.402823466e+39";
    const char lex_v_ran_iv_3[]="+1.175494351e-46";
    const char lex_v_ran_iv_4[]="-1.175494351e-46";

    XSValue::XSValue_Data lex_iv_ran_v_1; lex_iv_ran_v_1.fValue.f_float = (float)0.0;
    lex_iv_ran_v_1.fValue.f_floatType.f_floatEnum = XSValue::DoubleFloatType_PosINF;
    XSValue::XSValue_Data lex_iv_ran_v_2; lex_iv_ran_v_2.fValue.f_float = (float)0.0;
    lex_iv_ran_v_2.fValue.f_floatType.f_floatEnum = XSValue::DoubleFloatType_NegINF;
    XSValue::XSValue_Data lex_iv_ran_v_3; lex_iv_ran_v_3.fValue.f_float = (float)0.0;
    lex_iv_ran_v_3.fValue.f_floatType.f_floatEnum = XSValue::DoubleFloatType_Zero;
    XSValue::XSValue_Data lex_iv_ran_v_4; lex_iv_ran_v_4.fValue.f_float = (float)0.0;
    lex_iv_ran_v_4.fValue.f_floatType.f_floatEnum = XSValue::DoubleFloatType_Zero;

    const char lex_v_ran_iv_1_canrep[]="INF";  // " 3.402823466E39"
    const char lex_v_ran_iv_2_canrep[]="-INF"; // "-3.402823466E39";
    const char lex_v_ran_iv_3_canrep[]="0";    // " 1.175494351E-46";
    const char lex_v_ran_iv_4_canrep[]="0";    // "-1.175494351E-46";

    const char lex_iv_1[]="12x.e+10";
    const char lex_iv_2[]="12.e+1x";

/***
 * 3.2.4.2 Canonical representation
 *
 * The canonical representation for float is defined by prohibiting certain options from the Lexical
 * representation (3.2.4.1).
 * Specifically,
 * 1. the exponent must be indicated by "E".
 * 2. Leading zeroes and the preceding optional "+" sign are prohibited in the exponent.
 * 3. For the mantissa, the preceding optional "+" sign is prohibited and the decimal point is required.
 *    Leading and trailing zeroes are prohibited subject to the following:
 *    number representations must be normalized such that there is a single digit to the left of the decimal point
 *    and at least a single digit to the right of the decimal point.
 *
 ***/

    const char data_rawstr_1[]="   -123.45    \n";
    const char data_canrep_1[]="-1.2345E2";
    const char data_rawstr_2[]="+123.45";
    const char data_canrep_2[]="1.2345E2";
    const char data_rawstr_3[]="+123.45e+0012";
    const char data_canrep_3[]="1.2345E14";
    const char data_rawstr_4[]="+100.000e2";
    const char data_canrep_4[]="1.0E4";
    const char data_rawstr_5[]="00100.23e2";
    const char data_canrep_5[]="1.0023E4";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, range valid
    VALIDATE_TEST(lex_v_ran_v_0 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_3 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_4 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, range invalid
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_3 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_4 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        // lexical valid, range valid
        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_v_3,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_3);
        ACTVALUE_TEST(lex_v_ran_v_4,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_4);

        // lexical valid, range invalid
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_3, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_3);
        ACTVALUE_TEST(lex_v_ran_iv_4, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_4);

        // lexical invalid
        ACTVALUE_TEST(lex_iv_1      , dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2      , dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                            XMLCh          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        // lexical valid, range valid
        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    DONT_CARE);
        CANREP_TEST(data_rawstr_3,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_3,    DONT_CARE);
        CANREP_TEST(data_rawstr_4,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_4,    DONT_CARE);
        CANREP_TEST(data_rawstr_5,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_5,    DONT_CARE);

        // lexical invalid
        CANREP_TEST(lex_iv_1,         dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2,         dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

        // lexical valid, range invalid (however XML4C ignores that)
        CANREP_TEST(lex_v_ran_iv_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep,    DONT_CARE);
        CANREP_TEST(lex_v_ran_iv_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep,    DONT_CARE);
        CANREP_TEST(lex_v_ran_iv_3,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_3_canrep,    DONT_CARE);
        CANREP_TEST(lex_v_ran_iv_4,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_4_canrep,    DONT_CARE);
    }
}

/***
DBL_EPSILON 2.2204460492503131e-016
DBL_MAX 1.7976931348623158e+308
DBL_MIN 2.2250738585072014e-308
***/
void test_dt_double()
{

    const XSValue::DataType dt = XSValue::dt_double;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="   1234.e+10   \n";
    const char lex_v_ran_v_1[]="+1.7976931348623158e+308";
    const char lex_v_ran_v_2[]="-1.7976931348623158e+308";
    const char lex_v_ran_v_3[]="+2.2250738585072014e-308";
    const char lex_v_ran_v_4[]="-2.2250738585072014e-308";

    XSValue::XSValue_Data act_v_ran_v_0; act_v_ran_v_0.fValue.f_double = (double)1234.e+10;
    XSValue::XSValue_Data act_v_ran_v_1; act_v_ran_v_1.fValue.f_double = (double)+1.7976931348623158e+308;
    XSValue::XSValue_Data act_v_ran_v_2; act_v_ran_v_2.fValue.f_double = (double)-1.7976931348623158e+308;
    XSValue::XSValue_Data act_v_ran_v_3; act_v_ran_v_3.fValue.f_double = (double)+2.2250738585072014e-308;
    XSValue::XSValue_Data act_v_ran_v_4; act_v_ran_v_4.fValue.f_double = (double)-2.2250738585072014e-308;

    const char lex_v_ran_iv_1[]="+1.7976931348623158e+309";
    const char lex_v_ran_iv_2[]="-1.7976931348623158e+309";
    // on linux, hp, aix, the representable range is around e-324
    // or e-325, using e-329 to gain consistent result on all
    // platforms
    const char lex_v_ran_iv_3[]="+2.2250738585072014e-329";
    const char lex_v_ran_iv_4[]="-2.2250738585072014e-329";

    const char lex_v_ran_iv_1_canrep[]="INF";  // " 1.7976931348623158E309";
    const char lex_v_ran_iv_2_canrep[]="-INF"; // "-1.7976931348623158E309";
    const char lex_v_ran_iv_3_canrep[]="0";    // "2.2250738585072014E-329";
    const char lex_v_ran_iv_4_canrep[]="0";    // "-2.2250738585072014E-329";

    XSValue::XSValue_Data lex_iv_ran_v_1; lex_iv_ran_v_1.fValue.f_double = (double)0.0;
    lex_iv_ran_v_1.fValue.f_doubleType.f_doubleEnum = XSValue::DoubleFloatType_PosINF;
    XSValue::XSValue_Data lex_iv_ran_v_2; lex_iv_ran_v_2.fValue.f_double = (double)0.0;
    lex_iv_ran_v_2.fValue.f_doubleType.f_doubleEnum = XSValue::DoubleFloatType_NegINF;
    XSValue::XSValue_Data lex_iv_ran_v_3; lex_iv_ran_v_3.fValue.f_double = (double)0.0;
    lex_iv_ran_v_3.fValue.f_doubleType.f_doubleEnum = XSValue::DoubleFloatType_Zero;
    XSValue::XSValue_Data lex_iv_ran_v_4; lex_iv_ran_v_4.fValue.f_double = (double)0.0;
    lex_iv_ran_v_4.fValue.f_doubleType.f_doubleEnum = XSValue::DoubleFloatType_Zero;

    const char lex_iv_1[]="12x.e+10";
    const char lex_iv_2[]="12.e+1x";

/***
 * 3.2.5.2 Canonical representation
 *
 * The canonical representation for float is defined by prohibiting certain options from the Lexical
 * representation (3.2.5.1).
 * Specifically,
 * 1. the exponent must be indicated by "E".
 * 2. Leading zeroes and the preceding optional "+" sign are prohibited in the exponent.
 * 3. For the mantissa, the preceding optional "+" sign is prohibited and the decimal point is required.
 *    Leading and trailing zeroes are prohibited subject to the following:
 *    number representations must be normalized such that there is a single digit to the left of the decimal point
 *    and at least a single digit to the right of the decimal point.
 *
 ***/

    const char data_rawstr_1[]="    -123.45    \n";
    const char data_canrep_1[]="-1.2345E2";
    const char data_rawstr_2[]="+123.45";
    const char data_canrep_2[]="1.2345E2";
    const char data_rawstr_3[]="+123.45e+0012";
    const char data_canrep_3[]="1.2345E14";
    const char data_rawstr_4[]="+100.000e2";
    const char data_canrep_4[]="1.0E4";
    const char data_rawstr_5[]="00100.23e2";
    const char data_canrep_5[]="1.0023E4";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, range valid
    VALIDATE_TEST(lex_v_ran_v_0 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_3 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_4 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, range invalid however XML4C converts to INF / ZERO
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_3 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_4 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        // lexical valid, range valid
        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_v_3,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_3);
        ACTVALUE_TEST(lex_v_ran_v_4,  dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, act_v_ran_v_4);

        // lexical valid, range invalid
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_3, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_3);
        ACTVALUE_TEST(lex_v_ran_iv_4, dt, toValidate, EXP_RET_VALUE_TRUE, DONT_CARE, lex_iv_ran_v_4);

        // lexical invalid
        ACTVALUE_TEST(lex_iv_1      , dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2      , dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                            XMLCh          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        // lexical valid, range valid
        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    DONT_CARE);
        CANREP_TEST(data_rawstr_3,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_3,    DONT_CARE);
        CANREP_TEST(data_rawstr_4,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_4,    DONT_CARE);
        CANREP_TEST(data_rawstr_5,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_5,    DONT_CARE);

        // lexical invalid
        CANREP_TEST(lex_iv_1,         dt, toValidate, EXP_RET_CANREP_FALSE,  null_string,     XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2,         dt, toValidate, EXP_RET_CANREP_FALSE,  null_string,     XSValue::st_FOCA0002);

        // lexical valid, range invalid (XML4C doesn't treat as invalid)
        CANREP_TEST(lex_v_ran_iv_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep,    DONT_CARE);
        CANREP_TEST(lex_v_ran_iv_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep,    DONT_CARE);
        CANREP_TEST(lex_v_ran_iv_3,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_3_canrep,    DONT_CARE);
        CANREP_TEST(lex_v_ran_iv_4,    dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_4_canrep,    DONT_CARE);
    }
}

/***
 *  9223372036854775807
 * -9223372036854775808
 *  2147483647
 * -2147483648
 ***/

void test_dt_integer()
{
    const XSValue::DataType dt = XSValue::dt_integer;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="   1234    \n";
    const char lex_v_ran64_v_1[]="+9223372036854775807";
    const char lex_v_ran64_v_2[]="-9223372036854775808";
    const char lex_v_ran64_iv_1[]="+9223372036854775808";
    const char lex_v_ran64_iv_2[]="-9223372036854775809";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_1[]="+2147483647";
    const char lex_v_ran32_v_2[]="-2147483648";
    const char lex_v_ran32_iv_1[]="+2147483648";
    const char lex_v_ran32_iv_2[]="-2147483649";
#endif

    XSValue::XSValue_Data act_v_ran_v_1;   act_v_ran_v_1.fValue.f_long = (long)1234;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_1; act_v_ran64_v_1.fValue.f_long = (long)+9223372036854775807;
    XSValue::XSValue_Data act_v_ran64_v_2; act_v_ran64_v_2.fValue.f_long = (long)-9223372036854775808;
#endif
    XSValue::XSValue_Data act_v_ran32_v_1; act_v_ran32_v_1.fValue.f_long = (long)+2147483647;
    XSValue::XSValue_Data act_v_ran32_v_2; act_v_ran32_v_2.fValue.f_long = (long)-2147483648;

    const char lex_v_ran64_v_1_canrep[]="9223372036854775807";
    const char lex_v_ran64_v_2_canrep[]="-9223372036854775808";
    const char lex_v_ran64_iv_1_canrep[]="9223372036854775808";
    const char lex_v_ran64_iv_2_canrep[]="-9223372036854775809";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.13.2 Canonical representation
 *
 * The canonical representation for integer is defined by prohibiting certain options from the Lexical
 * representation (3.3.13.1). Specifically,
 * 1. the preceding optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="   +12345   \n";
    const char data_canrep_1[]="12345";
    const char data_rawstr_2[]="00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid       n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_1 , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_1 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_1);
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        ACTVALUE_TEST(lex_v_ran64_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_1);
        ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_1);
#else
        ACTVALUE_TEST(lex_v_ran32_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_v_ran32_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_1);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran64_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_1_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_1, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_1_canrep, DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2_canrep, DONT_CARE);
        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

}

void test_dt_nonPositiveInteger()
{
    const XSValue::DataType dt = XSValue::dt_nonPositiveInteger;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="    -1234   \n";
    const char lex_v_ran_iv_1[]="+1";

    const char lex_v_ran64_v_2[]="-9223372036854775808";
    const char lex_v_ran64_iv_2[]="-9223372036854775809";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_2[]="-2147483648";
    const char lex_v_ran32_iv_2[]="-2147483649";
#endif

    XSValue::XSValue_Data act_v_ran_v_1;     act_v_ran_v_1.fValue.f_long = (long)-1234;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_2;   act_v_ran64_v_2.fValue.f_long = (long)-9223372036854775808;
#endif
    XSValue::XSValue_Data act_v_ran32_v_2;   act_v_ran32_v_2.fValue.f_long = (long)-2147483648;

    const char lex_v_ran64_v_2_canrep[]="-9223372036854775808";
    const char lex_v_ran64_iv_2_canrep[]="-9223372036854775809";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";


/***
 * 3.3.14.2 Canonical representation
 *
 * The canonical representation for nonPositiveInteger is defined by prohibiting certain options from the
 * Lexical representation (3.3.14.1). Specifically,
 * 1. the sign must be omitted for token "0" and
 * 2. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="   0    \n";
    const char data_canrep_1[]="0";
    const char data_rawstr_2[]="-00012345";
    const char data_canrep_2[]="-12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid       n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_1   , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_2);
#else
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_2);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    DONT_CARE);

        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2_canrep, DONT_CARE);
        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

}

void test_dt_negativeInteger()
{
    const XSValue::DataType dt = XSValue::dt_negativeInteger;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="    -1234    \n";
    const char lex_v_ran_iv_1[]="0";

    const char lex_v_ran64_v_2[]="-9223372036854775808";
    const char lex_v_ran64_iv_2[]="-9223372036854775809";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_2[]="-2147483648";
    const char lex_v_ran32_iv_2[]="-2147483649";
#endif

    XSValue::XSValue_Data act_v_ran_v_1;     act_v_ran_v_1.fValue.f_long = (long)-1234;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_2;   act_v_ran64_v_2.fValue.f_long = (long)-9223372036854775808;
#endif
    XSValue::XSValue_Data act_v_ran32_v_2;   act_v_ran32_v_2.fValue.f_long = (long)-2147483648;

    const char lex_v_ran64_v_2_canrep[]="-9223372036854775808";
    const char lex_v_ran64_iv_2_canrep[]="-9223372036854775809";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";


/***
 * 3.3.15.2 Canonical representation
 *
 * The canonical representation for negativeInteger is defined by prohibiting certain options
 * from the Lexical representation (3.3.15.1). Specifically,
 * 1. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="    -00012345    \n";
    const char data_canrep_1[]="-12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid       n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_1   , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_2);
#else
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_2);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);

        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2_canrep, DONT_CARE);
        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

}

void test_dt_long()
{
    const XSValue::DataType dt = XSValue::dt_long;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="    1234    \n";
    const char lex_v_ran64_v_1[]="+9223372036854775807";
    const char lex_v_ran64_v_2[]="-9223372036854775808";
    const char lex_v_ran64_iv_1[]="+9223372036854775808";
    const char lex_v_ran64_iv_2[]="-9223372036854775809";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_1[]="+2147483647";
    const char lex_v_ran32_v_2[]="-2147483648";
    const char lex_v_ran32_iv_1[]="+2147483648";
    const char lex_v_ran32_iv_2[]="-2147483649";
#endif

    XSValue::XSValue_Data act_v_ran_v_1;     act_v_ran_v_1.fValue.f_long = (long)1234;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_1;   act_v_ran64_v_1.fValue.f_long = (long)+9223372036854775807;
    XSValue::XSValue_Data act_v_ran64_v_2;   act_v_ran64_v_2.fValue.f_long = (long)-9223372036854775808;
#endif
    XSValue::XSValue_Data act_v_ran32_v_1;   act_v_ran32_v_1.fValue.f_long = (long)+2147483647;
    XSValue::XSValue_Data act_v_ran32_v_2;   act_v_ran32_v_2.fValue.f_long = (long)-2147483648;

    const char lex_v_ran64_v_1_canrep[]="9223372036854775807";
    const char lex_v_ran64_v_2_canrep[]="-9223372036854775808";
    const char lex_v_ran64_iv_1_canrep[]="9223372036854775808";
    const char lex_v_ran64_iv_2_canrep[]="-9223372036854775809";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.16.2 Canonical representation
 *
 * The canonical representation for long is defined by prohibiting certain options from the
 * Lexical representation (3.3.16.1). Specifically,
 * 1. the the optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.

 ***/

    const char data_rawstr_1[]="    +12345   \n";
    const char data_canrep_1[]="12345";
    const char data_rawstr_2[]="00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran64_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_1 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_1);
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        ACTVALUE_TEST(lex_v_ran64_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_1);
        ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_1);
#else
        ACTVALUE_TEST(lex_v_ran32_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_v_ran32_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_1);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran64_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_1_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2_canrep,  DONT_CARE);

        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran64_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran64_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran64_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran64_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2_canrep, DONT_CARE);
}

void test_dt_int()
{
    const XSValue::DataType dt = XSValue::dt_int;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="    1234     \n";

    const char lex_v_ran_v_1[]="+2147483647";
    const char lex_v_ran_v_2[]="-2147483648";
    const char lex_v_ran_iv_1[]="+2147483648";
    const char lex_v_ran_iv_2[]="-2147483649";

    XSValue::XSValue_Data act_v_ran_v_0;   act_v_ran_v_0.fValue.f_int = (int)1234;
    XSValue::XSValue_Data act_v_ran_v_1;   act_v_ran_v_1.fValue.f_int = (int)+2147483647;
    XSValue::XSValue_Data act_v_ran_v_2;   act_v_ran_v_2.fValue.f_int = (int)-2147483648;

    const char lex_v_ran_v_1_canrep[]="2147483647";
    const char lex_v_ran_v_2_canrep[]="-2147483648";
    const char lex_v_ran_iv_1_canrep[]="2147483648";
    const char lex_v_ran_iv_2_canrep[]="-2147483649";

    const char lex_iv_1[]="1234.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.17.2 Canonical representation
 *
 * The canonical representation for int is defined by prohibiting certain options from the
 * Lexical representation (3.3.17.1). Specifically,
 * 1. the the optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="    +12345  \n";
    const char data_canrep_1[]="12345";
    const char data_rawstr_2[]="00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_0  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_0);

        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1, dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,        DONT_CARE);
        CANREP_TEST(data_rawstr_2, dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,        XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran_v_1, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_1_canrep, DONT_CARE);
        CANREP_TEST(lex_v_ran_v_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_2_canrep, DONT_CARE);

        CANREP_TEST(lex_iv_1,      dt, toValidate, EXP_RET_CANREP_FALSE, null_string,          XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2,      dt, toValidate, EXP_RET_CANREP_FALSE, null_string,          XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep, DONT_CARE);
}

// 32767
// -32768

void test_dt_short()
{
    const XSValue::DataType dt = XSValue::dt_short;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="    1234    \n";

    const char lex_v_ran_v_1[]="+32767";
    const char lex_v_ran_v_2[]="-32768";
    const char lex_v_ran_iv_1[]="+32768";
    const char lex_v_ran_iv_2[]="-32769";

    XSValue::XSValue_Data act_v_ran_v_0;   act_v_ran_v_0.fValue.f_short = (short)1234;
    XSValue::XSValue_Data act_v_ran_v_1;   act_v_ran_v_1.fValue.f_short = (short)+32767;
    XSValue::XSValue_Data act_v_ran_v_2;   act_v_ran_v_2.fValue.f_short = (short)-32768;

    const char lex_v_ran_v_1_canrep[]="32767";
    const char lex_v_ran_v_2_canrep[]="-32768";
    const char lex_v_ran_iv_1_canrep[]="32768";
    const char lex_v_ran_iv_2_canrep[]="-32769";

    const char lex_iv_1[]="1234.456";
    const char lex_iv_2[]="1234b56";

/***
 *
 * 3.3.18.2 Canonical representation
 *
 * The canonical representation for short is defined by prohibiting certain options from the
 * Lexical representation (3.3.18.1). Specifically,
 * 1. the the optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="   +12345   \n";
    const char data_canrep_1[]="12345";
    const char data_rawstr_2[]="00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_0  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_0);

        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1, dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,        DONT_CARE);
        CANREP_TEST(data_rawstr_2, dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,        XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran_v_1, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_1_canrep, DONT_CARE);
        CANREP_TEST(lex_v_ran_v_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_2_canrep, DONT_CARE);

        CANREP_TEST(lex_iv_1,      dt, toValidate, EXP_RET_CANREP_FALSE, null_string,          XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2,      dt, toValidate, EXP_RET_CANREP_FALSE, null_string,          XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep, DONT_CARE);
}

//127
//-128

void test_dt_byte()
{
    const XSValue::DataType dt = XSValue::dt_byte;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="   12   \n";

    const char lex_v_ran_v_1[]="+127";
    const char lex_v_ran_v_2[]="-128";
    const char lex_v_ran_iv_1[]="+128";
    const char lex_v_ran_iv_2[]="-129";

    XSValue::XSValue_Data act_v_ran_v_0;   act_v_ran_v_0.fValue.f_char = (char)12;
    XSValue::XSValue_Data act_v_ran_v_1;   act_v_ran_v_1.fValue.f_char = (char)+127;
    XSValue::XSValue_Data act_v_ran_v_2;   act_v_ran_v_2.fValue.f_char = (char)-128;

    const char lex_v_ran_v_1_canrep[]="127";
    const char lex_v_ran_v_2_canrep[]="-128";
    const char lex_v_ran_iv_1_canrep[]="128";
    const char lex_v_ran_iv_2_canrep[]="-129";

    const char lex_iv_1[]="1234.456";
    const char lex_iv_2[]="1234b56";

/***
 *
 * 3.3.19.2 Canonical representation
 *
 * The canonical representation for byte is defined by prohibiting certain options from the
 * Lexical representation (3.3.19.1). Specifically,
 * 1. the the optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="   +123   \n";
    const char data_canrep_1[]="123";
    const char data_rawstr_2[]="000123";
    const char data_canrep_2[]="123";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_0  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_0);

        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1, dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,        DONT_CARE);
        CANREP_TEST(data_rawstr_2, dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,        XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran_v_1, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_1_canrep, DONT_CARE);
        CANREP_TEST(lex_v_ran_v_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_2_canrep, DONT_CARE);

        CANREP_TEST(lex_iv_1,      dt, toValidate, EXP_RET_CANREP_FALSE, null_string,          XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2,      dt, toValidate, EXP_RET_CANREP_FALSE, null_string,          XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep, DONT_CARE);
}

void test_dt_nonNegativeInteger()
{
    const XSValue::DataType dt = XSValue::dt_nonNegativeInteger;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="   1234   \n";
    const char lex_v_ran_iv_1[]="-1";

    const char lex_v_ran64_v_2[]="+18446744073709551615";
    const char lex_v_ran64_iv_2[]="+18446744073709551616";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_2[]="4294967295";
    const char lex_v_ran32_iv_2[]="4294967296";
#endif

    XSValue::XSValue_Data act_v_ran_v_1;    act_v_ran_v_1.fValue.f_ulong = (unsigned long)1234;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_2;  act_v_ran64_v_2.fValue.f_ulong = (unsigned long)+18446744073709551615;
#endif
    XSValue::XSValue_Data act_v_ran32_v_2;  act_v_ran32_v_2.fValue.f_ulong = (unsigned long)4294967295;

    const char lex_v_ran64_v_2_canrep[]="18446744073709551615";
    const char lex_v_ran64_iv_2_canrep[]="18446744073709551616";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";


/***
 * 3.3.20.2 Canonical representation
 *
 * The canonical representation for nonNegativeInteger is defined by prohibiting certain options from the
 * Lexical representation (3.3.20.1). Specifically,
 * 1. the the optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="   0    \n";
    const char data_canrep_1[]="0";
    const char data_rawstr_2[]="+00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid       n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_1   , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_2);
#else
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_2);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    DONT_CARE);

        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2_canrep, DONT_CARE);
        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

}

//18446744073709551615
// 4294967295
void test_dt_unsignedLong()
{
    const XSValue::DataType dt = XSValue::dt_unsignedLong;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="    1234   \n";
    const char lex_v_ran64_v_1[]="+18446744073709551615";
    const char lex_v_ran64_v_2[]="0";
    const char lex_v_ran64_iv_1[]="+18446744073709551616";
    const char lex_v_ran64_iv_2[]="-1";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_1[]="+4294967295";
    const char lex_v_ran32_v_2[]="0";
    const char lex_v_ran32_iv_1[]="4294967296";
    const char lex_v_ran32_iv_2[]="-1";
#endif

    XSValue::XSValue_Data act_v_ran_v_1;    act_v_ran_v_1.fValue.f_ulong = (unsigned long)1234;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_1;  act_v_ran64_v_1.fValue.f_ulong = (unsigned long)+18446744073709551615;
    XSValue::XSValue_Data act_v_ran64_v_2;  act_v_ran64_v_2.fValue.f_ulong = (unsigned long)0;
#endif
    XSValue::XSValue_Data act_v_ran32_v_1;  act_v_ran32_v_1.fValue.f_ulong = (unsigned long)+4294967295;
    XSValue::XSValue_Data act_v_ran32_v_2;  act_v_ran32_v_2.fValue.f_ulong = (unsigned long)0;

    const char lex_v_ran64_v_1_canrep[]="18446744073709551615";
    const char lex_v_ran64_v_2_canrep[]="0";
    const char lex_v_ran64_iv_1_canrep[]="18446744073709551616";
    const char lex_v_ran64_iv_2_canrep[]="-1";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.16.2 Canonical representation
 *
 * The canonical representation for long is defined by prohibiting certain options from the
 * Lexical representation (3.3.16.1). Specifically,
 * 1. the the optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.

 ***/

    const char data_rawstr_1[]="    +12345   \n";
    const char data_canrep_1[]="12345";
    const char data_rawstr_2[]="00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran64_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_1 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_1);
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        ACTVALUE_TEST(lex_v_ran64_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_1);
        ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran64_v_1);
#else
        ACTVALUE_TEST(lex_v_ran32_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_v_ran32_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran64_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_1_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2_canrep,  DONT_CARE);

        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran64_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran64_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran64_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran64_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2_canrep, DONT_CARE);
}

//4294967295
void test_dt_unsignedInt()
{
    const XSValue::DataType dt = XSValue::dt_unsignedInt;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="   1234   \n";

    const char lex_v_ran_v_1[]="+4294967295";
    const char lex_v_ran_v_2[]="0";
    const char lex_v_ran_iv_1[]="4294967296";
    const char lex_v_ran_iv_2[]="-1";

    XSValue::XSValue_Data act_v_ran_v_0;  act_v_ran_v_0.fValue.f_uint = (unsigned int)1234;
    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_uint = (unsigned int)+4294967295;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_uint = (unsigned int)0;

    const char lex_v_ran_v_1_canrep[]="4294967295";
    const char lex_v_ran_v_2_canrep[]="0";
    const char lex_v_ran_iv_1_canrep[]="4294967296";
    const char lex_v_ran_iv_2_canrep[]="-1";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.22.2 Canonical representation
 *
 * The canonical representation for unsignedInt is defined by prohibiting certain options from the
 * Lexical representation (3.3.22.1). Specifically,
 * leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="   +12345   \n";
    const char data_canrep_1[]="12345";
    const char data_rawstr_2[]="00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_0  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_0);

        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_1_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_2_canrep,  DONT_CARE);

        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep, DONT_CARE);
}

//65535
void test_dt_unsignedShort()
{
    const XSValue::DataType dt = XSValue::dt_unsignedShort;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="    1234  \n";

    const char lex_v_ran_v_1[]="+65535";
    const char lex_v_ran_v_2[]="0";
    const char lex_v_ran_iv_1[]="+65536";
    const char lex_v_ran_iv_2[]="-1";

    XSValue::XSValue_Data act_v_ran_v_0;  act_v_ran_v_0.fValue.f_ushort = (unsigned short)1234;
    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_ushort = (unsigned short)+65535;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_ushort = (unsigned short)0;

    const char lex_v_ran_v_1_canrep[]="65535";
    const char lex_v_ran_v_2_canrep[]="0";
    const char lex_v_ran_iv_1_canrep[]="65536";
    const char lex_v_ran_iv_2_canrep[]="-1";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.23.2 Canonical representation
 *
 * The canonical representation for unsignedShort is defined by prohibiting certain options from the
 * Lexical representation (3.3.23.1). Specifically,
 * 1. the leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="    +12345   \n";
    const char data_canrep_1[]="12345";
    const char data_rawstr_2[]="00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_0  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_0);

        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_1_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_2_canrep,  DONT_CARE);

        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep, DONT_CARE);
}

// 255
void test_dt_unsignedByte()
{
    const XSValue::DataType dt = XSValue::dt_unsignedByte;
    bool  toValidate = true;

    const char lex_v_ran_v_0[]="   123   \n";

    const char lex_v_ran_v_1[]="+255";
    const char lex_v_ran_v_2[]="0";
    const char lex_v_ran_iv_1[]="+256";
    const char lex_v_ran_iv_2[]="-1";

    XSValue::XSValue_Data act_v_ran_v_0;  act_v_ran_v_0.fValue.f_uchar = (unsigned char)123;
    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_uchar = (unsigned char)+255;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_uchar = (unsigned char)0;

    const char lex_v_ran_v_1_canrep[]="255";
    const char lex_v_ran_v_2_canrep[]="0";
    const char lex_v_ran_iv_1_canrep[]="256";
    const char lex_v_ran_iv_2_canrep[]="-1";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.24.2 Canonical representation
 *
 * The canonical representation for unsignedByte is defined by prohibiting certain options from the
 * Lexical representation (3.3.24.1). Specifically,
 * 1. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="   +123  \n";
    const char data_canrep_1[]="123";
    const char data_rawstr_2[]="000123";
    const char data_canrep_2[]="123";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid                         false           st_FOCA0002
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid, valid range
    VALIDATE_TEST(lex_v_ran_v_0  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_1  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical valid, invalid range
    VALIDATE_TEST(lex_v_ran_iv_1 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_v_ran_iv_2 , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_0,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_0);

        ACTVALUE_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_ran_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_v_ran_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_0);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid                              0            st_FOCA0002
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid                            XMLCh         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    XSValue::st_FOCA0002);

        CANREP_TEST(lex_v_ran_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_1_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran_v_2_canrep,  DONT_CARE);

        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

    //validation on
    CANREP_TEST(lex_v_ran_iv_1, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
    CANREP_TEST(lex_v_ran_iv_2, dt, true,  EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    //validation off
    CANREP_TEST(lex_v_ran_iv_1, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_1_canrep, DONT_CARE);
    CANREP_TEST(lex_v_ran_iv_2, dt, false, EXP_RET_CANREP_TRUE,  lex_v_ran_iv_2_canrep, DONT_CARE);
}

void test_dt_positiveInteger()
{
    const XSValue::DataType dt = XSValue::dt_positiveInteger;
    bool  toValidate = true;

    const char lex_v_ran_v_1[]="   1234   \n";
    const char lex_v_ran_iv_1[]="0";

    const char lex_v_ran64_v_2[]="+18446744073709551615";
    const char lex_v_ran64_iv_2[]="+18446744073709551616";

#if SIZEOF_LONG != 8
    const char lex_v_ran32_v_2[]="4294967295";
    const char lex_v_ran32_iv_2[]="4294967296";
#endif

    XSValue::XSValue_Data act_v_ran_v_1;    act_v_ran_v_1.fValue.f_ulong = (unsigned long)1234;
#if SIZEOF_LONG == 8
    XSValue::XSValue_Data act_v_ran64_v_2;  act_v_ran64_v_2.fValue.f_ulong = (unsigned long)+18446744073709551615;
#endif
    XSValue::XSValue_Data act_v_ran32_v_2;  act_v_ran32_v_2.fValue.f_ulong = (unsigned long)+4294967295;

    const char lex_v_ran64_v_2_canrep[]="18446744073709551615";
    const char lex_v_ran64_iv_2_canrep[]="18446744073709551616";

    const char lex_iv_1[]="12b34.456";
    const char lex_iv_2[]="1234b56";

/***
 * 3.3.25.2 Canonical representation
 *
 * The canonical representation for positiveInteger is defined by prohibiting certain options from the
 * Lexical representation (3.3.25.1). Specifically,
 * 1. the optional "+" sign is prohibited and
 * 2. leading zeroes are prohibited.
 *
 ***/

    const char data_rawstr_1[]="  +1  \n";
    const char data_canrep_1[]="1";
    const char data_rawstr_2[]="+00012345";
    const char data_canrep_2[]="12345";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid
     *            range  valid                           true              n/a
     *            range  invalid       n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_ran_v_1    , dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_ran_iv_1   , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    VALIDATE_TEST(lex_v_ran64_v_2  , dt, EXP_RET_VALID_TRUE, DONT_CARE);
    VALIDATE_TEST(lex_v_ran64_iv_2 , dt, EXP_RET_VALID_TRUE, DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2         , dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XSValue         n/a
     *          range  invalid                              0            st_Unpresentable
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        ACTVALUE_TEST(lex_v_ran_v_1,    dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);

#if SIZEOF_LONG == 8
        ACTVALUE_TEST(lex_v_ran64_v_2 , dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran64_v_2);
        ACTVALUE_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran64_v_2);
#else
        ACTVALUE_TEST(lex_v_ran32_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_v_ran32_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0003, act_v_ran32_v_2);
#endif

        ACTVALUE_TEST(lex_iv_1,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
        ACTVALUE_TEST(lex_iv_2,         dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran32_v_2);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid
     *          range  valid                              XMLCh          n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid
     *          range  valid                              XMLCh         n/a
     *          range  invalid          n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j)? true : false;

        CANREP_TEST(data_rawstr_1,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_1,    DONT_CARE);
        CANREP_TEST(data_rawstr_2,    dt, toValidate, EXP_RET_CANREP_TRUE,  data_canrep_2,    DONT_CARE);

        CANREP_TEST(lex_v_ran64_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_v_2_canrep,  DONT_CARE);
        CANREP_TEST(lex_v_ran64_iv_2, dt, toValidate, EXP_RET_CANREP_TRUE,  lex_v_ran64_iv_2_canrep, DONT_CARE);
        CANREP_TEST(lex_iv_1        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2        , dt, toValidate, EXP_RET_CANREP_FALSE, null_string,      XSValue::st_FOCA0002);

    }

}

void test_dt_boolean()
{
    const XSValue::DataType dt = XSValue::dt_boolean;
    bool  toValidate = true;

    const char lex_v_1[]="  1  \n";
    const char lex_v_2[]="0";
    const char lex_v_3[]="true";
    const char lex_v_4[]="false";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_bool = true;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_bool = false;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_bool = true;
    XSValue::XSValue_Data act_v_ran_v_4;  act_v_ran_v_4.fValue.f_bool = false;

    const char lex_iv_1[]="2";

    const char lex_v_1_canrep[]="true";
    const char lex_v_2_canrep[]="false";
    const char lex_v_3_canrep[]="true";
    const char lex_v_4_canrep[]="false";

/***
 * 3.2.2.2 Canonical representation
 *
 * The canonical representation for boolean is the set of literals {true, false}.
 *
 ***/

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid                                  true              n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_4,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid                                    XSValue         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid                                    XSValue         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        // lexical valid
        ACTVALUE_TEST(lex_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(lex_v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);
        ACTVALUE_TEST(lex_v_4,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_4);

        // lexical invalid
        ACTVALUE_TEST(lex_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *  lexical valid                                    XMLCh            n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid                                    XMLCh            n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        // lexical valid
        CANREP_TEST(lex_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_1_canrep, DONT_CARE);
        CANREP_TEST(lex_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_2_canrep, DONT_CARE);
        CANREP_TEST(lex_v_3,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_3_canrep, DONT_CARE);
        CANREP_TEST(lex_v_4,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_4_canrep, DONT_CARE);

        // lexical invalid
        CANREP_TEST(lex_iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,   XSValue::st_FOCA0002);
    }

}

void test_dt_hexBinary()
{
    const XSValue::DataType dt = XSValue::dt_hexBinary;
    bool  toValidate = true;

    const char lex_v_1[]="   0fb7  \n";
    const char lex_v_2[]="1234";

    const char lex_iv_1[]="0gb7";
    const char lex_iv_2[]="123";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    act_v_ran_v_1.fValue.f_byteVal = new XMLByte[2];
    act_v_ran_v_1.fValue.f_byteVal[0] = 0xf;
    act_v_ran_v_1.fValue.f_byteVal[1] = 0xb7;
    act_v_ran_v_2.fValue.f_byteVal = new XMLByte[2];
    act_v_ran_v_2.fValue.f_byteVal[0] = 0x12;
    act_v_ran_v_2.fValue.f_byteVal[1] = 0x34;

    const char lex_v_1_canrep[]="0FB7";
    const char lex_v_2_canrep[]="1234";

/***
 * 3.2.15.2 Canonical Rrepresentation
 *
 * The canonical representation for hexBinary is defined by prohibiting certain options from the
 * Lexical Representation (3.2.15.1). Specifically,
 * 1. the lower case hexadecimal digits ([a-f]) are not allowed.
 *
 ***/

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid                                  true              n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid                                    XSValue         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid                                    XSValue         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        // lexical valid
        ACTVALUE_TEST(lex_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);

        // lexical invalid
        ACTVALUE_TEST(lex_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(lex_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *  lexical valid                                    XMLCh            n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid                                    XMLCh            n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        // lexical valid
        CANREP_TEST(lex_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_1_canrep, DONT_CARE);
        CANREP_TEST(lex_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_2_canrep, DONT_CARE);

        // lexical invalid
        CANREP_TEST(lex_iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,   XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,   XSValue::st_FOCA0002);
    }

}

void test_dt_base64Binary()
{
    const XSValue::DataType dt = XSValue::dt_base64Binary;
    bool  toValidate = true;

    const char lex_v_1[]="  134x cv56 gui0   \n";
    const char lex_v_2[]="wxtz 8e4k";

    const char lex_iv_2[]="134xcv56gui";
    const char lex_iv_1[]="wxtz8e4";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    //actual values:
    //"134x cv56 gui0"  :     D7 7E 31 72 FE 7A 82 E8 B4
    //"wxtz 8e4k"       :     C3 1B 73 F1 EE 24
    act_v_ran_v_1.fValue.f_byteVal = new XMLByte[9];
    act_v_ran_v_1.fValue.f_byteVal[0] = 0xd7;
    act_v_ran_v_1.fValue.f_byteVal[1] = 0x7e;
    act_v_ran_v_1.fValue.f_byteVal[2] = 0x31;
    act_v_ran_v_1.fValue.f_byteVal[3] = 0x72;
    act_v_ran_v_1.fValue.f_byteVal[4] = 0xfe;
    act_v_ran_v_1.fValue.f_byteVal[5] = 0x7a;
    act_v_ran_v_1.fValue.f_byteVal[6] = 0x82;
    act_v_ran_v_1.fValue.f_byteVal[7] = 0xe8;
    act_v_ran_v_1.fValue.f_byteVal[8] = 0xb4;
    act_v_ran_v_2.fValue.f_byteVal = new XMLByte[9];
    act_v_ran_v_2.fValue.f_byteVal[0] = 0xc3;
    act_v_ran_v_2.fValue.f_byteVal[1] = 0x1b;
    act_v_ran_v_2.fValue.f_byteVal[2] = 0x73;
    act_v_ran_v_2.fValue.f_byteVal[3] = 0xf1;
    act_v_ran_v_2.fValue.f_byteVal[4] = 0xee;
    act_v_ran_v_2.fValue.f_byteVal[5] = 0x24;
    act_v_ran_v_2.fValue.f_byteVal[6] = 0;
    act_v_ran_v_2.fValue.f_byteVal[7] = 0;
    act_v_ran_v_2.fValue.f_byteVal[8] = 0;

    const char lex_v_1_canrep[]="134xcv56gui0";
    const char lex_v_2_canrep[]="wxtz8e4k";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *    lexical valid                                  true              n/a
     *    lexical invalid                                false           st_FOCA0002
     *
     ***/

    // lexical valid
    VALIDATE_TEST(lex_v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(lex_v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    // lexical invalid
    VALIDATE_TEST(lex_iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(lex_iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *  lexical valid                                    XSValue         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid                                    XSValue         n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        // lexical valid
        ACTVALUE_TEST(lex_v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(lex_v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);

        // lexical invalid
        ACTVALUE_TEST(lex_iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(lex_iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *  lexical valid                                    XMLCh            n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *  lexical valid                                    XMLCh            n/a
     *  lexical invalid                                     0            st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        // lexical valid
        CANREP_TEST(lex_v_1,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_1_canrep, DONT_CARE);
        CANREP_TEST(lex_v_2,  dt, toValidate, EXP_RET_CANREP_TRUE, lex_v_2_canrep, DONT_CARE);

        // lexical invalid
        CANREP_TEST(lex_iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,   XSValue::st_FOCA0002);
        CANREP_TEST(lex_iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,   XSValue::st_FOCA0002);
    }

}

void test_dt_duration()
{
    const XSValue::DataType dt = XSValue::dt_duration;
    bool  toValidate = true;

    const char v_1[]="   P1Y1M1DT1H1M1S   \n";
    const char v_2[]="P1Y1M31DT23H119M120S";
    const char v_3[]="-P1Y1M1DT23H";

    const char iv_1[]="P-1Y2M3DT10H30M";
    const char iv_2[]="P1Y1M1DT1H1M1X";
    const char iv_3[]="P1Z1M1DT23H";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;
    act_v_ran_v_1.fValue.f_datetime.f_year    = 1;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 1;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 1;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 1;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 1;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 1;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 1;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 1;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 31;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 23;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 119;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 120;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = -1;
    act_v_ran_v_3.fValue.f_datetime.f_month   = -1;
    act_v_ran_v_3.fValue.f_datetime.f_day     = -1;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = -23;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;
    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));

    }

}

void test_dt_date()
{
    const XSValue::DataType dt = XSValue::dt_date;
    bool  toValidate = true;

    const char v_1[]="   1991-05-31   \n";
    const char v_2[]="9999-06-30Z";
    const char v_3[]="99991-07-31+14:00";

    const char iv_1[]="2000-12-32";
    const char iv_2[]="2001-02-29";
    const char iv_3[]="2001-06-31";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    const char v_1_canrep[]="1991-05-31";
    const char v_2_canrep[]="9999-06-30Z";
    const char v_3_canrep[]="99991-07-30-10:00";

    /*
     * Case Date               Actual Value    Canonical Value
     *    1 yyyy-mm-dd         yyyy-mm-dd          yyyy-mm-dd
     *    2 yyyy-mm-ddZ        yyyy-mm-ddT00:00Z   yyyy-mm-ddZ
     *    3 yyyy-mm-dd+00:00   yyyy-mm-ddT00:00Z   yyyy-mm-ddZ
     *    4 yyyy-mm-dd+00:01   YYYY-MM-DCT23:59Z   yyyy-mm-dd+00:01
     *    5 yyyy-mm-dd+12:00   YYYY-MM-DCT12:00Z   yyyy-mm-dd+12:00
     *    6 yyyy-mm-dd+12:01   YYYY-MM-DCT11:59Z   YYYY-MM-DC-11:59
     *    7 yyyy-mm-dd+14:00   YYYY-MM-DCT10:00Z   YYYY-MM-DC-10:00
     *    8 yyyy-mm-dd-00:00   yyyy-mm-ddT00:00Z   yyyy-mm-ddZ
     *    9 yyyy-mm-dd-00:01   yyyy-mm-ddT00:01Z   yyyy-mm-dd-00:01
     *   11 yyyy-mm-dd-11:59   yyyy-mm-ddT11:59Z   YYYY-MM-DD-11:59
     *   10 yyyy-mm-dd-12:00   yyyy-mm-ddT12:00Z   YYYY-MM-DD+12:00
     *   12 yyyy-mm-dd-14:00   yyyy-mm-ddT14:00Z   YYYY-MM-DD+10:00
     */

    const char c_1[] = " 1993-05-31  ";       const char r_1[] = "1993-05-31";
    const char c_2[] = " 1993-05-31Z  ";      const char r_2[] = "1993-05-31Z";
    const char c_3[] = " 1993-05-31+00:00 ";  const char r_3[] = "1993-05-31Z";
    const char c_4[] = " 1993-05-31+00:01 ";  const char r_4[] = "1993-05-31+00:01";
    const char c_5[] = " 1993-05-31+12:00 ";  const char r_5[] = "1993-05-31+12:00";
    const char c_6[] = " 1994-01-01+12:01 ";  const char r_6[] = "1993-12-31-11:59";
    const char c_7[] = " 1994-01-01+14:00 ";  const char r_7[] = "1993-12-31-10:00";
    const char c_8[] = " 1993-06-01-00:00 ";  const char r_8[] = "1993-06-01Z";
    const char c_9[] = " 1993-06-01-00:01 ";  const char r_9[] = "1993-06-01-00:01";
    const char c_a[] = " 1993-06-01-11:59 ";  const char r_a[] = "1993-06-01-11:59";
    const char c_b[] = " 1993-05-31-12:00 ";  const char r_b[] = "1993-06-01+12:00";
    const char c_c[] = " 1993-05-31-14:00 ";  const char r_c[] = "1993-06-01+10:00";

    act_v_ran_v_1.fValue.f_datetime.f_year    = 1991;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 05;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 31;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 9999;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 06;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 30;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = 99991;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 07;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 30;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_TRUE, v_1_canrep, DONT_CARE);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_TRUE, v_2_canrep, DONT_CARE);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_TRUE, v_3_canrep, DONT_CARE);

        CANREP_TEST(c_1,  dt, toValidate, EXP_RET_CANREP_TRUE, r_1, DONT_CARE);
        CANREP_TEST(c_2,  dt, toValidate, EXP_RET_CANREP_TRUE, r_2, DONT_CARE);
        CANREP_TEST(c_3,  dt, toValidate, EXP_RET_CANREP_TRUE, r_3, DONT_CARE);
        CANREP_TEST(c_4,  dt, toValidate, EXP_RET_CANREP_TRUE, r_4, DONT_CARE);
        CANREP_TEST(c_5,  dt, toValidate, EXP_RET_CANREP_TRUE, r_5, DONT_CARE);
        CANREP_TEST(c_6,  dt, toValidate, EXP_RET_CANREP_TRUE, r_6, DONT_CARE);
        CANREP_TEST(c_7,  dt, toValidate, EXP_RET_CANREP_TRUE, r_7, DONT_CARE);
        CANREP_TEST(c_8,  dt, toValidate, EXP_RET_CANREP_TRUE, r_8, DONT_CARE);
        CANREP_TEST(c_9,  dt, toValidate, EXP_RET_CANREP_TRUE, r_9, DONT_CARE);
        CANREP_TEST(c_a,  dt, toValidate, EXP_RET_CANREP_TRUE, r_a, DONT_CARE);
        CANREP_TEST(c_b,  dt, toValidate, EXP_RET_CANREP_TRUE, r_b, DONT_CARE);
        CANREP_TEST(c_c,  dt, toValidate, EXP_RET_CANREP_TRUE, r_c, DONT_CARE);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);

    }

}

void test_dt_gYearMonth()
{
    const XSValue::DataType dt = XSValue::dt_gYearMonth;
    bool  toValidate = true;

    const char v_1[]="   20000-02   \n";
    const char v_2[]="0200-11+14:00";
    const char v_3[]="2000-02-14:00";

    const char iv_1[]="0000-12";
    const char iv_2[]="+2000-11";
    const char iv_3[]="2000.90-02";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    act_v_ran_v_1.fValue.f_datetime.f_year    = 20000;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 02;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 200;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 11;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = 2000;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 02;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;
    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));

    }

}

void test_dt_gYear()
{
    const XSValue::DataType dt = XSValue::dt_gYear;
    bool  toValidate = true;

    const char v_1[]="   0001-14:00  \n";
    const char v_2[]="9999+14:00";
    const char v_3[]="-1999";

    const char iv_1[]="0000";
    const char iv_2[]="+2000";
    const char iv_3[]="2000.90";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    act_v_ran_v_1.fValue.f_datetime.f_year    = 1;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 9999;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = -1999;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));

    }

}

void test_dt_gMonthDay()
{
    const XSValue::DataType dt = XSValue::dt_gMonthDay;
    bool  toValidate = true;

    const char v_1[]="   --01-31+00:01   \n";
    const char v_2[]="--03-31-00:01";
    const char v_3[]="--04-01";

    const char iv_1[]="--14-31";
    const char iv_2[]="--12-32";
    const char iv_3[]="--02-30";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    act_v_ran_v_1.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 1;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 30;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 3;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 31;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 4;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 1;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;
    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));

    }

}

void test_dt_gDay()
{
    const XSValue::DataType dt = XSValue::dt_gDay;
    bool  toValidate = true;

    const char v_1[]="   ---31+01:30   \n";
    const char v_2[]="---01-01:30";
    const char v_3[]="---28";

    const char iv_1[]="---+31";
    const char iv_2[]="---28.00";
    const char iv_3[]="--31";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    act_v_ran_v_1.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 30;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 1;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 28;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_2);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_3);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));

    }

}

void test_dt_gMonth()
{
    const XSValue::DataType dt = XSValue::dt_gMonth;
    bool  toValidate = true;

    const char v_1[]="   --02+10:10   \n";
    const char v_2[]="--10-12:12";
    const char v_3[]="--12";

    const char iv_1[]="--+11";
    const char iv_2[]="---02.09";
    const char iv_3[]="--14--";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    act_v_ran_v_1.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 2;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 10;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 12;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_2);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_3);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));

    }

}

void test_dt_dateTime()
{
    const XSValue::DataType dt = XSValue::dt_dateTime;
    bool  toValidate = true;

    const char v_1[]="   2000-12-31T23:59:59.00389  \n";
    const char v_2[]="2000-10-01T11:10:20+13:30";
    const char v_3[]="2000-10-01T11:10:20-06:00";

    const char iv_1[]="0000-12-31T23:59:59";
    const char iv_2[]="+2000-11-30T23:59:59Z";
    const char iv_3[]="2000-02-28T23:59.1:59Z";
    const char iv_4[]="2000-11-30T01:01:01Z99";
    const char iv_5[]="2000-02-28T01:01:01Z10:61";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    const char v_1_canrep[]="2000-12-31T23:59:59.00389";
    const char v_2_canrep[]="2000-09-30T21:40:20Z";
    const char v_3_canrep[]="2000-10-01T17:10:20Z";

    act_v_ran_v_1.fValue.f_datetime.f_year    = 2000;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 12;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 31;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 23;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 59;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 59;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0.00389;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 2000;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 9;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 30;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 21;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 40;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 20;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = 2000;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 10;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 1;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 17;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 10;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 20;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;

 /***
 * E2-41
 *
 *  3.2.7.2 Canonical representation
 *
 *  Except for trailing fractional zero digits in the seconds representation,
 *  '24:00:00' time representations, and timezone (for timezoned values),
 *  the mapping from literals to values is one-to-one. Where there is more
 *  than one possible representation, the canonical representation is as follows:
 *  redundant trailing zero digits in fractional-second literals are prohibited.
 *  An hour representation of '24' is prohibited. Timezoned values are canonically
 *  represented by appending 'Z' to the nontimezoned representation. (All
 *  timezoned dateTime values are UTC.)
 *
 *  .'24:00:00' -> '00:00:00'
 *  .milisecond: trailing zeros removed
 *  .'Z'
 *
 ***/

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_4, dt, EXP_RET_VALID_FALSE, XSValue::st_FODT0003);
    VALIDATE_TEST(iv_5, dt, EXP_RET_VALID_FALSE, XSValue::st_FODT0003);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_4, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FODT0003, act_v_ran_v_1);
        ACTVALUE_TEST(iv_5, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FODT0003, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            n/a
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            n/a
     *   invalid                                          0              st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_TRUE, v_1_canrep, DONT_CARE);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_TRUE, v_2_canrep, DONT_CARE);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_TRUE, v_3_canrep, DONT_CARE);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_4, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FODT0003);
        CANREP_TEST(iv_5, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FODT0003);

    }

}

void test_dt_time()
{
    const XSValue::DataType dt = XSValue::dt_time;
    bool  toValidate = true;

    const char v_1[]="   23:59:59.38900Z   \n";
    const char v_2[]="24:00:00";
    const char v_3[]="23:59:59+00:01";

    const char iv_1[]="55:59:59";
    const char iv_2[]="03:99:59";
    const char iv_3[]="23:59.1:59";
    const char iv_4[]="01:01:01Z99";
    const char iv_5[]="01:01:01Z10:61";

    XSValue::XSValue_Data act_v_ran_v_1;
    XSValue::XSValue_Data act_v_ran_v_2;
    XSValue::XSValue_Data act_v_ran_v_3;

    const char v_1_canrep[]="23:59:59.389Z";
    const char v_2_canrep[]="00:00:00";
    const char v_3_canrep[]="23:58:59Z";

    act_v_ran_v_1.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_1.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_1.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_1.fValue.f_datetime.f_hour    = 23;
    act_v_ran_v_1.fValue.f_datetime.f_min     = 59;
    act_v_ran_v_1.fValue.f_datetime.f_second  = 59;
    act_v_ran_v_1.fValue.f_datetime.f_milisec = 0.389;

    act_v_ran_v_2.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_2.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_2.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_hour    = 24;
    act_v_ran_v_2.fValue.f_datetime.f_min     = 0;
    act_v_ran_v_2.fValue.f_datetime.f_second  = 0;
    act_v_ran_v_2.fValue.f_datetime.f_milisec = 0;

    act_v_ran_v_3.fValue.f_datetime.f_year    = 0;
    act_v_ran_v_3.fValue.f_datetime.f_month   = 0;
    act_v_ran_v_3.fValue.f_datetime.f_day     = 0;
    act_v_ran_v_3.fValue.f_datetime.f_hour    = 23;
    act_v_ran_v_3.fValue.f_datetime.f_min     = 58;
    act_v_ran_v_3.fValue.f_datetime.f_second  = 59;
    act_v_ran_v_3.fValue.f_datetime.f_milisec = 0;
/***
 * 3.2.8.2 Canonical representation
 *
 * The canonical representation for time is defined by prohibiting certain options
 * from the Lexical representation (3.2.8.1). Specifically,
 * 1. either the time zone must be omitted or,
 * 2. if present, the time zone must be Coordinated Universal Time (UTC)
 *    indicated by a "Z".
 * 3. Additionally, the canonical representation for midnight is 00:00:00.
 *
 ***/


    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, XSValue::st_FOCA0002);
    VALIDATE_TEST(iv_4, dt, EXP_RET_VALID_FALSE, XSValue::st_FODT0003);
    VALIDATE_TEST(iv_5, dt, EXP_RET_VALID_FALSE, XSValue::st_FODT0003);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XSValue         n/a
     *   invalid                                           0            st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_TRUE,  DONT_CARE, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_2, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_3, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FOCA0002, act_v_ran_v_1);
        ACTVALUE_TEST(iv_4, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FODT0003, act_v_ran_v_1);
        ACTVALUE_TEST(iv_5, dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_FODT0003, act_v_ran_v_1);
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                          XMLCh            n/a
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                          XMLCh            n/a
     *   invalid                                          0              st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_TRUE, v_1_canrep, DONT_CARE);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_TRUE, v_2_canrep, DONT_CARE);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_TRUE, v_3_canrep, DONT_CARE);

        //  invalid
        CANREP_TEST(iv_1, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_2, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_3, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FOCA0002);
        CANREP_TEST(iv_4, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FODT0003);
        CANREP_TEST(iv_5, dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_FODT0003);
    }

}

void test_dt_string()
{
    const XSValue::DataType dt = XSValue::dt_string;
    bool  toValidate = true;

    const char v_1[]="mystring";
    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                     n/a             false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             n/a
     *   invalid                      n/a                  0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             n/a
     *   invalid                      n/a                  0             st_FOCA0002
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);

        //  invalid
    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              n/a
     *   invalid                       n/a                0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              n/a
     *   invalid                       n/a                0              st_FOCA0002
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid

    }

}

void test_dt_anyURI()
{
    const XSValue::DataType dt = XSValue::dt_anyURI;
    bool  toValidate = true;

    const char v_1[]="  http://www.schemaTest.org/IBMd3_2_17v01  \n";
    const char v_2[]="gopher://spinaltap.micro.umn.edu/00/Weather/California/Los%20Angeles";
    const char v_3[]="ftp://www.noNamespace.edu";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;

    const char iv_1[]="+htp://peiyongz@:90";
    const char iv_2[]=">////1.2.3.4.";
    const char iv_3[]="<///www.ibm.9om";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE, DONT_CARE);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE, DONT_CARE);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE, DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoActVal), act_v_ran_v_1);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
    }

}

void test_dt_QName()
{
    const XSValue::DataType dt = XSValue::dt_QName;
    bool  toValidate = true;

    const char v_1[]="   Ant:Eater  \n";
    const char v_2[]="Minimum_Length";
    const char v_3[]="abcde:a2345";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;

    const char iv_1[]="Three:Two:One";
    const char iv_2[]=":my";
    const char iv_3[]="+name";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                                     false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,  dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1, dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2, dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3, dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoActVal), act_v_ran_v_1);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate? XSValue::st_FOCA0002: XSValue::st_NoCanRep));
    }

}

void test_dt_NOTATION()
{
    const XSValue::DataType dt = XSValue::dt_NOTATION;
    bool  toValidate = true;

    const char v_1[]="   http://www.ibm.com/test:notation1  \n";
    const char iv_1[]="invaliduri:notation2";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                         n/a               0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                         n/a               0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);

        //  invalid


    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                            n/a           0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                            n/a           0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid

    }

}

void test_dt_normalizedString()
{
    const XSValue::DataType dt = XSValue::dt_normalizedString;
    bool  toValidate = true;

    const char v_1[]="4+4=8";
    const char v_2[]="a  b";
    const char v_3[]="someChars=*_-";

    const char iv_1[]="a\tb";
    const char iv_2[]="a\nb";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_2);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_dt_token()
{
    const XSValue::DataType dt = XSValue::dt_token;
    bool  toValidate = true;

    const char v_1[]="4+4=8";
    const char v_2[]="Number2";
    const char v_3[]="someChars=*_-";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;

    const char iv_1[]="a\tb";
    const char iv_2[]="a\nb";
    const char iv_3[]="a  b";

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_dt_language()
{
    const XSValue::DataType dt = XSValue::dt_language;
    bool  toValidate = true;

    const char v_1[]="en-AT";
    const char v_2[]="ja";
    const char v_3[]="uk-GB";

    const char iv_1[]="ja_JP";
    const char iv_2[]="en+US";
    const char iv_3[]="12-en";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;
    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_dt_NMTOKEN()
{
    const XSValue::DataType dt = XSValue::dt_NMTOKEN;
    bool  toValidate = true;

    const char v_1[]="Four:-_.";
    const char v_2[]="Zeerochert";
    const char v_3[]="007";

    const char iv_1[]="#board";
    const char iv_2[]="@com";
    const char iv_3[]=";abc";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;
    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_2);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_3);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_dt_NMTOKENS()
{
    const XSValue::DataType dt = XSValue::dt_NMTOKENS;
    bool  toValidate = true;

    const char v_1[]="name1 name2 name3 ";
    const char v_2[]="Zeerochert total number";
    const char v_3[]="007 009 123";

    const char iv_1[]="#board";
    const char iv_2[]="@com";
    const char iv_3[]=";abc";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_dt_Name()
{
    const XSValue::DataType dt = XSValue::dt_Name;
    bool  toValidate = true;

    const char v_1[]="Four:-_.";
    const char v_2[]="_Zeerochert";
    const char v_3[]=":007";

    const char iv_1[]="9name";
    const char iv_2[]="-name";
    const char iv_3[]=".name";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;

    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_2);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_3);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_dt_NCName_ID_IDREF_ENTITY(XSValue::DataType dt)
{
    bool  toValidate = true;

    const char v_1[]="Four-_.";
    const char v_2[]="_Zeerochert";
    const char v_3[]="L007";

    const char iv_1[]=":Four-_.";
    const char iv_2[]="_Zeerochert:";
    const char iv_3[]="0:07";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;
    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_dt_IDREFS_ENTITIES(XSValue::DataType dt)
{
    bool  toValidate = true;

    const char v_1[]="Four-_. Five Seven";
    const char v_2[]="_Zeerochert _Hundere Bye";
    const char v_3[]="L007 L009 L008";

    const char iv_1[]=":Four-_.";
    const char iv_2[]="_Zeerochert:";
    const char iv_3[]="0:07";

    XSValue::XSValue_Data act_v_ran_v_1;  act_v_ran_v_1.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_2;  act_v_ran_v_2.fValue.f_strVal = 0;
    XSValue::XSValue_Data act_v_ran_v_3;  act_v_ran_v_3.fValue.f_strVal = 0;
    /***
     *
     * validate
     * ---------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *     valid                                       true              n/a
     *     invalid                      n/a            false             st_FOCA0002
     *
     ***/

    //  valid
    VALIDATE_TEST(v_1,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_2,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);
    VALIDATE_TEST(v_3,   dt, EXP_RET_VALID_TRUE,  DONT_CARE);

    //  invalid
    VALIDATE_TEST(iv_1,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_2,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);
    VALIDATE_TEST(iv_3,  dt, EXP_RET_VALID_FALSE,  DONT_CARE);

    /***
     *
     * getActualValue
     * ---------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *  validation on
     *  =============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                             0             st_NoActVal
     *   invalid                                           0             st_NoActVal
     *
     ***/

    for (int i = 0; i < 2; i++)
    {
        //validation on/off
        toValidate = ( 0 == i) ? true : false;

        //  valid
        ACTVALUE_TEST(v_1,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_1);
        ACTVALUE_TEST(v_2,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_2);
        ACTVALUE_TEST(v_3,  dt, toValidate, EXP_RET_VALUE_FALSE, XSValue::st_NoActVal, act_v_ran_v_3);

        //  invalid
        ACTVALUE_TEST(iv_1,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_2,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);
        ACTVALUE_TEST(iv_3,  dt, toValidate, EXP_RET_VALUE_FALSE,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoActVal), act_v_ran_v_1);

    }

    /***
     *
     * getCanonicalRepresentation
     * ---------------------------
     *                             availability        return value      context
     *                             ----------------------------------------------
     *
     *  validation on
     *  =============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_FOCA0002
     *
     *  validation off
     *  ==============
     *   valid                                            0              st_NoCanRep
     *   invalid                                          0              st_NoCanRep
     *
     ***/

    for (int j = 0; j < 2; j++)
    {
        //validation on/off
        toValidate = ( 0 == j) ? true : false;

        //  valid
        CANREP_TEST(v_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);
        CANREP_TEST(v_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string, XSValue::st_NoCanRep);

        //  invalid
        CANREP_TEST(iv_1,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_2,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));
        CANREP_TEST(iv_3,  dt, toValidate, EXP_RET_CANREP_FALSE, null_string,
            (toValidate ? XSValue::st_FOCA0002 : XSValue::st_NoCanRep));

    }

}

void test_DataType()
{

    DATATYPE_TEST( SchemaSymbols::fgDT_STRING,             XSValue::dt_string);
    DATATYPE_TEST( SchemaSymbols::fgDT_BOOLEAN,            XSValue::dt_boolean);
    DATATYPE_TEST( SchemaSymbols::fgDT_DECIMAL,            XSValue::dt_decimal);
    DATATYPE_TEST( SchemaSymbols::fgDT_FLOAT,              XSValue::dt_float);
    DATATYPE_TEST( SchemaSymbols::fgDT_DOUBLE,             XSValue::dt_double);
    DATATYPE_TEST( SchemaSymbols::fgDT_DURATION,           XSValue::dt_duration);
    DATATYPE_TEST( SchemaSymbols::fgDT_DATETIME,           XSValue::dt_dateTime);
    DATATYPE_TEST( SchemaSymbols::fgDT_TIME,               XSValue::dt_time);
    DATATYPE_TEST( SchemaSymbols::fgDT_DATE,               XSValue::dt_date);
    DATATYPE_TEST( SchemaSymbols::fgDT_YEARMONTH,          XSValue::dt_gYearMonth);
    DATATYPE_TEST( SchemaSymbols::fgDT_YEAR,               XSValue::dt_gYear);
    DATATYPE_TEST( SchemaSymbols::fgDT_MONTHDAY,           XSValue::dt_gMonthDay);
    DATATYPE_TEST( SchemaSymbols::fgDT_DAY,                XSValue::dt_gDay);
    DATATYPE_TEST( SchemaSymbols::fgDT_MONTH,              XSValue::dt_gMonth);
    DATATYPE_TEST( SchemaSymbols::fgDT_HEXBINARY,          XSValue::dt_hexBinary);
    DATATYPE_TEST( SchemaSymbols::fgDT_BASE64BINARY,       XSValue::dt_base64Binary);
    DATATYPE_TEST( SchemaSymbols::fgDT_ANYURI,             XSValue::dt_anyURI);
    DATATYPE_TEST( SchemaSymbols::fgDT_QNAME,              XSValue::dt_QName);
    DATATYPE_TEST( XMLUni::fgNotationString,               XSValue::dt_NOTATION);
    DATATYPE_TEST( SchemaSymbols::fgDT_NORMALIZEDSTRING,   XSValue::dt_normalizedString);
    DATATYPE_TEST( SchemaSymbols::fgDT_TOKEN,              XSValue::dt_token);
    DATATYPE_TEST( SchemaSymbols::fgDT_LANGUAGE,           XSValue::dt_language);
    DATATYPE_TEST( XMLUni::fgNmTokenString,                XSValue::dt_NMTOKEN);
    DATATYPE_TEST( XMLUni::fgNmTokensString,               XSValue::dt_NMTOKENS);
    DATATYPE_TEST( SchemaSymbols::fgDT_NAME,               XSValue::dt_Name);
    DATATYPE_TEST( SchemaSymbols::fgDT_NCNAME,             XSValue::dt_NCName);
    DATATYPE_TEST( XMLUni::fgIDString,                     XSValue::dt_ID);
    DATATYPE_TEST( XMLUni::fgIDRefString,                  XSValue::dt_IDREF);
    DATATYPE_TEST( XMLUni::fgIDRefsString,                 XSValue::dt_IDREFS);
    DATATYPE_TEST( XMLUni::fgEntityString,                 XSValue::dt_ENTITY);
    DATATYPE_TEST( XMLUni::fgEntitiesString,               XSValue::dt_ENTITIES);
    DATATYPE_TEST( SchemaSymbols::fgDT_INTEGER,            XSValue::dt_integer);
    DATATYPE_TEST( SchemaSymbols::fgDT_NONPOSITIVEINTEGER, XSValue::dt_nonPositiveInteger);
    DATATYPE_TEST( SchemaSymbols::fgDT_NEGATIVEINTEGER,    XSValue::dt_negativeInteger);
    DATATYPE_TEST( SchemaSymbols::fgDT_LONG,               XSValue::dt_long);
    DATATYPE_TEST( SchemaSymbols::fgDT_INT,                XSValue::dt_int);
    DATATYPE_TEST( SchemaSymbols::fgDT_SHORT,              XSValue::dt_short);
    DATATYPE_TEST( SchemaSymbols::fgDT_BYTE,               XSValue::dt_byte);
    DATATYPE_TEST( SchemaSymbols::fgDT_NONNEGATIVEINTEGER, XSValue::dt_nonNegativeInteger);
    DATATYPE_TEST( SchemaSymbols::fgDT_ULONG,              XSValue::dt_unsignedLong);
    DATATYPE_TEST( SchemaSymbols::fgDT_UINT,               XSValue::dt_unsignedInt);
    DATATYPE_TEST( SchemaSymbols::fgDT_USHORT,             XSValue::dt_unsignedShort);
    DATATYPE_TEST( SchemaSymbols::fgDT_UBYTE,              XSValue::dt_unsignedByte);
    DATATYPE_TEST( SchemaSymbols::fgDT_POSITIVEINTEGER,    XSValue::dt_positiveInteger);

    DATATYPE_TEST( XMLUni::fgLongMaxInc,                   XSValue::dt_MAXCOUNT);
}

void testErrorStatus()
{
/***
DataType	Interface	Inv Char	Out-Of-Bound	To Big for C Type
dt_decimal  canRep      st_FOCA0002  n.a.	        n.a.
	        actVal      st_FOCA0002	n.a.            st_FOCA0001
***/

    {
        const char d1[]="12b34.456";
        const char d2[]="44444444444466666666666666666666666666666666666666666666666666666555555555555555555555555555555555555555555555555444294967296444444444444444444444444444445555555555555555555555555555555555555555555555555555555555555555555555555222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222.999";

        testNoCanRep(d1, XSValue::dt_decimal, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_decimal, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_decimal, XSValue::st_FOCA0001);
    }

/***
dt_float    canRep	st_FOCA0002	st_FOCA0002	n.a.
	        actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="+3.402823466e+39";
        const char d2[]="-3.4028234x66";
        const char d3[]="+1.175494351e-39";

        testNoCanRep(d1, XSValue::dt_float, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_float, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_float, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_float, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_float, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_float, XSValue::st_FOCA0002);

    }

/***
dt_double	canRep	st_FOCA0002	st_FOCA0002	n.a.
	        actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="+3.402823466e+308";
        const char d2[]="-3.4028234x66";
        const char d3[]="+1.175494351e-329";

        testNoCanRep(d1, XSValue::dt_double, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_double, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_double, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_double, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_double, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_double, XSValue::st_FOCA0002);

    }

/***
dt_integer 	canRep	st_FOCA0002	n.a.	n.a.
        	actVal	st_FOCA0002	n.a.	st_FOCA0003
***/
    {
        const char d1[]="+2147483648";
        const char d2[]="-2147483649";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d3, XSValue::dt_integer, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_integer, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_integer, XSValue::st_FOCA0003);
        testNoActVal(d2, XSValue::dt_integer, XSValue::st_FOCA0003);
        testNoActVal(d3, XSValue::dt_integer, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_integer, XSValue::st_FOCA0002);
    }

/***
dt_negativeInteger canRep	st_FOCA0002	st_FOCA0002	n.a.
	               actVal	st_FOCA0002	st_FOCA0002	st_FOCA0003
***/
    {
        const char d1[]="0";
        const char d2[]="-2147483649";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_negativeInteger, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_negativeInteger, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_negativeInteger, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_negativeInteger, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_negativeInteger, XSValue::st_FOCA0003);
        testNoActVal(d3, XSValue::dt_negativeInteger, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_negativeInteger, XSValue::st_FOCA0002);
    }

/***
dt_nonPositiveInteger	canRep	st_FOCA0002	st_FOCA0002	n.a.
	                    actVal	st_FOCA0002	st_FOCA0002	st_FOCA0003
***/
    {
        const char d1[]="1";
        const char d2[]="-2147483649";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_nonPositiveInteger, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_nonPositiveInteger, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_nonPositiveInteger, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_nonPositiveInteger, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_nonPositiveInteger, XSValue::st_FOCA0003);
        testNoActVal(d3, XSValue::dt_nonPositiveInteger, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_nonPositiveInteger, XSValue::st_FOCA0002);
    }
/***
dt_nonNegativeInteger   	canRep	st_FOCA0002	st_FOCA0002	n.a.
	                        actVal	st_FOCA0002	st_FOCA0002	st_FOCA0003
***/
    {
        const char d1[]="-1";
        const char d2[]="+2147483649";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_nonNegativeInteger, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_nonNegativeInteger, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_nonNegativeInteger, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_nonNegativeInteger, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_nonNegativeInteger, XSValue::st_FOCA0003);
        testNoActVal(d3, XSValue::dt_nonNegativeInteger, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_nonNegativeInteger, XSValue::st_FOCA0002);
    }

/***
dt_positiveInteger    	canRep	st_FOCA0002	st_FOCA0002	n.a.
                     	actVal	st_FOCA0002	st_FOCA0002	st_FOCA0003
***/
    {
        const char d1[]="0";
        const char d2[]="+2147483649";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_positiveInteger, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_positiveInteger, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_positiveInteger, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_positiveInteger, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_positiveInteger, XSValue::st_FOCA0003);
        testNoActVal(d3, XSValue::dt_positiveInteger, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_positiveInteger, XSValue::st_FOCA0002);
    }

/***
dt_long   canRep	st_FOCA0002	st_FOCA0002	n.a.
	      actVal	st_FOCA0002	st_FOCA0002	st_FOCA0003.
***/
    {
        const char d1[]="-9223372036854775809";
        const char d2[]="+9223372036854775808";
        const char d3[]="123x456";
        const char d4[]="123.456";
        const char d5[]="-92233720368547758";
        const char d6[]="+92233720368547758";

        testNoCanRep(d1, XSValue::dt_long, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_long, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_long, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_long, XSValue::st_FOCA0002);

        testNoActVal(d5, XSValue::dt_long, XSValue::st_FOCA0003);
        testNoActVal(d6, XSValue::dt_long, XSValue::st_FOCA0003);
        testNoActVal(d3, XSValue::dt_long, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_long, XSValue::st_FOCA0002);
    }

/***
dt_int	canRep	st_FOCA0002	st_FOCA0002	n.a.
	    actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="-2147483649";
        const char d2[]="+2147483648";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_int, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_int, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_int, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_int, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_int, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_int, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_int, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_int, XSValue::st_FOCA0002);
    }

/***
dt_short	canRep	st_FOCA0002	st_FOCA0002	n.a.
	        actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="-32769";
        const char d2[]="+32768";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_short, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_short, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_short, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_short, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_short, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_short, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_short, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_short, XSValue::st_FOCA0002);
    }

/***
dt_byte	canRep	st_FOCA0002	st_FOCA0002	n.a.
	    actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="-129";
        const char d2[]="+128";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_byte, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_byte, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_byte, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_byte, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_byte, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_byte, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_byte, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_byte, XSValue::st_FOCA0002);
    }

/***
dt_unsignedLong	canRep	st_FOCA0002	st_FOCA0002	n.a.
	actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="-1";
        const char d2[]="+18446744073709551616";
        const char d3[]="123x456";
        const char d4[]="123.456";
        const char d5[]="-3";
        const char d6[]="+92233720368547758";

        testNoCanRep(d1, XSValue::dt_unsignedLong, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_unsignedLong, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_unsignedLong, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_unsignedLong, XSValue::st_FOCA0002);

        testNoActVal(d5, XSValue::dt_unsignedLong, XSValue::st_FOCA0002);
        testNoActVal(d6, XSValue::dt_unsignedLong, XSValue::st_FOCA0003);
        testNoActVal(d3, XSValue::dt_unsignedLong, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_unsignedLong, XSValue::st_FOCA0002);
    }

/***
dt_unsignedInt	canRep	st_FOCA0002	st_FOCA0002	n.a.
	            actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="-1";
        const char d2[]="+4294967296";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_unsignedInt, XSValue::st_FOCA0002);
    }

/***
dt_unsignedShort	canRep	st_FOCA0002	st_FOCA0002	n.a.
                	actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="-1";
        const char d2[]="+65536";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_unsignedShort, XSValue::st_FOCA0002);
    }
/***
dt_unsignedByte	canRep	st_FOCA0002	st_FOCA0002	n.a.
	actVal	st_FOCA0002	st_FOCA0002	n.a.
***/
    {
        const char d1[]="-1";
        const char d2[]="+256";
        const char d3[]="123x456";
        const char d4[]="123.456";

        testNoCanRep(d1, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);
        testNoCanRep(d2, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);
        testNoCanRep(d3, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);
        testNoCanRep(d4, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);

        testNoActVal(d1, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);
        testNoActVal(d2, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);
        testNoActVal(d3, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);
        testNoActVal(d4, XSValue::dt_unsignedByte, XSValue::st_FOCA0002);
    }

}

// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main()
{

    // Initialize the XML4C system
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
        StrX msg(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during initialization! Message:\n"
            << msg << XERCES_STD_QUALIFIER endl;
        return 1;
    }

    test_dt_string();
    test_dt_boolean();
    test_dt_decimal();
    test_dt_float();
    test_dt_double();
    test_dt_duration();
    test_dt_dateTime();
    test_dt_time();
    test_dt_date();
    test_dt_gYearMonth();
    test_dt_gYear();
    test_dt_gMonthDay();
    test_dt_gDay();
    test_dt_gMonth();
    test_dt_hexBinary();
    test_dt_base64Binary();
    test_dt_anyURI();
    test_dt_QName();
    test_dt_NOTATION();
    test_dt_normalizedString();
    test_dt_token();
    test_dt_language();
    test_dt_NMTOKEN();
    test_dt_NMTOKENS();
    test_dt_Name();
    test_dt_NCName_ID_IDREF_ENTITY(XSValue::dt_NCName);
    test_dt_NCName_ID_IDREF_ENTITY(XSValue::dt_ID);
    test_dt_NCName_ID_IDREF_ENTITY(XSValue::dt_IDREF);
    test_dt_IDREFS_ENTITIES(XSValue::dt_IDREFS);
    test_dt_NCName_ID_IDREF_ENTITY(XSValue::dt_ENTITY);
    test_dt_IDREFS_ENTITIES(XSValue::dt_ENTITIES);
    test_dt_integer();
    test_dt_nonPositiveInteger();
    test_dt_negativeInteger();
    test_dt_long();
    test_dt_int();
    test_dt_short();
    test_dt_byte();
    test_dt_nonNegativeInteger();
    test_dt_unsignedLong();
    test_dt_unsignedInt();
    test_dt_unsignedShort();
    test_dt_unsignedByte();
    test_dt_positiveInteger();

    test_DataType();

    printf("\nXSValueTest %s\n", errSeen? "Fail" : "Pass");

    // And call the termination method
    XMLPlatformUtils::Terminate();

    return 0;
}

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif
