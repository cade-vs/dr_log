/****************************************************************************
 #
 # DR LOG -- simple prefixed file logging facility
 #
 # Copyright (c) 2025 Vladi Belperchinov-Shabanski "Cade" 
 # https://cade.noxrun.com/  <cade@noxrun.com>
 # https://github.com/cade-vs/
 #
 # DISTRIBUTED UNDER GPLv2 LICENSE 
 # SEE `README',`LICENSE' OR `COPYING' FILE FOR LICENSE AND OTHER DETAILS!
 # OR AT https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 #
 ****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/file.h>
#include <sys/stat.h>

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#include "dr_log.h"

#define MAX_PREFIX 16

// forwards
FILE *fp_save( const char *name, FILE *fp );
FILE *fp_get( const char *name );
void fp_remove( const char *name, int all );


static struct timespec last_ts = { 0, 0 };
static int    _debug      = 0;
static int    _use_stderr = 1;
static int    _use_files  = 0;
static char   _log_dir[PATH_MAX]  = "";
static char   _log_fn[PATH_MAX] = "";
static FILE  *_log_fp = NULL;

// avoid deps
int mkpath( const char *path, mode_t mode );
char* strncpyz( char *dst, const char *src, size_t size );
char* strncatz( char *dst, const char *src, size_t size );

void dr_log_debug_enable( int enable )
{
  _debug = enable;
}

int dr_log_is_debug()
{
  return _debug;
}

void dr_log_use_stderr( int enable )
{
  _use_stderr = enable;
}

int  dr_log_using_stderr()
{
  return _use_stderr;
}

void dr_log_use_files( int enable )
{
  _use_files = enable;
}

int dr_log_using_files()
{
  return _use_files;
}

void dr_log_set_file( const char *fname )
{
  if( _log_fp ) 
    {
    fclose( _log_fp ); 
    _log_fp = NULL;
    }
  snprintf( _log_fn, sizeof( _log_fn ), "%s", fname ? fname : "" );
  dr_log_use_files( 1 );
}

const char *dr_log_get_file()
{
  return _log_fn;
}

void dr_log_set_dir( const char *path, mode_t mode )
{
  if( ! mode ) mode = 0755;
  snprintf( _log_dir, sizeof( _log_dir ), "%s", path ? path : "" );
  if( *_log_dir ) mkpath( _log_dir, mode );
  dr_log_use_files( 1 );
}

const char *dr_log_get_dir()
{
  return _log_dir;
}

static const char *__get_prefix( const char *msg, char *prefix, size_t psize )
{
  const char *col;
  size_t len;
  int i;

  prefix[0] = 0;

  col = strchr( msg, ':' );
  if( ! col ) return msg;

  len = col - msg;
  if( len == 0 || len > MAX_PREFIX || len >= psize ) return msg;

  /* validate prefix: letters, digits, underscore only */
  for (i = 0; i < len; i++)
    if ( !isalnum(msg[i]) && msg[i] != '_' ) return msg;

  strncpy( prefix, msg, len );
  prefix[len] = 0;

  col++;
  while( *col == ' ') col++; // skip whitespace

  return col;
}

// write log message to specific single open log file
static void __log_file_write_single( const char *msg_line )
{
  int fd;

  if( ! _log_fn[0] ) return;

  if( ! _log_fp )
    _log_fp = fopen( _log_fn, "a" );
  if( ! _log_fp )
    return;

  fd = fileno( _log_fp );
  if( flock( fd, LOCK_EX ) < 0 ) return;

  fputs( msg_line, _log_fp );

  flock( fd, LOCK_UN );
}

// write log message to file using prefixes
static void __log_file_write_prefix( const char *prefix, const char *msg_line )
{
  int fd;

  if( ! prefix ) 
    {
    if( _log_fp ) fputs( msg_line, _log_fp );
    return;
    }

  FILE *fp = fp_get( prefix );
  if( ! fp )
    {
    char fn[PATH_MAX];
    snprintf( fn, PATH_MAX, "%s/%s.log", _log_dir, prefix );
    fp = fopen( fn, "a" );
    fp_save( prefix, fp );
    }

  fd = fileno(fp);
  if( flock( fd, LOCK_EX ) < 0 ) return;

  fputs( msg_line, fp );

  flock( fd, LOCK_UN );
}

