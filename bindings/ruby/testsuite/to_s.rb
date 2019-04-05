#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestToS < Test::Unit::TestCase

  def test_to_s

    environment = Storage::Environment.new(true, Storage::ProbeMode_NONE, Storage::TargetMode_DIRECT)
    storage = Storage::Storage.new(environment)

    devicegraph = Storage::Devicegraph.new(storage)
    sda = Storage::Disk.create(devicegraph, "/dev/sda")
    gpt = sda.create_partition_table(Storage::PtType_GPT)

    assert_equal(sda.to_s[0..31], "Disk sid:42 displayname:/dev/sda")
    assert_equal(sda.displayname, "/dev/sda")

    assert_equal(gpt.to_s[0..25], "Gpt sid:43 displayname:gpt")
    assert_equal(gpt.displayname, "gpt")

  end

end
