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
    sda1 = gpt.create_partition("/dev/sda1", Storage::Region.new(0, 100, 512), Storage::PartitionType_PRIMARY)
    ext4 = sda1.create_blk_filesystem(Storage::FsType_EXT4)

    assert_equal(devicegraph.empty?, false)
    assert_equal(devicegraph.num_devices, 4)

    sda.size = 2**64 - 512
    assert_equal(sda.size, 2**64 - 512)

    sda1.region = Storage::Region.new(1, 2, 512)
    assert_equal(sda1.region.start, 1)
    assert_equal(sda1.region.length, 2)
    assert_equal(sda1.region.block_size, 512)
    assert_equal(sda1.region, Storage::Region.new(1, 2, 512))

    ext4.label = "test-label"
    assert_equal(ext4.label, "test-label")

    # TODO
    ext4.userdata = Storage::MapStringString.new()

    assert_equal(devicegraph.used_features(Storage::UsedFeaturesDependencyType_SUGGESTED) & Storage::UF_EXT4, Storage::UF_EXT4)
    assert_equal(devicegraph.used_features(Storage::UsedFeaturesDependencyType_SUGGESTED) & Storage::UF_BTRFS, 0)

    assert_not_equal(Storage::LIBSTORAGE_NG_VERSION_STRING, "")

  end

end
