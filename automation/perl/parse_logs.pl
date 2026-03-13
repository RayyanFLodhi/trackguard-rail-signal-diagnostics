use strict;
use warnings;

my $file = "../../logs/exports/simulator_events.csv";

open(my $fh, "<", $file) or die "Could not open file '$file': $!";

my $header = <$fh>;  # skip header

my $total = 0;
my $healthy = 0;
my $faulted = 0;
my %fault_counts;
my @faulted_devices;

while (my $line = <$fh>) {
    chomp $line;
    next if $line =~ /^\s*$/;

    my ($device_id, $type, $state, $fault) = split /,/, $line;

    $total++;

    if ($fault eq "NONE") {
        $healthy++;
    } else {
        $faulted++;
        $fault_counts{$fault}++;
        push @faulted_devices, "$device_id ($type) -> $fault";
    }
}

close($fh);

print "TrackGuard Device Summary\n";
print "-------------------------\n";
print "Total devices: $total\n";
print "Healthy devices: $healthy\n";
print "Faulted devices: $faulted\n\n";

print "Fault breakdown:\n";
foreach my $fault (sort keys %fault_counts) {
    print "$fault: $fault_counts{$fault}\n";
}

print "\nFaulted devices:\n";
if (@faulted_devices) {
    foreach my $entry (@faulted_devices) {
        print "$entry\n";
    }
} else {
    print "None\n";
}