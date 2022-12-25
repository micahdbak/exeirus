/********************************************
 * mkdoc:   Document processor for EXEIRUS. *
 * Author:  Micah Baker                     *
 * Date:    10/12/2022                      *
 ********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#define PATHSIZE   128

// increase these as need be
#define KEYSIZE    32
#define VALSIZE    100

#define TABLESIZE  50

struct keyMember
{
	char key[KEYSIZE],
	     val[VALSIZE];
	struct keyMember *next;
};

/*******************************************************
 * Title: The djb2 hashing algorithm                   *
 * Author: Dan Bernstein                               *
 * Availability: http://www.cse.yorku.ca/~oz/hash.html *
 *******************************************************/
unsigned long djb2(char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++) != '\0')
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

struct keyMember *pushKey(char *key, struct keyMember *node)
{
	struct keyMember *next;

	// this list is empty
	if (node->key[0] == '\0')
	{
		// initialize this list

		strcpy(node->key, key);
		node->next = NULL;

		return node;
	}

	// same key
	if (strcmp(key, node->key) == 0)
		return node;

	next = node->next;

	// for every node after [node] that isn't NULL
	for (; next != NULL; next = node->next)
	{
		node = next;

		// same key
		if (strcmp(key, node->key) == 0)
			return node;
	}

	// this key does not exist yet

	// create it
	node->next = malloc(sizeof(struct keyMember));
	node = node->next;

	// copy key to this node
	strcpy(node->key, key);

	return node;
}

struct keyMember *getKey(char *key, struct keyMember *node)
{
	// this list is not initialized
	if (node->key[0] == '\0')
		return NULL;

	// for every node in [node] that doesn't have the same key
	for (; node != NULL; node = node->next)
		// break if it has the same key
		if (strcmp(key, node->key) == 0)
			break;

	// return the node that matched
	// if no node matched, this returns NULL
	return node;
}

int main(int argc, char **argv)
{
	FILE *config,
	     *source, *dest;
	int i = 0,
	    c,
	    hash;
	char destPath[PATHSIZE], srcPath[PATHSIZE],
	     path[PATHSIZE],
	     key[KEYSIZE];
	struct keyMember table[TABLESIZE],
	                 *node, *next;
	DIR *srcDir;
	struct dirent *dirent;

	// initialize variables
	
	config = NULL;
	destPath[0] = '\0';
	srcDir = NULL;

	for (i = 1; i < argc; ++i)
	{
		// option
		if (argv[i][0] == '-')
		{
			// switch for option
			switch (argv[i][1])
			{
			// configuration file
			case 'c':
				// nothing proceeds this option
				if ((i + 1) == argc)
				{
					printf("Please provide the path to the configuration "
					       "file after \"-c\". Exiting.\n");
					
					exit(EXIT_FAILURE);
				}

				// open the configuration file
				config = fopen(argv[++i], "r");

				break;
			// source directory
			case 's':
				// nothing proceeds this option
				if ((i + 1) == argc)
				{
					printf("Please provide the path to the source "
					       "directory after \"-s\". Exiting.\n");

					exit(EXIT_FAILURE);
				}

				// copy and open the source path
				strcpy(srcPath, argv[++i]);
				srcDir = opendir(srcPath);

				break;
			// destination directory
			case 'd':
				// nothing proceeds this option
				if ((i + 1) == argc)
				{
					printf("Please provide the path to the destination "
					       "directory after \"-d\". Exiting.\n");

					exit(EXIT_FAILURE);
				}

				// copy the destination path
				strcpy(destPath, argv[++i]);

				break;
			// unknown option
			default:
				printf("Unknown option \"%s\" passed. Ignoring.\n", argv[i]);

				break;
			}
		// not an option
		} else
			printf("Not sure what you mean by \"%s\". Ignoring.\n", argv[i]);
	}

	// check if everything is OK to continue
	if (config == NULL || destPath[0] == '\0' || srcDir == NULL)
	{
		printf("Please provide options regarding the configuration file, "
		       "destination directory, and source directory. Exiting.\n");

		exit(EXIT_SUCCESS);
	}

	printf("All OK.\n");

	// initialize all keys in the table to be of no length
	for (i = 0; i < TABLESIZE; ++i)
		table[i].key[0] = '\0';

	c = getc(config);

	// for every line in the configuration file
	for (; c != EOF; c = getc(config))
	{
		// read a key

		for (i = 0; c != ':'; c = getc(config))
			key[i++] = c;

		key[i] = '\0';

		// calculate its hash
		hash = djb2(key) % TABLESIZE;

		// skip white-space

		c = getc(config);

		for (; isspace(c); c = getc(config))
			;

		// get the node for this key
		node = pushKey(key, &table[hash]);

		// read all characters into this node's val until a new-line

		for (i = 0; c != '\n'; c = getc(config))
			node->val[i++] = c;

		node->val[i] = '\0';

		printf("Got key %s with value \"%s\".\n", node->key, node->val);
	}

	// close the configuration file
	fclose(config);

	// the source directory is already open in [srcDir]

	dirent = readdir(srcDir);

	// for every file in the source directory
	for (; dirent != NULL; dirent = readdir(srcDir))
	{
		// skip special files
		if (strcmp(".", dirent->d_name) == 0
		 || strcmp("..", dirent->d_name) == 0)
			continue;

		// get the path to the source file

		strcpy(path, srcPath);
		strcat(path, "/");
		strcat(path, dirent->d_name);

		// open it
		source = fopen(path, "r");

		// get the path to the output file

		strcpy(path, destPath);
		strcat(path, "/");
		strcat(path, dirent->d_name);

		// open it
		dest = fopen(path, "w");

		// for every character in this source file

		c = getc(source);

		for (; c != EOF; c = getc(source))
		{
			// possibly start of a key
			if (c == '$')
			{
				c = getc(source);

				// just a dollar sign
				if (c == '$')
				{
					putc(c, dest);

					continue;
				}

				// read the key

				i = 0;

				do {
					key[i++] = c;
					c = getc(source);
				}
				while (c != '$');

				key[i] = '\0';

				hash = djb2(key) % TABLESIZE;

				// get the node for this key
				node = getKey(key, &table[hash]);

				// the node does not exist
				if (node == NULL)
				{
					printf("Warning: the key \"%s\" showed up in the "
					       "source file %s/%s but there is no value for "
					       "this key.\n", key, srcPath, dirent->d_name);

					continue;
				}

				// the node exists

				// print the node's value into the destination file
				for (i = 0; node->val[i] != '\0'; ++i)
					putc(node->val[i], dest);
			} else
				putc(c, dest);
		}

		printf("Made file %s/%s from %s/%s.\n", destPath, dirent->d_name, srcPath, dirent->d_name);

		// clean up

		fclose(source);
		fclose(dest);
	}

	// clean up

	closedir(srcDir);

	// for every list in the table
	for (i = 0; i < TABLESIZE; ++i)
		// if this list exists
		if (table[i].key[0] != '\0')
		{
			/*     Remember that [table] is an array of
			 * [struct keyMember], not [struct keyMember *].
			 * So we only need to free the members that come
			 * after the first member.
			 */
			node = table[i].next;

			for (; node != NULL; node = next)
			{
				next = node->next;
				free(node);
			}
		}

	printf("All done.\n");

	return EXIT_SUCCESS;
}
