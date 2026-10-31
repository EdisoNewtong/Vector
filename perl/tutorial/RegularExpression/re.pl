#!/usr/bin/perl 

# use strict;
# use strict;
# use warning;

=pod

  use re 'strict';
  use re '/msxx';
  use re '/u';
  use re '/d';
  use re '/xx';
  use re '/x';

=cut

$num11  = 123;
$str11  = "123";
$str12  = "";
print "type(num11) = ", ref(\$num11), "\n";   # output : SCALAR 
print "type(str11) = ", ref(\$str11), "\n";   # output : SCALAR 
print "type(str12) = ", ref(\$str12), "\n";   # output : SCALAR 

##################################################
# 
# Try the find string "World"  inside  "Hello World"
#   if found : return true
#       else : return false
#
##################################################
#   | subject   |         | pattern |
##################################################
#   "Hello World"        =~  /World/
#
#
##################################################
my $found_flag     = "Hello World"   =~   /World/;
my $not_found_flag = "Hello World"   !~   /World/;


if ( $found_flag ) {
    print("Found 'World' inside string 'Hello World' :) \n");
} else {
    print("[NOT] Found 'World' inside string 'Hello World' :( \n");
}

my $found_flag_with_m_prefix_1     = "Hello World"     =~   m!World!;
my $found_flag_with_m_prefix_2     = "Hello World"     =~   m{World};

# it match the string "/perl" after   /usr/bin
my $found_flag_with_m_prefix_3     = "/usr/bin/perl"   =~   m"/perl";
#                                             ^   ^
#                                             /perl


# doesn't match become  regular expression is case sensitive  w != W
my $found_flag_1     = "Hello World"   =~   /world/;

###############################################################################################
# Perl will always match at the earliest ( very beginning ) possible point in the string:
###############################################################################################
my $found_flag_2     = "Hello World"   =~   /o/;
#                           ^      ( It matchs the Hello's last ch 'o',  Not the 'o' after World's  'W' )

my $found_flag_3     = "That hat is red"   =~   /hat/;
#                        ^ ^     ( It matches T'hat'   rather then the   'hat' itself )

# / ... /    if '/' exists inside the surrounding / ... / , 
#   The inside '/' will be represented as \/  
#   Otherwise it will be treated as the end delimiter character ( because it is the closest character after the beginning '/' )
#
my $found_flag_4     = "/usr/bin/perl" =~ /\/usr\/bin\/perl/;  # matches

# case-insensitive   by  /i
print "1 -> true\n" if      "yes" =~ /yes/i;         
print "2 -> true\n" if      "Yes" =~ /yes/i;         
print "3 -> true\n" if      "yEs" =~ /yes/i;         
print "4 -> true\n" if      "YEs" =~ /yes/i;         
print "5 -> true\n" if      "yeS" =~ /yes/i;         
print "6 -> true\n" if      "YeS" =~ /yes/i;         
print "7 -> true\n" if      "yES" =~ /yes/i;         
print "8 -> true\n" if      "YES" =~ /yes/i;         

# If '-' is the first or last character in a character class, it is treated as an ordinary character.
my $range_1 = "-a" =~ /[-?]a/; # true
my $range_2 = "?a" =~ /[-?]a/; # true
my $range_3 = "-a" =~ /[?-]a/; # true
my $range_4 = "?a" =~ /[?-]a/; # true
my $range_5 = "-a" =~ /[\?-]a/; # true
my $range_6 = "?a" =~ /[\?-]a/; # true


if ( $range_1 ) { print("r1 = true\n"); } else { print("r1 = false\n"); }
if ( $range_2 ) { print("r2 = true\n"); } else { print("r2 = false\n"); }
if ( $range_3 ) { print("r3 = true\n"); } else { print("r3 = false\n"); }
if ( $range_4 ) { print("r4 = true\n"); } else { print("r4 = false\n"); }
if ( $range_5 ) { print("r5 = true\n"); } else { print("r5 = false\n"); }
if ( $range_6 ) { print("r6 = true\n"); } else { print("r6 = false\n"); }

my $b_at = "at" =~ /[^a]at/;  # => false ( NOT match )
if ( $b_at ) { print("b_at = true\n"); } else { print("b_at = false\n"); }

# Perl has several abbreviations for common character classes.
my $example_1 = "hello world" =~ /\w\W\w/;
my $example_2 = "2026/11" =~ /\w\W\w/;
my $example_3 = "2026" =~ /\w/;
if ( $example_3 ) { print("example_3 = true\n"); } else { print("example_3 = false\n"); }

