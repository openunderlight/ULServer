#!/usr/bin/perl

## put_leveldb.pl
##
## Create a Lyra level database entry from an input specification file.
##
## usage: put_leveldb.pl level.dat

require DBI;
require "../lib/common.pl";

# check args
if ($#ARGV < 0) {
    print "usage: put_leveldb.pl infile.dat\n";
    exit;
}

$infile = $ARGV[0];

my $level_user = LookupDBUser("ul_level");
my $level_pw = LookupDBPassword("ul_level");

my $item_user = LookupDBUser("ul_item");
my $item_pw = LookupDBPassword("ul_item");

if (! -r $infile) {
    print "make_level.pl: error: $infile not readable\n";
    exit;
}

use lib "../src";

# read level data
require $infile;

my $dbh = DBI->connect($level_db, $level_user, $level_pw);

if (not $dbh) {
    print "An error has occured while attempting to connect\n";
    exit(1);
}


# delete old level with same id, write basic level info 
my $level_id = $Level{ID};
my $level_name = $Level{Name};
my $num_rooms = $Level{Rooms};

my $level_delete = "DELETE FROM levl WHERE level_id = $level_id";
my $level_sql = $dbh->prepare($level_delete) || die $dbh->errstr;
$level_sql->execute || die $level_sql->errstr;

$level_delete = "DELETE FROM adj_room WHERE level_id = $level_id";
$level_sql = $dbh->prepare($level_delete) || die $dbh->errstr;
$level_sql->execute || die $level_sql->errstr;

$level_delete = "DELETE FROM item_gen WHERE level_id = $level_id";
$level_sql = $dbh->prepare($level_delete) || die $dbh->errstr;
$level_sql->execute || die $level_sql->errstr;

$level_delete = "DELETE FROM room WHERE level_id = $level_id";
$level_sql = $dbh->prepare($level_delete) || die $dbh->errstr;
$level_sql->execute || die $level_sql->errstr;

print "All old tables for level $level_id deleted\n";

my $level_insert = "INSERT INTO levl VALUES ($level_id, \'$level_name\')";
$level_sql = $dbh->prepare($level_insert) || die $dbh->errstr;
$level_sql->execute || die $level_sql->errstr;

# to hold room and item gen info
my %Room;
my %ItemGen;

# locals for storing room and gen info
my $room_insert, $room_sql;
my $portal_insert, $portal_sql, $num_portals, $port_dest; 
my $num_gens, $gen_insert, $gen_sql, $gen_id, $gen_type;
my $x, $y, $z, $angle;
my @gen_fields = ("BaseDelay", "VarDelay", "TimeToLive", "VarTimeToLive");
my @room_fields = ("Name", "MaxPlayers", "EntryArea", "Generators", "Portals", "NoReapItems");

# now  write the room info

for ($i = 1; $i <= $num_rooms; ++$i) {
#for ($i = 1; $i <= 1; ++$i) {

#    print "Writing room info for room $i\n";

    eval("%Room = %Room_$i;");

    # fix up with defaults if necessary
    foreach (@room_fields) {
	if ($Room{$_} eq "") {
	    $Room{$_} = $Room_Default{$_};
	}
    }  
    
    $room_insert = "INSERT INTO room VALUES ($level_id, $i, \'$Room{Name}\', $Room{EntryArea}, $Room{MaxPlayers}, $Room{NoReapItems})";
    print "Added entry for room $i\n";
#    print $room_insert;
    $room_sql = $dbh->prepare($room_insert) || die $dbh->errstr;
    $room_sql->execute || die $room_sql->errstr;

# now write the adjacent room info
    
    $num_portals = $Room{Portals};
    my @portals; # clear portal dest for each room
    my @connected_rooms; # rooms already connected for each level
    for ($j = 1; $j <= $num_portals; ++$j) {
   
	$portal_dest = $Room{"Portal_${j}"};
	($dest_level, $dest_room) = split(/\:/, $portal_dest);
	my $rooms_already = $portals["$dest_level"];
#	print "ra for $dest_level, $dest_room : $rooms_already\n";
	my $result = index("$rooms_already:", "::$dest_room:");

#	if (1)
	if ($result == -1) 
	{
	    $portals["$dest_level"].= "::$dest_room";
	    $portal_insert = "INSERT INTO adj_room VALUES ($level_id, $i, $dest_level, $dest_room)";
	    $portal_sql = $dbh->prepare($portal_insert) || die $dbh->errstr;
	    $portal_sql->execute;
	    print "Adding portal to $dest_level, $dest_room\n";
	    print $portal_insert,"\n";
	}
#	else { print "duplicate!\n" }
	
    }

# now write item generator info
    $num_gens = $Room{"Generators"};
    for ($j = 1; $j <= $num_gens; ++$j) {
	$gen_id = "${i}_${j}";
	# get generator type
	eval("%ItemGen = %ItemGen_${gen_id};");
	$gen_type = $ItemGen{GeneratorType};
	if (($gen_type < 1) || ($gen_type > 5)) {
	    print "make_level.pl: warning: unknown generator type $gen_type; using '1' instead\n";
	    $gen_type = 1;
	}
	# get defaults based on type
	eval("%ItemGen_Default = %ItemGen_Default_${gen_type};");
	
	# fix up with defaults if necessary
	foreach (@gen_fields) {
	    if ($ItemGen{$_} eq "") {
		$ItemGen{$_} = $ItemGen_Default{$_};
	    }
	}  

	# dig out the position info
	$gen_pos_values = $ItemGen{Position};

	($x, $y, $z, $angle) = split(/\./, $gen_pos_values);
	
	$gen_insert = "INSERT INTO item_gen VALUES ($level_id, $i, $ItemGen{BaseDelay}, $ItemGen{VarDelay}, $ItemGen{TimeToLive}, $ItemGen{VarTimeToLive}, $x, $y, $z, $angle, $gen_type)";
	$gen_sql = $dbh->prepare($gen_insert) || die $dbh->errstr;

	$gen_sql->execute || die $gen_sql->errstr;

#	print $gen_insert;
    }

    print "Item gens and room info for room $i written\n";

}

$dbh->disconnect;


# also set up the rows in the locations table in the item db so
# that sense_dreamers can work

my $i_dbh = DBI->connect($item_db, $item_user, $item_pw);

if (not $i_dbh) {
    print "An error has occured while attempting to connect\n";
    exit(1);
}

print "Removing entry in locations table for level $level_id\n";
my $delete = "DELETE FROM locations WHERE level_id = $level_id";
my $sql = $i_dbh->prepare($delete) || die $i_dbh->errstr;
$sql->execute || die $sql->errstr;

my $delete = "INSERT INTO locations VALUES($level_id, 0)";
my $sql = $i_dbh->prepare($delete) || die $i_dbh->errstr;
$sql->execute || die $sql->errstr;
print "Added entry in locations table for level $level_id\n";

$i_dbh->disconnect;


exit;

