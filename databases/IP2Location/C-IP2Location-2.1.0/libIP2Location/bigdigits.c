/* $Id: bigdigits.c,v 1.1 2006/10/21 17:19:15 peter Exp $ */

/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.1 originally written by David Ireland,
copyright (c) 2001-6 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
	Last updated:
	$Date: 2006/10/21 17:19:15 $
	$Revision: 1.1 $
	$Author: peter $
*/

/* Core code for BigDigits library "mp functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include "bigdigits.h"

/* Version numbers - added in ver 2.0.2 */
static const int kMajor = 2, kMinor = 1, kRelease = 0;
#ifdef USE_SPASM
static const int kUseSpasm = 1;
#else
static const int kUseSpasm = 0;
#endif

/* Useful definitions */
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

/* mpAlloc and related functions... */

/* Define alternative error messages if we run out of memory. 
   Hopefully we'll never see these. Change to suit. 
*/
#if defined(_WIN32) || defined(WIN32)
/* Win32 alternative */
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void mpFail(char *msg)
{
	MessageBox(NULL, msg, "BigDigits Error", MB_ICONERROR);
	exit(EXIT_FAILURE);
}
#else	/* Ordinary console program */
void mpFail(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
#endif /* _WIN32 */

DIGIT_T *mpAlloc(size_t ndigits)
{
	DIGIT_T *ptr;

	ptr = (DIGIT_T *)calloc(ndigits, sizeof(DIGIT_T));
	if (!ptr)
		mpFail("mpAlloc: Unable to allocate memory.");

	return ptr;
}

void mpFree(DIGIT_T **p)
{
	if (*p)
	{
		free(*p);
		*p = NULL;
	}
}

/* Force linker to include copyright notice in executable object image */

char *copyright_notice(void)
{
	return 
"Contains multiple-precision arithmetic code originally written by David Ireland,"
" copyright (c) 2001-6 by D.I. Management Services Pty Limited <www.di-mgt.com.au>,"
" and is used with permission.";
}

/* To use, include this statement somewhere in the final code:

	copyright_notice();	
	
It has no real effect at run time. 
Thanks to Phil Zimmerman for this idea.
*/

/* Version Info - added in ver 2.0.2 */
int mpVersion(void)
{
	return (kMajor * 1000 + kMinor * 100 + kRelease * 10 + kUseSpasm);
}

DIGIT_T mpAdd(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], 
			   size_t ndigits)
{
	/*	Calculates w = u + v
		where w, u, v are multiprecision integers of ndigits each
		Returns carry if overflow. Carry = 0 or 1.

		Ref: Knuth Vol 2 Ch 4.3.1 p 266 Algorithm A.
	*/

	DIGIT_T k;
	size_t j;

	assert(w != v);

	/* Step A1. Initialise */
	k = 0;

	for (j = 0; j < ndigits; j++)
	{
		/*	Step A2. Add digits w_j = (u_j + v_j + k)
			Set k = 1 if carry (overflow) occurs
		*/
		w[j] = u[j] + k;
		if (w[j] < k)
			k = 1;
		else
			k = 0;
		
		w[j] += v[j];
		if (w[j] < v[j])
			k++;

	}	/* Step A3. Loop on j */

	return k;	/* w_n = k */
}

DIGIT_T mpSubtract(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], 
			   size_t ndigits)
{
	/*	Calculates w = u - v where u >= v
		w, u, v are multiprecision integers of ndigits each
		Returns 0 if OK, or 1 if v > u.

		Ref: Knuth Vol 2 Ch 4.3.1 p 267 Algorithm S.
	*/

	DIGIT_T k;
	size_t j;

	assert(w != v);

	/* Step S1. Initialise */
	k = 0;

	for (j = 0; j < ndigits; j++)
	{
		/*	Step S2. Subtract digits w_j = (u_j - v_k - k)
			Set k = 1 if borrow occurs.
		*/
		w[j] = u[j] - k;
		if (w[j] > MAX_DIGIT - k)
			k = 1;
		else
			k = 0;
		
		w[j] -= v[j];
		if (w[j] > MAX_DIGIT - v[j])
			k++;

	}	/* Step S3. Loop on j */

	return k;	/* Should be zero if u >= v */
}

int mpMultiply(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], 
					size_t ndigits)
{
	/*	Computes product w = u * v
		where u, v are multiprecision integers of ndigits each
		and w is a multiprecision integer of 2*ndigits

		Ref: Knuth Vol 2 Ch 4.3.1 p 268 Algorithm M.
	*/

	DIGIT_T k, t[2];
	size_t i, j, m, n;

	assert(w != u && w != v);

	m = n = ndigits;

	/* Step M1. Initialise */
	for (i = 0; i < 2 * m; i++)
		w[i] = 0;

	for (j = 0; j < n; j++)
	{
		/* Step M2. Zero multiplier? */
		if (v[j] == 0)
		{
			w[j + m] = 0;
		}
		else
		{
			/* Step M3. Initialise i */
			k = 0;
			for (i = 0; i < m; i++)
			{
				/* Step M4. Multiply and add */
				/* t = u_i * v_j + w_(i+j) + k */
				spMultiply(t, u[i], v[j]);

				t[0] += k;
				if (t[0] < k)
					t[1]++;
				t[0] += w[i+j];
				if (t[0] < w[i+j])
					t[1]++;

				w[i+j] = t[0];
				k = t[1];
			}	
			/* Step M5. Loop on i, set w_(j+m) = k */
			w[j+m] = k;
		}
	}	/* Step M6. Loop on j */

	return 0;
}

/* mpDivide */

static DIGIT_T mpMultSub(DIGIT_T wn, DIGIT_T w[], const DIGIT_T v[],
					   DIGIT_T q, size_t n)
{	/*	Compute w = w - qv
		where w = (WnW[n-1]...W[0])
		return modified Wn.
	*/
	DIGIT_T k, t[2];
	size_t i;

	if (q == 0)	/* No change */
		return wn;

	k = 0;

	for (i = 0; i < n; i++)
	{
		spMultiply(t, q, v[i]);
		w[i] -= k;
		if (w[i] > MAX_DIGIT - k)
			k = 1;
		else
			k = 0;
		w[i] -= t[0];
		if (w[i] > MAX_DIGIT - t[0])
			k++;
		k += t[1];
	}

	/* Cope with Wn not stored in array w[0..n-1] */
	wn -= k;

	return wn;
}

static int QhatTooBig(DIGIT_T qhat, DIGIT_T rhat,
					  DIGIT_T vn2, DIGIT_T ujn2)
{	/*	Returns true if Qhat is too big
		i.e. if (Qhat * Vn-2) > (b.Rhat + Uj+n-2)
	*/
	DIGIT_T t[2];

	spMultiply(t, qhat, vn2);
	if (t[1] < rhat)
		return 0;
	else if (t[1] > rhat)
		return 1;
	else if (t[0] > ujn2)
		return 1;

	return 0;
}

