project('Lyra Library', 'cpp', default_options: ['default_library=static'])
add_project_arguments(	'-std=gnu++0x',
			'-DUSE_INLINE',
			'-DUSE_DEBUG',
			'-D_REENTRANT',
			'-DSERVER',
			'-DRELEASE',
			'-D_POSIX_PTHREAD_SEMANTICS',
			'-D_POSIX_PER_PROCESS_TIMER_SOURCE',
			language: 'cpp')
incdir = include_directories('../include')
rc = run_command('mysql_config', '--include')
libmysqlflags = rc.stdout().strip()

subdir('db')
subdir('dbi_gdbm')
subdir('dbi_mysql')
subdir('gdbm')
subdir('gmsg')
subdir('net')
subdir('private')
subdir('pth')
subdir('rmsg')
subdir('server')
subdir('shared')
subdir('smsg')
subdir('util')

#library('Lyra', objects: DB.extract_all_objects()) 
library('Lyra', 
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
