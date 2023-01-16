/**************************************************
 * hamming:  A Hamming code encoding and decoder. *
 * Author:   Micah Baker                          *
 * Date:     14/01/2023                           *
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>

/* The bit mask at area[i] is the parity bit's respective
 * area (at bit position 2^i) of a given block. */
const uint16_t area[4] = {
	0b0101010101010101,
	0b0011001100110011,
	0b0000111100001111,
	0b0000000011111111
};

// convert an arbitrary character array to a binary array
char *to_binary(char *arr)
{
	char byte[9], *r;
	int len,
	    c,
	    i, j,
	    tmp;

	// allocate the return string

	len = strlen(arr);
	r = (char *)calloc((len * 8) + 1, sizeof(char));

	// for every character in [arr]

	c = *arr++;

	for (i = 0; c != '\0'; c = *arr++)
	{
		// read all of the bits of [c] into [byte]
		for (j = 0; j < 8; ++j)
		{
			byte[j] = '0' + (c & 0b1);
			c >>= 1;
		}

		byte[j] = '\0';

		// reverse the order of byte
		for (j = 0; j < 4; ++j)
		{
			tmp = byte[j];
			byte[j] = byte[8 - j - 1];
			byte[8 - j - 1] = tmp;
		}

		// concatenate [byte] to [r]
		strcpy(&r[i], byte);

		// increment [i] by the length of [byte]
		i += 8;
	}

	r[i] = '\0';

	return r;
}

/* If n = 2^x, and x is an integer, is_po2
 * returns x. Otherwise, is_po2 returns -1. */
int is_po2(unsigned int n)
{
	// x: the number of times [n] is shifted right
	int x = 0;

	if (n == 0)
		return -1;

	// check every bit from the right, until a 1
	while ((n & 0b1) == 0)
	{
		n >>= 1;
		++x; // increment x
	}

	/* Check if this is the only 1 in the number.
	 * If it is, then the number must be to the
	 * power of 2. If it isn't, then it cannot be. */
	return n == 1 ? x : -1;
}

// return the number of bits that are 1 in [n]
int count1s(unsigned int n)
{
	int count = 0;

	while (n != 0)
	{
		if ((n & 0b1) == 1)
			++count;

		n >>= 1;
	}

	return count;
}

// print a block as binary characters
void print_block(uint16_t block)
{
	char arr[17];
	int i, tmp;

	// read 16 bits from [block] into [arr]
	for (i = 0; i < 16; ++i)
	{
		arr[i] = '0' + (block & 0b1);
		block >>= 1;
	}

	// reverse the order of characters in [arr]
	for (i = 0; i < 8; ++i)
	{
		tmp = arr[i];
		arr[i] = arr[15 - i];
		arr[15 - i] = tmp;
	}

	// ensure string terminator exists at arr[16]
	arr[16] = '\0';

	// print the block
	printf("%s", arr);
}

// convert a binary array to a sequence of Hamming code blocks
uint16_t *to_blocks(char *binary, int *nptr)
{
	int len,
	    i, j, k,
	    c,
	    x;
	uint16_t *blocks;
	
	// allocate the correct number of blocks

	len = strlen(binary);
	*nptr = len / 11;

	if (len % 11 != 0)
		++(*nptr);

	blocks = calloc(*nptr, sizeof(uint16_t));

	k = 0;

	// for every block

	for (i = 0; i < *nptr; ++i)
	{
		// set all bits to 0
		blocks[i] = 0;

		/* j = 0 is ignored. This is for the parity of the entire block. */

		// read data characters from [binary]
		for (j = 1; j < 16; ++j)
		{
			// bits that are a power of two are reserved for the parity of rows and columns
			if (is_po2(j) != -1)
				continue;

			// get the next character in [binary]
			c = binary[k++];

			// break if at end of [binary]
			if (c == '\0')
				break;

			// write bit in [blocks[i]] at position [j] with respect to [c == '1']
			if (c == '1')
				blocks[i] |= (0b1 << (15 - j));
		}

		// set the parity bits
		for (j = 1; j < 16; ++j)
		{
			x = is_po2(j);

			// skip non parity bits
			if (x == -1)
				continue;

			/* For j < 16, x = 0,1,2,3. area[x] is a bit
			 * mask of these parity bit's respective areas. */

			// if the number of 1s in this parity bit's area is odd
			if ((count1s(blocks[i] & area[x]) % 2) == 1)
				// set this parity bit to 1
				blocks[i] |= (0b1 << (15 - j));

		}

		// if the number of 1s in this block is odd
		if ((count1s(blocks[i]) % 2) == 1)
			// set the block's parity bit to 1
			blocks[i] |= (0b1 << 15);
	}

	return blocks;
}

