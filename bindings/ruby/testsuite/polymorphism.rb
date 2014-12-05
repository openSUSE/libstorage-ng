#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestPolymorphism < Test::Unit::TestCase

  def test_polymorphism

    device_graph = Storage::DeviceGraph.new()
    sda = Storage::Disk::create(device_graph, "/dev/sda")
    gpt = sda.createPartitionTable(Storage::GPT)

    assert_equal(sda.getSid(), 42)
    assert_equal(gpt.getSid(), 43)

    tmp = device_graph.findDevice(42)
    assert(Storage::castToDisk(tmp))
    assert_nil(Storage::castToPartitionTable(tmp))

    tmp = device_graph.findDevice(43)
    assert(Storage::castToPartitionTable(tmp))
    assert_nil(Storage::castToDisk(tmp))

  end

end
