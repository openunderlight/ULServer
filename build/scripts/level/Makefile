# Makefile

all:
	@echo "Must specify what to do: level, server."

# create level files

level:
	for f in level_?.dat level_??.dat; do ../db/put_leveldb.pl $$f prod; done


# create server file

live:
	../db/put_serverdb.pl live_server.dat prod

dev: 
	../db/put_serverdb.pl dev_server.dat prod

win:
	../db/put_serverdb.pl win_server.dat prod

one:    
	../db/put_serverdb.pl dev_server_one.dat prod

split:    
	../db/put_serverdb.pl split_server.dat prod