void dr_log_msg( const char* msg )
{
  struct tm *tm;
  struct timespec ts;
  char ts_str[32];    // strftime date + time
  char prefix[MAX_PREFIX];    // strftime date + time
  char tsp_line[128]; // hires timestamp + prefix message lead
  long diff_sec  = 0;
  long diff_nsec = 0;
  long diff_ms   = 0;
  const char *rest;
  char msg_line[4096];

  clock_gettime( CLOCK_REALTIME, &ts );
  tm = localtime( &ts.tv_sec );
  strftime( ts_str, sizeof( ts_str ), "%Y%m%d-%H%M%S", tm );

  if( last_ts.tv_sec > 0 )
    {
    diff_sec  = ts.tv_sec  - last_ts.tv_sec;
    diff_nsec = ts.tv_nsec - last_ts.tv_nsec;
    }

  last_ts = ts;

  rest = __get_prefix( msg, prefix, MAX_PREFIX );

  snprintf( msg_line, sizeof(msg_line), "%s.%03ld+%ld.%03ld [%d] (%s) %s\n", ts_str, ts.tv_nsec / 1000000, diff_sec, diff_nsec / 1000000, getpid(), prefix[0] ? prefix : "*", rest );
                                                                                                   
  if( _use_files )
    {
    if( _log_fn[0] ) __log_file_write_single( msg_line );
    __log_file_write_prefix( "global", msg_line );
    if( prefix[0] ) __log_file_write_prefix( prefix, msg_line );
    }
  else
    {
    fputs( msg_line, stderr );
    }
}

void dr_log( const char *fmt, ... )
{
  va_list ap;
  char msg[4096];

  va_start( ap, fmt );
  vsnprintf( msg, sizeof( msg ), fmt, ap );
  va_end( ap );

  dr_log_msg( msg );
}

void dr_dbg( const char *fmt, ...)
{
  va_list ap;
  char msg[4096];

  if ( ! _debug ) return;

  va_start( ap, fmt );
  vsnprintf( msg, sizeof( msg ) - 7, fmt, ap );
  va_end( ap );

  dr_log_msg( msg );
}

void dr_log_close_all()
{
  if( _log_fp ) 
    {
    fclose( _log_fp ); 
    _log_fp = NULL;
    }
  fp_remove( NULL, 1 );
}

/*** avoid deps *************************************************************/

char* strncpyz( char *dst, const char *src, size_t size )
{
    size_t len = strlen(src);

    if( size < 0 ) return NULL;
    
    size_t cz = len < size ? len : size - 1;
    memcpy( dst, src, cz );
    dst[cz] = '\0';
    
    return dst;
}

char* strncatz( char *dst, const char *src, size_t size )
{
    return strncat( dst, src, size - 1 );
}

int mkpath( const char *path, mode_t mode ) 
{
  char   tmp[PATH_MAX];
  char   *p = NULL;
  size_t len;
  int    n = 0;

  strncpyz( tmp, path, sizeof(tmp) );
  
  len = strlen( tmp );
  
  if( tmp[len - 1] != '/' ) strncat( tmp, "/", sizeof(tmp) - 1 );

  for( n = 1; tmp[n]; n++ )
    {
    if( tmp[n] != '/' ) continue;
    tmp[n] = 0;
    if( mkdir( tmp, mode ) != 0 && errno != EEXIST ) return -1;
    tmp[n] = '/';
    }
  
  return 0;
}

/****************************************************************************/

#define MAX_LOG_CACHE 64

typedef struct 
{
    char name[MAX_PREFIX];
    FILE *fp;
    unsigned long uc; // use count
} fp_cache_item;

static fp_cache_item file_table[MAX_LOG_CACHE];

void static __fp_remove_n( int n )
{
  if( n < 0 || n >= MAX_LOG_CACHE || ! file_table[n].fp ) return;
  fclose( file_table[n].fp );
  //printf( "fp removed item %d (%s)\n", n, file_table[n].name );
  file_table[n].fp = NULL;
  file_table[n].name[0] = 0;
  file_table[n].uc = 0;
}

void fp_remove( const char *name, int all )
{
  for( int i = 0; i < MAX_LOG_CACHE; i++ ) 
    if( all || strcmp( file_table[i].name, name ) == 0 )
      {
      __fp_remove_n( i );
      if( ! all ) return;
      }
}

