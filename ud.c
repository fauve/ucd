/* ud.c: Ukrainian (Curses) Dictionary ud v0.5
 * Author:	    Vitaliy Tsybulyak <fauve@ukr.net>
 * Last modified:   2011-09-27
 */

#include "dict.h"

/* print firs word; how depends on wcslen */
void print_first(const wchar_t * w, WINDOW * p)
{
    int width;
    int adj_sp = 0;

    if ((width = wcswidth(w, MAXIN)) < indent) {
        wprintw(p, "%ls", w);
        adj_sp = indent - (int) wcswidth(w, MAXIN);
        while (adj_sp != 0) {
            wprintw(p, "%lc", L' ');
            adj_sp--;
        }
    } else {
        wprintw(p, "%ls\n", w);
        countl++;
    }
    return;
}

int count_words(const wchar_t * s)
{
    int i, nw, state;

    state = OUT;
    nw = 0;

    for (i = 0; s[i] != L'\0'; i++) {
        if (s[i] == L' ')       /* \t and \n should be removed before */
            state = OUT;
        else if (state == OUT) {
            state = IN;
            ++nw;
        }
    }
    return nw;
}

/* insert spaces */
void insert_space(wchar_t * s)
{                              
    int i, j, nw, len;
    int sp, need_sp, add_sp;
    wchar_t tmp_s[column_width + 1];

    len = (int) wcswidth(s, MAXLINE);
    nw = count_words(s);                /* number of words */

    need_sp = column_width - len;       /* how many spaces missing */

    for (i = j = 0; s[i] != L'\0'; i++, j++) {
        if (s[i] == L' ') {
            tmp_s[j] = s[i];
            if (nw > 1)
                add_sp = need_sp / (nw - 1);
            else
                add_sp = 0;
            for (sp = 0; sp < add_sp; sp++)
                tmp_s[++j] = L' ';
            need_sp -= add_sp;
            nw--;
        } else
            tmp_s[j] = s[i];
    }
    tmp_s[j] = L'\0';
    wcscpy(s, tmp_s);
}

/* fills left margin with space */
void fill_left(WINDOW * p)
{
    int i = 0;

    while (i < indent) {
        wprintw(p, "%lc", L' ');
        i++;
    }
    return;
}

/* replaces tabs, removes newline, fills the end with NUL */
void normalize(wchar_t * ln)
{
    int i, j;
    int was_space = TRUE;

    for (i = j = 0; ln[i] != L'\0' && ln[j] != L'\0'; i++, j++) {
        if ((ln[j] == L'\t' || ln[j] == L' ') && was_space == TRUE)
            i--;
        else if (ln[j] == L' ') {
            ln[i] = ln[j];
            was_space = TRUE;
        } else if (ln[j] == L'\t') {
            ln[i] = L' ';
            was_space = TRUE;
        } else if (ln[j] == L'\n')
            ln[i] = L'\0';
        else {
            ln[i] = ln[j];
            was_space = FALSE;
        }
    }
   
    for (; i != MAXLINE; i++)
        ln[i] = L'\0';
    return;
}

/* fmt_print: formatted printing (calls the functions above) */
void fmt_print(wchar_t * ln, WINDOW * p)
{
    int i, idx;                                 /* next-stop index */
    int start_pos;                              /* starting index  */
    wchar_t first_word[column_width + 1];       /* dictionary first word */
    wchar_t pass_line[column_width + 1];        /* line passed for formatting */

    for (idx = 0; ln[idx] != L' ' && idx < column_width; idx++)
        first_word[idx] = ln[idx];
    first_word[idx] = L'\0';

    print_first(first_word, p);

    if (idx == column_width && ln[idx] != L' ')
        start_pos = idx;        /* not eating whitespace */
    else
        start_pos = idx++;

    while (idx + column_width < MAXLINE && ln[idx + column_width] != L'\0') {
        start_pos = idx;
        idx += column_width;
        /* searching for whitespace, till the starting position */
        while (ln[idx] != L' ' && idx != start_pos)
            idx--;
        if (idx == start_pos)   /* if no white found, just print the */
            break;              /* line */

        for (i = 0; start_pos < idx && i < column_width; start_pos++, i++)
            pass_line[i] = ln[start_pos];
        pass_line[i] = L'\0';
        insert_space(pass_line);
        if (countl > 0) {
            fill_left(p);
            wprintw(p, "%ls\n", pass_line);
            countl++;
        } else {
            wprintw(p, "%ls\n", pass_line);
            countl++;
        }
        idx++;
    }
    start_pos = idx++;

    if (countl > 0)
        fill_left(p);

    while (ln[start_pos] != L'\0')
        wprintw(p, "%lc", ln[start_pos++]);
    wprintw(p, "\n\n");
    countl += 2;

    return;
}

/*********************** end formatting and printing functions ********************/