################################################################################################################################
# The word anchor \b matches a **[Boundary]** between a word character and a non-word character \w\W or \W\w      : 
$x = "Housecat catenates house and cat";
my $ret123 = $x =~ /\bcat/;  # matches cat in 'catenates'
print("[NOW] ret123 = $ret123\n");
$x =~ /cat\b/;  # matches cat in 'housecat'

# In the last example, the end of the string is considered a word boundary.  
$x =~ /\bcat\b/;  # matches 'cat' at end of string 

# ??????????????????????????????????????????????????????????????????????????????????????????????????????????
#
# For natural language processing (so that, for example, apostrophes are included in words), use instead  "\b{wb}"
#   
# ??????????????????????????????????????????????????????????????????????????????????????????????????????????
#    I don't know
$yy =  "don't" =~ / .+? \b{wb} /x;  # matches the whole string
print "[NOW]  x'value = $yy  => ", 

####################################################################################################

/(a|b)b/;    # matches 'ab' or 'bb'
/(^a|b)c/;   # matches 'ac' at start of string or 'bc' anywhere

/house(cat|)/;  # matches either 'housecat' or 'house'
/house(cat(s|)|)/;  # matches either 'housecats' or 'housecat' or
                    # 'house'.  Note groups can be nested.

"20" =~ /(19|20|)\d\d/;  # matches the null alternative '()\d\d',
#               ^       ) after the 2nd | is <Empty>   =>  # because '20\d\d' can't match
#               ^

####################################################################################################
# Extracting matches
#
#    The grouping metacharacters () also allow the extraction of the parts of a string that matched. 
#      For each grouping, the part that matched inside goes into the special variables $1, $2, etc. ( Start from $1 )
#      They can be used just as ordinary variables:
#
# extract hours, minutes, seconds
$time =~ /(\d\d):(\d\d):(\d\d)/;  # match hh:mm:ss format
$hours = $1;
$minutes = $2;
$seconds = $3;


# In list context, a match /regex/ with groupings will return the list of matched values ($1,$2,...). So we could rewrite it as
($hours, $minutes, $second) = ($time =~ /(\d\d):(\d\d):(\d\d)/);



# If the groupings in a regex are nested, $1 gets the group with the leftmost opening parenthesis, $2 the next opening parenthesis, etc. For example, here is a complex regex and the matching variables indicated below it:

/(ab(cd|ef)((gi)|j))/;
#1  2      34

# Associated with the matching variables $1, $2, ... are the backreferences \g1, \g2, ... Backreferences are matching variables that can be used inside a regex:

# 'g' stands for *g*roup ,  the following example , it refers to the "a b " ( before 'c' )   inside the string "a b c d"  
/(\w\w\w)\s\g1/; # find sequences like 'the the' in string
# $1, $2, ... should only be used outside of a regex, and \g1, \g2, ... only inside a regex.

####################################################################################################
# Matching repetitions 
####################################################################################################
# a? = match 'a' 1 or 0 times    [0]
# 
# a* = match 'a' 0 or more times, i.e., any number of times
# 
# a+ = match 'a' 1 or more times, i.e., at least once
# 
# a{n,m} = match at least n times, but not more than m times.
# 
# a{n,} = match at least n or more times
# 
# a{,n} = match n times or fewer (Added in v5.34)
# 
# a{n} = match exactly n times
#

my $year4 = "2026";
my $year3 = "026";     # 3 digits doesn't matchs
my $year2 = "26";

my   $b_4 = $year4 =~ /^\d{4}$|^\d{2}$/; # true
my   $b_3 = $year3 =~ /^\d{4}$|^\d{2}$/; # false
my   $b_2 = $year2 =~ /^\d{4}$|^\d{2}$/; # true
if ( $b_4 ) { print("b_4 = true\n"); } else { print("b_4 = false\n"); }
if ( $b_3 ) { print("b_3 = true\n"); } else { print("b_3 = false\n"); }
if ( $b_2 ) { print("b_2 = true\n"); } else { print("b_2 = false\n"); }

# These quantifiers will try to match as **[MUCH]** of the string as possible, while still allowing the regex to match. So we have

$x = 'the cat in the hat';
$x =~ /^(.*)(at)(.*)$/; # matches,
                        # $1 = 'the cat in the h'
                        # $2 = 'at'
                        # $3 = ''   (0 matches)

# The first quantifier .* grabs as **[MUCH]** of the string as possible while still having the regex match. The second quantifier .* has no string left to it, so it matches 0 times. 


