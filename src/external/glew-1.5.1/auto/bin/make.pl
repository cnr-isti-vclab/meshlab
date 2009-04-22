##
## Copyright (C) 2002-2008, Marcelo E. Magallon <mmagallo[]debian org>
## Copyright (C) 2002-2008, Milan Ikits <milan ikits[]ieee org>
##
## This program is distributed under the terms and conditions of the GNU
## General Public License Version 2 as published by the Free Software
## Foundation or, at your option, any later version.

my %regex = (
    extname  => qr/^[A-Z][A-Za-z0-9_]+$/,
    exturl   => qr/^http.+$/,
    function => qr/^(.+) ([a-z][a-z0-9_]*) \((.+)\)$/i, 
    token    => qr/^([A-Z][A-Z0-9_x]*)\s+((?:0x)?[0-9A-F]+|[A-Z][A-Z0-9_]*)$/,
    type     => qr/^typedef\s+(.+)\s+([\*A-Za-z0-9_]+)$/,
    exact    => qr/.*;$/,
);

# prefix function name with glew
sub prefixname($)
{
    my $name = $_[0];
    $name =~ s/^(.*)gl/__$1glew/;
    return $name;
}

# prefix function name with glew
sub prefix_varname($)
{
    my $name = $_[0];
    $name =~ s/^(.*)GL(X*)EW/__$1GL$2EW/;
    return $name;
}

#---------------------------------------------------------------------------------------

sub make_exact($)
{
	my $exact = $_[0];
	$exact =~ s/(; |{)/$1\n/g;
    return $exact;
}

sub make_separator($)
{
    my $extname = $_[0];
    my $l = length $extname;
    my $s = (71 - $l)/2;
    print "/* ";
    my $j = 3;
    for (my $i = 0; $i < $s; $i++)
    {
	print "-";
	$j++;
    }
    print " $_[0] ";
    $j += $l + 2;
    while ($j < 76)
    {
	print "-";
	$j++;
    }
    print " */\n\n";
}

#---------------------------------------------------------------------------------------

sub parse_ext($)
{
    my $filename = shift;
    my %functions = ();
    my %tokens = ();
    my %types = ();
    my @exacts = ();
    my $extname = "";
    my $exturl = "";
    
    open EXT, "<$filename" or return;

    while(<EXT>)
    {
        chomp;
	if (/$regex{extname}/)
        {
            $extname = $_;
            next;
        }
	elsif (/$regex{exturl}/)
	{
	    $exturl = $_;
	}
        elsif (s/^\s+//)
        {
            if (/$regex{exact}/)
            {
				push @exacts, $_;
            }
            elsif (/$regex{type}/)
            {
                my ($value, $name) = ($1, $2);
                $types{$name} = $value;
            }
            elsif (/$regex{token}/)
            {
                my ($name, $value) = ($1, $2);
                $tokens{$name} = $value;
            }
            elsif (/$regex{function}/)
            {
                my ($return, $name, $parms) = ($1, $2, $3);
                $functions{$name} = {
		    rtype => $return,
		    parms => $parms,
		};
            }
        }
    }

    close EXT;

    return ($extname, $exturl, \%types, \%tokens, \%functions, \@exacts);
}

sub output_tokens($$)
{
    my ($tbl, $fnc) = @_;
    if (keys %{$tbl})
    {
        local $, = "\n";
        print "\n";
        print map { &{$fnc}($_, $tbl->{$_}) } sort { hex ${$tbl}{$a} <=> hex ${$tbl}{$b} } keys %{$tbl};
        print "\n";
    }
}

sub output_types($$)
{
    my ($tbl, $fnc) = @_;
    if (keys %{$tbl})
    {
        local $, = "\n";
        print "\n";
        print map { &{$fnc}($_, $tbl->{$_}) } sort { ${$tbl}{$a} cmp ${$tbl}{$b} } keys %{$tbl};
        print "\n";
    }
}

sub output_decls($$)
{
    my ($tbl, $fnc) = @_;
    if (keys %{$tbl})
    {
        local $, = "\n";
        print "\n";
        print map { &{$fnc}($_, $tbl->{$_}) } sort keys %{$tbl};
        print "\n";
    }
}

sub output_exacts($$)
{
    my ($tbl, $fnc) = @_;
    if (scalar @{$tbl})
    {
        local $, = "\n";
        print "\n";
        print map { &{$fnc}($_) } sort @{$tbl};
        print "\n";
    }
}

