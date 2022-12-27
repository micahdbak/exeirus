/*******************************
 * strcon:  A string Xcrypter. *
 * Author:  Micah Baker        *
 * Date:    26/12/2022         *
 *******************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "charsets.h"

#undef DEBUG

#define NAN  (~0)

#define ARRSIZE  1024
#define DECSIZE  (ARRSIZE / sizeof(int))

enum base {
	INVALID = -1,
	BINARY  = 0,
	OCTAL   = 1,
	DECIMAL = 2,
	HEX     = 3,
	TEXT    = 4
};

#define CAESAR    5
#define VIGENERE  6

void print_help(FILE *file)
{
	fprintf(file,
		"strcon: A string encrypter/decrypter.\n\n"

		"Usage: strcon [character-set] [mode] [input-text] [base] [cipher] ...\n\n"

		"\t[character-set]\n"
		"\t\tThis is a string containing the characters that should be\n"
		"\t\tconsidered, and modified. When a cipher is applied to some\n"
		"\t\tinput, if the addition of an integer changes a character\n"
		"\t\tto reside outside of this character-set, it will loop back\n"
		"\t\tto the beginning.\n\n"

		"\t[mode]: [encode/decode]\n"
		"\t\tThe mode must be specified. This lets the program know if\n"
		"\t\tit is to [encrypt] or [decrypt] the input-text.\n\n"

		"\t[input-text]\n"
		"\t\tThis is the text that strcon will work on.\n\n"

		"\t[base]: [binary/octal/decimal/hexadecimal]\n"
		"\t\tIf the mode is [encrypt], then [base] refers to the\n"
		"\t\tnumerical base of the output text.\n\n"

		"\t\tIf the mode is [decrypt], then [base] refers to the\n"
		"\t\tnumerical base of the input text. When decrypting, the\n"
		"\t\toutput text will always be output as ASCII text.\n\n"

		"\t[cipher]: [none/caesar/vigenere]\n"
		"\t\tIf [cipher] is set to [none], then no cipher will be\n"
		"\t\tapplied to the [input-text], and the argument-list can\n"
		"\t\tbe terminated proceeding [none].\n\n"

		"\t\tIf [cipher] is set to [caesar], then a number must be\n"
		"\t\tprovided specifying the shift value.\n\n"

		"\t\tIf [cipher] is set to [vigenere], then a string must be\n"
		"\t\tprovided specifying the shift string.\n\n"

		"\t\tBoth of the additional arguments for [caesar] or [vigenere]\n"
		"\t\tciphers must be provided directly following [cipher].\n\n"

		"\t\tIf the mode is [encrypt], then [caesar] will add [shift]\n"
		"\t\tto each character of [input-text]. Likewise, [vigenere]\n"
		"\t\twill add each character of [vigenere-text] to [input-text].\n"
		"\t\tIf the mode is [decrypt], then these ciphers will subtract\n"
		"\t\tinstead of add.\n");
}

// Print one byte of (byte) as binary.
void print_binary(int byte)
{
	// string that stores the binary
	char binary[9];
	int i;

	// for eight bits of (byte), from the right of (binary)
	for (i = 7; i >= 0; --i)
	{
		// set index (i) of (binary) to be the rightmost bit of (byte)
		binary[i] = byte & 1 ? '1' : '0';
		// shift (byte) one bit right and discard rightmost bit
		byte >>= 1;
	}

	// terminate the string
	binary[8] = '\0';

	printf("%s", binary);
}

/* Returns the index of (arr) in a list of strings.
 * Returns -1 if (arr) isn't contained in the list. */
int for_arr(char *arr, int ncmp, ...)
{
	va_list ap;  // argument list pointer
	int i;       // index in (arr)
	char *cmp;   // string to compare

	va_start(ap, ncmp);

	// for every string in the list
	for (i = 0; i < ncmp; ++i)
	{
		cmp = va_arg(ap, char *);

		// break if they are the same
		if (strcmp(arr, cmp) == 0)
			break;
	}

	va_end(ap);

	/* return -1 if (arr) wasn't contained in the lest,
	 * otherwise return the index of the matched string. */
	return i == ncmp ? -1 : i;
}

/* Read a field of base (base) as an integer and return it.
 * This assumes that (arr) contains several fields, so (pos)
 * is the current position in (arr). When (field_to_int) is
 * finished, it will update (pos) to be at the end of the
 * just read field. */
