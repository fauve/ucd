/* dict.h: include file for Ucrainian (Curses) Dictionary ud v0.5
 * Author:	    Vitaliy Tsybulyak <fauve@ukr.net>
 * Last modified:   2011-09-27
 */
#ifndef __dict_h
#define __dict_h

#define _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>		/* we need it for strcpy and memset */
#include <locale.h>
#include <wchar.h>
#include <termios.h>
#include <unistd.h>

#define FNAME   "/usr/local/share/dict/enua.dict"
#define MAXIN   70
#define MAXLINE 3000

/* key definitions */
#define ENTER   10
#define ESCAPE  27

/* pad size */
#define PADSIZE 510

/* true\false: */
#define TRUE    1
#define FALSE   0

/* for word counting: */
#define IN	1		/* inside the word */
#define OUT	0		/* outside the word */

/* global variables: */
int countl;
int width = 76;
int indent = 10;
int column_width;		/* the length of formatted column (width - ident) */

/* formatting and printing functions: */
void print_first(const wchar_t *, WINDOW *);
int count_words(const wchar_t *);
void insert_space(wchar_t *);
void fill_left(WINDOW *);
void normalize(wchar_t *);
void nullify(wchar_t * arr);
void breakln(const wchar_t *, WINDOW *);
void fmt_print(wchar_t *, WINDOW *);

/* input, pad manipulation and regex functions: */
void insert(int, wchar_t, wchar_t *);
void delete(int, wchar_t *);
int last_i(wchar_t *);
void scroll_pad(WINDOW *, int, int, int);
int match(const wchar_t *, wchar_t *);

#endif /* __dict_h */

