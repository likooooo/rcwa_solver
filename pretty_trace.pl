#!/usr/bin/perl

# When compiled with -DENABLE_RS_TRACE, the trace output can be formatted
# with this script into an indented form.
# ./example  2>&1 | perl ../pretty_trace.pl
my $level = 0;

while(<STDIN>){
	if(/^>/){
		print (' ' x $level++);
	}elsif(/^</){
		print (' ' x --$level);
	}
	print;
}
