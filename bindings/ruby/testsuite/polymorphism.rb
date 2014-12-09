#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestPolymorphism < Test::Unit::TestCase

  def test_polymorphism

    devicegraph = Storage::Devicegraph.new()
    sda = Storage::Disk.create(devicegraph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::GPT)

    assert_equal(sda.sid, 42)
    assert_equal(gpt.sid, 43)

    tmp1 = devicegraph.find_device(42)
    assert(Storage::to_disk(tmp1))
    assert_nil(Storage::to_partition_table(tmp1))

    tmp2 = devicegraph.find_device(43)
    assert(Storage::to_partition_table(tmp2))
    assert_nil(Storage::to_disk(tmp2))

  end

end
