/**
	@file	FNVPlusPlus.h

	This header contains the Fowler/Noll/Vo- hash, specifically the 32-bit 
	FNV-1a version.

	@note	This header was created by starting with the original source 
			code file called fnv.h and inserting hash_32a.c.  This was done 
			to allow the hash function to be inlined.  For details, refer to 
			the source code since the original comments have been retained 
			(they haven't been @e doxygenized) or refer to 
			http://www.isthe.com/chongo/tech/comp/fnv/index.html where the 
			source was obtained.

	@note	The few changes made to the original source code are usually 
			identified by comments prefixed with FNVMOD.
*/

/*
 * fnv - Fowler/Noll/Vo- hash code
 *
 * @(#) $Revision: 1.5 $
 * @(#) $Id: fnv.h,v 1.5 2003/10/03 20:35:52 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/fnv/RCS/fnv.h,v $
 *
 ***
 *
 * Fowler/Noll/Vo- hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 *
 ***
 *
 * NOTE: The FNV-0 historic hash is not recommended.  One should use
 *	 the FNV-1 hash instead.
 *
 * To use the 32 bit FNV-0 historic hash, pass FNV0_32_INIT as the
 * Fnv32_t hashval argument to fnv_32_buf() or fnv_32_str().
 *
 * To use the 64 bit FNV-0 historic hash, pass FNV0_64_INIT as the
 * Fnv64_t hashval argument to fnv_64_buf() or fnv_64_str().
 *
 * To use the recommended 32 bit FNV-1 hash, pass FNV1_32_INIT as the
 * Fnv32_t hashval argument to fnv_32_buf() or fnv_32_str().
 *
 * To use the recommended 64 bit FNV-1 hash, pass FNV1_64_INIT as the
 * Fnv64_t hashval argument to fnv_64_buf() or fnv_64_str().
 *
 * To use the recommended 32 bit FNV-1a hash, pass FNV1_32A_INIT as the
 * Fnv32_t hashval argument to fnv_32a_buf() or fnv_32a_str().
 *
 * To use the recommended 64 bit FNV-1a hash, pass FNV1_64A_INIT as the
 * Fnv64_t hashval argument to fnv_64a_buf() or fnv_64a_str().
 *
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *	chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!	:-)
 */

	
#if !defined(__FNV_H__)
#define __FNV_H__


/*
 * 32 bit FNV-0 hash type
 */
typedef unsigned long Fnv32_t;


/*
 * 32 bit FNV-0 zero initial basis
 *
 * This historic hash is not recommended.  One should use
 * the FNV-1 hash and initial basis instead.
 */
#define FNV0_32_INIT ((Fnv32_t)0)


/*
 * 32 bit FNV-1 and FNV-1a non-zero initial basis
 *
 * The FNV-1 initial basis is the FNV-0 hash of the following 32 octets:
 *
 *              chongo <Landon Curt Noll> /\../\
 *
 * NOTE: The \'s above are not back-slashing escape characters.
 * They are literal ASCII  backslash 0x5c characters.
 *
 * NOTE: The FNV-1a initial basis is the same value as FNV-1 by definition.
 */
#define FNV1_32_INIT ((Fnv32_t)0x811c9dc5)
#define FNV1_32A_INIT FNV1_32_INIT


// FNVMOD:  This isn't currently needed.
#if 0
	/*
	* determine how 64 bit unsigned values are represented
	*/
	#include "longlong.h"


	/*
	* 64 bit FNV-0 hash
	*/
	#if defined(HAVE_64BIT_LONG_LONG)
	typedef unsigned long long Fnv64_t;
	#else
	typedef struct {
		unsigned long w32[2];
	} Fnv64_t;
	#endif


	/*
	* 64 bit FNV-0 zero initial basis
	*
	* This historic hash is not recommended.  One should use
	* the FNV-1 hash and initial basis instead.
	*/
	#if defined(HAVE_64BIT_LONG_LONG)
	#define FNV0_64_INIT ((Fnv64_t)0)
	#else
	extern const Fnv64_t fnv1_64_init;
	#define FNV0_64_INIT (fnv1_64_init)
	#endif


	/*
	* 64 bit FNV-1 non-zero initial basis
	*
	* The FNV-1 initial basis is the FNV-0 hash of the following 32 octets:
	*
	*              chongo <Landon Curt Noll> /\../\
	*
	* NOTE: The \'s above are not back-slashing escape characters.
	* They are literal ASCII  backslash 0x5c characters.
	*
	* NOTE: The FNV-1a initial basis is the same value as FNV-1 by definition.
	*/
	#if defined(HAVE_64BIT_LONG_LONG)
	#define FNV1_64_INIT ((Fnv64_t)0xcbf29ce484222325ULL)
	#define FNV1_64A_INIT FNV1_64_INIT
	#else
	extern const Fnv64_t fnv1_64_init;
	extern const Fnv64_t fnv1_64a_init;
	#define FNV1_64_INIT (fnv1_64_init)
	#define FNV1_64A_INIT FNV1_64_INIT
	#endif
