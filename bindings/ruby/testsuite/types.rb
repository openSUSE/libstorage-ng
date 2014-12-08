#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestTypes < Test::Unit::TestCase

  def test_types

    device_graph = Storage::DeviceGraph.new()
    sda = Storage::Disk.create(device_graph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::GPT)
    sda1 = gpt.create_partition("/dev/sda")
    ext4 = sda1.create_filesystem(Storage::EXT4)

    assert_equal(device_graph.is_empty(), false)
    assert_equal(device_graph.num_devices(), 4)

    sda.set_size_k(2**64 - 1)
    assert_equal(sda.get_size_k(), 2**64 - 1)

    ext4.set_label("test-label")
    assert_equal(ext4.get_label(), "test-label")

  end

end
