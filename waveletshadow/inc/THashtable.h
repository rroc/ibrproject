#pragma once

#include "basic.h"
#include <hash_map>

#include <stdio.h>      /* defines printf for tests */
#include <time.h>       /* defines time_t for timings in the test */
#include "stdint.h"     /* defines uint32_t etc */
#include "TVector3.h"

//
//hash_map::begin
//hash_map::clear
//hash_map::count
//hash_map::empty
//hash_map::end
//hash_map::equal_range
//hash_map::erase
//hash_map::find
//hash_map::get_allocator
//hash_map::hash_map
//hash_map::insert
//hash_map::key_compare
//hash_map::lower_bound
//hash_map::max_size
//hash_map::rbegin
//hash_map::rend
//hash_map::size
//hash_map::swap
//hash_map::upper_bound
//hash_map::value_comp

/*
//EXAMPLE USAGE:
//-------------------------
THashTable hash;

TSquare key1={1,1,1};
float value1=0.5f;
pair<TSquare, float> item = make_pair( key1, value1);
hash.insert( item );

TSquare key2={5,4,3};
float value2=123.56f;
pair<TSquare, float> item2 = make_pair( key2, value2);
hash.insert( item2 );

TSquare key3={10,15,21};
float value3=8.12878784132;
pair<TSquare, float> item3 = make_pair( key3, value3);
hash.insert( item3 );

TSquare x={10,15,21};
THashTable::iterator it = hash.find( x );

if ( it == hash.end( ) )
{
cout << "The hash_map doesn't have an element ";
}
else
{
float c = (*it).second;
TSquare d = (*it).first;
std::cout << "[" << d.l << d.x << d.y << "] = " << c << std::endl;
}
//-------------------------
*/



#define HASH_LITTLE_ENDIAN 1
#define HASH_BIG_ENDIAN 0


#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