my $tmpBool_1 = "x}" =~ /x}/;
my $tmpBool_2 = "x}" =~ /x\}/;
if ( $tmpBool_1 ) { print("tmpBool_1 = true\n"); } else { print("tmpBool_1 = false\n"); }
if ( $tmpBool_2 ) { print("tmpBool_2 = true\n"); } else { print("tmpBool_2 = false\n"); }
print("\n");

=pod
my $sss1 = "kkk 
abc";

my $sss2 = "kkk \nabc";


if ( $sss1 =~  /kkk abc/  ) { print("tmpBool_3 = true\n"); } else { print("tmpBool_3 = false\n"); } 
if ( $sss1 =~ m/kkk abc/  ) { print("tmpBool_4 = true\n"); } else { print("tmpBool_4 = false\n"); } 

if ( $sss2 =~  /kkk abc/ ) { print("tmpBool_5 = true\n");  } else { print("tmpBool_5 = false\n"); } 
if ( $sss2 =~ m/kkk abc/  ) { print("tmpBool_6 = true\n"); } else { print("tmpBool_6 = false\n"); } 
=cut


=pod 
my $aaa = 3;
my $bbb = 4;
my $ssss1 = "abc";
my $ssss2 = 'abc';

my $varInfo1 = ref($aaa);    
print "|", ref(\$aaa), "|\n"; print "\n";    print "ref(varInfo1) = ", ref($varInfo1) , "\n";
my $varInfo2 = ref($ssss1);  print $varInfo2; print "\n";    print "ref(varInfo2) = ", ref($varInfo2) , "\n";
=cut



my $s_con1 = 'aa' . '\\' . 'bb';
my $s_con2 = 'aa' . '\\\\' . 'bb';
print("s_con1 = $s_con1\n");
print("s_con2 = $s_con2\n");


if ( $s_con1 =~ /aa\\\\bb/ ) {
    print "$s_con1 =~ ! aa\\\\bb !  => true\n";
} else {
    print "$s_con1 =~ ! aa\\\\bb !  => false\n";
}

if ( $s_con1 =~ /aa\\bb/ ) {
    print "$s_con1 =~ ! aa\\bb !   => true\n";
} else {
    print "$s_con1 =~ ! aa\\bb !   => false\n";
}

if ( $s_con2 =~ /aa\\\\bb/ ) {
    print "$s_con2 =~ ! aa\\\\bb !   => true\n";
} else {
    print "$s_con2 =~ ! aa\\\\bb !  => false\n";
}

if ( $s_con2 =~ /aa\\bb/ ) {
    print "$s_con2 =~ ! aa\\bb !   => true\n";
} else {
    print "$s_con2 =~ ! aa\\bb !   => false\n";
}


print "\n\n";
if ( "aa cc" =~ /aa\bcc/ ) {
    print "1. Match back-slash B \n";
} else {
    print "1. [NOT] match back-slash B \n";
}


if ( "aa cc" =~ /aa\\bcc/ ) {
    print "2. Match back-slash B \n";
} else {
    print "2. [NOT] match back-slash B \n";
}


if ( "aa-cc" =~ /aa\\bcc/ ) {
    print "3. Match back-slash B \n";
} else {
    print "3. [NOT] match back-slash B  \n";
}


=pod

Perl Meta-Characters : { } [ ] ( ) ^ $ . | * + ? \  
####################################################################################################

           PURPOSE                                  WHERE
\   Escape the next character                    Always, except when
                                                 escaped by another \
^   Match the beginning of the string            Not in []
      (or line, if /m is used)
^   Complement the [] class                      At the beginning of []
.   Match any single character except newline    Not in []
      (under /s, includes newline)
$   Match the end of the string                  Not in [], but can
      (or before newline at the end of the       mean interpolate a
      string; or before any newline if /m is     scalar
      used)
|   Alternation                                  Not in []
()  Grouping                                     Not in []
[   Start Bracketed Character class              Not in []
]   End Bracketed Character class                Only in [], and
                                                   not first
*   Matches the preceding element 0 or more      Not in []
      times
+   Matches the preceding element 1 or more      Not in []
      times
?   Matches the preceding element 0 or 1         Not in []
      times
{   Starts a sequence that gives number(s)       Not in []
      of times the preceding element can be
      matched
{   when following certain escape sequences
      starts a modifier to the meaning of the
      sequence
}   End sequence started by {
-   Indicates a range                            Only in [] interior
#   Beginning of comment, extends to line end    Only with /x modifier

####################################################################################################

=cut



