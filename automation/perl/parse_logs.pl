use strict;
use warnings;

my $file = "logs/exports/control_center_export.csv";

open(my $fh, "<", $file) or die "Could not open file '$file': $!";

my $header = <$fh>;

my $total = 0;
my $faulted = 0;

print "TrackGuard Device Export Summary\n";
print "--------------------------------\n";

while (my $line = <$fh>) {
    chomp $line;
    my ($device_id, $type, $state, $fault) = split /,/, $line;

    $total++;

    if ($fault ne "NONE") {
        $faulted++;
        print "Fault detected: $device_id ($type) -> $fault\n";
    }
}

close($fh);

print "--------------------------------\n";
print "Total devices: $total\n";
print "Devices with faults: $faulted\n";
print "Devices healthy: " . ($total - $faulted) . "\n";