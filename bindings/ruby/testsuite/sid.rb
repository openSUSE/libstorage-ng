#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestSid < Test::Unit::TestCase

  def test_sid

    environment = Storage::Environment.new(true, Storage::ProbeMode_NONE, Storage::TargetMode_DIRECT)
    storage = Storage::Storage.new(environment)

    devicegraph = Storage::Devicegraph.new(storage)
    sda = Storage::Disk::create(devicegraph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::PtType_GPT)

    assert_equal(sda.sid, 42)
    assert_equal(gpt.sid, 43)

    assert_equal(devicegraph.find_device(42).sid, 42)

    assert_raises(Storage::DeviceNotFoundBySid) { devicegraph.find_device(99) }

    assert_equal(devicegraph.find_holder(42, 43).source_sid, 42)
    assert_equal(devicegraph.find_holder(42, 43).target_sid, 43)

    assert_raises(Storage::HolderNotFoundBySids) { devicegraph.find_holder(99, 99) }

  end

end
