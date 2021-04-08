#use strict;

# usage perl harness.pl [testNum]
#   where testNum is an integer from 1 to 41
#   testNum may also be a list of integers denoting tests to be run
#   ommitting testNum will lead to all tests being run
#
#  this harness outputs the results to the file results.html as well 
#   as reporting them to stderr.
#
#  note that the XInclude sample application must be discoverable on 
#   the current PATH.

#
# Find out the platform from 'uname -s'
#
open(PLATFORM, "uname -s|");
$platform = <PLATFORM>;
chomp($platform);
close (PLATFORM);

if ($platform =~ m/Windows/i  || $platform =~ m/CYGWIN/i) {
    $pathsep = "\\";
}
else {
    $pathsep = "/";
}

@CORRECT_TEST_RESULTS = (
        -3,
	1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
	0, 0, 0, 0, 1, 1, 1, 0, 1, 1,
	1, 0, 1, 0, 0, 0, 1, 1, 0, 1,
	1, 0, 1, 1, 1, 1, 1, 1, 0, 0,
       	1
);

#    private static boolean[] TEST_RESULTS = new boolean[] {
#        // one value for each test
#        true, true, true, true, true, true, false, true, false, true, // 10
#        false, false, false, false, true, true, true, false, true, true, // 20
#        true, false, true, false, false, false, true, true, false, true, // 30
#        true, false, true, true, true, true, true, true, false, false, // 40
#        true, };

#no need to add one to this value since there is a dummy value in the tables
$NUM_TESTS = $#CORRECT_TEST_RESULTS;

@ACTUAL_TEST_RESULTS = (
        -3,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1
);

print "Running XInclude Tests\n";

$dateAndTime = localtime();

open(outfile, ">results.html");
print outfile "<html><head><title>XInclude results generated at $dateAndTime </title>";
print outfile "</head><body bgcolor=\"black\" text=\"white\"><center>";

print outfile "<table border cellspacing=\"0\" cellpadding=\"5\"><caption align=\"bottom\">Results of XI Test Run</caption>";
print outfile "<tr><th>Test Number</th><th>Test Input File</th><th>Generated Output File</th>";
print outfile "<th>Reference Result File</th><th>Result</th><th>Expected Result</th><th>Test Status</th></tr>";

$successes = 0;
$correct = 0;
$numTestsToRun = $#ARGV + 1;
while (<@ARGV>){
   $i = $_;
   if ($i < 10) {
      $testNum = "0$i";
   } else {
      $testNum = "$i";
   }
   $input_file =  "tests".$pathsep."xinclude".$pathsep."tests".$pathsep."test$testNum.xml";
   if ($CORRECT_TEST_RESULTS[$i] == 1) {
      $output_file = "tests".$pathsep."xinclude".$pathsep."written".$pathsep."test$testNum.xml";
      $expected_result_file = "tests".$pathsep."xinclude".$pathsep."cppoutput".$pathsep."test$testNum.xml";
   } else {
      $output_file = "tests".$pathsep."xinclude".$pathsep."written".$pathsep."test$testNum.txt";
      $expected_result_file = "tests".$pathsep."xinclude".$pathsep."cppoutput".$pathsep."test$testNum.txt";
   }

   # run the tests and apture the output
   $command = "XInclude $input_file $output_file";
   print "R[$command]\n";
   system("$command");

   $outcome = 0;
   if (! -e $output_file) {
    $ACTUAL_TEST_RESULTS[$i] = 0;
    # we didn't generate an output, we are correct if the test was expected to fail
    if($CORRECT_TEST_RESULTS[$i] eq 0) {
      $outcome = 1;
    }
   } else {
    $ACTUAL_TEST_RESULTS[$i] = 1;
    # we generated an output, we are correct if our result matches
    if($CORRECT_TEST_RESULTS[$i] eq 1) {
      $outcome = &compareFiles($expected_result_file, $output_file);
    }
   }

   print outfile "<tr bgcolor=";
   if ($outcome eq 1) {
      print outfile "green";
   } else {
      print outfile "red";
   }

   print outfile ">";
   #test number
   print outfile "<td>$testNum</td>";
   print outfile "<td>";
   $anchor = &createHTMLAnchor("$input_file");
   print outfile "$anchor";
   print outfile "</td><td>";
   $anchor = &createHTMLAnchor("$output_file");
   print outfile "$anchor";
   print outfile "</td><td>";
   $anchor = &createHTMLAnchor("$expected_result_file");
   print outfile "$anchor";
   print outfile "</td>";
   # actual result
   $result = (($ACTUAL_TEST_RESULTS[$i])?"true":"false");
   print outfile "<td>$result</td>";

   # correct result
   $result = (($CORRECT_TEST_RESULTS[$i])?"true":"false");
   print outfile "<td>$result</td>";

   if ($outcome eq 1) {
      $successes++;
      $result = "Passed";
      print "[test $testNum PASSED]\n";
      $correct++;
   } else {
      $result = "Failed";
      print "[test $testNum FAILED]\n";      
   }
   print outfile "<td>$result</td></tr>";
}

