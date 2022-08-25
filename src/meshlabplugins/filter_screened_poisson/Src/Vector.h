/*
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#ifndef __VECTOR_HPP
#define __VECTOR_HPP


//#define Assert assert
#include <assert.h>
#include "Array.h"

template< class T >
class Vector
{
public:
	Vector( void );
	Vector( const Vector<T>& V );
	Vector( size_t N );
	Vector( size_t N, ConstPointer( T ) pV );
	~Vector( void );

	const T& operator () (size_t i) const;
	T& operator () (size_t i);
	const T& operator [] (size_t i) const;
	T& operator [] (size_t i);

	void SetZero();

	size_t Dimensions() const;
	void Resize( size_t N );

	Vector operator * (const T& A) const;
	Vector operator / (const T& A) const;
	Vector operator - (const T& A) const;
	Vector operator + (const T& A) const;
	Vector operator - (const Vector& V) const;
	Vector operator + (const Vector& V) const;

	Vector& operator *= ( const T& A );
	Vector& operator /= ( const T& A );
	Vector& operator += ( const T& A );
	Vector& operator -= ( const T& A );
	Vector& operator += ( const Vector& V );
	Vector& operator -= ( const Vector& V );

	Vector& Add( const Vector* V , int count );
	Vector& AddScaled( const Vector& V , const T& scale );
	Vector& SubtractScaled( const Vector& V , const T& scale );
	static void Add( const Vector& V1 , const T& scale1 , const Vector& V2 , const T& scale2 , Vector& Out );
	static void Add( const Vector& V1 , const T& scale1 , const Vector& V2 , Vector& Out );

	Vector operator - () const;

	Vector& operator = (const Vector& V);

	T Dot( const Vector& V ) const;

	T Length() const;

	T Average() const;

	T Norm( size_t Ln ) const;
	void Normalize();

	bool write( FILE* fp ) const;
	bool write( const char* fileName ) const;
	bool read( FILE* fp );
	bool read( const char* fileName );

	Pointer( T ) m_pV;
protected:
	size_t m_N;

};

#if ARRAY_DEBUG
template< class C >      Array< C > GetPointer(       Vector< C >& v ){ return      Array< C >::FromPointer( &v[0] , v.Dimensions() ); }
template< class C > ConstArray< C > GetPointer( const Vector< C >& v ){ return ConstArray< C >::FromPointer( &v[0] , v.Dimensions() ); }
#else // !ARRAY_DEBUG
template< class C >       C* GetPointer(       Vector< C >& v ){ return &v[0]; }
template< class C > const C* GetPointer( const Vector< C >& v ){ return &v[0]; }
#endif // ARRAY_DEBUG

#include "Vector.inl"

#endif
