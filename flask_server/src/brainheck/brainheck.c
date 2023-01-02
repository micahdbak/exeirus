#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// arbitrary, change if necessary
#define PROGRAMSIZE  1024

struct list
{
	int value;
	struct list *next;
	struct list *prev;
};

int contains(char *arr, char c)
{
	for (; *arr != '\0' && *arr != c; ++arr)
		;

	return *arr == c;
}

int main(void)
{
	char program[PROGRAMSIZE];
	struct list *list, *node;
	int c,
	    i, j,
	    value,
	    bstart, bi;

	/*
	for (i = 0; i < 128; ++i)
		printf("%d: %c\n", i, i);
	*/

	c = getc(stdin);

	for (i = 0; c != EOF; c = getc(stdin))
	{
		if (!contains("><}{+-.,!|", c) && !isdigit(c))
			continue;

		program[i++] = c;
	}

	program[i] = '\0';

	for (i = 0; program[i] != '\0'; ++i)
		putc(program[i], stdout);

	putc('\n', stdout);

	list = malloc(sizeof(struct list));
	list->value = 0;
	list->next = NULL;
	list->prev = NULL;

	node = list;

	bstart = -1;

	for (i = 0; program[i] != '\0'; ++i)
	{
		value = 0;

		if (isdigit(program[i]))
			continue;

		if (isdigit(program[i + 1]))
			j = program[i + 1] - '0';
		else
			j = 1;

		switch (program[i])
		{
		// copy right in the array
		case '}':
			value = node->value;

			// continues into > operator, with value set

		// move right in the array
		case '>':
			for (; j > 0; --j)
			{
				if (node->next == NULL)
				{
					node->next = malloc(sizeof(struct list));
					node->next->value = value;
					node->next->next = NULL;
					node->next->prev = node;
				}

				node = node->next;
			}

			break;
		// copy left in the array
		case '{':
			value = node->value;

			// continues into < operator, with value set

		// move left in the array
		case '<':
			for (; j > 0; --j)
			{
				if (node->prev == NULL)
				{
					node->prev = malloc(sizeof(struct list));
					node->prev->value = value;
					node->prev->next = node;
					node->prev->prev = NULL;
				}

				node = node->prev;
			}

			break;
		// increment the index
		case '+':
			node->value += j;

			break;
		// decrement the index
		case '-':
			node->value -= j;

			break;
		// print the index
		case '.':
			for (; j > 0; --j)
				putc(node->value, stdout);
				//printf("%d\n", node->value);

			break;
		// read to the index
		case ',':
			for (; j > 0; --j)
				node->value = getc(stdin);

			break;
		// move backwards
		case '!':
			j = node->value;

			i -= j;

			// loop ends with i++
			i--;

			break;
		// repeat block
		case '|':
			if (bstart == -1)
			{
				bstart = i;
				bi = -1;
			} else
			if (bi == -1)
			{
				i = bstart;
				bi = j - 1;
			} else
			if (bi == 0)
			{
				bstart = -1;

				continue;
			} else {
				i = bstart;
				--bi;
			}

			break;
		default:
			fprintf(stderr, "[ERROR] %c wasn't considered.\n", program[i]);

			break;
		}
	}

	return EXIT_SUCCESS;
}