int mpDivide(DIGIT_T q[], DIGIT_T r[], const DIGIT_T u[],
	size_t udigits, DIGIT_T v[], size_t vdigits)
{	/*	Computes quotient q = u / v and remainder r = u mod v
		where q, r, u are multiple precision digits
		all of udigits and the divisor v is vdigits.

		Ref: Knuth Vol 2 Ch 4.3.1 p 272 Algorithm D.

		Do without extra storage space, i.e. use r[] for
		normalised u[], unnormalise v[] at end, and cope with
		extra digit Uj+n added to u after normalisation.

		WARNING: this trashes q and r first, so cannot do
		u = u / v or v = u mod v.
		It also changes v temporarily so cannot make it const.
	*/
	size_t shift;
	int n, m, j;
	DIGIT_T bitmask, overflow;
	DIGIT_T qhat, rhat, t[2];
	DIGIT_T *uu, *ww;
	int qhatOK, cmp;

	/* Clear q and r */
	mpSetZero(q, udigits);
	mpSetZero(r, udigits);

	/* Work out exact sizes of u and v */
	n = (int)mpSizeof(v, vdigits);
	m = (int)mpSizeof(u, udigits);
	m -= n;

	/* Catch special cases */
	if (n == 0)
		return -1;	/* Error: divide by zero */

	if (n == 1)
	{	/* Use short division instead */
		r[0] = mpShortDiv(q, u, v[0], udigits);
		return 0;
	}

	if (m < 0)
	{	/* v > u, so just set q = 0 and r = u */
		mpSetEqual(r, u, udigits);
		return 0;
	}

	if (m == 0)
	{	/* u and v are the same length */
		cmp = mpCompare(u, v, (size_t)n);
		if (cmp < 0)
		{	/* v > u, as above */
			mpSetEqual(r, u, udigits);
			return 0;
		}
		else if (cmp == 0)
		{	/* v == u, so set q = 1 and r = 0 */
			mpSetDigit(q, 1, udigits);
			return 0;
		}
	}

	/*	In Knuth notation, we have:
		Given
		u = (Um+n-1 ... U1U0)
		v = (Vn-1 ... V1V0)
		Compute
		q = u/v = (QmQm-1 ... Q0)
		r = u mod v = (Rn-1 ... R1R0)
	*/

	/*	Step D1. Normalise */
	/*	Requires high bit of Vn-1
		to be set, so find most signif. bit then shift left,
		i.e. d = 2^shift, u' = u * d, v' = v * d.
	*/
	bitmask = HIBITMASK;
	for (shift = 0; shift < BITS_PER_DIGIT; shift++)
	{
		if (v[n-1] & bitmask)
			break;
		bitmask >>= 1;
	}

	/* Normalise v in situ - NB only shift non-zero digits */
	overflow = mpShiftLeft(v, v, shift, n);

	/* Copy normalised dividend u*d into r */
	overflow = mpShiftLeft(r, u, shift, n + m);
	uu = r;	/* Use ptr to keep notation constant */

	t[0] = overflow;	/* Extra digit Um+n */

	/* Step D2. Initialise j. Set j = m */
	for (j = m; j >= 0; j--)
	{
		/* Step D3. Set Qhat = [(b.Uj+n + Uj+n-1)/Vn-1] 
		   and Rhat = remainder */
		qhatOK = 0;
		t[1] = t[0];	/* This is Uj+n */
		t[0] = uu[j+n-1];
		overflow = spDivide(&qhat, &rhat, t, v[n-1]);

		/* Test Qhat */
		if (overflow)
		{	/* Qhat == b so set Qhat = b - 1 */
			qhat = MAX_DIGIT;
			rhat = uu[j+n-1];
			rhat += v[n-1];
			if (rhat < v[n-1])	/* Rhat >= b, so no re-test */
				qhatOK = 1;
		}
		/* [VERSION 2: Added extra test "qhat && "] */
		if (qhat && !qhatOK && QhatTooBig(qhat, rhat, v[n-2], uu[j+n-2]))
		{	/* If Qhat.Vn-2 > b.Rhat + Uj+n-2 
			   decrease Qhat by one, increase Rhat by Vn-1
			*/
			qhat--;
			rhat += v[n-1];
			/* Repeat this test if Rhat < b */
			if (!(rhat < v[n-1]))
				if (QhatTooBig(qhat, rhat, v[n-2], uu[j+n-2]))
					qhat--;
		}


		/* Step D4. Multiply and subtract */
		ww = &uu[j];
		overflow = mpMultSub(t[1], ww, v, qhat, (size_t)n);

		/* Step D5. Test remainder. Set Qj = Qhat */
		q[j] = qhat;
		if (overflow)
		{	/* Step D6. Add back if D4 was negative */
			q[j]--;
			overflow = mpAdd(ww, ww, v, (size_t)n);
		}

		t[0] = uu[j+n-1];	/* Uj+n on next round */

	}	/* Step D7. Loop on j */

	/* Clear high digits in uu */
	for (j = n; j < m+n; j++)
		uu[j] = 0;

	/* Step D8. Unnormalise. */

	mpShiftRight(r, r, shift, n);
	mpShiftRight(v, v, shift, n);

	return 0;
}


int mpSquare(DIGIT_T w[], const DIGIT_T x[], size_t ndigits)
/* New in Version 2 */
{
	/*	Computes square w = x * x
		where x is a multiprecision integer of ndigits
		and w is a multiprecision integer of 2*ndigits

		Ref: Menezes p596 Algorithm 14.16 with errata.
	*/

	DIGIT_T k, p[2], u[2], cbit, carry;
	size_t i, j, t, i2, cpos;

	assert(w != x);

	t = ndigits;

	/* 1. For i from 0 to (2t-1) do: w_i = 0 */
	i2 = t << 1;
	for (i = 0; i < i2; i++)
		w[i] = 0;

	carry = 0;
	cpos = i2-1;
	/* 2. For i from 0 to (t-1) do: */
	for (i = 0; i < t; i++)
	{
		/* 2.1 (uv) = w_2i + x_i * x_i, w_2i = v, c = u 
		   Careful, w_2i may be double-prec
		*/
		i2 = i << 1; /* 2*i */
		spMultiply(p, x[i], x[i]);
		p[0] += w[i2];
		if (p[0] < w[i2])
			p[1]++;
		k = 0;	/* p[1] < b, so no overflow here */
		if (i2 == cpos && carry)
		{
			p[1] += carry;
			if (p[1] < carry)
				k++;
			carry = 0;
		}
		w[i2] = p[0];
		u[0] = p[1];
		u[1] = k;

		/* 2.2 for j from (i+1) to (t-1) do:
		   (uv) = w_{i+j} + 2x_j * x_i + c,
		   w_{i+j} = v, c = u,
		   u is double-prec 
		   w_{i+j} is dbl if [i+j] == cpos
		*/
		k = 0;
		for (j = i+1; j < t; j++)
		{
			/* p = x_j * x_i */
			spMultiply(p, x[j], x[i]);
			/* p = 2p <=> p <<= 1 */
			cbit = (p[0] & HIBITMASK) != 0;
			k =  (p[1] & HIBITMASK) != 0;
			p[0] <<= 1;
			p[1] <<= 1;
			p[1] |= cbit;
			/* p = p + c */
			p[0] += u[0];
			if (p[0] < u[0])
			{
				p[1]++;
				if (p[1] == 0)
					k++;
			}
			p[1] += u[1];
			if (p[1] < u[1])
				k++;
			/* p = p + w_{i+j} */
			p[0] += w[i+j];
			if (p[0] < w[i+j])
			{
				p[1]++;
				if (p[1] == 0)
					k++;
			}
			if ((i+j) == cpos && carry)
			{	/* catch overflow from last round */
				p[1] += carry;
				if (p[1] < carry)
					k++;
				carry = 0;
			}
			/* w_{i+j} = v, c = u */
			w[i+j] = p[0];
			u[0] = p[1];
			u[1] = k;
		}
		/* 2.3 w_{i+t} = u */
		w[i+t] = u[0];
		/* remember overflow in w_{i+t} */
		carry = u[1];	
		cpos = i+t;
	}

	/* (NB original step 3 deleted in errata) */

	/* Return w */

	return 0;
}

