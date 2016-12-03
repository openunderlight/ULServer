#!/usr/bin/perl


while (<STDIN>) {
    $filename = $_;

# count the # of each type of gen on each level
    open(INFILE, $filename) or die "Could not open infile!\n";
    open(OUTFILE, ">outfile.txt") or die "Could not open outfile!\n";

    while (<INFILE>) {
	$full_line = $_;
	($logger, $text, $rest) = 
	    split(/\"/);
	$logger2 = $logger;
	$logger2 =~ s/[ \t\n\/]//g;
#	print "full line: $full_line\n";
	if (
	    ($logger2 eq "PDEBUG((") ||
	    ($logger2 eq "SECLOG(-2,") ||
	    ($logger2 eq "SECLOG(2,") ||
	    ($logger2 eq "SECLOG(3,") ||
	    ($logger2 eq "SECLOG(4,") ||
	    ($logger2 eq "SECLOG(6,") ||
	    ($logger2 eq "SECLOG(-4,") ||
	    ($logger2 eq "SECLOG(5,") ||
	    ($logger2 eq "SECLOG(-7,") ||
	    ($logger2 eq "SECLOG(7,") ||
	    ($logger2 eq "SECLOG(-8,") ||
	    ($logger2 eq "SECLOG(8,")
	    ) {
#	    print "full line: $full_line";
	    $outline = $logger.'_T("'.$text.'")'.$rest;
#	    print "logger: $logger text: $text rest: $rest";
#	    print "outline:.$outline";
	    print(OUTFILE $outline);
	    print "\n";
	} else {
	    print(OUTFILE $full_line);
	}
    }
    close(INFILE);
    close(OUTFILE);
    $command = "cp ./outfile.txt ./$filename";
    system $command;

}

exit;

