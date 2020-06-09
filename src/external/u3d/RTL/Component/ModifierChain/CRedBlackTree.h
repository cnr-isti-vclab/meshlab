//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file	CRedBlackTree.h

			This iterative Red Black tree implementation with parent pointers
			is based  on the explanation and algorithms in the book
			Introduction to Algorithms
			by  Cormen , Leiserson, Riverst (MIT Press)
*/

#ifndef _CRED_BLACK_TREE_H_
#define _CRED_BLACK_TREE_H_

#include "IFXDataTypes.h"
#include "IFXUnitAllocator.h"


// Default Comparators
template<class K1, class K2>
class DEFAULT_COMPARE
{
public:
	I32 operator()( const K1 & arg1, const K2 & arg2) const
	{
		I32 result = arg1 == arg2 ? 0:1;
		if(result)
			result = arg1 > arg2 ? 1:-1;
		return result;
	}
};


enum eNodeColor {BLACK, RED, _FORCE_32BIT = 0xFFFFFFFF};

/**
	T == data element stored in the RB tree
	K == Key element in T used for comparisons
	CMP == the Comparator class used to compare 2 instances of T
*/
template <class T, class K, class CMP >
class CRedBlackTree
{
	struct RBNode
	{
		RBNode* parent;
		RBNode* right;
		RBNode* left;
		eNodeColor color;
		T data; // really an uninitialized block of memory sizeof(T)
#ifdef _DEBUGTREE
		U32 cnt;
		U8 Address[8];
#endif
	};

	RBNode * m_Root;
	RBNode * m_NIL;
	IFXUnitAllocator m_Allocator;
	CMP compare;

	void rotateLeft(RBNode* pNode);
	void rotateRight(RBNode* pNode);
	void rebalanceAfterInsert(RBNode* pInsertedNode);
	void rebalanceAfterDelete(RBNode* pParentOfDeletedNode);
	BOOL remove (RBNode * nodeToDelete);

public:


	class iterator
	{
		friend class CRedBlackTree;
	public :
		RBNode* iter;
		iterator(RBNode * p = 0):iter(p){};
		T operator*() const { return iter->data;}
	};

	CRedBlackTree(U32 noElements=8,U32 noGrowElements=8)
	{
		m_Root = 0;
		m_Allocator.Initialize(sizeof(RBNode),noElements,noGrowElements);
		m_NIL = (RBNode*)m_Allocator.Allocate();

		IFXASSERT(m_NIL);

		if( NULL != m_NIL )
		{
			m_NIL->parent = 0;
			m_NIL->right = m_NIL;
			m_NIL->left =  m_NIL;
			m_NIL->color = BLACK;
			m_NIL->data =0;
			m_Root = m_NIL;
		}
	}

	virtual ~CRedBlackTree()
	{
		if(m_Root && m_Root !=  m_NIL)
			deleteNode(m_Root);
		if(m_NIL)
			m_Allocator.Deallocate((U8*)m_NIL);

		m_Root = 0;
		m_NIL = 0;
	}

	void deleteNode(RBNode* n)
	{
		if(n->left && n->left != m_NIL)
			deleteNode(n->left);
		if(n->right && n->right != m_NIL)
			deleteNode(n->right);
		m_Allocator.Deallocate((U8*)n);
	}

	//not const as UnitAllocator does not return a const;
	U32  size() { return m_Allocator.NumAllocatedUnits(); }
	BOOL empty() const
	{
		BOOL isEmpty = TRUE;
		if(m_Root && m_Root != m_NIL)
			isEmpty = FALSE;
		return isEmpty;
	}

	void insert(const T& element);
	BOOL find(const K& data) const ;
	BOOL find(const T& data) const;
	BOOL find(const K& data, iterator& iter) const;

	void clear()
	{
		if(m_Root && m_Root != m_NIL)
			deleteNode(m_Root);
		m_Root = m_NIL;
	}

	RBNode* _find(const T& data) const
	{
		RBNode *current = m_Root;
		I16 comp =0;
		while(current != m_NIL)
		{
			comp = compare(data, current->data);
			if(comp==0)
				return (current);
			else
				current = (comp < 0) ? current->left : current->right;
		}
		return(0);
	}

	BOOL remove(iterator inIter){ return remove(inIter.iter);}
	BOOL remove(T data){ return remove(_find(data));}

	void traverseInOrder(void (*nf)(iterator iter,void* data), void * inData)
	{
		if(m_Root && m_Root != m_NIL)
			inOrderNode(m_Root,nf,inData);
	}

	void inOrderNode(RBNode* n, void (*nf)(iterator, void* data), void * inData)
	{
		if(n->left && n->left != m_NIL)
			inOrderNode(n->left,nf, inData);
		nf(iterator(n), inData);
		if(n->right && n->right != m_NIL)
			inOrderNode(n->right,nf, inData);
	}

