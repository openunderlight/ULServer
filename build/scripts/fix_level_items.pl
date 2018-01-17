#!/usr/bin/perl

# This script is intended to be run ONLY when the servers are DOWN
# It will go through all the items owned by rooms and levels and ensure
# that levels own no database items and that rooms do not have more than
# the max # of items

use DBI;

require "../lib/common.pl";

if (UnderlightRunning() == 1) {
    print "Underlight appears to be currently running - you must shut it down before running this script\n";
    exit(0);
}

$hostID = $ARGV[0];

if (!$hostID) {
    print "usage: fix_level_items.pl <host ID>\n";
    exit(-1);
}

# this is the magic number for max room items
my $MAX_ROOM_ITEMS = 64;

my $level_user = LookupDBUser("ul_level");
my $level_pw = LookupDBPassword("ul_level");

my $server_user = LookupDBUser("ul_server");
my $server_pw = LookupDBPassword("ul_server");

my $item_user = LookupDBUser("ul_item");
my $item_pw = LookupDBPassword("ul_item");

my @level_data;
my @level_ids;
my @item_data;
my @item_ids;
my $i;
my $num_levels = 0;

my $sdbh = DBI->connect($server_db, $server_user, $server_pw);

if (not $sdbh) {
    print "An error has occured while attempting to connect to the server db\n";
}

# first we need to fill the array level_ids with the ids of all the
# valid levels
my $levels = $sdbh->prepare("SELECT arg2 FROM server WHERE host_id = $hostID AND server_type = 'L'") || die $sdbh->errstr;
$levels->execute || die $levels->errstr;

while (@level_data = $levels->fetchrow_array) {
    my $level_id = $level_data[0];
    $level_ids[$num_levels] = $level_id;
    $num_levels++;
#    print "Adding level $level_id\n";
}
$levels->finish;

$sdbh->disconnect;

my $ldbh = DBI->connect($level_db, $level_user, $level_pw);

if (not $ldbh) {
    print "An error has occured while attempting to connect to the level db\n";
}

# now connect to the item database
my $idbh = DBI->connect($item_db, $item_user, $item_pw);

if (not $idbh) {
    print "An error has occured while attempting to connect to the item db\n";
}