FILE *fp_get( const char *name )
{
  if( ! name ) return NULL;
  
  for (int i = 0; i < MAX_LOG_CACHE; i++) 
    if( strcmp( file_table[i].name, name ) == 0 ) 
      {
      file_table[i].uc++;
      //printf( "fp cache hit slot %d count %d (%s) <<<---\n", i, file_table[i].uc, name );
      return file_table[i].fp;
      }
  
  return NULL;
}

FILE *fp_save( const char *name, FILE *fp )
{
  if ( ! name || ! fp ) return NULL;

  int n = -1; // found place or least-used item index
  int c =  0; // current least used count

  for( int i = 0; i < MAX_LOG_CACHE; i++ )
    {
    if( ! file_table[i].fp )
      {
      n = i;
      //printf( "fp save to empty slot %d (%s)\n", n, name );
      break;
      }
    
    if( strcmp( file_table[i].name, name ) == 0 ) 
      {
      file_table[i].fp = fp;
      //printf( "fp save to cache hit %d (%s)\n", n, name );
      return fp;
      }
    else
      {
      if( c > file_table[i].uc || c == 0 )
        {
        c = file_table[i].uc;
        n = i;
        //printf( "fp lru slot %d count %d (%s) searching for (%s)\n", n, c, file_table[i].name, name );
        }
      }  
    }
  
  if( file_table[n].fp ) __fp_remove_n( n );
  //printf( "fp save to released slot %d (-> %s)\n", n, name );
  strncpyz( file_table[n].name, name, MAX_PREFIX );
  file_table[n].fp = fp;
  
  return fp;
}

/****************************************************************************/

#ifdef DR_LOG_TEST

int main()
{
  /* default: logs to stderr */
  dr_log("BEGIN");
  dr_log( "auth: User %s logged in", "jimmy" );

  /* switch to file logging */
  dr_log_use_files(1);
  dr_log_set_dir( "/tmp/dr_log/path/to/app", 0755 );
  dr_log_set_file( "/tmp/dr_log/path/to/app/my-separate-log-file.log" );
  
  dr_log("auth: Session created for %s", "john");

  dr_log("db: Connected to %s", "postgres");
  sleep(2);
  dr_log("db: Query executed in %dms", 42);

  dr_dbg("auth: 111 Token: %s", "abc123");
  dr_log_debug_enable(1);
  dr_dbg("auth: 222 Token: %s", "abc123");

  dr_log("test1: 222 Token: %s", "abc123");
  dr_log("test2: 222 Token: %s", "abc123");
  dr_log("test3: 222 Token: %s", "abc123");
  dr_log("test4: 222 Token: %s", "abc123");
  dr_log("test5: 222 Token: %s", "abc123");
  dr_log("test1: 222 Token: %s", "abc123");
  dr_log("test6: 222 Token: %s", "abc123");
  dr_log("test7: 222 Token: %s", "abc123");
  dr_log("test1: 222 Token: %s", "abc123");
  dr_log("test8: 222 Token: %s", "abc123");

  dr_log_close_all();

  dr_log("se8: 222 Token: %s", "abc123");
  dr_log("se2: 222 Token: %s", "abc123");
  dr_log("se5: 222 Token: %s", "abc123");
  dr_log("se2: 222 Token: %s", "abc123");
  dr_log("se8: 222 Token: %s", "abc123");
  dr_log("se7: 222 Token: %s", "abc123");
  dr_log("se8: 222 Token: %s", "abc123");
  dr_log("se9: 222 Token: %s", "abc123");

  dr_log("test1: 222 Token: %s", "abc123");
  dr_log("test2: 222 Token: %s", "abc123");
  dr_log("test3: 222 Token: %s", "abc123");
  dr_log("test4: 222 Token: %s", "abc123");
  dr_log("test5: 222 Token: %s", "abc123");
  dr_log("test1: 222 Token: %s", "abc123");
  dr_log("test6: 222 Token: %s", "abc123");
  dr_log("test7: 222 Token: %s", "abc123");
  dr_log("test1: 222 Token: %s", "abc123");
  dr_log("test8: 222 Token: %s", "abc123");

  dr_log("END");

  //printf( "fp_cache_item size %d\n", sizeof( fp_cache_item ) );

  dr_log_close_all();

  return 0;
}

#endif
