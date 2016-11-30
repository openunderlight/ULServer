project('Underlight Server', 'cpp', default_options: ['default_library=static'])
add_project_arguments(	'-std=c++0x',
			'-fpermissive',
			'-DUSE_INLINE',
			'-DUSE_DEBUG',
			'-D_REENTRANT',
			'-DSERVER',
			'-DRELEASE',
			'-D_POSIX_PTHREAD_SEMANTICS',
			'-D_POSIX_PER_PROCESS_TIMER_SOURCE',
			language: 'cpp')
add_project_link_arguments('-rdynamic', '-L/usr/lib64/mysql', '-lmysqlclient', '-lz', '-lcrypt', '-lnsl', '-lm', '-lssl', '-lcrypto', '-lpth', '-lgdbm', language: 'cpp')
incdir = include_directories('include')
rc = run_command('mysql_config', '--include')
libmysqlflags = rc.stdout().strip()

subdir('libsrc/db')
subdir('libsrc/dbi_gdbm')
subdir('libsrc/dbi_mysql')
subdir('libsrc/gdbm')
subdir('libsrc/gmsg')
subdir('libsrc/net')
subdir('libsrc/private')
subdir('libsrc/pth')
subdir('libsrc/rmsg')
subdir('libsrc/server')
subdir('libsrc/shared')
subdir('libsrc/smsg')
subdir('libsrc/util')

LYRA = library('Lyra', 
	objects: [	DB.extract_all_objects(),
			DBI_GDBM.extract_all_objects(),
			DBI_MYSQL.extract_all_objects(),
			GDBM.extract_all_objects(),
			GMSG.extract_all_objects(),
			NET.extract_all_objects(),
			PRIVATE.extract_all_objects(),
			PTH.extract_all_objects(),
			RMSG.extract_all_objects(),
			SERVER.extract_all_objects(),
			SHARED.extract_all_objects(),
			SMSG.extract_all_objects(),
			UTIL.extract_all_objects()
	])

subdir('server/gamed')
subdir('server/leveld')
subdir('server/masterd')
subdir('util')
