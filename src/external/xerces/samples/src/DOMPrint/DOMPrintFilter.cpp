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

#include "DOMPrintFilter.hpp"
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>

static const XMLCh  element_person[]=
{
	chLatin_p, chLatin_e, chLatin_r, chLatin_s, chLatin_o, chLatin_n, chNull
};

static const XMLCh  element_link[]=
{
	chLatin_l, chLatin_i, chLatin_n, chLatin_k, chNull
};

DOMPrintFilter::DOMPrintFilter(ShowType whatToShow)
:fWhatToShow(whatToShow)
{}

DOMNodeFilter::FilterAction DOMPrintFilter::
acceptNode(const DOMNode* node) const
{
	//
	// The DOMLSSerializer shall call getWhatToShow() before calling
	// acceptNode(), to show nodes which are supposed to be
	// shown to this filter.
	//
	// REVISIT: In case the DOMLSSerializer does not follow the protocol,
	//          Shall the filter honour, or NOT, what it claims
	//          it is interested in ?
	//
	// The DOMLS specs does not specify that acceptNode() shall do
	// this way, or not, so it is up the implementation,
	// to skip the code below for the sake of performance ...
	//
	if ((getWhatToShow() & (1 << (node->getNodeType() - 1))) == 0)
		return DOMNodeFilter::FILTER_ACCEPT;

	switch (node->getNodeType())
	{
	case DOMNode::ELEMENT_NODE:
		{
			// for element whose name is "person", skip it
			if (XMLString::compareString(node->getNodeName(), element_person)==0)
				return DOMNodeFilter::FILTER_SKIP;
			// for element whose name is "line", reject it
			if (XMLString::compareString(node->getNodeName(), element_link)==0)
				return DOMNodeFilter::FILTER_REJECT;
			// for rest, accept it
			return DOMNodeFilter::FILTER_ACCEPT;

			break;
		}
	case DOMNode::COMMENT_NODE:
		{
			// the WhatToShow will make this no effect
			return DOMNodeFilter::FILTER_REJECT;
			break;
		}
	case DOMNode::TEXT_NODE:
		{
			// the WhatToShow will make this no effect
			return DOMNodeFilter::FILTER_REJECT;
			break;
		}
	case DOMNode::DOCUMENT_TYPE_NODE:
		{
			// even we say we are going to process document type,
			// we are not able be to see this node since
			// DOMLSSerializerImpl (a XercesC's default implementation
			// of DOMLSSerializer) will not pass DocumentType node to
			// this filter.
			//
			return DOMNodeFilter::FILTER_REJECT;  // no effect
			break;
		}
	case DOMNode::DOCUMENT_NODE:
		{
			// same as DOCUMENT_NODE
			return DOMNodeFilter::FILTER_REJECT;  // no effect
			break;
		}
	default :
		{
			return DOMNodeFilter::FILTER_ACCEPT;
			break;
		}
	}

	return DOMNodeFilter::FILTER_ACCEPT;
}
