#!/usr/bin/ruby

require 'test/unit'
require 'storage'

class TestSid < Test::Unit::TestCase

  def test_sid

    device_graph = Storage::DeviceGraph.new()

    sda = Storage::Disk::create(device_graph, "/dev/sda")

    gpt = sda.createPartitionTable(Storage::GPT)

    assert_equal(sda.getSid(), 42)
    assert_equal(gpt.getSid(), 43)

    assert_equal(device_graph.findDevice(42).getSid(), 42)

    assert_raises(Storage::DeviceNotFound) { device_graph.findDevice(99) }

    assert_equal(device_graph.findHolder(42, 43).getSourceSid(), 42)
    assert_equal(device_graph.findHolder(42, 43).getTargetSid(), 43)

    assert_raises(Storage::HolderNotFound) { device_graph.findHolder(99, 99) }

  end

end