print outfile "</table>";

print "Tests Passed: $correct\n";
$percentSucceeding = $correct / $numTestsToRun * 100.0;
print outfile "<hr/><h2>$percentSucceeding% of tests that were run passed ($correct out of $numTestsToRun).</h2>";

$shPass = 0;
$shFail = 0;
for ($i = 1; $i < $NUM_TESTS+1; $i++){
   if ($CORRECT_TEST_RESULTS[$i]){
       $shPass++;
   } else {
       $shFail++;
   }
}

print outfile "<h2>$shPass should pass ($successes) did</h2>";
$failures = $NUM_TESTS - $successes;
print outfile "<h2>$shFail should fail ($failures) did</h2>";

$percentSucceeding = $correct / $NUM_TESTS * 100.0; 
print outfile "<h2>$percentSucceeding% of all available tests passed ($correct out of $NUM_TESTS).</h2>";

$percentSucceeding = $numTestsToRun / $NUM_TESTS * 100.0;
print outfile "<h2>$percentSucceeding% of all available tests were run ($numTestsToRun out of $NUM_TESTS).</h2>";

print outfile "</center><hr/></body></html>";
close(outfile);

# quick and dirty but functional
sub compareFiles 
{
   $result = 1;
   print "C[$_[0]]:[$_[1]]\n";

   if (! -e $_[0]) {
      print "No such file as $_[0] - cannot compare\n";
      return 0;
   }

   if (! -e $_[1]) {
      print "No such file as $_[1] - cannot compare\n";
      return 0;
   }

   open(expected, "<$_[0]");
   open(actual, "<$_[1]");

   #compare the files
   @expectedData = <expected>;
   @actualData = <actual>;

   close(expected);
   close(actual);

   for ($x = 0, $a = 0; $x < $#expectedData && $a < $#actualData; $x++, $a++){
      $dataLineEx = $expectedData[$x];
      $dataLineAc = $actualData[$a];
      chomp($dataLineEx);
      chomp($dataLineAc);
      if ($dataLineEx ne $dataLineAc) {
         # check if its a warning and can be ignored at this stage
         if ($dataLineEx =~ /Warning/) {
            print "probably just a warning line:\n";
            print "[$dataLineEx]\n";
            $a--
         } elsif ( $dataLineEx =~ m/^\s*$/ && $dataLineAc =~ m/^\s*$/ ) {
            $line_num = $x + 1;
            print "Line $line_num ws difference: \n[$dataLineEx] != \n[$dataLineAc]\n";
            print "probably just white space on both lines\n";
            $a--
         } else {
            $line_num = $x + 1;
            print "Line $line_num difference: \n[$dataLineEx] != \n[$dataLineAc]\n";
            print "From files: $_[0] $_[1]\n";
            return 0;
         }
      }
   }

   return 1;
}

sub createHTMLAnchor {
   "<a target=\"_blank\" href=\"$_[0]\">$_[0]</a>";
}