int mpEqual(const DIGIT_T a[], const DIGIT_T b[], size_t ndigits)
{
	/*	Returns true if a == b, else false
	*/

	if (ndigits == 0) return -1;

	while (ndigits--)
	{
		if (a[ndigits] != b[ndigits])
			return 0;	/* False */
	}

	return (!0);	/* True */
}

int mpCompare(const DIGIT_T a[], const DIGIT_T b[], size_t ndigits)
{
	/*	Returns sign of (a - b)
	*/

	if (ndigits == 0) return 0;

	while (ndigits--)
	{
		if (a[ndigits] > b[ndigits])
			return 1;	/* GT */
		if (a[ndigits] < b[ndigits])
			return -1;	/* LT */
	}

	return 0;	/* EQ */
}

int mpIsZero(const DIGIT_T a[], size_t ndigits)
{
	/*	Returns true if a == 0, else false
	*/

	size_t i;
	if (ndigits == 0) return -1;

	for (i = 0; i < ndigits; i++)	/* Start at lsb */
	{
		if (a[i] != 0)
			return 0;	/* False */
	}

	return (!0);	/* True */
}

size_t mpSizeof(const DIGIT_T a[], size_t ndigits)
{	/* Returns size of significant digits in a */
	
	while(ndigits--)
	{
		if (a[ndigits] != 0)
			return (++ndigits);
	}
	return 0;
}

size_t mpBitLength(const DIGIT_T d[], size_t ndigits)
/* Returns no of significant bits in d */
{
	size_t n, i, bits;
	DIGIT_T mask;

	if (!d || ndigits == 0)
		return 0;

	n = mpSizeof(d, ndigits);
	if (0 == n) return 0;

	for (i = 0, mask = HIBITMASK; mask > 0; mask >>= 1, i++)
	{
		if (d[n-1] & mask)
			break;
	}

	bits = n * BITS_PER_DIGIT - i;

	return bits;
}

void mpSetEqual(DIGIT_T a[], const DIGIT_T b[], size_t ndigits)
{	/* Sets a = b */
	size_t i;
	
	for (i = 0; i < ndigits; i++)
	{
		a[i] = b[i];
	}
}

void mpSetZero(DIGIT_T a[], size_t ndigits)
{	/* Sets a = 0 */

	/* Prevent optimiser ignoring this */
	volatile DIGIT_T optdummy;
	DIGIT_T *p = a;

	while (ndigits--)
		a[ndigits] = 0;
	
	optdummy = *p;
}

void mpSetDigit(DIGIT_T a[], DIGIT_T d, size_t ndigits)
{	/* Sets a = d where d is a single digit */
	size_t i;
	
	for (i = 1; i < ndigits; i++)
	{
		a[i] = 0;
	}
	a[0] = d;
}

DIGIT_T mpShiftLeft(DIGIT_T a[], const DIGIT_T *b,
	size_t shift, size_t ndigits)
{	/* Computes a = b << shift */
	/* [v2.1] Modified to cope with shift > BITS_PERDIGIT */
	size_t i, y, nw, bits;
	DIGIT_T mask, carry, nextcarry;

	/* Do we shift whole digits? */
	if (shift >= BITS_PER_DIGIT)
	{
		nw = shift / BITS_PER_DIGIT;
		i = ndigits;
		while (i--)
		{
			if (i >= nw)
				a[i] = b[i-nw];
			else
				a[i] = 0;
		}
		/* Call again to shift bits inside digits */
		bits = shift % BITS_PER_DIGIT;
		carry = b[ndigits-nw] << bits;
		if (bits) 
			carry |= mpShiftLeft(a, a, bits, ndigits);
		return carry;
	}
	else
	{
		bits = shift;
	}

	/* Construct mask = high bits set */
	mask = ~(~(DIGIT_T)0 >> bits);
	
	y = BITS_PER_DIGIT - bits;
	carry = 0;
	for (i = 0; i < ndigits; i++)
	{
		nextcarry = (b[i] & mask) >> y;
		a[i] = b[i] << bits | carry;
		carry = nextcarry;
	}

	return carry;
}

DIGIT_T mpShiftRight(DIGIT_T a[], const DIGIT_T b[], size_t shift, size_t ndigits)
{	/* Computes a = b >> shift */
	/* [v2.1] Modified to cope with shift > BITS_PERDIGIT */
	size_t i, y, nw, bits;
	DIGIT_T mask, carry, nextcarry;

	/* Do we shift whole digits? */
	if (shift >= BITS_PER_DIGIT)
	{
		nw = shift / BITS_PER_DIGIT;
		for (i = 0; i < ndigits; i++)
		{
			if ((i+nw) < ndigits)
				a[i] = b[i+nw];
			else
				a[i] = 0;
		}
		/* Call again to shift bits inside digits */
		bits = shift % BITS_PER_DIGIT;
		carry = b[nw-1] >> bits;
		if (bits) 
			carry |= mpShiftRight(a, a, bits, ndigits);
		return carry;
	}
	else
	{
		bits = shift;
	}

	/* Construct mask to set low bits */
	/* (thanks to Jesse Chisholm for suggesting this improved technique) */
	mask = ~(~(DIGIT_T)0 << bits);
	
	y = BITS_PER_DIGIT - bits;
	carry = 0;
	i = ndigits;
	while (i--)
	{
		nextcarry = (b[i] & mask) << y;
		a[i] = b[i] >> bits | carry;
		carry = nextcarry;
	}

	return carry;
}

void mpModPowerOf2(DIGIT_T a[], size_t ndigits, size_t L)
	/* Computes a = a mod 2^L */
	/* i.e. clears all bits >= L */
{
	size_t i, nw, bits;
	DIGIT_T mask;

	/* High digits to clear */
	nw = L / BITS_PER_DIGIT;
	for (i = nw+1; i < ndigits; i++)
		a[i] = 0;
	/* Low bits to keep */
	bits = L % BITS_PER_DIGIT;
	mask = ~(~0 << bits);
	if (ndigits > nw)
		a[nw] &= mask;
}

void mpXorBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits)
	/* Computes bitwise a = b XOR c */
{
	size_t i;
	for (i = 0; i < ndigits; i++)
		a[i] = b[i] ^ c[i];
}

void mpOrBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits)
	/* Computes bitwise a = b OR c */
{
	size_t i;
	for (i = 0; i < ndigits; i++)
		a[i] = b[i] | c[i];
}

void mpAndBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits)
	/* Computes bitwise a = b AND c */
{
	size_t i;
	for (i = 0; i < ndigits; i++)
		a[i] = b[i] & c[i];
}

DIGIT_T mpShortAdd(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, 
			   size_t ndigits)
{
	/*	Calculates w = u + v
		where w, u are multiprecision integers of ndigits each
		and v is a single precision digit.
		Returns carry if overflow.

		Ref: Derived from Knuth Algorithm A.
	*/

	DIGIT_T k;
	size_t j;

	k = 0;

	/* Add v to first digit of u */
	w[0] = u[0] + v;
	if (w[0] < v)
		k = 1;
	else
		k = 0;

	/* Add carry to subsequent digits */
	for (j = 1; j < ndigits; j++)
	{
		w[j] = u[j] + k;
		if (w[j] < k)
			k = 1;
		else
			k = 0;
	}

	return k;
}

DIGIT_T mpShortSub(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, 
			   size_t ndigits)
{
	/*	Calculates w = u - v
		where w, u are multiprecision integers of ndigits each
		and v is a single precision digit.
		Returns borrow: 0 if u >= v, or 1 if v > u.

		Ref: Derived from Knuth Algorithm S.
	*/

	DIGIT_T k;
	size_t j;

	k = 0;

	/* Subtract v from first digit of u */
	w[0] = u[0] - v;
	if (w[0] > MAX_DIGIT - v)
		k = 1;
	else
		k = 0;

	/* Subtract borrow from subsequent digits */
	for (j = 1; j < ndigits; j++)
	{
		w[j] = u[j] - k;
		if (w[j] > MAX_DIGIT - k)
			k = 1;
		else
			k = 0;
	}

	return k;
}

DIGIT_T mpShortMult(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, 
					size_t ndigits)
{
	/*	Computes product w = u * v
		Returns overflow k
		where w, u are multiprecision integers of ndigits each
		and v, k are single precision digits

		Ref: Knuth Algorithm M.
	*/

	DIGIT_T k, t[2];
	size_t j;

	if (v == 0) 
	{	/* [2005-08-29] Set w = 0 */
		for (j = 0; j < ndigits; j++)
			w[j] = 0;
		return 0;
	}

	k = 0;
	for (j = 0; j < ndigits; j++)
	{
		/* t = x_i * v */
		spMultiply(t, u[j], v);
		/* w_i = LOHALF(t) + carry */
		w[j] = t[0] + k;
		/* Overflow? */
		if (w[j] < k)
			t[1]++;
		/* Carry forward HIHALF(t) */
		k = t[1];
	}

	return k;
}

DIGIT_T mpShortDiv(DIGIT_T q[], const DIGIT_T u[], DIGIT_T v, 
				   size_t ndigits)
{
	/*	Calculates quotient q = u div v
		Returns remainder r = u mod v
		where q, u are multiprecision integers of ndigits each
		and r, v are single precision digits.

		Makes no assumptions about normalisation.
		
		Ref: Knuth Vol 2 Ch 4.3.1 Exercise 16 p625
	*/
	size_t j;
	DIGIT_T t[2], r;
	size_t shift;
	DIGIT_T bitmask, overflow, *uu;

	if (ndigits == 0) return 0;
	if (v == 0)	return 0;	/* Divide by zero error */

	/*	Normalise first */
	/*	Requires high bit of V
		to be set, so find most signif. bit then shift left,
		i.e. d = 2^shift, u' = u * d, v' = v * d.
	*/
	bitmask = HIBITMASK;
	for (shift = 0; shift < BITS_PER_DIGIT; shift++)
	{
		if (v & bitmask)
			break;
		bitmask >>= 1;
	}

	v <<= shift;
	overflow = mpShiftLeft(q, u, shift, ndigits);
	uu = q;
	
	/* Step S1 - modified for extra digit. */
	r = overflow;	/* New digit Un */
	j = ndigits;
	while (j--)
	{
		/* Step S2. */
		t[1] = r;
		t[0] = uu[j];
		overflow = spDivide(&q[j], &r, t, v);
	}

	/* Unnormalise */
	r >>= shift;
	
	return r;
}

DIGIT_T mpShortMod(const DIGIT_T a[], DIGIT_T d, size_t ndigits)
{
	/*	Calculates r = a mod d
		where a is a multiprecision integer of ndigits
		and r, d are single precision digits
		using bit-by-bit method from left to right.

		Use remainder from divide function.

		Updated Version 2 to use malloc.
	*/

	DIGIT_T *q;
	DIGIT_T r = 0;

	q = mpAlloc(ndigits * 2);

	r = mpShortDiv(q, a, d, ndigits);

	mpSetZero(q, ndigits);
	mpFree(&q);

	return r;
}

int mpShortCmp(const DIGIT_T a[], DIGIT_T b, size_t ndigits)
{
	/* Returns sign of (a - b) where b is a single digit */

	size_t i;

	/* Changed in Ver 2.0 to cope with zero-length a */
	if (ndigits == 0) return (b ? -1 : 0);

	for (i = 1; i < ndigits; i++)
	{
		if (a[i] != 0)
			return 1;	/* GT */
	}

	if (a[0] < b)
		return -1;		/* LT */
	else if (a[0] > b)
		return 1;		/* GT */

	return 0;	/* EQ */
}

/* [v2.1] changed to use C99 format types */
void mpPrint(const DIGIT_T *p, size_t len)
{
	while (len--)
	{
		printf("%08" PRIxBIGD " ", p[len]);
	}
}

void mpPrintNL(const DIGIT_T *p, size_t len)
{
	size_t i = 0;

	while (len--)
	{
		if ((i % 8) == 0 && i)
			printf("\n");
		printf("%08" PRIxBIGD " ", p[len]);
		i++;
	}
	printf("\n");
}

void mpPrintTrim(const DIGIT_T *p, size_t len)
{
	/* Trim leading zeroes */
	while (len--)
	{
		if (p[len] != 0)
			break;
	}
	len++;
	/* Catch empty len to show 0 */
	if (0 == len) len = 1;

	mpPrint(p, len);
}

void mpPrintTrimNL(const DIGIT_T *p, size_t len)
{
	/* Trim leading zeroes */
	while (len--)
	{
		if (p[len] != 0)
			break;
	}
	len++;
	/* Catch empty len to show 0 */
	if (0 == len) len = 1;

	mpPrintNL(p, len);
}

