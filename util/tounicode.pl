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
	if (($logger2 eq "TLOG_Error(") ||
	    ($logger2 eq "TLOG_Debug(") ||
	    ($logger2 eq "TLOG_Log(") ||
	    ($logger2 eq "TLOG_Warning(") ||
	    ($logger2 eq "TLOG_Security(") ||
	    ($logger2 eq "log_->Debug(") ||
	    ($logger2 eq "log_->Error(") ||
	    ($logger2 eq "SECLOG(1,") ||
	    ($logger2 eq "SECLOG(-1,") ||
	    ($logger2 eq "SECLOG(-9") ||
	    ($logger2 eq "main->Log()->Error(") ||
	    ($logger2 eq "main->Log()->Warning(") ||
	    ($logger2 eq "main_->Log()->Error(") ||
	    ($logger2 eq "main_->Log()->Debug(") ||
	    ($logger2 eq "main_->Log()->Warning(") ||
	    ($logger2 eq "SECLOG(-3,") ||
	    ($logger2 eq "_ftprintf(f,") ||
	    ($logger2 eq "_tprintf(") ||
	    ($logger2 eq "_tscanf(") ||
	    ($logger2 eq "bail(") ||
	    ($logger2 eq "_stprintf(root_dir,") ||
	    ($logger2 eq "_ftprintf(outf,") ||
	    ($logger2 eq "_stprintf(pid_file,") ||
	    ($logger2 eq "SECLOG(6")) {
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


