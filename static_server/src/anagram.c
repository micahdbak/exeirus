#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char **argv)
{
	int len,
	    i, j;
	char tmp, *cpy;

	// exit if insufficient arguments
	if (argc == 1)
	{
		printf("Usage: anagram <anagram-text>\n");

		exit(EXIT_SUCCESS);
	}

	srand(time(NULL));
	len = strlen(argv[1]);

	// copy argv[1] to cpy
	cpy = malloc(len * sizeof(char));
	strcpy(cpy, argv[1]);

	// for every character in the provided argument
	for (i = 0; i < len; ++i)
	{
		// swap it with a random character

		j = rand() % len;

		// swap with self; skip
		if (i == j)
			continue;

		// swap cpy[i] with cpy[j]
		tmp = cpy[i];
		cpy[i] = cpy[j];
		cpy[j] = tmp;
	}

	printf("Your anagram: %s\n", cpy);

	free(cpy);

	return EXIT_SUCCESS;
}
