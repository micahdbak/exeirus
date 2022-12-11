/*********************************************
 * mkblog:  Static blog factory for EXEIRUS. *
 * Author:  Micah Baker                      *
 * Date:    10/12/2022                       *
 *********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define TITLESIZE   100
#define AUTHORSIZE  100
#define DATESIZE    50

#define PATHSIZE  128

// blog-script definitions

#define LINE_BREAK    "<br>"
#define PARA_BREAK    "</p>\n<p>"

#define ITALIC_START  "<i>"
#define ITALIC_END    "</i>"

#define BOLD_START    "<b>"
#define BOLD_END      "</b>"

#define STRIKE_START  "<s>"
#define STRIKE_END    "</s>"

#define UNDER_START   "<u>"
#define UNDER_END     "</u>"

#define MONO_START    "<span style=\"font-family: monospace;\">"
#define MONO_END      "</span>"

#define URL_START     "<a href=\""
#define URL_END       "\">Click Here</a>"

struct post
{
	char title[TITLESIZE],
	     author[AUTHORSIZE],
	     date[DATESIZE],
	     filename[PATHSIZE];
	struct post *next;
};

int readLine(char *arr, FILE *file)
{
	int c, i;

	c = getc(file);

	for (i = 0; c != '\n' && c != EOF; c = getc(file))
		arr[i++] = c;

	arr[i] = '\0';

	return c == '\n' ? 0 : -1;
}

int main(int argc, char **argv)
{
	int i,
	    c, doneContent, isStart,
	    pos;
	DIR *posts;
	char postDir[PATHSIZE], destDir[PATHSIZE],
	     path[PATHSIZE];
	FILE *postSkel, *indexSkel,
	     *post, *dest,
	     *index;
	struct post *list, *node;
	struct dirent *dirent;

	// initialize needed variables
	posts = NULL;
	destDir[0] = '\0';
	postSkel = NULL;
	indexSkel = NULL;

	// for every argument
	for (i = 1; i < argc; ++i)
	{
		// option
		if (argv[i][0] == '-')
		{
			// switch for option
			switch (argv[i][1])
			{
			// posts directory
			case 'p':
				if ((i + 1) == argc)
				{
					printf("No posts directory provided after \"-p\". Exiting.\n");

					exit(EXIT_FAILURE);
				}

				strcpy(postDir, argv[++i]);

				printf("Got posts directory %s\n", postDir);

				posts = opendir(postDir);

				break;
			// destination directory
			case 'd':
				if ((i + 1) == argc)
				{
					printf("No destination directory provided after \"-d\". Exiting.\n");

					exit(EXIT_FAILURE);
				}

				strcpy(destDir, argv[++i]);

				break;
			// post skeleton file
			case 's':
				if ((i + 1) == argc)
				{
					printf("No post skeleton file provided after \"-d\". Exiting.\n");

					exit(EXIT_FAILURE);
				}

				postSkel = fopen(argv[++i], "r");

				break;
			// index skeleton file
			case 'i':
				if ((i + 1) == argc)
				{
					printf("No index skeleton file provided after \"-d\". Exiting.\n");

					exit(EXIT_FAILURE);
				}

				indexSkel = fopen(argv[++i], "r");

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

	// ensure that everything needed was provided
	if (posts == NULL || destDir[0] == '\0' || postSkel == NULL || indexSkel == NULL)
	{
		printf("Please provide the posts directory, destination directory, post skeleton file, and index skeleton file.\n"); 

		exit(EXIT_FAILURE);
	}

	printf("All OK.\n");

	list = malloc(sizeof(struct post));
	node = list;

	dirent = readdir(posts);

	for (;;)
	{
		// skip special files
		if (strcmp(".", dirent->d_name) == 0
		 || strcmp("..", dirent->d_name) == 0)
		{
			dirent = readdir(posts);

			if (dirent == NULL)
				break;

			continue;
		}

		// copy the file name to this node
		strcpy(node->filename, dirent->d_name);

		// get the path to the post file
		strcpy(path, postDir);
		strcat(path, "/");
		strcat(path, dirent->d_name);

		// open the post file
		post = fopen(path, "r");

		// read the post's metadata

		readLine(node->title, post);
		readLine(node->author, post);
		readLine(node->date, post);

		printf("Got post file \"%s\"; title: \"%s\", author: %s, date: %s.\n", path, node->title, node->author, node->date);

		// prepare the post's destination file

		// get the path to the destination file
		strcpy(path, destDir);
		strcat(path, "/");
		strcat(path, dirent->d_name);

		printf("Writing to destination file \"%s\".\n", path);

		// open the destination file
		dest = fopen(path, "w");

		// go to the beginning of the post skeleton file
		fseek(postSkel, 0, SEEK_SET);

		// write to the destination file

		// variable to check if the content was already written
		doneContent = 0;
		c = getc(postSkel);

		// for every character in the post skeleton file
		for (i = 0; c != EOF; c = getc(postSkel))
		{
			// check if [c] is the escape character '#'
			if (c == '#')
			{
				// switch for the next character
				switch (getc(postSkel))
				{
				// title
				case 't':
					fprintf(dest, "%s", node->title);

					break;
				// author
				case 'a':
					fprintf(dest, "%s", node->author);

					break;
				// date
				case 'd':
					fprintf(dest, "%s", node->date);

					break;
				// filename
				case 'f':
					fprintf(dest, "%s", node->filename);

					break;
				// content
				case 'c':
					doneContent = 1;

					// the post file is ready for us to read its content

					c = getc(post);

					for (; c != EOF; c = getc(post))
					{
						// check if [c] is the escape character '\'
						if (c == '\\')
						{
							c = getc(post);

							switch (c)
							{
							// field modifiers
							
							// field start
							case '(':
								isStart = 1;

								break;
							// field end
							case ')':
								isStart = 0;

								break;
							// special characters

							// tab
							case 't':
								putc('\t', dest);

								continue;
							// new-line
							case 'n':
								fprintf(dest, LINE_BREAK);

								continue;
							// new paragraph
							case 'P':
								fprintf(dest, PARA_BREAK);

								continue;
							// unknown character
							default:
								// write it to the destination file
								putc(c, dest);

								continue;
							}

							/* To be here, the loop must not have [continue]'d.
							 * Therefore it must be a field modifier. */

							// switch for the next character
							switch (getc(post))
							{
							// italic text
							case 'i':
								if (isStart)  fprintf(dest, ITALIC_START);
								else          fprintf(dest, ITALIC_END);

								break;
							// bold text
							case 'b':
								if (isStart)  fprintf(dest, BOLD_START);
								else          fprintf(dest, BOLD_END);

								break;
							// strikethrough text
							case 's':
								if (isStart)  fprintf(dest, STRIKE_START);
								else          fprintf(dest, STRIKE_END);

								break;
							// underlined text
							case 'u':
								if (isStart)  fprintf(dest, UNDER_START);
								else          fprintf(dest, UNDER_END);

								break;
							// monospace text
							case 'm':
								if (isStart)  fprintf(dest, MONO_START);
								else          fprintf(dest, MONO_END);

								break;
							// URL
							case 'a':
								if (isStart)  fprintf(dest, URL_START);
								else          fprintf(dest, URL_END);

								break;
							// unknown field modifier
							default:
								// do nothing

								break;
							}
						} else
							/* This is not a special character. Simply
							 * write it to the destination file. */
							putc(c, dest);
					}

					break;
				}
			} else
				/* This is not a special character. Simply
				 * write it to the destination file. */
				putc(c, dest);
		}

		/*     The post file is read, and
		 * the destination file is written. */

		// clean up

		fclose(post);
		fclose(dest);

		// prepare next node if there is another post

		dirent = readdir(posts);

		if (dirent != NULL)
		{
			node->next = malloc(sizeof(struct post));
			node = node->next;
		} else
			// break loop if there are no more posts
			break;
	}
 
	// write the index file now that we have a post list

	// open the destination index file
	
	strcpy(path, destDir);
	strcat(path, "/index.html");

	index = fopen(path, "w");

	printf("Creating index file at \"%s\".\n", path);

	c = getc(indexSkel);

	for (node = list; c != EOF; c = getc(indexSkel))
	{
		// check if [c] is the special character '#'
		if (c == '#')
		{
			c = getc(indexSkel);
			
			switch (c)
			{
			// start of post listing
			case '(':
				// remember this position
				pos = ftell(indexSkel);

				break;
			/* We have reached the end of the post listing.
			 * If there is another post to be written, seek
			 * the beginning of the post listing and write
			 * again. */
			case ')':
				// set node to be the next post
				node = node->next;

				// check if this post exists
				if (node != NULL)
				{
					// go to the beginning of the post listing
					fseek(indexSkel, pos, SEEK_SET);
				}

				break;
			// title
			case 't':
				fprintf(index, "%s", node->title);

				break;
			// author
			case 'a':
				fprintf(index, "%s", node->author);

				break;
			// date
			case 'd':
				fprintf(index, "%s", node->date);

				break;
			// filename
			case 'f':
				fprintf(index, "%s", node->filename);

				break;
			// unknown character
			default:
				// write it to the index file
				putc(c, index);

				break;
			}
		} else
			/* This is not a special character.
			 * Simply write it to the index file. */
			putc(c, index);
	}

	// clean up

	closedir(posts);
	fclose(postSkel);
	fclose(indexSkel);
	fclose(index);

	// free the post list
	for (; list != NULL; list = node)
	{
		node = list->next;
		free(list);
	}

	printf("All done.\n");

	return EXIT_SUCCESS;
}
