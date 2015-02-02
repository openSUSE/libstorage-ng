#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestTypes < Test::Unit::TestCase

  def test_types

    devicegraph = Storage::Devicegraph.new()

    sda = Storage::Disk.create(devicegraph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::GPT)
    sda1 = gpt.create_partition("/dev/sda", Storage::PRIMARY)
    ext4 = sda1.create_filesystem(Storage::EXT4)

    assert_equal(devicegraph.empty?, false)
    assert_equal(devicegraph.num_devices, 4)

    sda.size_k = 2**64 - 1
    assert_equal(sda.size_k, 2**64 - 1)

    sda1.region = Storage::Region.new(1, 2)
    assert_equal(sda1.region.start, 1)
    assert_equal(sda1.region.length, 2)
    assert_equal(sda1.region, Storage::Region.new(1, 2))

    ext4.label = "test-label"
    assert_equal(ext4.label, "test-label")

    # TODO
    ext4.userdata = Storage::MapStringString.new()

  end

end
