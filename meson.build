project('Underlight Server', 'cpp', default_options: ['default_library=static'])
add_project_arguments(	'-std=c++0x',
			'-fpermissive',
			'-DUSE_INLINE',
			'-DUSE_DEBUG',
			'-DRELEASE',
			language: 'cpp')
add_project_link_arguments('libLyra.a', language: 'cpp')
add_project_link_arguments('-rdynamic', '-L/usr/lib64/mysql', '-lmysqlclient', '-lz', '-lcrypt', '-lnsl', '-lm', '-lssl', '-lcrypto', '-lpth', '-lgdbm', language: 'cpp')
incdir = include_directories('include')
rc = run_command('mysql_config', '--include')
libmysqlflags = rc.stdout().strip()
mysqlincdir = run_command('mysql_config','--variable=pkgincludedir').stdout().strip()
incdir = include_directories('include', mysqlincdir)

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
	DB_FILES,
	DBI_GDBM_FILES,
	DBI_MYSQL_FILES,
	GDBM_FILES,
	GMSG_FILES,
	NET_FILES,
	PRIVATE_FILES,
	PTH_FILES,
	RMSG_FILES,
	SERVER_FILES,
	SHARED_FILES,
	SMSG_FILES,
	UTIL_FILES,
	include_directories: incdir
)

subdir('server/gamed')
subdir('server/leveld')
subdir('server/masterd')
subdir('util')
