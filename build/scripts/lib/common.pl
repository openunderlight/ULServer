#!/usr/bin/perl

# Lyra Studios Perl library - meant to be included by Perl scripts only

# look up database passwords from shared password file

$ENV{UL_HOME} = "$ENV{HOME}";
$ENV{UL_PW_FILE} = $ENV{UL_HOME} . "/lyra/src/pw.txt";

# this is kinda ugly and slow, but its simple and it works

$db_host = LookupDBHost();
if($db_host == "localhost") {
  $db_host = "127.0.0.1";
}
$db_port = LookupDBPort();
$mysql_db = "DBI:mysql:mysql:$db_host:$db_port";
$billing_db = "DBI:mysql:ul_billing:$db_host:$db_port";
$item_db = "DBI:mysql:ul_item:$db_host:$db_port";
$player_db = "DBI:mysql:ul_player:$db_host:$db_port";
$guild_db = "DBI:mysql:ul_guild:$db_host:$db_port";
$level_db = "DBI:mysql:ul_level:$db_host:$db_port";
$server_db = "DBI:mysql:ul_server:$db_host:$db_port";

sub LookupDBPassword {
    local($db_key) = @_;
    local($pw_return);

    $pw_return = "";
    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($db, $user, $pw) = split;
	if ($db eq $db_key) {
	    $pw_return = $pw;
	}
    }
    close(FILE);
    return $pw_return;
}

sub LookupDBUser {
    local($db_key) = @_;
    local($user_return);
    
    $user_return = "";
    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($db, $user, $pw) = split;
	if ($db eq $db_key) {
	    $user_return = $user;
	}
    }
    close(FILE);

    return $user_return;
}

sub LookupDBHost {
    local($host_return);
    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($key, $host) = split;
	if ($key eq "DBHOST") {
	    $host_return = $host;
	}
    }
    close(FILE);

    return $host_return;
}


sub LookupDBPort {
    local($port_return);
    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($key, $port) = split;
	if ($key eq "DBPORT") {
	    $port_return = $port;
	}
    }
    close(FILE);

    
    return $port_return;
}

sub LookupDBAdmin {
    local($admin_return);
    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($key, $admin) = split;
	if ($key eq "DBADMIN") {
	    $admin_return = $admin;
	}
    }
    close(FILE);

    return $admin_return;
}

sub LookupDBReturnEmail {
    local($email_return);

    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($key, $email) = split;
	if ($key eq "DBRETURNEMAIL") {
	    $email_return = $email;
	}
    }
    close(FILE);

    return $email_return;
}

sub LookupDBKey {
    local($key_return);
    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($key, $part1, $part2) = split;
	if ($key eq "DBKEY") {
	    $key_return = pack($part1, $part2);
	}
    }
    close(FILE);

    return $key_return;
}

sub LookupDBSalt {
    local($salt_return);
    open(FILE, $ENV{UL_PW_FILE});
    while (<FILE>) {
	local ($key, $salt) = split;
	if ($key eq "DBSALT") {
	    $salt_return = $salt;
	}
    }
    close(FILE);

    return $salt_return;
}


sub UnderlightRunning {
    my $pidfile = $ENV{UL_HOME}."/var/pid/masterd.pid";
    $running = 0;
# check that input file exists and is readable
    if (-r $pidfile) {
	$running = 1;
    }
    return $running;
}



#
# Hostname interface
#

sub Hostname {
    my ($hostname, $error);
    local *HOSTNAME;
    local %ENV;

    %ENV = %::ENV;
    delete($ENV{IFS});
    $error = 0;
    $hostname = undef;
    open HOSTNAME, "< /proc/sys/kernel/hostname" or
	open HOSTNAME, "/bin/hostname |" or
	    open HOSTNAME, "/bin/uname -n |" or $error = 1;
    if(!$error) {
	$hostname = <HOSTNAME>;
	close HOSTNAME;
	chomp $hostname;
    } else {
	$hostname = $::ENV{HOSTNAME} if exists($::ENV{HOSTNAME});
    }

    if($hostname =~ /^([\d\w\-.]+)/) {
	$hostname = $1;
    }
    return $hostname;
}

1; # so require returns true