size_t mpConvFromOctets(DIGIT_T a[], size_t ndigits, const unsigned char *c, size_t nbytes)
/* Converts nbytes octets into big digit a of max size ndigits
   Returns actual number of digits set (may be larger than mpSizeof)
*/
{
	size_t i;
	int j, k;
	DIGIT_T t;

	mpSetZero(a, ndigits);

	/* Read in octets, least significant first */
	/* i counts into big_d, j along c, and k is # bits to shift */
	for (i = 0, j = nbytes - 1; i < ndigits && j >= 0; i++)
	{
		t = 0;
		for (k = 0; j >= 0 && k < BITS_PER_DIGIT; j--, k += 8)
			t |= ((DIGIT_T)c[j]) << k;
		a[i] = t;
	}

	return i;
}

size_t mpConvToOctets(const DIGIT_T a[], size_t ndigits, unsigned char *c, size_t nbytes)
/* Convert big digit a into string of octets, in big-endian order,
   padding on the left to nbytes or truncating if necessary.
   Return number of octets required excluding leading zero bytes.
*/
{
	int j, k, len;
	DIGIT_T t;
	size_t i, noctets, nbits;

	nbits = mpBitLength(a, ndigits);
	noctets = (nbits + 7) / 8;

	len = (int)nbytes;

	for (i = 0, j = len - 1; i < ndigits && j >= 0; i++)
	{
		t = a[i];
		for (k = 0; j >= 0 && k < BITS_PER_DIGIT; j--, k += 8)
			c[j] = (unsigned char)(t >> k);
	}

	for ( ; j >= 0; j--)
		c[j] = 0;

	return (size_t)noctets;
}

static size_t uiceil(double x)
/* Returns ceil(x) as a non-negative integer or 0 if x < 0 */
{
	size_t c;

	if (x < 0) return 0;
	c = (size_t)x;
	if ((x - c) > 0.0)
		c++;

	return c;
}

static size_t conv_to_base(const DIGIT_T a[], size_t ndigits, char *s, size_t smax, int base)
/* Convert big digit a into a string in given base format, 
   where s has max size smax.
   Return number of chars set excluding leading zeroes.
   smax can be 0 to find out the required length.
*/
{
	const char DEC_DIGITS[] = "0123456789";
	const char HEX_DIGITS[] = "0123456789abcdef";
	size_t newlen, nbytes, nchars;
	unsigned char *bytes, *newdigits;
	size_t n;
	unsigned long t;
	size_t i, j, isig;
	const char *digits;
	double factor;

	switch (base)
	{
	case 10:
		digits = DEC_DIGITS;
		factor = 2.40824;	/* log(256)/log(10)=2.40824 */
		break;
	case 16:
		digits = HEX_DIGITS;
		factor = 2.0;	/* log(256)/log(16)=2.0 */
		break;
	default:
		assert (10 == base || 16 == base);
		return 0;
	}

	/* Set up output string with null chars */
	if (smax > 0 && s)
	{
		memset(s, '0', smax-1);
		s[smax-1] = '\0';
	}

	/* Catch zero input value (return 1 not zero) */
	if (mpIsZero(a, ndigits))
	{
		if (smax > 0 && s)
			s[1] = '\0';
		return 1;
	}

	/* First, we convert to 8-bit octets (bytes), which are easier to handle */
	nbytes = ndigits * BITS_PER_DIGIT / 8;
	bytes = calloc(nbytes, 1);
	if (!bytes) mpFail("conv_to_base: Not enough memory: " __FILE__);

	n = mpConvToOctets(a, ndigits, bytes, nbytes);

	/* Create some temp storage for int values */
	newlen = uiceil(n * factor);
	newdigits = calloc(newlen, 1);
	if (!newdigits) mpFail("conv_to_base: Not enough memory: " __FILE__);

	for (i = 0; i < nbytes; i++)
	{
		t = bytes[i];
		for (j = newlen; j > 0; j--)
		{
			t += (unsigned long)newdigits[j-1] * 256;
			newdigits[j-1] = (unsigned char)(t % base);
			t /= base;
		}
	}

	/* Find index of leading significant digit */
	for (isig = 0; isig < newlen; isig++)
		if (newdigits[isig])
			break;

	nchars = newlen - isig;

	/* Convert to a null-terminated string of decimal chars */
	/* up to limit, unless user has specified null or size == 0 */
	if (smax > 0 && s)
	{
		for (i = 0; i < nchars && i < smax-1; i++)
		{
			s[i] = digits[newdigits[isig+i]];
		}
		s[i] = '\0';
	}

	free(bytes);
	free(newdigits);

	return nchars;
}

size_t mpConvToDecimal(const DIGIT_T a[], size_t ndigits, char *s, size_t smax)
/* Convert big digit a into a string in decimal format, 
   where s has max size smax.
   Return number of chars set excluding leading zeroes.
*/
{
	return conv_to_base(a, ndigits, s, smax, 10);
}

size_t mpConvToHex(const DIGIT_T a[], size_t ndigits, char *s, size_t smax)
/* Convert big digit a into a string in hexadecimal format, 
   where s has max size smax.
   Return number of chars set excluding leading zeroes.
*/
{
	return conv_to_base(a, ndigits, s, smax, 16);
}

size_t mpConvFromDecimal(DIGIT_T a[], size_t ndigits, const char *s)
/* Convert a string in decimal format to a big digit.
   Return actual number of digits set (may be larger than mpSizeof).
   Just ignores invalid characters in s.
*/
{
	size_t newlen;
	unsigned char *newdigits;
	size_t n;
	unsigned long t;
	size_t i, j;
	const int base = 10;

	mpSetZero(a, ndigits);

	/* Create some temp storage for int values */
	n = strlen(s);
	if (0 == n) return 0;
	newlen = uiceil(n * 0.41524);	/* log(10)/log(256)=0.41524 */
	newdigits = calloc(newlen, 1);
	if (!newdigits) mpFail("mpConvFromDecimal: Not enough memory: " __FILE__);

	/* Work through zero-terminated string */
	for (i = 0; s[i]; i++)
	{
		t = s[i] - '0';
		if (t > 9 || t < 0) continue;
		for (j = newlen; j > 0; j--)
		{
			t += (unsigned long)newdigits[j-1] * base;
			newdigits[j-1] = (unsigned char)(t & 0xFF);
			t >>= 8;
		}
	}

	/* Convert bytes to big digits */
	n = mpConvFromOctets(a, ndigits, newdigits, newlen);

	/* Clean up */
	free(newdigits);

	return n;
}


