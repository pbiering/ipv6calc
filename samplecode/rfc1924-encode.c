/*
 * Base 85 (rfc1924) encodings of IPv6 addresses
 */

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>

extern char *strchr();

typedef struct {
	unsigned char b[128];		/* never anything but 0 or 1 */
} bitvec;

char ChSet[86] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '!', '#', '$', '%', '&', '(', ')', '*',
	'+', '-', ';', '<', '=', '>', '?', '@', '^', '_',
	'`', '{', '|', '}', '~',
	0
};

bitvec
lsl(bv, n)
	bitvec bv;
	int n;
{
	register i;
	bitvec r;

	for (i = 128; --i >= 128-n; )
		r.b[i] = 0;

	while (i >= 0) {
		r.b[i] = bv.b[i + n];
		i--;
	}

	return r;
}

bitvec
lsr(bv, n)
	bitvec bv;
	int n;
{
	register i;
	bitvec r;

	for (i = 0; i < n; i++)
		r.b[i] = 0;

	while (i < 128) {
		r.b[i] = bv.b[i - n];
		i++;
	}

	return r;
}

bitvec
asr(bv, n)
	bitvec bv;
	int n;
{
	register i;
	bitvec r;

	for (i = 0; i < n; i++)
		r.b[i] = bv.b[0];

	while (i < 128) {
		r.b[i] = bv.b[i - n];
		i++;
	}

	return r;
}

int
topbit(bv)
	bitvec bv;
{
	return bv.b[0];
}

int
botbit(bv)
	bitvec bv;
{
	return bv.b[127];
}

void
settop(bv, v)
	bitvec *bv;
	int v;
{
	bv->b[0] = v;
}

void
setbot(bv, v)
	bitvec *bv;
	int v;
{
	bv->b[127] = v;
}

bitvec
zbv()
{
	bitvec r;
	register i;

	for (i = 0; i < 128; i++)
		r.b[i] = 0;
	return r;
}

bitvec
addbv(a, b)
	bitvec a, b;
{
	register i, j;
	bitvec r;

	j = 0;
	for (i = 128; --i >= 0; ) {
		j += a.b[i] + b.b[i];
		r.b[i] = j & 1;
		j >>= 1;
	}

	return r;
}

bitvec
subbv(a, b)
	bitvec a, b;
{
	register i;
	register unsigned j;
	register unsigned k;
	bitvec r;

	j = 0;
	for (i = 128; --i >= 0; ) {
		k = (a.b[i] ^ b.b[i] ^ j) & 1;
		j = ~j & ~a.b[i] & b.b[i] | j & ~a.b[i] | j & b.b[i];
		r.b[i] = k;
	}
	return r;
}

int
cmpbv(a, b)
	bitvec a, b;
{
	register i, j;

	for (i = 0; i < 128; i++) {
		j = (int)a.b[i] - (int)b.b[i];
		if (j != 0)
			break;
	}
	return j;
}

bitvec
bvx85(bv)
	bitvec bv;
{
	/* 85 == 64 + 16 + 4 + 1 */
	return addbv(lsl(bv, 6), addbv(lsl(bv, 4), addbv(lsl(bv, 2), bv)));
}

bitvec
bvdiv85(bv, rem)
	bitvec bv;
	int *rem;
{
	bitvec e5;
	bitvec div;
	bitvec r;
	register i, j;

	r = zbv();
	setbot(&r, 1);
	e5 = addbv(lsl(r, 6), addbv(lsl(r, 4), addbv(lsl(r, 2), r)));


	div = lsl(e5, 121);		/* e5 is 7 bits, 121 + 7 == 128 */


	r = zbv();

	for (i = 0; i < 122; i++) {
		if (cmpbv(bv, div) < 0) {
			div = lsr(div, 1);
			r = lsl(r, 1);
			continue;
		}
		bv = subbv(bv, div);
		div = lsr(div, 1);
		r = lsl(r, 1);
		setbot(&r, 1);
	}

	j = 0;
	bv = lsl(bv, 121);

	for (i = 0; i < 7; i++) {
		j <<= 1;
		j |= topbit(bv);
		bv = lsl(bv, 1);
	}
	if (rem)
		*rem = j;

	return r;
}

bitvec
smallbv(n)
	register int n;
{
	bitvec r, t;
	register i;

	r = zbv();

	i = 0;
	while (n != 0) {
		t = lsl(t, 1);
		setbot(&t, n & 1);
		n >>= 1;
		i++;
	}

	while (--i >= 0) {
		r = lsl(r, 1);
		setbot(&r, botbit(t));
		t = lsr(t, 1);
	}
	return r;
}

bitvec
unpk85(str)
	register char *str;
{
	bitvec r;
	register char c;
	register int i, j;
	register char *p;

	r = zbv();
	for (i = 0; i < 20; i++) {
		c = *str++;
		if (c == 0 || (p = strchr(ChSet, c)) == 0)
			return zbv();
		r = addbv(bvx85(r), smallbv(p - ChSet));
	}

	return (r);
}

char *
pk85(bv)
	bitvec bv;
{
	int vals[20];
	register i;
	static char str[21];
	register char *p;

	for (i = 0; i < 20; i++)
		bv = bvdiv85(bv, &vals[i]);

	p = str;
	while (--i >= 0)
		*p++ = ChSet[vals[i]];
	*p = '\0';

	return str;
}

bitvec
hex2bv(str)
	char *str;
{
	register char c;
	register v;
	bitvec r;

	r = zbv();
	while (c = *str++) {
		if (!isascii(c) || !isxdigit(c))
			continue;
		if (c <= '9')
			v = c - '0';
		else if (c <= 'Z')
			v = c - 'A' + 10;
		else
			v = c - 'a' + 10;

		r = addbv(lsl(r, 4), smallbv(v));
	}
	return r;
}

char *
bv2hex(bv)
	bitvec bv;
{
	register i, j, k;
	static char str[128/4 + 1];
	char *p = str;

	for (i = 0; i < 128; i += 4) {
		k = 0;
		for (j = 0; j < 4; j++) {
			k <<= 1;
			k |= topbit(bv);
			bv = lsl(bv, 1);
		}
		if (k < 10)
			*p++ = '0' + k;
		else
			*p++ = 'A' + k - 10;
	}

	return str;
}

main()
{
	char buf[128];
	bitvec bv;

	/*
	 * reads (using gets() so absurdly bug prone)
	 * either a [20-rfc1924-character] string, or
	 * a 32 hex (06E5222FDB4C8E8DB1AFF3152F9CC60C) string, with
	 * or without punctuation (it is ignored) - but no IPv6
	 * encoding shortcuts (no :: nor elided 0's), and prints
	 * the value read, and its conversion to the other form.
	 *
	 * Non-hex digits are simply ignored when reading hex,
	 * Anything but a 20 char string of rfc1924 chars will
	 * cause 00000000000000000000000000000000 to be returned
	 * as the result (as would [00000000000000000000])
	 */

	while (gets(buf)) {
		if (buf[0] == '[') {
			bv = unpk85(buf + 1);
			printf("%s = %s\n", buf, bv2hex(bv));
		} else {
			bv = hex2bv(buf);
			printf("%s = [%s]\n", buf, pk85(bv));
		}
	}
	exit (0);
}
