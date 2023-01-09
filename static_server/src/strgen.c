/***************************************
 * strgen:  A random string generator. *
 * Author:  Micah Baker                *
 * Date:    10/12/2022                 *
 ***************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "charsets.h"

int main(void)
{
	const char *information =
	     "-1: Exit\n"
	     "0: Base-64\n"
	     "1: Base-32\n"
	     "2: Base-16\n"
	     "3: Base-10\n"
	     "4: Base-8\n"
	     "5: Base-2\n"
	     "6: Alphabetical\n"
	     "7: Alphanumeric\n"
	     "8: Alphanumeric (Case Sensitive)\n"
	     "9: ASCII Symbols\n";
	const char *chars[] = {
		CS_64, CS_32, CS_hex, CS_dec, CS_oct, CS_bin, CS_alp, CS_an, CS_anc, CS_asc
	};
	int choice = 0, nchars, len, i;

	for (;;)
	{
		printf("%sChoice > ", information);
		scanf("%d", &choice);

		if (choice == -1)
			break;

		nchars = strlen(chars[choice]);

		printf("Will generate a key of base %d, using characters %s.\nWhat length? > ", nchars, chars[choice]);
		scanf("%d", &len);

		printf("Your key of length %d: ", len);

		srand(time(0));

		for (i = 0; i < len; ++i)
			putc(chars[choice][rand() % nchars], stdout);

		putc('\n', stdout);
	}

	return EXIT_SUCCESS;
}
