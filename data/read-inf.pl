use 5.022;
use strict;
use warnings;

my $inf_file = $ARGV[0];
die "Need a file as argument." unless defined $inf_file;

my %attrs = ReadInf($inf_file);
my @dimen = (split /:/, $attrs{Resolution});
say "x: $dimen[0], y: $dimen[1], z: $dimen[2]";

sub ReadInf {
  my $file = shift;
  my %attr;
  open my $fh, '<', $file;

  while(my $line = <$fh>) {
    chomp $line;
    my ($k, $v) = split /=/, $line;
    die "Wrong format." unless defined $k && defined $v;
    $attr{$k} = $v;
  }
  return %attr;
}
