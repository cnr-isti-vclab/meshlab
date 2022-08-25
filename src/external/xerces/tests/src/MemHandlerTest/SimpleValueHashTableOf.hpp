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

//
// This is just a version of the ValueHashTableOf implementation from
// xercesc/util; we need a new one here because this cannot use the
// pluggable memory management facilities.
//

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/IllegalArgumentException.hpp>
#include <xercesc/util/NoSuchElementException.hpp>
#include <xercesc/util/NullPointerException.hpp>
#include <xercesc/util/RuntimeException.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/XMLEnumerator.hpp>
#include <xercesc/util/Hashers.hpp>

XERCES_CPP_NAMESPACE_USE

//  Forward declare the enumerator so it can be our friend.
//
template <class TVal, class THasher = PtrHasher>
class ValueHashTableOfEnumerator;


//
//  This should really be a nested class, but some of the compilers we
//  have to support cannot deal with that!
//
template <class TVal>
struct ValueHashTableBucketElem
{
    ValueHashTableBucketElem(void* key, const TVal& value, ValueHashTableBucketElem<TVal>* next)
		: fData(value), fNext(next), fKey(key)
        {
        }

    TVal                            fData;
    ValueHashTableBucketElem<TVal>* fNext;
    void*                           fKey;
};


template <class TVal, class THasher = PtrHasher>
class SimpleValueHashTableOf
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    SimpleValueHashTableOf(const XMLSize_t modulus);
    SimpleValueHashTableOf(const XMLSize_t modulus, const THasher& hasher);
    ~SimpleValueHashTableOf();


    // -----------------------------------------------------------------------
    //  Element management
    // -----------------------------------------------------------------------
    bool isEmpty() const;
    bool containsKey(const void* const key) const;
    void removeKey(const void* const key);
    void removeAll();


    // -----------------------------------------------------------------------
    //  Getters
    // -----------------------------------------------------------------------
    TVal& get(const void* const key);
    const TVal& get(const void* const key) const;


    // -----------------------------------------------------------------------
    //  Putters
    // -----------------------------------------------------------------------
    void put(void* key, const TVal& valueToAdopt);


private :
    // -----------------------------------------------------------------------
    //  Declare our friends
    // -----------------------------------------------------------------------
    friend class ValueHashTableOfEnumerator<TVal, THasher>;

private:

    // -----------------------------------------------------------------------
    //  Private methods
    // -----------------------------------------------------------------------
    ValueHashTableBucketElem<TVal>* findBucketElem(const void* const key, XMLSize_t& hashVal);
    const ValueHashTableBucketElem<TVal>* findBucketElem(const void* const key, XMLSize_t& hashVal) const;
    void removeBucketElem(const void* const key, XMLSize_t& hashVal);
    void initialize(const XMLSize_t modulus);


    // -----------------------------------------------------------------------
    //  Data members
    //
    //  fBucketList
    //      This is the array that contains the heads of all of the list
    //      buckets, one for each possible hash value.
    //
    //  fHashModulus
    //      The modulus used for this hash table, to hash the keys. This is
    //      also the number of elements in the bucket list.
	//
	//  fHash
	//      The hasher for the key data type.
    // -----------------------------------------------------------------------
    ValueHashTableBucketElem<TVal>** fBucketList;
    XMLSize_t                        fHashModulus;
    THasher                          fHasher;
};



//
//  An enumerator for a value array. It derives from the basic enumerator
//  class, so that value vectors can be generically enumerated.
//
template <class TVal, class THasher>
class ValueHashTableOfEnumerator : public XMLEnumerator<TVal>
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    ValueHashTableOfEnumerator(SimpleValueHashTableOf<TVal, THasher>* const toEnum, const bool adopt = false);
    virtual ~ValueHashTableOfEnumerator();


    // -----------------------------------------------------------------------
    //  Enum interface
    // -----------------------------------------------------------------------
    bool hasMoreElements() const;
    TVal& nextElement();
    void Reset();


private :
    // -----------------------------------------------------------------------
    //  Private methods
    // -----------------------------------------------------------------------
    void findNext();


    // -----------------------------------------------------------------------
    //  Data Members
    //
    //  fAdopted
    //      Indicates whether we have adopted the passed vector. If so then
    //      we delete the vector when we are destroyed.
    //
    //  fCurElem
    //      This is the current bucket bucket element that we are on.
    //
    //  fCurHash
    //      The is the current hash buck that we are working on. Once we hit
    //      the end of the bucket that fCurElem is in, then we have to start
    //      working this one up to the next non-empty bucket.
    //
    //  fToEnum
    //      The value array being enumerated.
    // -----------------------------------------------------------------------
    bool                            fAdopted;
    ValueHashTableBucketElem<TVal>* fCurElem;
    XMLSize_t                       fCurHash;
    SimpleValueHashTableOf<TVal, THasher>* fToEnum;

};

// ---------------------------------------------------------------------------
//  ValueHashTableOf: Constructors and Destructor
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
SimpleValueHashTableOf<TVal, THasher>::SimpleValueHashTableOf(const XMLSize_t modulus
                                                     , const THasher& hasher)
    : fBucketList(0), fHashModulus(modulus), fHasher (hasher)
{
  initialize(modulus);
}

