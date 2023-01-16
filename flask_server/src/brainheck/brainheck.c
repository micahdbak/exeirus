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

int main(int argc, char **argv)
{
	FILE *progfile;
	char program[PROGRAMSIZE];
	struct list *list, *node;
	int c,
	    i, j, flag,
	    value,
	    bstart, bi;

	if (argc == 1)
	{
		fprintf(stderr, "Please provide a program file.\n");

		exit(EXIT_FAILURE);
	}

	progfile = fopen(argv[1], "r");
	
	if (progfile == NULL)
	{
		fprintf(stderr, "Could not open the program file.\n");

		exit(EXIT_FAILURE);
	}

	c = getc(progfile);

	for (i = 0; c != EOF; c = getc(progfile))
	{
		/* Left
		 * Right
		 * Increment
		 * Decrement
		 * Print
		 * Scan
		 * Jump
		 * Head
		 * Tail
		 * Equals
		 */
		if (!contains("LRIDPSJFE", c) && !isdigit(c))
			continue;

		program[i++] = c;
	}

	program[i] = '\0';

	printf("Your program is:\n%s\nThe following is program output.\n\n", program);

	list = malloc(sizeof(struct list));
	list->value = 0;
	list->next = NULL;
	list->prev = NULL;

	node = list;

	bstart = -1;

	// an unset flag will jump to the beginning of the program
	flag = -1;

	for (i = 0; program[i] != '\0'; ++i)
	{
		j = 1;

		if (isdigit(program[i]))
		{
			sscanf(&program[i], "%d", &j);
			
			do
				++i;
			while (isdigit(program[i]));
		}

		for (; j > 0; --j)
			switch (program[i])
			{
			// move right in the array
			case 'R':
				if (node->next == NULL)
				{
					node->next = malloc(sizeof(struct list));
					node->next->value = 0;
					node->next->next = NULL;
					node->next->prev = node;
				}

				node = node->next;

				break;
			// move left in the array
			case 'L':
				if (node->prev == NULL)
				{
					node->prev = malloc(sizeof(struct list));
					node->prev->value = 0;
					node->prev->next = node;
					node->prev->prev = NULL;
				}

				node = node->prev;

				break;
			// increment the index
			case 'I':
				node->value++;

				break;
			// decrement the index
			case 'D':
				node->value--;

				break;
			// print the index
			case 'P':
				putc((char)node->value, stdout);
				//printf("%d ", node->value);

				break;
			// read to the index
			case 'S':
				node->value = (int)getc(stdin);

				break;
			// jump over n program characters
			case 'J':
				i += j;
				j = 1;

				break;
			// flag character; ignore
			case 'F':
				// do nothing

				break;
			// jump to flag if this node equals the next
			case 'E':
				if (node->value == node->next->value)
					// move backwards until a head is met
					do
						--i;
					while (i >= 0 && program[i] != 'F');

				// do nothing otherwise

				break;
			default:
				fprintf(stderr, "[ERROR] %c wasn't considered.\n", program[i]);

				break;
			}
	}

	return EXIT_SUCCESS;
}
