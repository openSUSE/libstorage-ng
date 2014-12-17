#!/usr/bin/ruby

require 'storage'


Storage.numSuffixes().times do |i|
    print Storage.getSuffix(i, false), " "
end
puts

print Storage.byteToHumanString(123456, false, 2, false), "\n"