	void inorderIterative( void (*process)(iterator iter, void* data),void * inData)
	{
		RBNode* current;
		current = m_Root;
		if(current != m_NIL )
		{
			while( current->left != m_NIL )
				current = current->left;
			while(1)
			{
				process(iterator(current),inData);
				if( current->right != m_NIL )
				{
					current = current->right;
					while( current->left != m_NIL )
						current = current->left;
				}
				else
				{
					if( current != m_Root && current->parent->left == current)
					{
						current = current->parent;
					}
					else
					{
						while( current != m_Root && current->parent->right == current )
							current = current->parent;
						if( current == m_Root )
							return;
						current = current->parent;
					}
				}
			}
		}
	}

#ifdef _DEBUGTREE
	FILE * fp;
	void dump();
	void dumpNode(RBNode* n);
#endif
};

template <class T, class K, class CMP >
void CRedBlackTree<T, K, CMP>::insert(const T& data)
{
	RBNode *pCurrentNode, *pParentNode, *pNewNode;
	// Find to be position of node in the tree
	pCurrentNode = m_Root;
	pParentNode = 0;
	I32 cmp =0;
	while (pCurrentNode != m_NIL)
	{
		cmp = compare(data, pCurrentNode->data);
		if (!cmp) // Data element already exists
			return;
		pParentNode = pCurrentNode;
		pCurrentNode = cmp < 0 ?    pCurrentNode->left : pCurrentNode->right;
	}

	// Alloc & init new node
	pNewNode = (RBNode *)m_Allocator.Allocate();
	IFXASSERT(pNewNode);

	pNewNode->data = data;
	pNewNode->parent = pParentNode;
	pNewNode->left = m_NIL;
	pNewNode->right = m_NIL;
	pNewNode->color = RED;

	// Insert new node in tree
	if(pParentNode)
	{
		cmp = compare(data, pParentNode->data);
		if(cmp < 0)
			pParentNode->left  = pNewNode;
		else
			pParentNode->right = pNewNode;
	}
	else
	{
		// First node being added
		m_Root = pNewNode;
	}

	rebalanceAfterInsert(pNewNode);
}


template <class T, class K, class CMP >
BOOL CRedBlackTree<T,  K, CMP>::remove(RBNode *pInNodeBeingDeleted)
{
	BOOL bFound = TRUE;
	if (pInNodeBeingDeleted && pInNodeBeingDeleted != m_NIL)
	{
		RBNode*x, *y, *z;
		z = pInNodeBeingDeleted;
		if (z->left == m_NIL || z->right == m_NIL)
		{
			// y has a m_NIL node as a child
			y = z;
		}
		else
		{
			// Find tree successor with a m_NIL node as a child
			y = z->right;
			while (y->left != m_NIL)
				y = y->left;
		}

		// x is y's only child
		if (y->left != m_NIL)
			x = y->left;
		else
			x = y->right;

		// Remove y from the parent chain
		x->parent = y->parent;

		if (y->parent)
		{
			if (y == y->parent->left)
				y->parent->left = x;
			else
				y->parent->right = x;
		}
		else
		{
			m_Root = x;
		}

		// Copy data between the y , z  and delete y instead of z
		// to reduce replacing z by y and adjusting parent & child pointers
		if (y != z)
			z->data = y->data;

		// Rebalance the tree if Node is black
		if (y->color == BLACK)
			rebalanceAfterDelete(x);

		m_Allocator.Deallocate((U8*)y);
	}
	else
	{
		bFound = FALSE;
	}

	return bFound;
}


#ifdef _DEBUGTREE
FILE * fp;
template <class T, class K, class CMP >
void CRedBlackTree<T,  K, CMP>::dump()
{
	if(m_Root && m_Root != m_NIL)
	{
		fp = fopen("RBTreeDump.txt","at");
		fprintf(fp,"\n\n Red Black tree dump ... \n Address  %x \n",this);
		m_Root->cnt =1;
		m_Root->Address[0] = 0;
		dump(m_Root);
		fclose(fp);
	}
}

template <class T, class K, class CMP >
void CRedBlackTree<T,  K, CMP>::dump(RBNode* n)
{
	U32 k =0;
	if(n->left && n->left != m_NIL)
	{
		n->left->cnt = n->cnt+1;
		for(k = 0;k<n->cnt;k++)
			n->left->Address[k] = n->Address[k];
		n->left->Address[n->cnt] = 1;
		dump(n->left);
	}
	fprintf(fp,"Node Address ");
	for(k = 0;k<n->cnt;k++)
		fprintf(fp,"%d",n->Address[k]);
	SIFXObserverRequest * tmp = (SIFXObserverRequest *)n->data;
	fprintf(fp," Node Data %x - Color % d \n",tmp->pObserver, n->color);
	if(n->right && n->right != m_NIL)
	{
		n->right->cnt = n->cnt+1;
		for(k = 0;k<n->cnt;k++)
			n->right->Address[k] = n->Address[k];
		n->right->Address[n->cnt] = 2;
		dump(n->right);
	}
}
#endif


template <class T, class K, class CMP >
IFXINLINE BOOL CRedBlackTree<T,  K, CMP>::find(const K& data) const
{
	RBNode *current = m_Root;
	I16 comp =0;
	while(current != m_NIL)
	{
		comp = compare(current->data, data);
		if(comp==0)
			return TRUE;
		else
			current = (comp > 0) ? current->left : current->right;
	}

	return FALSE;
}

