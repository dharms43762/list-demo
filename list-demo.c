/*
  Program: list-demo.c
  Author: Douglas Harms
  Modified By:  <insert student name here>
  
  Description: This program demonstates the use of the list library in the
  pintos code.  It creates several lists of student information.

*/

#include <stdio.h>
#include <string.h>	// strcmp
#include <stdlib.h>	// malloc
#include "list.h"

struct student_info {
  char id[10];
  char last_name[20];
  char first_name[20];
  char major[4];
  char advisor[20];
  double gpa;
  struct list_elem elem_all;
  struct list_elem elem_major;
};

struct list all_students;
struct list majors;

/*
  return true iff the name of a is less than the name of b.  Used to
  maintain the all_students list in alphabetical order.
*/
bool
student_name_less( const struct list_elem *a_,
		   const struct list_elem *b_,
		   void *aux ) {
  struct student_info *a = list_entry( a_, struct student_info, elem_all );
  struct student_info *b = list_entry( b_, struct student_info, elem_all );

  // compare last names and return true if a's last name comes before b's
  // last name, or false if a's last name comes after b's last name
  int cmp = strcmp( a->last_name, b->last_name );
  if( cmp < 0 )
    return true;
  if( cmp > 0 )
    return false;

  // the last names are the same, so return true if a's first name comes
  // before b's first name, and return false otherwise
  cmp = strcmp( a->first_name, b->first_name);
  return cmp < 0;
}

/*
  define all students and place the records on appropriate lists
*/
void
read_students( char* filename )
{
  FILE *file = fopen( filename, "r" );
  char line[100];
  struct student_info *student;
  char *token;
  char *ptr;

  if( !file ) {
    printf( "error opening file \"%s\"\n", filename );
    return;
  }

  while( fgets( line, sizeof(line), file ) ) {
    
    // if the line is empty (or only contains the \n), we're done
    if( strlen(line) <= 1 )
      break;
     
    // allocate memory for the student
    student = malloc( sizeof( struct student_info ) );

    // extract the tokens on the line.  Assume that commas separate the
    // values, and that a new line \n terminates the line.  Note that I could
    // have used strtok instead of strtok_r, but I wanted to demostrate the
    // use of strtok_r since that is what students will use in Project 2.
    token = strtok_r( line, ",", &ptr );
    strncpy( student->id, token, 10 );
    token = strtok_r( NULL, ",", &ptr );
    strncpy( student->last_name, token, 20 );
    token = strtok_r( NULL, ",", &ptr );
    strncpy( student->first_name, token, 20 );
    token = strtok_r( NULL, ",", &ptr );
    strncpy( student->major, token, 4 );
    token = strtok_r( NULL, ",", &ptr );
    strncpy( student->advisor, token, 20 );
    token = strtok_r( NULL, "\n", &ptr );
    student->gpa = atof( token );

    // add the student to the all_students list, maintaining alphabetical
    // ordering
    list_insert_ordered( &all_students, &student->elem_all,
			 student_name_less, NULL );

    // add the student to the end of the majors list if they're a CSC major
    if( strcmp( student->major, "csc" ) == 0 )
      list_push_back( &majors, &student->elem_major );
  }

  // close the file and return
  fclose( file );
}

/*
  free up all memory used by the all_students list
*/
void
destroy_students (void) {
  struct student_info *s;
  struct list_elem *e;
  while( !list_empty( &all_students ) ) {
    e = list_pop_front( &all_students );
    s = list_entry( e, struct student_info, elem_all );
    free( s ) ;
  }
}

/*
  print information about all students
*/
void
print_all_students (void) {

  printf("All students are:\n");
  struct list_elem *e;
  for( e=list_begin(&all_students);e!=list_end(&all_students);e=list_next(e) ){
    struct student_info *s = list_entry(e,struct student_info, elem_all);
    printf( "%s, %s:\n", s->last_name, s->first_name );
    printf( "  id: %s\n  advisor: %s\n  major: %s\n  gpa: %f\n",
	    s->id, s->advisor, s->major, s->gpa );
  }
}


/* 
   print names of all csc majors
*/
void
print_csc(void) {
  struct list_elem *e;

  if( list_empty( &majors ) )
    printf("There are no CSC majors :-(\n");
  else {
    printf("CSC majors:\n");
    for(e=list_begin(&majors);e!=list_end(&majors);e=list_next(e)) {
      struct student_info *s = list_entry(e,struct student_info,elem_major);
      printf("%s %s\n",s->first_name, s->last_name );
    }
  }
}

/*
  return true iff student a's gpa is < student b's gpa.  Assumes the list
  is the all_students list (i.e., elem_all field )
*/
bool
gpa_less (const struct list_elem *a_, const struct list_elem *b_, void *aux ) {
  struct student_info *a = list_entry(a_,struct student_info,elem_all);
  struct student_info *b = list_entry(b_,struct student_info,elem_all);
  return a->gpa < b->gpa;
}

/*
  returns the student with the lowest gpa of all students
*/
struct student_info*
lowest_gpa(void) {
  struct list_elem *e = list_min( &all_students, gpa_less, NULL );
  struct student_info *s = list_entry(e, struct student_info, elem_all );
  return s;
}

/*
  main program
*/

int
main( int argc, char* argv[] ) {

  // initialize things
  list_init( &all_students );
  list_init( &majors );

  // check the command line arguments.  The command line must specify
  // the name of the file.
  if( argc != 2 ) {
    printf("usage:\n");
    printf("  ./list-demo <student-file-name>\n");
    return 1;  // error
  }
  
  // read in info
  read_students( argv[1] );

  // print them all out
  print_all_students();
  print_csc();

  // print the name of the student with the lowest gpa
  struct student_info *s = lowest_gpa();
  printf("%s %s has the lowest gpa of all students, which is %f\n",
	 s->first_name,s->last_name,s->gpa);
  
  // free up all memory
  destroy_students();

  return 0;  // success
}
