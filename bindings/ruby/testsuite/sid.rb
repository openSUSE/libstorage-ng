#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestSid < Test::Unit::TestCase

  def test_sid

    device_graph = Storage::DeviceGraph.new()
    sda = Storage::Disk::create(device_graph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::GPT)

    assert_equal(sda.get_sid(), 42)
    assert_equal(gpt.get_sid(), 43)

    assert_equal(device_graph.find_device(42).get_sid(), 42)

    assert_raises(Storage::DeviceNotFound) { device_graph.find_device(99) }

    assert_equal(device_graph.find_holder(42, 43).get_source_sid(), 42)
    assert_equal(device_graph.find_holder(42, 43).get_target_sid(), 43)

    assert_raises(Storage::HolderNotFound) { device_graph.find_holder(99, 99) }

  end

end