template <class T, class K, class CMP >
IFXINLINE BOOL CRedBlackTree<T,  K, CMP>::find(const T& data) const
{
	RBNode *current = m_Root;
	I16 comp =0;
	while(current != m_NIL)
	{
		comp = compare(current->data, data);
		if(comp==0)
			return TRUE;
		else
			current = (comp > 0) ? current->left : current->right;
	}

	return FALSE;
}

template <class T, class K, class CMP >
IFXINLINE BOOL CRedBlackTree<T,  K, CMP>::find(const K& data, iterator& outIter) const
{
	RBNode *current = m_Root;
	I16 comp =0;
	while(current != m_NIL)
	{
		comp = compare(current->data, data);
		if(comp==0)
		{
			outIter = iterator(current);
			return TRUE;
		}
		else
			current = (comp > 0) ? current->left : current->right;
	}

	return FALSE;
}

template <class T, class K, class CMP >
IFXINLINE void CRedBlackTree<T,  K, CMP>::rotateLeft(RBNode* pInNode)
{

	RBNode* pRightChild  = pInNode->right;

	// Establish x->right link
	pInNode->right = pRightChild->left;
	if (pRightChild->left != m_NIL) pRightChild->left->parent = pInNode;

	// Establish y->parent link
	if (pRightChild != m_NIL)
		pRightChild->parent = pInNode->parent;

	if (pInNode->parent)
	{
		if (pInNode == pInNode->parent->left)
			pInNode->parent->left = pRightChild;
		else
			pInNode->parent->right = pRightChild;
	}
	else
	{
		m_Root = pRightChild;
	}

	// link x and y
	pRightChild->left = pInNode;
	if (pInNode != m_NIL) pInNode->parent = pRightChild;
}

template <class T, class K, class CMP >
IFXINLINE void CRedBlackTree<T,  K, CMP>::rotateRight(RBNode* pInNode)
{
	RBNode *pLeftChild = pInNode->left;

	// Establish x->left link
	pInNode->left = pLeftChild->right;
	if (pLeftChild->right != m_NIL) pLeftChild->right->parent = pInNode;

	// Establish y->parent link
	if (pLeftChild != m_NIL) pLeftChild->parent = pInNode->parent;

	if (pInNode->parent)
	{
		if (pInNode == pInNode->parent->right)
			pInNode->parent->right = pLeftChild;
		else
			pInNode->parent->left = pLeftChild;
	}
	else
	{
		m_Root = pLeftChild;
	}

	// Link x and y
	pLeftChild->right = pInNode;

	if (pInNode != m_NIL)
		pInNode->parent = pLeftChild;
}

template <class T, class K, class CMP >
void CRedBlackTree<T,  K, CMP>::rebalanceAfterInsert(RBNode* x)
{
	// Check Red-Black properties
	while (x != m_Root && x->parent->color == RED)
	{
		// we have a violation
		if (x->parent == x->parent->parent->left)
		{
			RBNode*y = x->parent->parent->right;
			if (y->color == RED)
			{

				// uncle is RED
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			}
			else
			{

				// uncle is BLACK
				if (x == x->parent->right)
				{
					// make x a left child
					x = x->parent;
					rotateLeft(x);
				}

				// recolor and rotate
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				rotateRight(x->parent->parent);
			}
		}
		else
		{
			// Same as code above with right & left interchanged .
			RBNode*y = x->parent->parent->left;
			if (y->color == RED)
			{

				// Uncle is RED
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			}
			else
			{

				// Uncle is BLACK
				if (x == x->parent->left)
				{
					x = x->parent;
					rotateRight(x);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				rotateLeft(x->parent->parent);
			}
		}
	}
	m_Root->color = BLACK;
}


template <class T, class K, class CMP >
void CRedBlackTree<T,  K, CMP>::rebalanceAfterDelete(RBNode* x)
{
	while (x && x != m_Root && x->color == BLACK)
	{
		if (x == x->parent->left)
		{
			RBNode*w = x->parent->right;
#ifdef _DEBUGTREE
			if(!w)
			{
				dump();
				return;
			}
#endif
			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rotateLeft (x->parent);
				w = x->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				if (w->right->color == BLACK)
				{
					w->left->color = BLACK;
					w->color = RED;
					rotateRight (w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rotateLeft (x->parent);
				x = m_Root;

			}
		}
		else
		{
			// Same code as above with left and right interchanged
			RBNode*w = x->parent->left;
#ifdef _DEBUGTREE
			if(!w)
			{
				dump();
				return;
			}
#endif

			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rotateRight (x->parent);
				w = x->parent->left;
			}
			if (w->right->color == BLACK && w->left->color == BLACK)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				if (w->left->color == BLACK)
				{
					w->right->color = BLACK;
					w->color = RED;
					rotateLeft (w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rotateRight (x->parent);
				x = m_Root;
			}
		}
	}
	x->color = BLACK;
}


#endif