// read blocks from [stdin]
uint16_t *read_blocks(int *nptr)
{
	/* Arbitrary maximum of 1024.
	 * Increase if necessary. */
	char raw[1024];
	uint16_t *r;
	int c, n,
	    i, j;

	c = getc(stdin);

	for (i = 0; c != '\n' && c != EOF; c = getc(stdin))
		raw[i++] = c;

	raw[i] = '\0';

	// assumes there are no separating characters, and block size is 16 bits
	n = i / 16;
	*nptr = n;

	r = calloc(n, sizeof(uint16_t));

	// prepare first block

	r[0] = 0;
	i = -1; // set [i] to -1 because [j] will incrememnt [i] on first loop
	j = 0;

	// for every character in [raw]

	c = raw[j++];

	for (; c != '\0'; c = raw[j++])
	{
		// incrememnt [i] for every new block
		if ((j - 1) % 16 == 0)
			++i;

		// assume that [c] is either 0 or 1

		r[i] <<= 1; // shift bits left
		r[i] |= c - '0'; // read the next bit
	}

	return r;
}

// repair [n] blocks from [blocks] and return the data as text
char *decode_blocks(uint16_t *blocks, int n)
{
	char *binary, *r;
	int i, j, k,
	    sz,
	    x;
	uint16_t bit;

	// there are 11 data bits for every block
	binary = calloc(n * 11, sizeof(char));

	k = 0;

	// for every block
	for (i = 0; i < n; ++i)
	{
		// block is damaged
		if ((count1s(blocks[i]) % 2) != 0)
		{
			// repair the block

			bit = ~(uint16_t)0;

			// for every bit in the block
			for (j = 1; j < 16; ++j)
			{
				x = is_po2(j);

				if (x == -1)
					// skip bits that are not a power of two
					continue;

				// this bit is a parity bit

				if ((count1s(blocks[i] & area[x]) % 2) != 0)
					// if the count of 1s in this area isn't even,
					// then the damaged bit is in this area
					bit &= area[x];
				else
					// if the count of 1s in this area is even,
					// then the damaged bit is not in this area
					bit -= bit & area[x];
			}

			// ^ the block at this bit such that it will be switched
			blocks[i] ^= bit;
		}

		// for every bit in this block
		for (j = 1; j < 16; ++j)
		{
			// skip parity bits
			if (is_po2(j) != -1)
				continue;

			// read this data bit into [binary]
			binary[k++] = '0' + ((blocks[i] >> (15 - j)) & 0b1);
		}
	}

	// remove redundant bits
	sz = k - k % 8;

	binary[sz] = '\0';

	// allocate the return string
	
	r = calloc((k / 8) + 1, sizeof(char));

	// for every byte in [binary]
	for (i = 0, k = 0; i < sz; i += 8, ++k)
	{
		// ensure all bits are 0
		r[k] = 0;

		// for each bit in this byte
		for (j = 0; j < 8; ++j)
		{
			// write it to the character [r[k]]

			r[k] <<= 1;
			r[k] |= binary[i + j] - '0';
		}
	}

	r[k] = '\0';

	// free [binary]
	free(binary);

	return r;
}

void damage_blocks(uint16_t *blocks, int n)
{
	int i;

	// for every block in [blocks]
	for (i = 0; i < n; ++i)
		// switch a random bit
		blocks[i] ^= 0b1 << (rand() % 16);
}

int main(int argc, char **argv)
{
	char *binary, *text;
	uint16_t *blocks;
	int i, n;

	srand(time(0));

	if (argc == 1)
	{
		printf("Nothing to do.\n");

		return EXIT_SUCCESS;
	}

	if (strcmp(argv[1], "encode") == 0)
	{
		if (argc < 3)
		{
			printf("Insufficient arguments. Please provide a string to encode.\n");

			return EXIT_FAILURE;
		}

		binary = to_binary(argv[2]);
		blocks = to_blocks(binary, &n);

		damage_blocks(blocks, n);

		// print every block
		for (i = 0; i < n; ++i)
			print_block(blocks[i]);

		putc('\n', stdout);

		free(binary);
		free(blocks);
	} else
	if (strcmp(argv[1], "decode") == 0)
	{
		printf("Please enter your blocks.\n> ");

		blocks = read_blocks(&n);
		text = decode_blocks(blocks, n);

		printf("%s\n", text);

		free(blocks);
		free(text);
	} else {
		printf("Unknown operation %s.\n", argv[1]);

		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