#endif

// FNVMOD:  Inserted hash_32a.c to facilitate inlining.

/*
 * hash_32 - 32 bit Fowler/Noll/Vo FNV-1a hash code
 *
 * @(#) $Revision: 1.1 $
 * @(#) $Id: hash_32a.c,v 1.1 2003/10/03 20:38:53 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/fnv/RCS/hash_32a.c,v $
 *
 ***
 *
 * Fowler/Noll/Vo hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 ***
 *
 * To use the recommended 32 bit FNV-1a hash, pass FNV1_32A_INIT as the
 * Fnv32_t hashval argument to fnv_32a_buf() or fnv_32a_str().
 *
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *	chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!	:-)
 */

#include <stdlib.h>
// FNVMOD:  Changed necessary includes.
#if 1
	#include "IFXDataTypes.h"
#else
	#include "fnv.h"
#endif

// FNVMOD:  Added possible define.
#if 1
///	@todo	Disable this define if GCC is used.
#define	NO_FNV_GCC_OPTIMIZATION	TRUE
#endif

/*
 * 32 bit magic FNV-1a prime
 */
#define FNV_32_PRIME ((Fnv32_t)0x01000193)


// FNVMOD:  This isn't needed.
#if 0
	/*
	* fnv_32a_buf - perform a 32 bit Fowler/Noll/Vo FNV-1a hash on a buffer
	*
	* input:
	*	buf	- start of buffer to hash
	*	len	- length of buffer in octets
	*	hval	- previous hash value or 0 if first call
	*
	* returns:
	*	32 bit hash as a static hash type
	*
	* NOTE: To use the recommended 32 bit FNV-1a hash, use FNV1_32A_INIT as the
	* 	 hval arg on the first call to either fnv_32a_buf() or fnv_32a_str().
	*/
	Fnv32_t
	fnv_32a_buf(void *buf, size_t len, Fnv32_t hval)
	{
		unsigned char *bp = (unsigned char *)buf;	/* start of buffer */
		unsigned char *be = bp + len;		/* beyond end of buffer */

		/*
		* FNV-1a hash each octet in the buffer
		*/
		while (bp < be) {

		/* xor the bottom with the current octet */
		hval ^= (Fnv32_t)*bp++;

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
		#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV_32_PRIME;
		#else
		hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		#endif
		}

		/* return our new hash value */
		return hval;
	}
#endif

/*
 * fnv_32a_str_with_hash_size - perform a 32 bit Fowler/Noll/Vo FNV-1a hash 
 * on a string to generated an integer hash value in the range [0,hashcount).
 *
 * input:
 *	str		- string to hash
 *	hval	- previous hash value or 0 if first call
 *  hcount	- total number of hash values to generate
 * returns:
 *	32 bit hash as a static hash type
 *
 * NOTE: To use the recommended 32 bit FNV-1a hash, use FNV1_32A_INIT as the
 *  	 hval arg on the first call to either fnv_32a_buf() or fnv_32a_str().
 */
// FNVMOD:  Made inline, changed name and added functionality.
#if 1
IFXFORCEINLINE	
Fnv32_t	fnv_32_str_with_hash_size(
			char*		str,
			Fnv32_t		hval,
			Fnv32_t		hcount )
#else
Fnv32_t
fnv_32a_str(char *str, Fnv32_t hval)
#endif
{
	// FNVMOD:  Made sure a valid hashcount is specified.
	#if 1
	if ( hcount ) {
	#endif

		unsigned char *s = (unsigned char *)str;	/* unsigned string */

		/*
		* FNV-1a hash each octet in the buffer
		*/
		while (*s) {

		/* xor the bottom with the current octet */
		hval ^= (Fnv32_t)*s++;

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
		#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV_32_PRIME;
		#else
		hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		#endif
		}

		// FNVMOD:  Perform a lazy mod to constrain the hash value generated 
		// to the desired count of hash values.
		#if 1
		hval %= hcount;
		#endif

	// FNVMOD:  End block.
	#if 1
	}
	#endif

    /* return our new hash value */
    return hval;
}

// FNVMOD:  These aren't needed.
#if 0
	/*
	* external functions
	*/
	extern Fnv32_t fnv_32_buf(void *buf, size_t len, Fnv32_t hashval);
	extern Fnv32_t fnv_32_str(char *buf, Fnv32_t hashval);
	extern Fnv64_t fnv_64_buf(void *buf, size_t len, Fnv64_t hashval);
	extern Fnv64_t fnv_64_str(char *buf, Fnv64_t hashval);
	extern Fnv32_t fnv_32a_buf(void *buf, size_t len, Fnv32_t hashval);
	extern Fnv32_t fnv_32a_str(char *buf, Fnv32_t hashval);
	extern Fnv64_t fnv_64a_buf(void *buf, size_t len, Fnv64_t hashval);
	extern Fnv64_t fnv_64a_str(char *buf, Fnv64_t hashval);
#endif


#endif /* __FNV_H__ */


