/*  more03.c  - version 0.3 of more
 *	read and print xx lines then pause for a few special commands
 *	v03:
 */
#include <stdio.h>
#include <stdlib.h>

#define  PAGELEN	24
#define  ERROR		1
#define  SUCCESS	0
#define  has_more_data(x)   (!feof(x))
#define	CTL_DEV	"/dev/tty"		/* source of control commands	*/

int  do_more(FILE *);
int  how_much_more(FILE *);
void print_one_line(FILE *);

// more03 changes
#include "termfuncs.h"
int update_page_height();
int update_page_width();
//

int main( int ac , char *av[] )
{
    FILE	*fp;			/* stream to view with more	*/
    int	result = SUCCESS;	/* return status from main	*/

    if ( ac == 1 )
        result = do_more( stdin );
    else
        while ( result == SUCCESS && --ac )
            if ( (fp = fopen( *++av , "r" )) != NULL ) {
                result = do_more( fp ) ;
                fclose( fp );
            }
            else
                result = ERROR;
    return result;
}
/*  do_more -- show a page of text, then call how_much_more() for instructions
 *      args: FILE * opened to text to display
 *      rets: SUCCESS if ok, ERROR if not
 */
int do_more( FILE *fp )
{
    // more03 changes
    // Get the current terminal height
    int page_height = update_page_height();
    //

    int	space_left = page_height ;		/* space left on screen */
    int	reply;				/* user request		*/
    FILE	*fp_tty;			/* stream to keyboard	*/

    fp_tty = fopen( CTL_DEV, "r" );		/* connect to keyboard	*/
    while ( has_more_data( fp ) ) {		/* more input	*/
        if ( space_left <= 0 ) {		/* screen full?	*/
            reply = how_much_more(fp_tty);	/* ask user	*/
            if ( reply == 0 )		/*    n: done   */
                break;
            space_left = reply;		/* reset count	*/
        }
        print_one_line( fp );
        space_left--;				/* count it	*/
    }
    fclose( fp_tty );			/* disconnect keyboard	*/
    return SUCCESS;				/* EOF => done		*/
}

/*  print_one_line(fp) -- copy data from input to stdout until \n or EOF */
void print_one_line( FILE *fp )
{
    //more03 changes
    // Get the current terminal width
    int page_width = update_page_width();
    // Overwrite the current standard output line to eliminate "more ?"
    printf("\r");
    // Counter variable so we don't word wrap
    int count = 1;
    //

    int	c;

    while( ( c = getc(fp) ) != EOF && c != '\n' )
        // Check count against page width to determine if we should keep writing characters
        if (count <= page_width) {
            putchar(c);
            count++;
        }
    putchar('\n');
}

/*  how_much_more -- ask user how much more to show
 *      args: none
 *      rets: number of additional lines to show: 0 => all done
 *	note: space => screenful, 'q' => quit, '\n' => one line
 */
int how_much_more(FILE *fp)
{
    // more03 changes
    // Get the current terminal height
    int page_height = update_page_height();
    //

    int	c;

    printf("\033[7m more? \033[m");		/* reverse on a vt100	*/
    // more03 changes
    //while( (c = getc(fp)) != EOF )		/* get user input	*/
    // Use the rawgetc in termfuncs.c so we don't need to press Enter
    while( (c = rawgetc(fp)) != EOF )
    //
    {
        if ( c == 'q' )			/* q -> N		*/
            return 0;
        if ( c == ' ' )			/* ' ' => next page	*/
            return page_height;		/* how many to show	*/
        if ( c == '\n' )		/* Enter key => 1 line	*/
            return 1;
    }
    return 0;
}

/*  update_page_len -- get the height of the terminal
 *      args: none
 *      rets: number of rows in the terminal
 *	note:
 */
int update_page_height()
{
    // Update the page length
    int page_height = PAGELEN;
    int row_cols[2] = {0, 0};

    // Get the terminal dimensions
    get_term_size(row_cols);

    // Check for an error code
    if ( get_term_size(row_cols) != ERROR)
    {
        page_height = row_cols[0];
    }
    else
        printf("There was an error calling get_term_size");

    return page_height;
}

/*  update_page_width -- get the terminal width
 *      args: none
 *      rets: number of cols in the terminal
 *	note:
 */
int update_page_width()
{
    // Update the page length
    int page_width = 0;
    int row_cols[2] = {0, 0};

    // Get the terminal dimensions
    get_term_size(row_cols);

    // Check for an error code
    if ( get_term_size(row_cols) != ERROR)
    {
        page_width = row_cols[1];
    }
    else
        printf("There was an error calling get_term_size");

    return page_width;
}