# for each level, ensure there are not too many items owned either by the 
# level or by the rooms
for ($i = 0; $i < $num_levels; $i++) {


    # first, clear out any items owned by the level itself
    my $level_id = $level_ids[$i];
#    print "Checking level $level_id...\n";

    DeleteLevelItems($level_id);

    my $num_rooms = CountNumRooms($level_id);

#    print "Counted $num_rooms rooms in level $level_id\n";
    
    # now count the number of items for each room, and attempt to reduce
    # it if necessary through various measures
    for ($room_id = 0; $room_id < $num_rooms; $room_id++) {
	my $num_items = 0;
	my ($del_items, $items);
	
	$num_items = CountRoomItems($level_id, $room_id);
	if ($num_items > 0) {
#	    print"Counted $num_items in room $room_id in level $level_id\n";
	}
	if ($num_items > $MAX_ROOM_ITEMS) { # too many items!!!
	    # first, delete those with (x,y) = (0,0)
	    print "Too many items $num_items in room $room_id in level $level_id; attempting to remove all items with x,y=0,0\n";
	    $del_items = "DELETE FROM item WHERE owner_type = 0 AND owner_id = $level_id AND owner_subid = $room_id AND x = 0 AND y = 0";
	    my $items = $idbh->prepare($del_items) || die $idbh->errstr;
	    $items->execute || die $items->errstr;
	    $items->finish;
	}

	$num_items = CountRoomItems($level_id, $room_id);
	if ($num_items > $MAX_ROOM_ITEMS) { # too many items!!!
	    # next, delete essences
	    print "Too many items $num_items in room $room_id in level $level_id; attempting to remove all essence items\n";
	    $del_items = "DELETE FROM item WHERE owner_type = 0 AND owner_id = $level_id AND owner_subid = $room_id AND item_name LIKE '%Essence'";
	    my $items = $idbh->prepare($del_items) || die $idbh->errstr;
	    $items->execute || die $items->errstr;
	    $items->finish;
	}

	$num_items = CountRoomItems($level_id, $room_id);
	if ($num_items > $MAX_ROOM_ITEMS) { # too many items!!!
	    # next, delete charms
	    print "Too many items $num_items in room $room_id in level $level_id; attempting to remove all essence items\n";
	    $del_items = "DELETE FROM item WHERE owner_type = 0 AND owner_id = $level_id AND owner_subid = $room_id AND item_name LIKE '%Charm%'";
	    my $items = $idbh->prepare($del_items) || die $idbh->errstr;
	    $items->execute || die $items->errstr;
	    $items->finish;
	}

        # at this point, we loop and just start deleting items until
        # we are below the limit
	$num_items = CountRoomItems($level_id, $room_id);
	if ($num_items > $MAX_ROOM_ITEMS) {
	    print "Too many items $num_items in room $room_id in level $level_id; attempting to remove items arbitrarily\n";
	    # first, get a list of all the remaining item id's
	    $del_items = "SELECT item_id FROM item WHERE owner_type = 0 AND owner_id = $level_id AND owner_subid = $room_id";
	    my $items = $idbh->prepare($del_items) || die $idbh->errstr;
	    $items->execute || die $items->errstr;
	    my $k = 0; # counter

	    while (@item_data = $items->fetchrow_array) {
		my $item_id = $item_data[0];
		$item_ids[$k] = $item_id;
		$k++;
	    }
	    $items->finish;
	    
	    # now delete the appropriate number arbitrarily
	    $num_to_delete = $num_items - $MAX_ROOM_ITEMS;
	    for ($k = 0; $k < $num_to_delete; $k++) {
		my $item_id = $item_ids[$k];
		$del_items = "DELETE FROM item WHERE item_id = $item_id";
		my $items = $idbh->prepare($del_items) || die $idbh->errstr;
		$items->execute || die $items->errstr;
		$items->finish;
	    }
	}

	$num_items = CountRoomItems($level_id, $room_id);
	if ($num_items > $MAX_ROOM_ITEMS) { 
	    die "ERROR: even after attempting cleaning, there are still $num_items left in room $room_id in level $level_id; this must be reduced to less than $MAX_ROOM_ITEMS in order to start the level server";
	}
    }
}


$levels->finish;
$idbh->disconnect;
$ldbh->disconnect;

exit(0);

sub DeleteLevelItems {
    my ($level_id) = @_;

    my $del_items = "DELETE FROM item WHERE owner_type = 0 AND owner_id = $level_id AND owner_subid = 0";
    my $items = $idbh->prepare($del_items) || die $idbh->errstr;
    $items->execute || die $items->errstr;
    $items->finish;
}


# count the number of rooms in a level
sub CountNumRooms {
    my ($level_id) = @_;
    my $room_count = "SELECT COUNT(*) FROM room WHERE level_id = $level_id";
    my $rooms = $ldbh->prepare($room_count) || die $ldbh->errstr;
    $rooms->execute || die $rooms->errstr;
    @level_data = $rooms->fetchrow_array;
    my $num_rooms = $level_data[0];
    $rooms->finish;
    return $num_rooms;
}

sub CountRoomItems {
    my ($level_id, $room_id) = @_;

    my $item_count = "SELECT COUNT(*) FROM item WHERE owner_type = 0 AND owner_id = $level_id AND owner_subid = $room_id";
    my $item_sql = $idbh->prepare($item_count) || die $idbh->errstr;
    $item_sql->execute || die $item_sql->errstr;
    @item_data = $item_sql->fetchrow_array;
    my $num_items = $item_data[0];
    $item_sql->finish;
    return $num_items;
}