template <class TVal, class THasher>
SimpleValueHashTableOf<TVal, THasher>::SimpleValueHashTableOf(const XMLSize_t modulus)
	: fBucketList(0), fHashModulus(modulus)
{
  initialize(modulus);
}

template <class TVal, class THasher>
void SimpleValueHashTableOf<TVal, THasher>::initialize(const XMLSize_t modulus)
{
	if (modulus == 0)
        ThrowXML(IllegalArgumentException, XMLExcepts::HshTbl_ZeroModulus);

    // Allocate the bucket list and zero them
    fBucketList = new ValueHashTableBucketElem<TVal>*[fHashModulus];
    for (XMLSize_t index = 0; index < fHashModulus; index++)
        fBucketList[index] = 0;
}

template <class TVal, class THasher>
SimpleValueHashTableOf<TVal, THasher>::~SimpleValueHashTableOf()
{
    removeAll();

    // Then delete the bucket list & hasher
    delete [] fBucketList;
}


// ---------------------------------------------------------------------------
//  SimpleValueHashTableOf: Element management
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
bool SimpleValueHashTableOf<TVal, THasher>::isEmpty() const
{
    // Just check the bucket list for non-empty elements
    for (XMLSize_t buckInd = 0; buckInd < fHashModulus; buckInd++)
    {
        if (fBucketList[buckInd] != 0)
            return false;
    }
    return true;
}

template <class TVal, class THasher>
bool SimpleValueHashTableOf<TVal, THasher>::
containsKey(const void* const key) const
{
    XMLSize_t hashVal;
    const ValueHashTableBucketElem<TVal>* findIt = findBucketElem(key, hashVal);
    return (findIt != 0);
}

template <class TVal, class THasher>
void SimpleValueHashTableOf<TVal, THasher>::
removeKey(const void* const key)
{
    XMLSize_t hashVal;
    removeBucketElem(key, hashVal);
}

template <class TVal, class THasher>
void SimpleValueHashTableOf<TVal, THasher>::removeAll()
{
    // Clean up the buckets first
    for (XMLSize_t buckInd = 0; buckInd < fHashModulus; buckInd++)
    {
        // Get the bucket list head for this entry
        ValueHashTableBucketElem<TVal>* curElem = fBucketList[buckInd];
        ValueHashTableBucketElem<TVal>* nextElem;
        while (curElem)
        {
            // Save the next element before we hose this one
            nextElem = curElem->fNext;

            // delete the current element and move forward
            delete curElem;
            curElem = nextElem;
        }

        // Clean out this entry
        fBucketList[buckInd] = 0;
    }
}


// ---------------------------------------------------------------------------
//  SimpleValueHashTableOf: Getters
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
TVal& SimpleValueHashTableOf<TVal, THasher>::get(const void* const key)
{
    XMLSize_t hashVal;
    ValueHashTableBucketElem<TVal>* findIt = findBucketElem(key, hashVal);
    if (!findIt)
        ThrowXML(NoSuchElementException, XMLExcepts::HshTbl_NoSuchKeyExists);

    return findIt->fData;
}

template <class TVal, class THasher>
const TVal& SimpleValueHashTableOf<TVal, THasher>::
get(const void* const key) const
{
    XMLSize_t hashVal;
    const ValueHashTableBucketElem<TVal>* findIt = findBucketElem(key, hashVal);
    if (!findIt)
        ThrowXML(NoSuchElementException, XMLExcepts::HshTbl_NoSuchKeyExists);

    return findIt->fData;
}


// ---------------------------------------------------------------------------
//  SimpleValueHashTableOf: Putters
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
void SimpleValueHashTableOf<TVal, THasher>::put(void* key, const TVal& valueToAdopt)
{
    // First see if the key exists already
    XMLSize_t hashVal;
    ValueHashTableBucketElem<TVal>* newBucket = findBucketElem(key, hashVal);

    //
    //  If so,then update its value. If not, then we need to add it to
    //  the right bucket
    //
    if (newBucket)
    {
        newBucket->fData = valueToAdopt;
		newBucket->fKey = key;
    }
     else
    {
        newBucket = new ValueHashTableBucketElem<TVal>(key, valueToAdopt, fBucketList[hashVal]);
        fBucketList[hashVal] = newBucket;
    }
}



// ---------------------------------------------------------------------------
//  SimpleValueHashTableOf: Private methods
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
ValueHashTableBucketElem<TVal>* SimpleValueHashTableOf<TVal, THasher>::
findBucketElem(const void* const key, XMLSize_t& hashVal)
{
    // Hash the key
    hashVal = fHasher.getHashVal(key, fHashModulus);
    if (hashVal > fHashModulus)
        ThrowXML(RuntimeException, XMLExcepts::HshTbl_BadHashFromKey);

    // Search that bucket for the key
    ValueHashTableBucketElem<TVal>* curElem = fBucketList[hashVal];
    while (curElem)
    {
		if (fHasher.equals(key, curElem->fKey))
            return curElem;

        curElem = curElem->fNext;
    }
    return 0;
}

