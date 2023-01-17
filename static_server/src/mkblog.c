/*********************************************
 * mkblog:  Static blog factory for EXEIRUS. *
 * Author:  Micah Baker                      *
 * Date:    20/12/2022                       *
 *********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define TITLESIZE   100
#define AUTHORSIZE  100
#define DATESIZE    50

#define PATHSIZE  128

#define DEST_EXT  ".html"

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

#define MONO_START    "<span class=\"monospace\">"
#define MONO_END      "</span>"

#define PRE_START     "<pre class=\"monospace\">"
#define PRE_END       "</pre>"

#define LARGE_START   "<span style=\"font-size: 16pt\">"
#define LARGE_END     "</span>"

#define SMALL_START   "<span style=\"font-size: 10pt\">"
#define SMALL_END     "</span>"

#define URL_START     "<a href=\""
#define URL_END       "\">External Link</a>"

// data regarding a list of posts
struct post
{
	char title[TITLESIZE],
	     author[AUTHORSIZE],
	     filename[PATHSIZE];
	int id, day, month, year;
	struct post *next;
};

/* Sorted tree of posts. The (lth) node of a given tree
 * implies that that node's (post) has a lesser value
 * compared to the tree's (post). Likewise with (gth),
 * except the comparison is greater than. */
struct tree
{
	/* (post) is a sorted list of posts posted on a certain
	 * date. Individual posts here are sorted by post ids. */
	struct post *post;
	int value;
	struct tree *lth, *gth;
};

// sorted list of groups of posts
struct list
{
	struct post *post;
	struct list *next, *prev;
};

/* Returns the "value" of a post, for sorting purposes.
 * The value is determined by the date information of
 * a given post. */
int postValue(struct post *post)
{
	return (post->year * 10000) + (post->month * 100) + post->day;
}

/* Branch from a tree node (node) with respect to (post). */
void branch(struct post *post, struct tree *node)
{
	struct post *pnode, *pnext;
	int value;

	// negative post ID's are unlisted
	if (post->id < 0)
		return;

	value = postValue(post);

	if (value < node->value)
	// the value of this post is lesser than the value of this node
	{
		// create (lth) if it doesn't exist, branch if it does

		if (node->lth == NULL)
		{
			node->lth = malloc(sizeof(struct tree));
			node = node->lth;

			node->post = malloc(sizeof(struct post));
			memcpy(node->post, post, sizeof(struct post));
			node->post->next = NULL;

			node->value = value;
			node->lth = NULL;
			node->gth = NULL;
		} else
			branch(post, node->lth);
	} else
	if (value > node->value)
	// the value of this post is greater than the value of this node
	{
		// create (gth) if it doesn't exist, branch if it does

		if (node->gth == NULL)
		{
			node->gth = malloc(sizeof(struct tree));
			node = node->gth;

			node->post = malloc(sizeof(struct post));
			memcpy(node->post, post, sizeof(struct post));
			node->post->next = NULL;

			node->value = value;
			node->lth = NULL;
			node->gth = NULL;
		} else
			branch(post, node->gth);
	} else {
	// the value of this post is equal to the value of this node
		if (post->id < node->post->id)
		{
			pnode = malloc(sizeof(struct post));
			memcpy(pnode, post, sizeof(struct post));

			pnode->next = node->post;
			node->post = pnode;
		} else {
			pnode = node->post;
			pnext = pnode->next;

			while (pnext != NULL && post->id > pnext->id)
			{
				pnode = pnext;
				pnext = pnode->next;
			}

			pnode->next = malloc(sizeof(struct post));
			pnode = pnode->next;

			memcpy(pnode, post, sizeof(struct post));
			pnode->next = pnext;
		}
	}
}

/* Pushes (post) into list (lnode). Create a list if (lnode) is NULL. */
struct list *pushPost(struct post *post, struct list *lnode)
{
	struct list *lprev = NULL;

	if (lnode == NULL)
		lnode = malloc(sizeof(struct list));
	else {
		lnode->next = malloc(sizeof(struct list));
		lprev = lnode;
		lnode = lnode->next;
	}

	lnode->post = post;
	lnode->next = NULL;
	lnode->prev = lprev;

	return lnode;
}

// parses tree (tnode) into list (lnode)
struct list *pushTree(struct tree *tnode, struct list *lnode)
{
	if (tnode == NULL)
		return lnode;

	lnode = pushTree(tnode->lth, lnode);
	lnode = pushPost(tnode->post, lnode);
	lnode = pushTree(tnode->gth, lnode);

	return lnode;
}

// frees a tree
void freeTree(struct tree *node)
{
	if (node == NULL)
		return;

	freeTree(node->lth);
	freeTree(node->gth);

	free(node);
}

/* Returns a sorted and grouped list of posts with
 * respect to the provided post list (node). */
struct list *sortPosts(struct post *node)
{
	struct tree *root;
	struct list *list;

	// allocate the tree's root
	root = malloc(sizeof(struct tree));

	root->post = malloc(sizeof(struct post));

	memcpy(root->post, node, sizeof(struct post));
	root->post->next = NULL;

	root->value = postValue(node);
	root->lth = NULL;
	root->gth = NULL;

	node = node->next;

	// sort each post into the tree
	for (; node != NULL; node = node->next)
		branch(node, root);

	list = pushTree(root, NULL);

	freeTree(root);

	while (list->prev != NULL)
		list = list->prev;

	return list;
}

