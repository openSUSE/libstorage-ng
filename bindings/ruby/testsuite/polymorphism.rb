#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestPolymorphism < Test::Unit::TestCase

  def test_polymorphism

    environment = Storage::Environment.new(true, Storage::ProbeMode_NONE, Storage::TargetMode_DIRECT)
    storage = Storage::Storage.new(environment)

    devicegraph = Storage::Devicegraph.new(storage)

    sda = Storage::Disk.create(devicegraph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::PtType_GPT)

    assert_equal(sda.sid, 42)
    assert_equal(gpt.sid, 43)

    tmp1 = devicegraph.find_device(42)
    assert(Storage::disk?(tmp1))
    assert(Storage::to_disk(tmp1))
    assert(!Storage::partition_table?(tmp1))
    assert_raises(Storage::DeviceHasWrongType) { Storage::to_partition_table(tmp1) }

    tmp2 = devicegraph.find_device(43)
    assert(Storage::partition_table?(tmp2))
    assert(Storage::to_partition_table(tmp2))
    assert(!Storage::disk?(tmp2))
    assert_raises(Storage::DeviceHasWrongType) { Storage::to_disk(tmp2) }

  end

end