int field_to_int(char *arr, enum base base, int *pos)
{
	int mul,        // multiplier
	    i,          // index in (arr)
	    c,          // character in (arr)
	    val, cval;  // return (val), character value (cval).

	// set the multiplier based on (base)
	switch (base)
	{
	case BINARY:	mul = 2;	break;
	case OCTAL:	mul = 8;	break;
	case DECIMAL:	mul = 10;	break;
	case HEX:	mul = 16;	break;
	default:
		printf("ERROR:\tInvalid base given to (field_to_int).\n");

		// return 0 on invalid (base)
		return 0;
	}

	val = 0;

	c = arr[i++];

	for (i = 0; !isspace(c) && c != '\0'; c = arr[i++])
	{
		switch (base)
		{
		// binary, octal, decimal only use digits
		case BINARY:
		case OCTAL:
		case DECIMAL:
			cval = c - '0';

			break;
		// hexadecimal has alphabetic characters
		case HEX:
			if (isalpha(c))
				cval = 10 + c - 'a';
			else
				cval = c - '0';

			break;
		default:

			break;
		}

		val = val * mul + cval;
	}

	// skip remaining white-space
	while (c != '\0' && isspace(c))
		c = arr[i++];

	// don't skip non-white-space
	--i;

	// update the position in (arr)
	*pos = *pos + i;

	return val;
}

// returns the first index of (c) in (arr)
int cpos(char c, char *arr)
{
	int i;  // index in (arr)

	for (i = 0; arr[i] != '\0'; ++i)
		// break when (c) is found
		if (c == arr[i])
			break;

	// return -1 if (c) wasn't found in (arr)
	return arr[i] == '\0' ? -1 : i;
}