size_t mpConvFromHex(DIGIT_T a[], size_t ndigits, const char *s)
/* Convert a string in hexadecimal format to a big digit.
   Return actual number of digits set (may be larger than mpSizeof).
   Just ignores invalid characters in s.
*/
{
	size_t newlen;
	unsigned char *newdigits;
	size_t n;
	unsigned long t;
	size_t i, j;

	mpSetZero(a, ndigits);

	/* Create some temp storage for int values */
	n = strlen(s);
	if (0 == n) return 0;
	newlen = uiceil(n * 0.5);	/* log(16)/log(256)=0.5 */
	newdigits = calloc(newlen, 1);
	if (!newdigits) mpFail("mpConvFromHex: Not enough memory: " __FILE__);

	/* Work through zero-terminated string */
	for (i = 0; s[i]; i++)
	{
		t = s[i];
		if ((t >= '0') && (t <= '9')) t = (t - '0');
		else if ((t >= 'a') && (t <= 'f')) t = (t - 'a' + 10);
		else if ((t >= 'A') && (t <= 'F')) t = (t - 'A' + 10);
		else continue;
		for (j = newlen; j > 0; j--)
		{
			t += (unsigned long)newdigits[j-1] << 4;
			newdigits[j-1] = (unsigned char)(t & 0xFF);
			t >>= 8;
		}
	}

	/* Convert bytes to big digits */
	n = mpConvFromOctets(a, ndigits, newdigits, newlen);

	/* Clean up */
	free(newdigits);

	return n;
}

int mpModulo(DIGIT_T r[], const DIGIT_T u[], size_t udigits, 
			 DIGIT_T v[], size_t vdigits)
{
	/*	Computes r = u mod v
		where r, v are multiprecision integers of length vdigits
		and u is a multiprecision integer of length udigits.
		r may overlap v.

		Note that r here is only vdigits long, 
		whereas in mpDivide it is udigits long.

		Use remainder from mpDivide function.

		Updated Version 2 to use malloc.
	*/

	DIGIT_T *qq, *rr;
	size_t nn = max(udigits, vdigits);

	qq = mpAlloc(udigits);
	rr = mpAlloc(nn);

	/* rr[nn] = u mod v */
	mpDivide(qq, rr, u, udigits, v, vdigits);

	/* Final r is only vdigits long */
	mpSetEqual(r, rr, vdigits);
	mpSetZero(rr, udigits);
	mpSetZero(qq, udigits);
	mpFree(&rr);
	mpFree(&qq);

	return 0;
}

int mpModMult(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], 
			  const DIGIT_T m[], size_t ndigits)
{	/*	Computes a = (x * y) mod m */
	/*	Updated Version 2 to use malloc. */
	
	/* Double-length temp variable p */
	DIGIT_T *p;
	DIGIT_T *tm;

	p = mpAlloc(ndigits * 2);
	tm = mpAlloc(ndigits);
	mpSetEqual(tm, m, ndigits);

	/* Calc p[2n] = x * y */
	mpMultiply(p, x, y, ndigits);

	/* Then modulo */
	mpModulo(a, p, ndigits * 2, tm, ndigits);

	mpSetZero(p, ndigits * 2);
	mpSetZero(tm, ndigits);
	mpFree(&p);
	mpFree(&tm);
	return 0;
}

/* mpModExp */

/* Version 2: added new funcs with temps 
to avoid having to alloc and free repeatedly
and added mpSquare function for slight speed improvement
*/

static int moduloTemp(DIGIT_T r[], const DIGIT_T u[], size_t udigits, 
			 DIGIT_T v[], size_t vdigits, DIGIT_T tqq[], DIGIT_T trr[])
{
	/*	Calculates r = u mod v
		where r, v are multiprecision integers of length vdigits
		and u is a multiprecision integer of length udigits.
		r may overlap v.

		Same as mpModulo without allocs & free.
		Requires temp mp's tqq and trr of length udigits each
	*/

	mpDivide(tqq, trr, u, udigits, v, vdigits);

	/* Final r is only vdigits long */
	mpSetEqual(r, trr, vdigits);

	return 0;
}

static int modMultTemp(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], 
			  DIGIT_T m[], size_t ndigits, 
			  DIGIT_T temp[], DIGIT_T tqq[], DIGIT_T trr[])
{	/*	Computes a = (x * y) mod m */
	/*	Requires 3 x temp mp's of length 2 * ndigits each */

	/* Calc p[2n] = x * y */
	mpMultiply(temp, x, y, ndigits);

	/* Then modulo m */
	moduloTemp(a, temp, ndigits * 2, m, ndigits, tqq, trr);

	return 0;
}


static int modSquareTemp(DIGIT_T a[], const DIGIT_T x[], 
			  DIGIT_T m[], size_t ndigits, 
			  DIGIT_T temp[], DIGIT_T tqq[], DIGIT_T trr[])
{	/*	Computes a = (x * x) mod m */
	/*	Requires 3 x temp mp's of length 2 * ndigits each */
	
	/* Calc p[2n] = x^2 */
	mpSquare(temp, x, ndigits);

	/* Then modulo m */
	moduloTemp(a, temp, ndigits * 2, m, ndigits, tqq, trr);

	return 0;
}

int mpModExp(DIGIT_T yout[], const DIGIT_T x[], 
			const DIGIT_T e[], const DIGIT_T m[], size_t ndigits)
{	/*	Computes y = x^e mod m */
	/*	Binary left-to-right method */
	DIGIT_T mask;
	size_t n, nn;
	DIGIT_T *t1, *t2, *t3, *tm, *y;
	
	if (ndigits == 0) return -1;

	/* Create some temps */
	nn = ndigits * 2;
	t1 = mpAlloc(nn);
	t2 = mpAlloc(nn);
	t3 = mpAlloc(nn);
	tm = mpAlloc(ndigits);
	y = mpAlloc(ndigits);
	mpSetEqual(tm, m, ndigits);

	/* Find second-most significant bit in e */
	n = mpSizeof(e, ndigits);
	for (mask = HIBITMASK; mask > 0; mask >>= 1)
	{
		if (e[n-1] & mask)
			break;
	}
	mpNEXTBITMASK(mask, n);

	/* Set y = x */
	mpSetEqual(y, x, ndigits);

	/* For bit j = k-2 downto 0 */
	while (n)
	{
		/* Square y = y * y mod n */
		modSquareTemp(y, y, tm, ndigits, t1, t2, t3);	
		if (e[n-1] & mask)
		{	/*	if e(j) == 1 then multiply
				y = y * x mod n */
			modMultTemp(y, y, x, tm, ndigits, t1, t2, t3);
		} 
		
		/* Move to next bit */
		mpNEXTBITMASK(mask, n);
	}

	/* return y */
	mpSetEqual(yout, y, ndigits);

	mpSetZero(t1, nn);
	mpSetZero(t2, nn);
	mpSetZero(t3, nn);
	mpSetZero(tm, ndigits);
	mpSetZero(y, ndigits);
	mpFree(&t1);
	mpFree(&t2);
	mpFree(&t3);
	mpFree(&tm);
	mpFree(&y);

	return 0;
}

