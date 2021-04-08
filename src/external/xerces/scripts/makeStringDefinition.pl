#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#

use vars qw($opt_h $opt_x);
use Getopt::Std;

my %charMapping = ( ':', 'chColon',
                    '\\', 'chBackSlash',
                    '[', 'chOpenSquare',
                    ']', 'chCloseSquare',
                    '{', 'chOpenCurly',
                    '}', 'chCloseCurly',
                    '?', 'chQuestion',
                    '(', 'chOpenParen',
                    ')', 'chCloseParen',
                    '/', 'chForwardSlash',
                    '*', 'chAsterisk',
                    '+', 'chPlus',
                    '.', 'chPeriod',
                    '-', 'chDash',
                    '|', 'chPipe',
                    '_', 'chUnderscore',
                    ',', 'chComma',
                    '&', 'chAmpersand',
                    '0', 'chDigit_0', '1', 'chDigit_1', '2', 'chDigit_2',
                    '3', 'chDigit_3', '4', 'chDigit_4', '5', 'chDigit_5',
                    '6', 'chDigit_6', '7', 'chDigit_7', '8', 'chDigit_8',
                    '9', 'chDigit_9');

#
# usage: display usage message
#
sub usage() {
    print<<EOF;
usage: $0 [ options ] word

Takes a word and produces a static XMLCh * definition for it.

Options:
    -h Displays this help message
    -x add the XERCES_CPP_NAMESPACE_QUALIFIER before each item
EOF
    exit(1);
}

#
# main:
#

getopts("hx");

if ($opt_h or @ARGV == 0) {
    usage();
}

my $word = $ARGV[0];

print "{ ";

while ($word=~s/^(.)//) {
  if (defined($charMapping{$1})) {
    $ch = $charMapping{$1};
  } else {
    $ch = $1;
    if ($ch=~/[A-Za-z]/) {
      $ch = "chLatin_$ch";
    } else {
      $ch = "UNKNOWN_CHAR_$ch";
    }
  }
	if($opt_x) {
			print "XERCES_CPP_NAMESPACE_QUALIFIER ";
	}
	print "$ch, ";
}

	if($opt_x) {
			print "XERCES_CPP_NAMESPACE_QUALIFIER ";
	}
print "chNull };\n";
