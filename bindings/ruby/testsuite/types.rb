#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestTypes < Test::Unit::TestCase

  def test_types

    environment = Storage::Environment.new(true, Storage::ProbeMode_NONE, Storage::TargetMode_DIRECT)
    storage = Storage::Storage.new(environment)

    devicegraph = Storage::Devicegraph.new(storage)

    sda = Storage::Disk.create(devicegraph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::PtType_GPT)
    sda1 = gpt.create_partition("/dev/sda", Storage::Region.new(0, 100, 262144), Storage::PRIMARY)
    ext4 = sda1.create_filesystem(Storage::EXT4)

    assert_equal(devicegraph.empty?, false)
    assert_equal(devicegraph.num_devices, 4)

    sda.size_k = 2**64 - 1
    assert_equal(sda.size_k, 2**64 - 1)

    sda1.region = Storage::Region.new(1, 2, 262144)
    assert_equal(sda1.region.start, 1)
    assert_equal(sda1.region.length, 2)
    assert_equal(sda1.region.block_size, 262144)
    assert_equal(sda1.region, Storage::Region.new(1, 2, 262144))

    ext4.label = "test-label"
    assert_equal(ext4.label, "test-label")

    # TODO
    ext4.userdata = Storage::MapStringString.new()

  end

end