int mpModInv(DIGIT_T inv[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits)
{	/*	Computes inv = u^(-1) mod v */
	/*	Ref: Knuth Algorithm X Vol 2 p 342
		ignoring u2, v2, t2
		and avoiding negative numbers.
		Updated Version 2 to use malloc 
		and to return non-zero if inverse undefined.
	*/
	/* Hard-coded arrays removed in version 2:
	//DIGIT_T u1[MAX_DIG_LEN], u3[MAX_DIG_LEN], v1[MAX_DIG_LEN], v3[MAX_DIG_LEN];
	//DIGIT_T t1[MAX_DIG_LEN], t3[MAX_DIG_LEN], q[MAX_DIG_LEN];
	//DIGIT_T w[2*MAX_DIG_LEN];
	*/
	DIGIT_T *u1, *u3, *v1, *v3, *t1, *t3, *q, *w;
	int bIterations;
	int result;

	/* Allocate temp storage */
	u1 = mpAlloc(ndigits);
	u3 = mpAlloc(ndigits);
	v1 = mpAlloc(ndigits);
	v3 = mpAlloc(ndigits);
	t1 = mpAlloc(ndigits);
	t3 = mpAlloc(ndigits);
	q  = mpAlloc(ndigits);
	w  = mpAlloc(2 * ndigits);

	/* Step X1. Initialise */
	mpSetDigit(u1, 1, ndigits);		/* u1 = 1 */
	mpSetEqual(u3, u, ndigits);		/* u3 = u */
	mpSetZero(v1, ndigits);			/* v1 = 0 */
	mpSetEqual(v3, v, ndigits);		/* v3 = v */

	bIterations = 1;	/* Remember odd/even iterations */
	while (!mpIsZero(v3, ndigits))		/* Step X2. Loop while v3 != 0 */
	{					/* Step X3. Divide and "Subtract" */
		mpDivide(q, t3, u3, ndigits, v3, ndigits);
						/* q = u3 / v3, t3 = u3 % v3 */
		mpMultiply(w, q, v1, ndigits);	/* w = q * v1 */
		mpAdd(t1, u1, w, ndigits);		/* t1 = u1 + w */

		/* Swap u1 = v1; v1 = t1; u3 = v3; v3 = t3 */
		mpSetEqual(u1, v1, ndigits);
		mpSetEqual(v1, t1, ndigits);
		mpSetEqual(u3, v3, ndigits);
		mpSetEqual(v3, t3, ndigits);

		bIterations = -bIterations;
	}

	if (bIterations < 0)
		mpSubtract(inv, v, u1, ndigits);	/* inv = v - u1 */
	else
		mpSetEqual(inv, u1, ndigits);	/* inv = u1 */

	/* Make sure u3 = gcd(u,v) == 1 */
	if (mpShortCmp(u3, 1, ndigits) != 0)
	{
		result = 1;
		mpSetZero(inv, ndigits);
	}
	else
		result = 0;

	/* Clear up */
	mpSetZero(u1, ndigits);
	mpSetZero(v1, ndigits);
	mpSetZero(t1, ndigits);
	mpSetZero(u3, ndigits);
	mpSetZero(v3, ndigits);
	mpSetZero(t3, ndigits);
	mpSetZero(q, ndigits);
	mpSetZero(w, 2*ndigits);
	mpFree(&u1);
	mpFree(&v1);
	mpFree(&t1);
	mpFree(&u3);
	mpFree(&v3);
	mpFree(&t3);
	mpFree(&q);
	mpFree(&w);

	return result;
}

int mpGcd(DIGIT_T g[], const DIGIT_T x[], const DIGIT_T y[], size_t ndigits)
{	
	/* Computes g = gcd(x, y) */
	/* Ref: Schneier  */

	/*	NB This function requires temp storage */
	/*	Updated Version 2 to use malloc.
	//DIGIT_T yy[MAX_DIG_LEN], xx[MAX_DIG_LEN];
	 */
	DIGIT_T *yy, *xx;

	yy = mpAlloc(ndigits);
	xx = mpAlloc(ndigits);
	
	mpSetZero(yy, ndigits);
	mpSetZero(xx, ndigits);
	mpSetEqual(xx, x, ndigits);
	mpSetEqual(yy, y, ndigits);

	mpSetEqual(g, yy, ndigits);		/* g = y */
	
	while (!mpIsZero(xx, ndigits))	/* while (x > 0) */
	{
		mpSetEqual(g, xx, ndigits);	/* g = x */
		mpModulo(xx, yy, ndigits, xx, ndigits);	/* x = y mod x */
		mpSetEqual(yy, g, ndigits);	/* y = g; */
	}

	mpSetZero(xx, ndigits);
	mpSetZero(yy, ndigits);
	mpFree(&xx);
	mpFree(&yy);

	return 0;	/* gcd is in g */
}

int mpSqrt(DIGIT_T s[], const DIGIT_T x[], size_t ndigits)
	/* Computes integer square root s = floor(sqrt(x)) */
	/* [Added v2.1] Based on lsqrt() function */
{
	DIGIT_T *v0, *q0, *x0, *x1, *t;

	/* if (x <= 1) return x */
	if (mpShortCmp(x, 1, ndigits) <= 0)
	{
		mpSetEqual(s, x, ndigits);
		return 0;
	}

	/* Allocate temp storage */
	v0 = mpAlloc(ndigits);
	q0 = mpAlloc(ndigits);
	x0 = mpAlloc(ndigits);
	x1 = mpAlloc(ndigits);
	t  = mpAlloc(ndigits);

	/* v0 = x - 1 */
	mpShortSub(v0, x, 1, ndigits);
	/* x0 = x/2 */
	mpShortDiv(x0, x, 2, ndigits);

	while (1)
	{
		/* q0 = v0/x0 */
		mpDivide(q0, t, v0, ndigits, x0, ndigits);

		/* x1 = (x0 + q0)/2 */
		mpAdd(t, x0, q0, ndigits);
		mpShortDiv(x1, t, 2, ndigits);

		/* if (q0 >= x0) break */
		if (mpCompare(q0, x0, ndigits) >= 0)
			break;

		/* x0 = x1 */
		mpSetEqual(x0, x1, ndigits);
	}

	/* return x1 */
	mpSetEqual(s, x1, ndigits);

	mpFree(&v0);
	mpFree(&q0);
	mpFree(&x0);
	mpFree(&x1);
	mpFree(&t);

	return 0;
}

/* mpIsPrime: Changes in Version 2: 
   Added mpAlloc for dynamic allocation
   Increased no of small primes
   Broke out mpRabinMiller() as a separate function
*/

static DIGIT_T SMALL_PRIMES[] = {
	3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 
	47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 
	103, 107, 109, 113,
	127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
	179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
	233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
	283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
	353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
	419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
	467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
	547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
	607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
	661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
	739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
	811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
	877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
	947, 953, 967, 971, 977, 983, 991, 997,
};
#define N_SMALL_PRIMES (sizeof(SMALL_PRIMES)/sizeof(DIGIT_T))

int mpIsPrime(const DIGIT_T w[], size_t ndigits, size_t t)
{	/*	Returns true if w > 2 is a probable prime */
	/*	Version 2: split out mpRabinMiller. */

	size_t i;

	/* Check the obvious */
	if (mpISEVEN(w, ndigits))
		return 0;

	/* First check for small primes, unless we could be one ourself */
	if (mpShortCmp(w, SMALL_PRIMES[N_SMALL_PRIMES-1], ndigits) > 0)
	{
		for (i = 0; i < N_SMALL_PRIMES; i++)
		{
			if (mpShortMod(w, SMALL_PRIMES[i], ndigits) == 0)
			return 0; /* Failed */
		}
	}
	else
	{	/* w is a small number, so check directly */
		for (i = 0; i < N_SMALL_PRIMES; i++)
		{
			if (mpShortCmp(w, SMALL_PRIMES[i], ndigits) == 0)
				return 1;	/* w is a small prime */
		}
		return 0;	/* w is not a small prime */
	}

	return mpRabinMiller(w, ndigits, t);
}

/* Local, simple rng functions used in Rabin-Miller */
static void rand_seed(DIGIT_T seed);
static DIGIT_T rand_between(DIGIT_T lower, DIGIT_T upper);

int mpRabinMiller(const DIGIT_T w[], size_t ndigits, size_t t)
{	
/*	Returns true (1) if w is a probable prime using the
	Rabin-Miller Probabilistic Primality Test.
	Carries out t iterations specified by user.
	Ref: FIPS-186-2 Appendix 2 Section 2.1.
	Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379
	and ANSI 9.42-2003 Annex B.1.1.

	DSS Standard and ANSI 9.42 recommend using t >= 50
	for probability of error less than or equal to 2^-100.
	Ferguson & Schneier recommend t = 64 for prob error < 2^-128
	In practice, most random composites are caught in the first
	round or two and so specifying a large t will only affect
	the final check.

	[v2.1] Updated range of bases from [2, N-1] to [2, N-2]
	See ANSI 9.42-2003 Annex F.1.1 `Range of bases in Miller-Rabin test'
	(NB this does not impact existing implementations because N-1 
	is unlikely to be chosen as a base).
*/

	/* Temp big digits */
	DIGIT_T *m, *a, *b, *z, *w1, *j;
	DIGIT_T maxrand;
	int failed, isprime;
	size_t i;

	/* Catch w <= 1 */
	if (mpShortCmp(w, 1, ndigits) <= 0) return 0;
	
	/* Allocate temp storage */
	m = mpAlloc(ndigits);
	a = mpAlloc(ndigits);
	b = mpAlloc(ndigits);
	z = mpAlloc(ndigits);
	w1 = mpAlloc(ndigits);
	j = mpAlloc(ndigits);

	/* Seed the simple RNG for later on */
	rand_seed((DIGIT_T)time(NULL));

	/*	Rabin-Miller from FIPS-186-2 Appendix 2. 
	    Step 1. Set i = 1 [but do # tests requested by user].
	  	Step 2. Find a and m where w = 1 + (2^a)m
		m is odd and 2^a is largest power of 2 dividing w - 1 
	*/
	mpShortSub(w1, w, 1, ndigits);	/* Store w1 = w - 1 */
	mpSetEqual(m, w1, ndigits);		/* Set m = w - 1 */
	/* for (a = 0; iseven(m); a++) */
	for (mpSetZero(a, ndigits); mpISEVEN(m, ndigits); 
		mpShortAdd(a, a, 1, ndigits))
	{	/* Divide by 2 until m is odd */
		mpShiftRight(m, m, 1, ndigits);
	}

	/* assert((1 << a) * m + 1 == w); */

	/* Catch a small w */
	if (mpSizeof(w, ndigits) == 1)
		maxrand = w[0] - 2;	/* [v2.1] changed 1 to 2 */
	else
		maxrand = MAX_DIGIT;

	isprime = 1;
	for (i = 0; i < t; i++)
	{
		failed = 1;	/* Assume fail unless passed in loop */
		/* Step 3. Generate random integer b, 1 < b < w */
		/* [v2.1] changed to 1 < b < w-1 (see ANSI X9.42-2003 Annex B.1.1) */
		mpSetZero(b, ndigits);
		do
		{
			b[0] = rand_between(2, maxrand);
		} while (mpCompare(b, w, ndigits) >= 0);

		/* assert(1 < b && b < w); */

		/* Step 4. Set j = 0 and z = b^m mod w */
		mpSetZero(j, ndigits);
		mpModExp(z, b, m, w, ndigits);
		do
		{
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			/* i.e. if ((j == 0 && z == 1) || (z == w - 1)) */
			if ((mpIsZero(j, ndigits) 
				&& mpShortCmp(z, 1, ndigits) == 0)
				|| (mpCompare(z, w1, ndigits) == 0))
			{	/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (!mpIsZero(j, ndigits) 
				&& (mpShortCmp(z, 1, ndigits) == 0))
			{	/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			mpShortAdd(j, j, 1, ndigits);
			if (mpCompare(j, a, ndigits) < 0)
				mpModMult(z, z, z, w, ndigits);
			/* Loop: if j < a go to Step 5 */
		} while (mpCompare(j, a, ndigits) < 0);

		if (failed)
		{	/* Step 8. Not a prime - stop */
			isprime = 0;
			break;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
	/* Else, if i = n, w is probably prime => success */

	/* Clean up */
	mpSetZero(m, ndigits);
	mpSetZero(a, ndigits);
	mpSetZero(b, ndigits);
	mpSetZero(z, ndigits);
	mpSetZero(w1, ndigits);
	mpSetZero(j, ndigits);
	mpFree(&m);
	mpFree(&a);
	mpFree(&b);
	mpFree(&z);
	mpFree(&w1);
	mpFree(&j);

	return isprime;
}

/* Internal functions used for "simple" random numbers */
static void rand_seed(DIGIT_T seed)
{
	srand(seed);
}

static DIGIT_T rand_between(DIGIT_T lower, DIGIT_T upper)
/* Returns a single pseudo-random digit between lower and upper.
   Uses rand(). Assumes srand() already called. */
{
	DIGIT_T d, range;
	unsigned char *bp;
	int i, nbits;
	DIGIT_T mask;

	if (upper <= lower) return lower;
	range = upper - lower;

	do
	{
		/* Generate a random DIGIT byte-by-byte using rand() */
		bp = (unsigned char *)&d;
		for (i = 0; i < sizeof(DIGIT_T); i++)
		{
			bp[i] = rand() & 0xFF;
		}

		/* Trim to next highest bit above required range */
		mask = HIBITMASK;
		for (nbits = BITS_PER_DIGIT; nbits > 0; nbits--, mask >>= 1)
		{
			if (range & mask)
				break;
		}
		if (nbits < BITS_PER_DIGIT)
		{
			mask <<= 1;
			mask--;
		}
		else
			mask = MAX_DIGIT;

		d &= mask;

	} while (d > range); 

	return (lower + d);
}

DIGIT_T spSimpleRand(DIGIT_T lower, DIGIT_T upper)
{	/*	Returns a pseudo-random digit.
		Handles own seeding using time.
		NOT for cryptographically-secure random numbers.
		NOT thread-safe because of static variable.
		Changed in Version 2 to use internal funcs.
	*/
	static unsigned seeded = 0;

	if (!seeded)
	{
		/* seed with system time */
		rand_seed((DIGIT_T)time(NULL));
		seeded = 1;
	}
	return rand_between(lower, upper);
}


