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

#ifndef DR_LOG_H
#define DR_LOG_H

void dr_log_debug_enable( int enable );
int  dr_log_is_debug();

void dr_log_use_files( int enable );
int  dr_log_using_files();

void        dr_log_set_file( const char *fname );
const char* dr_log_get_file();

void        dr_log_set_dir( const char *path, mode_t mode );
const char* dr_log_get_dir();

void dr_log_msg( const char* msg );
void dr_log( const char *fmt, ... );
void dr_dbg( const char *fmt, ... );

void dr_log_close_all();

#endif