/* insert: insert a wchar_t at index */
void insert(int idx, wchar_t ch, wchar_t * arr)
{
    int i;
    wchar_t prev_ch, tmp_ch;
    prev_ch = arr[idx];
    arr[idx++] = ch;
   
    for (i = idx; i < (int) wcslen(arr) && arr[i] != L'\0'; i++) {
	tmp_ch = arr[i];
	arr[i] = prev_ch;
	prev_ch = tmp_ch;
    }
    arr[i++] = prev_ch;
    //arr[i] = L'\0';
    while (i++ < MAXIN)
	arr[i] = L'\0';
    return;
}

/* eraze a character at a given position */
void delete(int idx, wchar_t * arr)
{
    int i;
    /* wcslen shouldn't be changed to wcswidth? */
    for (i = idx; i < (int) wcslen(arr) - 1 && arr[i] != L'\0'; i++)
	arr[i] = arr[i + 1];
    arr[i] = L'\0';
}

void nullify(wchar_t * arr)
{
    int i, len;
    len = (int) wcslen(arr);

    for (i = 0; i < len - 1 && arr[i] != L'\0'; i++)
	arr[i] = L'\0';
}

/* last_i: find out the last index (wcslen(arr) - 1)*/
int last_i(wchar_t * arr)
{
    int i;			/* probably change it to size_t */

    for (i = 0; i < (int) wcslen(arr) && arr[i] != L'\0'; i++);
    return --i;
}

/* scroll_pad: by changing the variable r (row), we change the  *
 * region displayed in the pad, thus srolling it.               */
void scroll_pad(WINDOW * pad_win, int r, int y, int x)
{
    prefresh(pad_win, r, 0, 0, 0, y - 3, x);
}

/* match: simple match  */
int match(const wchar_t * s, wchar_t * p)
{
    return wcsncmp(s, p, wcslen(p));
}

/****************************************************************************/