// print error message and exit
void fail(char *error)
{
	printf("ERROR:\t%s", error);
	print_help(stderr);

	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	char charset[ARRSIZE],     // charset: character set
	     arr[ARRSIZE],         // arr: input text
	     vigenere[ARRSIZE];    // vigenere: vigenere shift string
	int argi,                  // argi: index of the current argument being parsed
	    c,                     // c: character in a string
	    decode[DECSIZE],       // decode: array of integers to decode into text
	    nchars, chari, charj,  // nchars: number of characters in the character set,
				   // chari: index of a character in the character set,
				   // charj: secondary occurrence of (chari).
	    i, j,                  // i, j: indices used in for loops
	    shift,                 // shift: caesar cipher shift value
	    ascii;                 // ASCII: character set is ascii
	enum mode {
		UNSET = -1,
		ENCODE = 0,
		DECODE = 1
	} mode;          // mode
	int choice;      // choice
	enum base base;  // base

	argi = 1;

	// print help given no arguments
	if (argi == argc)
	{
		print_help(stderr);

		exit(EXIT_SUCCESS);
	}

	// get the character set
	strcpy(charset, argv[argi++]);

	// character set by default is not ASCII
	ascii = 0;

	// check if the provided character set is actually a request for a predetermined one
	switch (for_arr(charset, 10,
	                "--64",   // CS_64
			"--u32",  // CS_32
			"--l32",  // cs_32
			"--abc",  // cs_26
			"--hex",  // CS_hex
			"--dec",  // CS_dec
			"--oct",  // CS_oct
			"--bin",  // CS_bin
			"--AN",   // CS_AN
			"--ascii"))
	{
	// switch for the option
	case 0:  strcpy(charset, CS_64);   break;
	case 1:  strcpy(charset, CS_32);   break;
	case 2:  strcpy(charset, cs_32);   break;
	case 3:  strcpy(charset, cs_26);   break;
	case 4:  strcpy(charset, CS_hex);  break;
	case 5:  strcpy(charset, CS_dec);  break;
	case 6:  strcpy(charset, CS_oct);  break;
	case 7:  strcpy(charset, CS_bin);  break;
	case 8:  strcpy(charset, CS_AN);   break;
	case 9:
		 // set the character set to ASCII
		 ascii = 1;

		 break;
	default:
		 // treat (charset) as the character set

		 break;
	}

	// set (nchars) to the length of (charset)
	nchars = strlen(charset);

	if (argi == argc)
		fail("Not enough arguments provided.\n\n");

	// get the whether strcon is to encode or decode
	mode = for_arr(argv[argi++], 2, "encode", "decode");

	if (mode == UNSET)
		fail("Mode is unset. Exiting.\n\n");

#ifdef DEBUG
	fprintf(stderr, "%s\n", mode == ENCODE ? "Encoding." : "Decoding.");
#endif

	if (argi == argc)
		fail("Not enough arguments provided.\n\n");

	// read input text
	strcpy(arr, argv[argi++]);
#ifdef DEBUG
	fprintf(stderr, "Got input text \"%s\".\n\n", arr);
#endif

	if (argi == argc)
		fail("Not enough arguments provided.\n\n");

	// read the base
	base = for_arr(argv[argi++], 5, "binary", "octal", "decimal", "hexadecimal", "text");
#ifdef DEBUG
	fprintf(stderr, "\t(Of output if encoding, of input if decoding.)\n"
	                "\t0:  Binary      (0,1)\n"
	                "\t1:  Octal       (0-7)\n"
	                "\t2:  Decimal     (0-9)\n"
	                "\t3:  Hexademical (0-9,a-f)\n"
			"\t4:  Text\n"
	                "Got base type %d.\n\n", base);
#endif

	if (base == INVALID)
		fail("Invalid base.\n");

	if (argi == argc)
		fail("Not enough arguments provided.\n\n");

	// read the cipher
	choice = for_arr(argv[argi++], 3, "none", "caesar", "vigenere");
#ifdef DEBUG
	fprintf(stderr, "\t(Encoding will add, decoding will subtract.)\n"
	                "\t0:  Nothing\n"
	                "\t1:  Caesar Cipher\n"
	                "\t2:  Vigenere Cipher\n"
	                "Got cipher type %d.\n\n", choice);
#endif

	if (choice == -1)
		fail("Invalid choice.\n");

	// if character set is ASCII, do not perform any ciphers
	if (choice == 0 || ascii)
		choice = base;
	else
		choice = choice == 1 ? CAESAR : VIGENERE;

	// if strcon is decoding, and the input is of a numerical sort...
	if (mode == DECODE && base != TEXT)
	{
		// ...prepare a list of integer values from the input
		for (i = 0, j = 0; arr[j] != '\0';)
			decode[i++] = field_to_int(&arr[j], base, &j);

		// terminate the list with (NAN)
		decode[i] = NAN;
	}

	// operate on the input text
	switch (choice)
	{
	// caesar cipher
	case CAESAR:
		if (argi == argc)
			fail("Not enough arguments provided.\n\n");

		// read the shift value for the caesar cipher
		sscanf(argv[argi++], "%d", &shift);
	#ifdef DEBUG
		fprintf(stderr, "Got caesar shift %d.\n\n", shift);
	#endif

		// encode the input text
		if (mode == ENCODE)
		{
			// for every character in (arr)
			for (i = 0; arr[i] != '\0'; ++i)
			{
				// set (chari) to be the position of (arr[i]) in (charset)
				chari = cpos(arr[i], charset);

				// if (arr[i]) isn't in [charset]...
				if (chari == -1)
				{
					// ...print it anyways if the output will be text
					if (base == TEXT)
						putc(arr[i], stdout);

					// continue to the next character
					continue;
				}

				// (arr[i]) is in [charset]

				// add the shift value to (chari)
				chari += shift;
				// ensure that (chari) is from 0 to (nchars)
				chari %= nchars;

				// print (chari) depending on the output format
				switch (base)
				{
				case BINARY:
					print_binary(chari);
					putc(' ', stdout);

					break;
				case OCTAL:
					printf("%o ", chari);

					break;
				case DECIMAL:
					printf("%d ", chari);

					break;
				case HEX:
					printf("%x ", chari);

					break;
				case TEXT:
					putc(charset[chari], stdout);

					break;
				default:

					break;
				}
			}
		// decode the input text
		} else {
			// if the input is text
			if (base == TEXT)
			{
				// for every character in (arr)
				for (i = 0; arr[i] != '\0'; ++i)
				{
					// set (chari) to be the position of (arr[i]) in (charset)
					chari = cpos(arr[i], charset);

					// if (chari) isn't in (charset)...
					if (chari == -1)
					{
						// ...print it as-is
						putc(arr[i], stdout);

						// continue to the next character
						continue;
					}

					// subtract the shift value from (chari)
					chari -= shift;
					// ensure that (chari) is between -(nchars) and (nchars)
					chari %= nchars;

					// if (chari) is less than zero...
					if (chari < 0)
						// add (nchars) to it to get the equivalent positive value
						chari += nchars;

					// print the (chari) character of (charset)
					putc(charset[chari], stdout);
				}
			} else {
				// for every integer in the decode list
				for (i = 0; decode[i] != NAN; ++i)
				{
					// set (chari) to (decode[i])
					chari = decode[i];
					// subtract the shift value from (chari)
					chari -= shift;
					// ensure that (chari) is between -(nchars) and (nchars)
					chari %= nchars;

					// if (chari) is less than zero...
					if (chari < 0)
						// add (nchars) to it to get the equivalent positive value
						chari += nchars;

					// print the (chari) character of (charset)
					putc(charset[chari], stdout);
				}
			}
		}

		putc('\n', stdout);

		break;
	case VIGENERE:
		if (argi == argc)
			fail("Not enough arguments provided.\n");

		// read vigenere shift string
		strcpy(vigenere, argv[argi++]);
	#ifdef DEBUG
		fprintf(stderr, "Got vigenere string \"%s\".\n\n", vigenere);
	#endif

		if (mode == ENCODE)
		{
			// for every character in (arr[i])
			for (i = 0, j = 0; arr[i] != '\0'; ++i)
			{
				// set (chari) to be the position of (arr[i]) in (charset)
				chari = cpos(arr[i], charset);

				// if (arr[i]) wasn't in (charset)...
				if (chari < 0)
				{
					// ...print (arr[i]) if the output is text
					if (base == TEXT)
						putc(arr[i], stdout);

					// continue to the next character
					continue;
				}

				if (vigenere[j] == '\0')
					j = 0;

				// this assumes that the shift string contains only characters contained in (charset)
				charj = cpos(vigenere[j++], charset);

				// add (charj) to (chari)
				chari += charj;
				// ensure that (chari) is within the bounds 0 to (nchars)
				chari %= nchars;

				// print the value of (chari)
				switch (base)
				{
				case BINARY:
					print_binary(chari);
					putc(' ', stdout);

					break;
				case OCTAL:
					printf("%o ", chari);

					break;
				case DECIMAL:
					printf("%d ", chari);

					break;
				case HEX:
					printf("%x ", chari);

					break;
				case TEXT:
					putc(charset[chari], stdout);

					break;
				default:

					break;
				}
			}
		} else {
			// decode text
			if (base == TEXT)
			{
				// for every character in (arr[i])
				for (i = 0, j = 0; arr[i] != '\0'; ++i)
				{
					// set (chari) to be the position of (arr[i]) in (charset)
					chari = cpos(arr[i], charset);

					// if (arr[i]) isn't contained in (charset)...
					if (chari < 0)
					{
						// ...print (arr[i])
						putc(arr[i], stdout);

						// continue to the next character
						continue;
					}

					if (vigenere[j] == '\0')
						j = 0;

					// this assumes that the shift string contains only characters contained in (charset)
					charj = cpos(vigenere[j++], charset);

					// subtract (charj) from (chari)
					chari -= charj;
					// ensure that (chari) is from -(nchars) to (nchars)
					chari %= nchars;

					// if (chari) is less than zero...
					if (chari < 0)
						// ...set it to its equivalent positive form
						chari += nchars;

					// print the character in (charset) at (chari)
					putc(charset[chari], stdout);
				}
			} else {
				// for every integer in the decode list
				for (i = 0, j = 0; decode[i] != NAN; ++i)
				{
					// set (chari) to the value of (decode[i])
					chari = decode[i];

					if (vigenere[j] == '\0')
						j = 0;

					// this assumes that every character in the shift string is found in (charset)
					charj = cpos(vigenere[j++], charset);

					// subtract (charj) from (chari)
					chari -= charj;
					// ensure that (chari) is from -(nchars) to (nchars)
					chari %= nchars;

					// if chari is less than zero...
					if (chari < 0)
						// ...set it to its equivalent positive form
						chari += nchars;

					// print the character in (charset) at (chari)
					putc(charset[chari], stdout);
				}
			}
		}

		putc('\n', stdout);

		break;
	default:
		if (mode == ENCODE)
			for (i = 0; arr[i] != '\0'; ++i)
			{
				// custom character set
				if (!ascii)
				{
					// set (chari) to the position of (arr[i]) in (charset)
					chari = cpos(arr[i], charset);

					// if (arr[i]) isn't in (charset)
					if (chari < 0)
					{
						// print it if the output is text
						if (base == TEXT)
							putc(arr[i], stdout);

						// continue to the next character
						continue;
					}
				// ASCII character set
				} else
					// (chari) is the integer value of (arr[i])
					chari = arr[i];

				// print (chari) with respect to the output base
				switch (base)
				{
				case BINARY:
					print_binary(chari);
					putc(' ', stdout);

					break;
				case OCTAL:
					printf("%o ", chari);

					break;
				case DECIMAL:
					printf("%d ", chari);

					break;
				case HEX:
					printf("%x ", chari);

					break;
				case TEXT:
					putc(charset[chari], stdout);

					break;
				default:

					break;
				}
			}
		else {
			if (base == TEXT)
				// decoding text without a cipher is redundant
				printf("%s", arr);
			else {
				// for every integer in the decode list
				for (i = 0; decode[i] != NAN; ++i)
				{
					// custom character set
					if (!ascii)
					{
						// set (chari) to (decode[i])
						chari = decode[i];

						// print the character at (chari) in (charset)
						putc(charset[chari], stdout);
					// ASCII character set
					} else
						putc(decode[i], stdout);
				}
			}
		}

		putc('\n', stdout);

		break;
	}

	return EXIT_SUCCESS;
}