template <class TVal, class THasher>
const ValueHashTableBucketElem<TVal>* SimpleValueHashTableOf<TVal, THasher>::
findBucketElem(const void* const key, XMLSize_t& hashVal) const
{
    // Hash the key
    hashVal = fHasher.getHashVal(key, fHashModulus);
    if (hashVal > fHashModulus)
        ThrowXML(RuntimeException, XMLExcepts::HshTbl_BadHashFromKey);

    // Search that bucket for the key
    const ValueHashTableBucketElem<TVal>* curElem = fBucketList[hashVal];
    while (curElem)
    {
        if (fHasher.equals(key, curElem->fKey))
            return curElem;

        curElem = curElem->fNext;
    }
    return 0;
}


template <class TVal, class THasher>
void SimpleValueHashTableOf<TVal, THasher>::
removeBucketElem(const void* const key, XMLSize_t& hashVal)
{
    // Hash the key
    hashVal = fHasher.getHashVal(key, fHashModulus);
    if (hashVal > fHashModulus)
        ThrowXML(RuntimeException, XMLExcepts::HshTbl_BadHashFromKey);

    //
    //  Search the given bucket for this key. Keep up with the previous
    //  element so we can patch around it.
    //
    ValueHashTableBucketElem<TVal>* curElem = fBucketList[hashVal];
    ValueHashTableBucketElem<TVal>* lastElem = 0;

    while (curElem)
    {
        if (fHasher.equals(key, curElem->fKey))
        {
            if (!lastElem)
            {
                // It was the first in the bucket
                fBucketList[hashVal] = curElem->fNext;
            }
             else
            {
                // Patch around the current element
                lastElem->fNext = curElem->fNext;
            }

            // Delete the current element
            delete curElem;

            return;
        }

        // Move both pointers upwards
        lastElem = curElem;
        curElem = curElem->fNext;
    }

    // We never found that key
    ThrowXML(NoSuchElementException, XMLExcepts::HshTbl_NoSuchKeyExists);
}


// ---------------------------------------------------------------------------
//  ValueHashTableOfEnumerator: Constructors and Destructor
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
ValueHashTableOfEnumerator<TVal, THasher>::
ValueHashTableOfEnumerator(SimpleValueHashTableOf<TVal, THasher>* const toEnum, const bool adopt)
	: fAdopted(adopt), fCurElem(0), fCurHash((XMLSize_t)-1), fToEnum(toEnum)
{
    if (!toEnum)
        ThrowXML(NullPointerException, XMLExcepts::CPtr_PointerIsZero);

    //
    //  Find the next available bucket element in the hash table. If it
    //  comes back zero, that just means the table is empty.
    //
    //  Note that the -1 in the current hash tells it to start from the
    //  beginning.
    //
    findNext();
}

template <class TVal, class THasher>
ValueHashTableOfEnumerator<TVal, THasher>::~ValueHashTableOfEnumerator()
{
    if (fAdopted)
        delete fToEnum;
}


// ---------------------------------------------------------------------------
//  ValueHashTableOfEnumerator: Enum interface
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
bool ValueHashTableOfEnumerator<TVal, THasher>::hasMoreElements() const
{
    //
    //  If our current has is at the max and there are no more elements
    //  in the current bucket, then no more elements.
    //
    if (!fCurElem && (fCurHash == fToEnum->fHashModulus))
        return false;
    return true;
}

template <class TVal, class THasher>
TVal& ValueHashTableOfEnumerator<TVal, THasher>::nextElement()
{
    // Make sure we have an element to return
    if (!hasMoreElements())
        ThrowXML(NoSuchElementException, XMLExcepts::Enum_NoMoreElements);

    //
    //  Save the current element, then move up to the next one for the
    //  next time around.
    //
    ValueHashTableBucketElem<TVal>* saveElem = fCurElem;
    findNext();

    return saveElem->fData;
}


template <class TVal, class THasher>
void ValueHashTableOfEnumerator<TVal, THasher>::Reset()
{
    fCurHash = (XMLSize_t)-1;
    fCurElem = 0;
    findNext();
}



// ---------------------------------------------------------------------------
//  ValueHashTableOfEnumerator: Private helper methods
// ---------------------------------------------------------------------------
template <class TVal, class THasher>
void ValueHashTableOfEnumerator<TVal, THasher>::findNext()
{
    //
    //  If there is a current element, move to its next element. If this
    //  hits the end of the bucket, the next block will handle the rest.
    //
    if (fCurElem)
        fCurElem = fCurElem->fNext;

    //
    //  If the current element is null, then we have to move up to the
    //  next hash value. If that is the hash modulus, then we cannot
    //  go further.
    //
    if (!fCurElem)
    {
        fCurHash++;
        if (fCurHash == fToEnum->fHashModulus)
            return;

        // Else find the next non-empty bucket
        while (true)
        {
            if (fToEnum->fBucketList[fCurHash])
                break;

            // Bump to the next hash value. If we max out return
            fCurHash++;
            if (fCurHash == fToEnum->fHashModulus)
                return;
        }
        fCurElem = fToEnum->fBucketList[fCurHash];
    }
}