int main(int argc, char *argv[])
{
    FILE *fp;
    int y, x, code, curs_pos, row;
    wint_t ch, dummy_ch;
    wchar_t wch;
    cchar_t cch;
    int total_lines;		/* the total number of lines (for scrolling function) */
    int found;			/* the number of matches (to display as W:001) */
    int adjust = TRUE;		/* wether output should be adjusted */
    wchar_t line[MAXLINE];
    wchar_t ptt[MAXIN];
    char progn[MAXLINE];
    WINDOW *wins, *padw;

    setlocale(LC_CTYPE, "");

    strcpy(progn, argv[0]);
    column_width = width - indent;	/* the width of formatted column; 66 by default */
    row = curs_pos = countl = total_lines = 0;
    ptt[0] = L'\0';

    /* opening default file: */
    fp = fopen(FNAME, "r");
    if (fp == NULL)
	fprintf(stderr, "%s: can't open file %s\n", progn, FNAME);

    /* getting command-line arguments:
     *  -f name  dictionary file
     *  -h       help 
     *  -i N     left indent N chars
     *  -n       no adjusting
     *  -r       use clasic regex 
     *  -w N     width N characters (default 80)
     */
    while (--argc > 0 && (*++argv)[0] == '-') {
        int c = *(argv[0]+1);
        switch (c) {
            case 'f':
                argv++;
                argc--;
                fclose(fp);
                fp = fopen(argv[0], "r");
                if (fp == NULL) {               /* unable to open file */
                    fprintf(stderr, "%s: can't open file %s\n", progn, argv[0]);
                    return 1;
                }
                break;
            case 'h':
                printf("Usage:\n");
                printf("\tud [-option] [parameter]\n");
                printf("Options:\n");
                printf("\t-f name  dictionary file\n");
                printf("\t-h       help\n");
                printf("\t-i N     left indent N characters\n");
                printf("\t-n       no adjusting (default yes)\n");
                printf("\t-w N     width N characters (default 80)\n");
                return 0;
            case 'i':
                argv++;
                argc--;
                indent = atoi(argv[0]);
                column_width = width - indent;
                break;
            case 'n':
                adjust = FALSE;  /* no adjusting */
                break;
            case 'w':
                argv++;
                argc--;
                width = atoi(argv[0]);
                column_width = width - indent;
                break;          
            default:
                fprintf(stderr, "%s: illegal option  -%c\n", progn, c);
        }
    }

    
    
    /* curses */
    (void) initscr();
    //(void) raw();
    (void) noecho();
    (void) cbreak();		/* take input chars one at a time, no wait for \n */
    keypad(stdscr, TRUE);	/* turn on function keys */
    /* idlok(stdscr, TRUE); */
    getmaxyx(stdscr, y, x);	/* getting the size of the screen */

    /* creating input window */
    wins = newwin(1, x, y - 1, 0);
    attron(A_REVERSE | A_BOLD);
    mvhline(y - 2, 0, ' ', x - 6);
    mvprintw(y - 2, x - 6, "W:%03d ", countl);
    attroff(A_REVERSE | A_BOLD);
    refresh();

    /* creating pad: rows, wide */
    padw = newpad(PADSIZE, x);
    idlok(padw, TRUE);
    scrollok(padw, TRUE);

    move(y - 1, curs_pos);	/* cursor */

    while ((code = get_wch(&ch)) != ERR) { 
	switch (ch) {
	case KEY_RESIZE:		/* the screen has been resized */
	    memset(&ch, 0, sizeof(ch)); 
	    getmaxyx(stdscr, y, x);     /* geting new size; avoiding ioctl fo now */
	    resize_term(y, x);          /* trying to resize internally */
	    get_wch(&dummy_ch);         /* stupid hack; there is no enother way to get 
					   this characer out off input buffer, so get_wch 
					   does not read it at the top */
	    attron(A_REVERSE | A_BOLD);         /* still, seems that we have to paint */
	    mvhline(y - 2, 0, ' ', x - 6);      /* some things by ourselves */
	    mvprintw(y - 2, x - 6, "W:%03d ", countl);
	    attroff(A_REVERSE | A_BOLD);
	    delwin(wins);
	    wins = newwin(1, x, y - 1, 0);
	    refresh();
	    curs_pos = 0;             /* the resize operation seems to move the cursor */
	    move(y - 1, curs_pos);    /* added this to force cursor back to start */
	    delete(curs_pos, ptt);  
	    padw = newpad(PADSIZE, x);
	    wattron(padw, A_REVERSE | A_BOLD);
	    mvwprintw(padw, 0, x - 20, "ROWS:%4d COLS:%4d ", y, x); /* this one goes into TODO */
	    wattroff(padw, A_REVERSE | A_BOLD);
	    prefresh(padw, 0, 0, 0, 0, y - 3, x);       /*  must add a temp buffer to */
	    break;
	case ENTER:
	    row = 0;
	    found = 0;
	    wclear(padw);
	    while ((fgetws(line, MAXLINE, fp)) != NULL
		   && total_lines < 500) {
		if (adjust == FALSE) {
		    if ((match(line, ptt)) == 0) {
			found++;
			wprintw(padw, "%ls", line);	/* or waddwstr? */
			total_lines++;
			countl = 0;
		    }
		} else {
		    if ((match(line, ptt)) == 0) {
			found++;
			normalize(line);	/* remove extra ' ', '\t', '\n' */
			fmt_print(line, padw);
			total_lines += countl;
			countl = 0;	/* resseting line count */
		    }
		}
	    }			/* end while */
	    prefresh(padw, 0, 0, 0, 0, y - 3, x);
	    werase(wins);
	    wrefresh(wins);
	    curs_pos = 0;	/* resetting cursor and ptt index */
	    nullify(ptt);
	    rewind(fp);		/* go to the start of the file */
	    attron(A_REVERSE | A_BOLD);
	    mvprintw(y - 2, x - 6, "W:%03d ", found);
	    attroff(A_REVERSE | A_BOLD);
	    curs_set(0);
	    break;
            /* breaking only here */
	case KEY_BACKSPACE:	/* delete the prev. char, move to that position */
	    if (curs_pos > 0) {
		curs_pos--;
		wmove(wins, 0, curs_pos);
		mvwdelch(wins, 0, curs_pos);
		delete(curs_pos, ptt);
		wrefresh(wins);
	    }
	    break;
	case KEY_LEFT:
	    if (curs_pos > 0) {
		curs_pos--;
		wmove(wins, 0, curs_pos);
		wrefresh(wins);
	    }
	    break;
	case KEY_RIGHT:
	    if (curs_pos > last_i(ptt));
	    else {
		curs_pos++;
		wmove(wins, 0, curs_pos);
		wrefresh(wins);
	    }
	    break;
	case KEY_DOWN:
	    curs_set(0);	/* no cursor */
	    if (row < total_lines) {
		++row;
		scroll_pad(padw, row, y, x);
	    }
	    break;
	case KEY_UP:
	    curs_set(0);	/* no cursor */
	    if (row > 0) {
		--row;
		scroll_pad(padw, row, y, x);
	    }
	    break;
	case ESCAPE:
	    goto finish;
	    break;
	default:
	    wch = ch;
	    memset(&cch, 0, sizeof(cch));

	    if (setcchar(&cch, &wch, A_NORMAL, 0, NULL) != ERR) {
		curs_set(1);
		total_lines = 0;	       /* resetting total line count */
		insert(curs_pos, wch, ptt);
		mvwins_wch(wins, 0, curs_pos++, &cch);	/*  */
		wmove(wins, 0, curs_pos);
		wrefresh(wins);
	    }
	}
    }

  finish:
    fclose(fp);
    delwin(padw);
    delwin(wins);
    endwin();
    return 0;
}
