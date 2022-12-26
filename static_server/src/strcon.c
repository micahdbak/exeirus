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

#define NAN  (~0)

#define ARRSIZE  1024
#define DECSIZE  (ARRSIZE / sizeof(int))

enum base {
	INVALID = -1,
	BINARY  = 0,
	OCTAL   = 1,
	DECIMAL = 2,
	HEX     = 3
};

#define CAESAR    4
#define VIGENERE  5

void print_help(FILE *file)
{
	fprintf(file,
		"strcon: A string encrypter/decrypter.\n\n"

		"Usage: strcon [mode] [input-text] [base] [cipher] ...\n\n"

		"\t[mode]: [encrypt/decrypt]\n"
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

void print_binary(int byte)
{
	char binary[9];
	int i;

	for (i = 7; i >= 0; --i)
	{
		binary[i] = byte & 1 ? '1' : '0';
		byte >>= 1;
	}

	binary[8] = '\0';

	printf("%s", binary);
}

int for_arr(char *arr, int ncmp, ...)
{
	va_list ap;
	int i;
	char *cmp;

	va_start(ap, ncmp);

	for (i = 0; i < ncmp; ++i)
	{
		cmp = va_arg(ap, char *);

		if (strcmp(arr, cmp) == 0)
			break;
	}

	va_end(ap);

	return i == ncmp ? -1 : i;
}

int field_to_int(char *arr, enum base base, int *pos)
{
	int mul, i, val, cval, c;

	switch (base)
	{
	case BINARY:	mul = 2;	break;
	case OCTAL:	mul = 8;	break;
	case DECIMAL:	mul = 10;	break;
	case HEX:	mul = 16;	break;
	default:
		printf("Invalid base provided to (field_to_int).\n");

		break;
	}

	i = 0;
	val = 0;

	c = arr[i++];

	for (; !isspace(c) && c != '\0'; c = arr[i++])
	{
		switch (base)
		{
		case BINARY:
		case OCTAL:
		case DECIMAL:
			cval = c - '0';

			break;
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

	while (c != '\0' && isspace(c))
		c = arr[i++];

	--i;

	*pos = *pos + i;

	return val;
}

int main(int argc, char **argv)
{
	char arr[ARRSIZE],
	     vigenere[ARRSIZE];
	int argi,
	    c,
	    decode[DECSIZE],
	    i, j,
	    val;
	enum direction { UNSET = -1, ENCODE = 0, DECODE = 1 } direction;
	int choice;
	enum base base;

	argi = 1;

	// print help on no arguments
	if (argi == argc)
	{
		print_help(stderr);

		exit(EXIT_SUCCESS);
	}

	direction = for_arr(argv[argi++], 2, "encode", "decode");

	if (direction == UNSET)
	{
		fprintf(stderr, "ERROR:\tMode is unset. Exiting.\n\n");
		print_help(stderr);

		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "%s\n", direction == ENCODE ? "Encoding." : "Decoding.");

	if (argi == argc)
	{
		fprintf(stderr, "ERROR:\tNot enough arguments provided.\n\n");
		print_help(stderr);

		exit(EXIT_FAILURE);
	}

	strcpy(arr, argv[argi++]);
	fprintf(stderr, "Got input text \"%s\".\n\n", arr);

	if (argi == argc)
	{
		fprintf(stderr, "ERROR:\tNot enough arguments provided.\n\n");
		print_help(stderr);

		exit(EXIT_FAILURE);
	}

	base = for_arr(argv[argi++], 4, "binary", "octal", "decimal", "hexadecimal");
	fprintf(stderr, "\t(Of output if encoding, of input if decoding.)\n"
	                "\t0:  Binary      (0,1)\n"
	                "\t1:  Octal       (0-7)\n"
	                "\t2:  Decimal     (0-9)\n"
	                "\t3:  Hexademical (0-9,a-f)\n"
	                "Got base type %d.\n\n", base);

	if (base == INVALID)
	{
		fprintf(stderr, "ERROR:\tInvalid base.\n");
		print_help(stderr);

		exit(EXIT_FAILURE);
	}

	if (argi == argc)
	{
		fprintf(stderr, "ERROR:\tNot enough arguments provided.\n\n");
		print_help(stderr);

		exit(EXIT_FAILURE);
	}

	choice = for_arr(argv[argi++], 3, "none", "caesar", "vigenere");
	fprintf(stderr, "\t(Encoding will add, decoding will subtract.)\n"
	                "\t0:  Nothing\n"
	                "\t1:  Caesar Cipher\n"
	                "\t2:  Vigenere Cipher\n"
	                "Got cipher type %d.\n\n", choice);

	if (choice == -1)
	{
		fprintf(stderr, "ERROR:\tInvalid choice.\n");
		print_help(stderr);

		exit(EXIT_FAILURE);
	}

	if (choice == 0)
		choice = base;
	else
		choice = choice == 1 ? CAESAR : VIGENERE;

	if (c != '\n')
		printf("err\n");

	if (direction == DECODE)
	{
		for (i = 0, j = 0; arr[j] != '\0';)
			decode[i++] = field_to_int(&arr[j], base, &j);

		decode[i] = NAN;
	}

	switch (choice)
	{
	case CAESAR:
		if (argi == argc)
		{
			fprintf(stderr, "ERROR:\tNot enough arguments provided.\n\n");
			print_help(stderr);

			exit(EXIT_FAILURE);
		}

		sscanf(argv[argi++], "%d", &val);
		fprintf(stderr, "Got caesar shift %d.\n\n", val);

		if (direction == ENCODE)
		{
			for (i = 0; arr[i] != '\0'; ++i)
			{
				switch (base)
				{
				case BINARY:
					print_binary((int)arr[i] + val);
					putc(' ', stdout);

					break;
				case OCTAL:
					printf("%o ", (int)arr[i] + val);

					break;
				case DECIMAL:
					printf("%d ", (int)arr[i] + val);

					break;
				case HEX:
					printf("%x ", (int)arr[i] + val);

					break;
				default:

					break;
				}
			}
		} else
			for (i = 0; decode[i] != NAN; ++i)
				putc(decode[i] - val, stdout);


		putc('\n', stdout);

		break;
	case VIGENERE:
		if (argi == argc)
		{
			fprintf(stderr, "ERROR:\tNot enough arguments provided.\n\n");
			print_help(stderr);

			exit(EXIT_FAILURE);
		}

		strcpy(vigenere, argv[argi++]);
		fprintf(stderr, "Got vigenere string \"%s\".\n\n", vigenere);

		if (direction == ENCODE)
		{
			for (i = 0, j = 0; arr[i] != '\0'; ++i, ++j)
			{
				if (vigenere[j] == '\0')
					j = 0;

				switch (base)
				{
				case BINARY:
					print_binary((int)arr[i] + (int)vigenere[j]);
					putc(' ', stdout);

					break;
				case OCTAL:
					printf("%o ", (int)arr[i] + (int)vigenere[j]);

					break;
				case DECIMAL:
					printf("%d ", (int)arr[i] + (int)vigenere[j]);

					break;
				case HEX:
					printf("%x ", (int)arr[i] + (int)vigenere[j]);

					break;
				default:

					break;
				}
			}
		} else
			for (i = 0, j = 0; decode[i] != NAN; ++i, ++j)
			{
				if (vigenere[j] == '\0')
					j = 0;

				putc(decode[i] - (int)vigenere[j], stdout);
			}


		putc('\n', stdout);

		break;
	default:
		if (direction == ENCODE)
			for (i = 0; arr[i] != '\0'; ++i)
			{
				switch (base)
				{
				case BINARY:
					print_binary((int)arr[i]);
					putc(' ', stdout);

					break;
				case OCTAL:
					printf("%o ", (int)arr[i]);

					break;
				case DECIMAL:
					printf("%d ", (int)arr[i]);

					break;
				case HEX:
					printf("%x ", (int)arr[i]);

					break;
				default:

					break;
				}
			}
		else
			for (i = 0; decode[i] != NAN; ++i)
				putc((char)decode[i], stdout);

		putc('\n', stdout);

		break;
	}

	return EXIT_SUCCESS;
}