// reads a line from (file) into (arr)
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
	const char *month[12] = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};
	int i,
	    c, doneContent, isStart,
	    lpos, ppos;
	DIR *posts;
	char postDir[PATHSIZE], destDir[PATHSIZE],
	     path[PATHSIZE];
	FILE *postSkel, *indexSkel,
	     *post, *dest,
	     *index;
	struct post *listRaw, *pnode, *pnext;
	struct list *list, *lnode, *lnext;
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

	listRaw = malloc(sizeof(struct post));
	pnode = listRaw;

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
		strcpy(pnode->filename, dirent->d_name);

		// get the path to the post file
		strcpy(path, postDir);
		strcat(path, "/");
		strcat(path, dirent->d_name);

		// open the post file
		post = fopen(path, "r");

		// read the post's metadata

		readLine(pnode->title, post);
		readLine(pnode->author, post);
		fscanf(post, "%d %d %d %d\n", &pnode->id, &pnode->day, &pnode->month, &pnode->year);

		printf("Got post file \"%s\"; title: \"%s\", author: %s, date: %d %s %d; id: %d\n",
		       path, pnode->title, pnode->author, pnode->day, month[pnode->month - 1], pnode->year, pnode->id);

		// prepare the post's destination file

		// get the path to the destination file
		strcpy(path, destDir);
		strcat(path, "/");
		strcat(path, dirent->d_name);
		strcat(path, DEST_EXT);

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
				// post id
				case 'i':
					if (pnode->id < 0)
						fprintf(dest, "unlisted");
					else
						fprintf(dest, "%d", pnode->id);

					break;	
				// title
				case 't':
					fprintf(dest, "%s", pnode->title);

					break;
				// author
				case 'a':
					fprintf(dest, "%s", pnode->author);

					break;
				// date
				case 'd':
					fprintf(dest, "%d %s %d", pnode->day, month[pnode->month - 1], pnode->year);

					break;
				// filename
				case 'f':
					fprintf(dest, "%s", pnode->filename);

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
							// pre-formatted text
							case 'M':
								if (isStart)  fprintf(dest, PRE_START);
								else          fprintf(dest, PRE_END);

								break;
							// large text
							case 'L':
								if (isStart)  fprintf(dest, LARGE_START);
								else          fprintf(dest, LARGE_END);

								break;
							// small text
							case 'S':
								if (isStart)  fprintf(dest, SMALL_START);
								else          fprintf(dest, SMALL_END);

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
			pnode->next = malloc(sizeof(struct post));
			pnode = pnode->next;
		} else
			// break loop if there are no more posts
			break;
	}

	pnode->next = NULL;
 
	list = sortPosts(listRaw);

	// free (listRaw)
	
	pnode = listRaw;

	for (; pnode != NULL; pnode = pnext)
	{
		pnext = pnode->next;
		free(pnode);
	}

	lnode = list;
	pnode = lnode->post;

	printf("Sorted and grouped posts.\n");

	// write the index file now that we have a post list

	// open the destination index file
	
	strcpy(path, destDir);
	strcat(path, "/index.html");

	index = fopen(path, "w");

	printf("Creating index file at \"%s\".\n", path);

	c = getc(indexSkel);

	for (; c != EOF; c = getc(indexSkel))
	{
		// check if [c] is the special character '#'
		if (c == '#')
		{
			c = getc(indexSkel);
			
			switch (c)
			{
			// start of group listing
			case '{':
				// remember this position
				lpos = ftell(indexSkel);

				break;
			/* We have reached the end of a group listing.
			 * If there is another group to be written, seek
			 * the beginning of the group listing and write
			 * again. */
			case '}':
				// get the next node
				lnode = lnode->next;

				// check if this node exists
				if (lnode != NULL)
				{
					// go to the beginning of the group listing
					fseek(indexSkel, lpos, SEEK_SET);

					// set (pnode) to be the first post in (lnode)
					pnode = lnode->post;
				}

				/* If (lnode) is NULL, then (indexSkel) will
				 * continue being read from where it was. */

				break;
			// start of post listing
			case '(':
				// remember this position
				ppos = ftell(indexSkel);

				break;
			/* We have reached the end of a post listing.
			 * If there is another post to be written, seek
			 * the beginning of the post listing and write
			 * again. */
			case ')':
				// set the post node to be the next node
				pnode = pnode->next;

				if (pnode != NULL)
					// go to the beginning of the post listing
					fseek(indexSkel, ppos, SEEK_SET);

				/* If (pnode) is NULL, then (indexSkel) will
				 * continue being read from where it was. */

				break;
			// group date
			case 'D':
				fprintf(index, "%d %s %d", pnode->day, month[pnode->month - 1], pnode->year);

				break;
			// post id
			case 'i':
				fprintf(index, "%d", pnode->id);

				break;
			// title
			case 't':
				fprintf(index, "%s", pnode->title);

				break;
			// author
			case 'a':
				fprintf(index, "%s", pnode->author);

				break;
			// date
			case 'd':
				fprintf(index, "%d %s %d", pnode->day, month[pnode->month - 1], pnode->year);

				break;
			// filename
			case 'f':
				fprintf(index, "%s", pnode->filename);

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

	printf("Cleaning up.\n");

	closedir(posts);
	fclose(postSkel);
	fclose(indexSkel);
	fclose(index);

	// free the sorted and grouped post list

	lnode = list;

	for (; lnode != NULL; lnode = lnext)
	{
		lnext = lnode->next;
		pnode = lnode->post;

		for (; pnode != NULL; pnode = pnext)
		{
			pnext = pnode->next;
			free(pnode);
		}

		free(lnode);
	}

	printf("All done.\n");

	return EXIT_SUCCESS;
}
