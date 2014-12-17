#!/usr/bin/ruby

require 'storage_legacy'


Storage_legacy.numSuffixes().times do |i|
    print Storage_legacy.getSuffix(i, false), " "
end
puts

print Storage_legacy.byteToHumanString(123456, false, 2, false), "\n"
