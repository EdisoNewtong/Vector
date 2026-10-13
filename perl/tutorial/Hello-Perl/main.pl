#!/usr/bin/perl 


=pod
  use strict;
  use warnings;
=cut 


####################################################################################################
#
# After character Hash '#'      This single line is commented 
#
####################################################################################################

=pod After =pod
     line 1 is commented 
     line 2 is commented 
     line 3 is commented 

=cut 



print "Hello Perl 5 #1\n";
print("Hello Perl 5 #2\n");


print "\n";


print 
"Hello
    << Perl 5 >>\n";

print ("\n");

# my is a 'keyword' to define a new varible
my $student_count = 5;
print("At the beginning , student count is $student_count\n");


$student_count = $student_count + 1;
print("After a new student come in, currently the new student count is $student_count\n");



print ("\n");
my $long_str_1 = '
    Breaking News :
News 1 : Content of news #1
News 2 : Content of news #2
   ...
News N : Content of news #N
';


print("long string #1 = $long_str_1\n");

print ("\n");
my $long_str_2 = "
    Breaking News :
News 1 : Content of news #1
News 2 : Content of news #2
   ...
News N : Content of news #N
";


print("long string #2 = $long_str_2\n");

