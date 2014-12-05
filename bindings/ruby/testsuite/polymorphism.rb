#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestPolymorphism < Test::Unit::TestCase

  def test_polymorphism

    device_graph = Storage::DeviceGraph.new()
    sda = Storage::Disk.create(device_graph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::GPT)

    assert_equal(sda.get_sid(), 42)
    assert_equal(gpt.get_sid(), 43)

    tmp = device_graph.find_device(42)
    assert(Storage::to_disk(tmp))
    assert_nil(Storage::to_partition_table(tmp))

    tmp = device_graph.find_device(43)
    assert(Storage::to_partition_table(tmp))
    assert_nil(Storage::to_disk(tmp))

  end

end