class TSquareHasher : public stdext::hash_compare <TSquare>
	{
	public:

		//hash operator
		size_t operator() (const TSquare& s) const
			{
			return hashlittle( (void*)&s, sizeof(TSquare), 0x12345678 );
			}

		//compare hash
		bool operator() (const TSquare& s1, const TSquare& s2) const
			{
			return ( (s1.x > s2.x) || (s1.y > s2.y) );
			}

//------------------------------------------------------------
// The following Extracted from 
// lookup3.c, by Bob Jenkins, May 2006, Public Domain.
//------------------------------------------------------------
		/*
		-------------------------------------------------------------------------------
		mix -- mix 3 32-bit values reversibly.

		This is reversible, so any information in (a,b,c) before mix() is
		still in (a,b,c) after mix().

		If four pairs of (a,b,c) inputs are run through mix(), or through
		mix() in reverse, there are at least 32 bits of the output that
		are sometimes the same for one pair and different for another pair.
		This was tested for:
		* pairs that differed by one bit, by two bits, in any combination
		of top bits of (a,b,c), or in any combination of bottom bits of
		(a,b,c).
		* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
		the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
		is commonly produced by subtraction) look like a single 1-bit
		difference.
		* the base values were pseudorandom, all zero but one bit set, or 
		all zero plus a counter that starts at zero.

		Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
		satisfy this are
		4  6  8 16 19  4
		9 15  3 18 27 15
		14  9  3  7 17  3
		Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
		for "differ" defined as + with a one-bit base and a two-bit delta.  I
		used http://burtleburtle.net/bob/hash/avalanche.html to choose 
		the operations, constants, and arrangements of the variables.

		This does not achieve avalanche.  There are input bits of (a,b,c)
		that fail to affect some output bits of (a,b,c), especially of a.  The
		most thoroughly mixed value is c, but it doesn't really even achieve
		avalanche in c.

		This allows some parallelism.  Read-after-writes are good at doubling
		the number of bits affected, so the goal of mixing pulls in the opposite
		direction as the goal of parallelism.  I did what I could.  Rotates
		seem to cost as much as shifts on every machine I could lay my hands
		on, and rotates are much kinder to the top and bottom bits, so I used
		rotates.
		-------------------------------------------------------------------------------
		*/
#define mix(a,b,c) \
			{ \
			a -= c;  a ^= rot(c, 4);  c += b; \
			b -= a;  b ^= rot(a, 6);  a += c; \
			c -= b;  c ^= rot(b, 8);  b += a; \
			a -= c;  a ^= rot(c,16);  c += b; \
			b -= a;  b ^= rot(a,19);  a += c; \
			c -= b;  c ^= rot(b, 4);  b += a; \
				}

		/*
		-------------------------------------------------------------------------------
		final -- final mixing of 3 32-bit values (a,b,c) into c

		Pairs of (a,b,c) values differing in only a few bits will usually
		produce values of c that look totally different.  This was tested for
		* pairs that differed by one bit, by two bits, in any combination
		of top bits of (a,b,c), or in any combination of bottom bits of
		(a,b,c).
		* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
		the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
		is commonly produced by subtraction) look like a single 1-bit
		difference.
		* the base values were pseudorandom, all zero but one bit set, or 
		all zero plus a counter that starts at zero.

		These constants passed:
		14 11 25 16 4 14 24
		12 14 25 16 4 14 24
		and these came close:
		4  8 15 26 3 22 24
		10  8 15 26 3 22 24
		11  8 15 26 3 22 24
		-------------------------------------------------------------------------------
		*/
#define final(a,b,c) \
			{ \
			c ^= b; c -= rot(b,14); \
			a ^= c; a -= rot(c,11); \
			b ^= a; b -= rot(a,25); \
			c ^= b; c -= rot(b,16); \
			a ^= c; a -= rot(c,4);  \
			b ^= a; b -= rot(a,14); \
			c ^= b; c -= rot(b,24); \
				}

		/*
		-------------------------------------------------------------------------------
		hashlittle() -- hash a variable-length key into a 32-bit value
		k       : the key (the unaligned variable-length array of bytes)
		length  : the length of the key, counting by bytes
		initval : can be any 4-byte value
		Returns a 32-bit value.  Every bit of the key affects every bit of
		the return value.  Two keys differing by one or two bits will have
		totally different hash values.

		The best hash table sizes are powers of 2.  There is no need to do
		mod a prime (mod is sooo slow!).  If you need less than 32 bits,
		use a bitmask.  For example, if you need only 10 bits, do
		h = (h & hashmask(10));
		In which case, the hash table should have hashsize(10) elements.

		If you are hashing n strings (uint8_t **)k, do it like this:
		for (i=0, h=0; i<n; ++i) h = hashlittle( k[i], len[i], h);

		By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
		code any way you wish, private, educational, or commercial.  It's free.

		Use for hash table lookup, or anything where one collision in 2^^32 is
		acceptable.  Do NOT use for cryptographic purposes.
		-------------------------------------------------------------------------------
		*/

		uint32_t hashlittle( const void *key, size_t length, uint32_t initval) const
			{
			uint32_t a,b,c;                                          /* internal state */
			union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */

			/* Set up the internal state */
			a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;

			u.ptr = key;
			if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) {
				const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
				const uint8_t  *k8;

				/*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
				while (length > 12)
					{
					a += k[0];
					b += k[1];
					c += k[2];
					mix(a,b,c);
					length -= 12;
					k += 3;
					}

				/*----------------------------- handle the last (probably partial) block */
				/* 
				* "k[2]&0xffffff" actually reads beyond the end of the string, but
				* then masks off the part it's not allowed to read.  Because the
				* string is aligned, the masked-off tail is in the same word as the
				* rest of the string.  Every machine with memory protection I've seen
				* does it on word boundaries, so is OK with this.  But VALGRIND will
				* still catch it and complain.  The masking trick does make the hash
				* noticably faster for short strings (like English words).
				*/
#ifndef VALGRIND

				switch(length)
					{
					case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
					case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
					case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
					case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
					case 8 : b+=k[1]; a+=k[0]; break;
					case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
					case 6 : b+=k[1]&0xffff; a+=k[0]; break;
					case 5 : b+=k[1]&0xff; a+=k[0]; break;
					case 4 : a+=k[0]; break;
					case 3 : a+=k[0]&0xffffff; break;
					case 2 : a+=k[0]&0xffff; break;
					case 1 : a+=k[0]&0xff; break;
					case 0 : return c;              /* zero length strings require no mixing */
					}

#else /* make valgrind happy */

				k8 = (const uint8_t *)k;
				switch(length)
					{
					case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
					case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
					case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
					case 9 : c+=k8[8];                   /* fall through */
					case 8 : b+=k[1]; a+=k[0]; break;
					case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
					case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
					case 5 : b+=k8[4];                   /* fall through */
					case 4 : a+=k[0]; break;
					case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
					case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
					case 1 : a+=k8[0]; break;
					case 0 : return c;
					}

#endif /* !valgrind */

				} else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
					const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
					const uint8_t  *k8;

					/*--------------- all but last block: aligned reads and different mixing */
					while (length > 12)
						{
						a += k[0] + (((uint32_t)k[1])<<16);
						b += k[2] + (((uint32_t)k[3])<<16);
						c += k[4] + (((uint32_t)k[5])<<16);
						mix(a,b,c);
						length -= 12;
						k += 6;
						}

					/*----------------------------- handle the last (probably partial) block */
					k8 = (const uint8_t *)k;
					switch(length)
						{
						case 12: c+=k[4]+(((uint32_t)k[5])<<16);
							b+=k[2]+(((uint32_t)k[3])<<16);
							a+=k[0]+(((uint32_t)k[1])<<16);
							break;
						case 11: c+=((uint32_t)k8[10])<<16;     /* fall through */
						case 10: c+=k[4];
							b+=k[2]+(((uint32_t)k[3])<<16);
							a+=k[0]+(((uint32_t)k[1])<<16);
							break;
						case 9 : c+=k8[8];                      /* fall through */
						case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
							a+=k[0]+(((uint32_t)k[1])<<16);
							break;
						case 7 : b+=((uint32_t)k8[6])<<16;      /* fall through */
						case 6 : b+=k[2];
							a+=k[0]+(((uint32_t)k[1])<<16);
							break;
						case 5 : b+=k8[4];                      /* fall through */
						case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
							break;
						case 3 : a+=((uint32_t)k8[2])<<16;      /* fall through */
						case 2 : a+=k[0];
							break;
						case 1 : a+=k8[0];
							break;
						case 0 : return c;                     /* zero length requires no mixing */
						}

				} else {                        /* need to read the key one byte at a time */
					const uint8_t *k = (const uint8_t *)key;

					/*--------------- all but the last block: affect some 32 bits of (a,b,c) */
					while (length > 12)
						{
						a += k[0];
						a += ((uint32_t)k[1])<<8;
						a += ((uint32_t)k[2])<<16;
						a += ((uint32_t)k[3])<<24;
						b += k[4];
						b += ((uint32_t)k[5])<<8;
						b += ((uint32_t)k[6])<<16;
						b += ((uint32_t)k[7])<<24;
						c += k[8];
						c += ((uint32_t)k[9])<<8;
						c += ((uint32_t)k[10])<<16;
						c += ((uint32_t)k[11])<<24;
						mix(a,b,c);
						length -= 12;
						k += 12;
						}

					/*-------------------------------- last block: affect all 32 bits of (c) */
					switch(length)                   /* all the case statements fall through */
						{
						case 12: c+=((uint32_t)k[11])<<24;
						case 11: c+=((uint32_t)k[10])<<16;
						case 10: c+=((uint32_t)k[9])<<8;
						case 9 : c+=k[8];
						case 8 : b+=((uint32_t)k[7])<<24;
						case 7 : b+=((uint32_t)k[6])<<16;
						case 6 : b+=((uint32_t)k[5])<<8;
						case 5 : b+=k[4];
						case 4 : a+=((uint32_t)k[3])<<24;
						case 3 : a+=((uint32_t)k[2])<<16;
						case 2 : a+=((uint32_t)k[1])<<8;
						case 1 : a+=k[0];
							break;
						case 0 : return c;
						}
					}

				final(a,b,c);
				return c;
			}
	};

	class TIntHasher : public stdext::hash_compare <int>
		{
		public:

			//hash operator
			size_t operator() (const int& s) const
				{
				return hashlittle( (void*)&s, sizeof(int), 0x12345678 );
				}

			//compare hash
			bool operator() (const int& s1, const int& s2) const
				{
				return (s1 > s2);
				}

			//------------------------------------------------------------
			// The following Extracted from 
			// lookup3.c, by Bob Jenkins, May 2006, Public Domain.
			//------------------------------------------------------------
			/*
			-------------------------------------------------------------------------------
			mix -- mix 3 32-bit values reversibly.

			This is reversible, so any information in (a,b,c) before mix() is
			still in (a,b,c) after mix().

			If four pairs of (a,b,c) inputs are run through mix(), or through
			mix() in reverse, there are at least 32 bits of the output that
			are sometimes the same for one pair and different for another pair.
			This was tested for:
			* pairs that differed by one bit, by two bits, in any combination
			of top bits of (a,b,c), or in any combination of bottom bits of
			(a,b,c).
			* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
			the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
			is commonly produced by subtraction) look like a single 1-bit
			difference.
			* the base values were pseudorandom, all zero but one bit set, or 
			all zero plus a counter that starts at zero.

			Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
			satisfy this are
			4  6  8 16 19  4
			9 15  3 18 27 15
			14  9  3  7 17  3
			Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
			for "differ" defined as + with a one-bit base and a two-bit delta.  I
			used http://burtleburtle.net/bob/hash/avalanche.html to choose 
			the operations, constants, and arrangements of the variables.

			This does not achieve avalanche.  There are input bits of (a,b,c)
			that fail to affect some output bits of (a,b,c), especially of a.  The
			most thoroughly mixed value is c, but it doesn't really even achieve
			avalanche in c.

			This allows some parallelism.  Read-after-writes are good at doubling
			the number of bits affected, so the goal of mixing pulls in the opposite
			direction as the goal of parallelism.  I did what I could.  Rotates
			seem to cost as much as shifts on every machine I could lay my hands
			on, and rotates are much kinder to the top and bottom bits, so I used
			rotates.
			-------------------------------------------------------------------------------
			*/
#define mix(a,b,c) \
				{ \
				a -= c;  a ^= rot(c, 4);  c += b; \
				b -= a;  b ^= rot(a, 6);  a += c; \
				c -= b;  c ^= rot(b, 8);  b += a; \
				a -= c;  a ^= rot(c,16);  c += b; \
				b -= a;  b ^= rot(a,19);  a += c; \
				c -= b;  c ^= rot(b, 4);  b += a; \
					}

			/*
			-------------------------------------------------------------------------------
			final -- final mixing of 3 32-bit values (a,b,c) into c

			Pairs of (a,b,c) values differing in only a few bits will usually
			produce values of c that look totally different.  This was tested for
			* pairs that differed by one bit, by two bits, in any combination
			of top bits of (a,b,c), or in any combination of bottom bits of
			(a,b,c).
			* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
			the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
			is commonly produced by subtraction) look like a single 1-bit
			difference.
			* the base values were pseudorandom, all zero but one bit set, or 
			all zero plus a counter that starts at zero.

			These constants passed:
			14 11 25 16 4 14 24
			12 14 25 16 4 14 24
			and these came close:
			4  8 15 26 3 22 24
			10  8 15 26 3 22 24
			11  8 15 26 3 22 24
			-------------------------------------------------------------------------------
			*/
#define final(a,b,c) \
				{ \
				c ^= b; c -= rot(b,14); \
				a ^= c; a -= rot(c,11); \
				b ^= a; b -= rot(a,25); \
				c ^= b; c -= rot(b,16); \
				a ^= c; a -= rot(c,4);  \
				b ^= a; b -= rot(a,14); \
				c ^= b; c -= rot(b,24); \
					}

			/*
			-------------------------------------------------------------------------------
			hashlittle() -- hash a variable-length key into a 32-bit value
			k       : the key (the unaligned variable-length array of bytes)
			length  : the length of the key, counting by bytes
			initval : can be any 4-byte value
			Returns a 32-bit value.  Every bit of the key affects every bit of
			the return value.  Two keys differing by one or two bits will have
			totally different hash values.

			The best hash table sizes are powers of 2.  There is no need to do
			mod a prime (mod is sooo slow!).  If you need less than 32 bits,
			use a bitmask.  For example, if you need only 10 bits, do
			h = (h & hashmask(10));
			In which case, the hash table should have hashsize(10) elements.

			If you are hashing n strings (uint8_t **)k, do it like this:
			for (i=0, h=0; i<n; ++i) h = hashlittle( k[i], len[i], h);

			By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
			code any way you wish, private, educational, or commercial.  It's free.

			Use for hash table lookup, or anything where one collision in 2^^32 is
			acceptable.  Do NOT use for cryptographic purposes.
			-------------------------------------------------------------------------------
			*/

			uint32_t hashlittle( const void *key, size_t length, uint32_t initval) const
				{
				uint32_t a,b,c;                                          /* internal state */
				union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */

				/* Set up the internal state */
				a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;

				u.ptr = key;
				if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) {
					const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
					const uint8_t  *k8;

					/*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
					while (length > 12)
						{
						a += k[0];
						b += k[1];
						c += k[2];
						mix(a,b,c);
						length -= 12;
						k += 3;
						}

					/*----------------------------- handle the last (probably partial) block */
					/* 
					* "k[2]&0xffffff" actually reads beyond the end of the string, but
					* then masks off the part it's not allowed to read.  Because the
					* string is aligned, the masked-off tail is in the same word as the
					* rest of the string.  Every machine with memory protection I've seen
					* does it on word boundaries, so is OK with this.  But VALGRIND will
					* still catch it and complain.  The masking trick does make the hash
					* noticably faster for short strings (like English words).
					*/
#ifndef VALGRIND

					switch(length)
						{
						case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
						case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
						case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
						case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
						case 8 : b+=k[1]; a+=k[0]; break;
						case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
						case 6 : b+=k[1]&0xffff; a+=k[0]; break;
						case 5 : b+=k[1]&0xff; a+=k[0]; break;
						case 4 : a+=k[0]; break;
						case 3 : a+=k[0]&0xffffff; break;
						case 2 : a+=k[0]&0xffff; break;
						case 1 : a+=k[0]&0xff; break;
						case 0 : return c;              /* zero length strings require no mixing */
						}

#else /* make valgrind happy */

					k8 = (const uint8_t *)k;
					switch(length)
						{
						case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
						case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
						case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
						case 9 : c+=k8[8];                   /* fall through */
						case 8 : b+=k[1]; a+=k[0]; break;
						case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
						case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
						case 5 : b+=k8[4];                   /* fall through */
						case 4 : a+=k[0]; break;
						case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
						case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
						case 1 : a+=k8[0]; break;
						case 0 : return c;
						}

#endif /* !valgrind */

					} else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
						const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
						const uint8_t  *k8;

						/*--------------- all but last block: aligned reads and different mixing */
						while (length > 12)
							{
							a += k[0] + (((uint32_t)k[1])<<16);
							b += k[2] + (((uint32_t)k[3])<<16);
							c += k[4] + (((uint32_t)k[5])<<16);
							mix(a,b,c);
							length -= 12;
							k += 6;
							}

						/*----------------------------- handle the last (probably partial) block */
						k8 = (const uint8_t *)k;
						switch(length)
							{
							case 12: c+=k[4]+(((uint32_t)k[5])<<16);
								b+=k[2]+(((uint32_t)k[3])<<16);
								a+=k[0]+(((uint32_t)k[1])<<16);
								break;
							case 11: c+=((uint32_t)k8[10])<<16;     /* fall through */
							case 10: c+=k[4];
								b+=k[2]+(((uint32_t)k[3])<<16);
								a+=k[0]+(((uint32_t)k[1])<<16);
								break;
							case 9 : c+=k8[8];                      /* fall through */
							case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
								a+=k[0]+(((uint32_t)k[1])<<16);
								break;
							case 7 : b+=((uint32_t)k8[6])<<16;      /* fall through */
							case 6 : b+=k[2];
								a+=k[0]+(((uint32_t)k[1])<<16);
								break;
							case 5 : b+=k8[4];                      /* fall through */
							case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
								break;
							case 3 : a+=((uint32_t)k8[2])<<16;      /* fall through */
							case 2 : a+=k[0];
								break;
							case 1 : a+=k8[0];
								break;
							case 0 : return c;                     /* zero length requires no mixing */
							}

					} else {                        /* need to read the key one byte at a time */
						const uint8_t *k = (const uint8_t *)key;

						/*--------------- all but the last block: affect some 32 bits of (a,b,c) */
						while (length > 12)
							{
							a += k[0];
							a += ((uint32_t)k[1])<<8;
							a += ((uint32_t)k[2])<<16;
							a += ((uint32_t)k[3])<<24;
							b += k[4];
							b += ((uint32_t)k[5])<<8;
							b += ((uint32_t)k[6])<<16;
							b += ((uint32_t)k[7])<<24;
							c += k[8];
							c += ((uint32_t)k[9])<<8;
							c += ((uint32_t)k[10])<<16;
							c += ((uint32_t)k[11])<<24;
							mix(a,b,c);
							length -= 12;
							k += 12;
							}

						/*-------------------------------- last block: affect all 32 bits of (c) */
						switch(length)                   /* all the case statements fall through */
							{
							case 12: c+=((uint32_t)k[11])<<24;
							case 11: c+=((uint32_t)k[10])<<16;
							case 10: c+=((uint32_t)k[9])<<8;
							case 9 : c+=k[8];
							case 8 : b+=((uint32_t)k[7])<<24;
							case 7 : b+=((uint32_t)k[6])<<16;
							case 6 : b+=((uint32_t)k[5])<<8;
							case 5 : b+=k[4];
							case 4 : a+=((uint32_t)k[3])<<24;
							case 3 : a+=((uint32_t)k[2])<<16;
							case 2 : a+=((uint32_t)k[1])<<8;
							case 1 : a+=k[0];
								break;
							case 0 : return c;
							}
						}

					final(a,b,c);
					return c;
				}
		};



typedef stdext::hash_map<TSquare, float, TSquareHasher> TSquareHashTable;
typedef stdext::hash_map<int, float, TIntHasher> TIntHashTable;
typedef stdext::hash_map<int, TVector3,TIntHasher> TIntColorHashTable;