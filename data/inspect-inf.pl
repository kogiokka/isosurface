use 5.022;
use strict;
use warnings;

use File::Find;

my @dir = (".");

find(\&wanted, @dir);

sub wanted {
  if ($_ !~ m/.*.inf$/) {
    return;
  }

  open my $fh, '<', $_;
  say "========== Reading <$_> ==========";
  while (my $line = <$fh>) {
    chomp $line;
    say $line;
  }
  say "========== <$_> END ==========\n";
}


