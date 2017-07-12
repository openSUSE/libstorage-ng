#!/usr/bin/ruby

require 'test/unit'
require 'storage'


class TestFunctionCall < Test::Unit::TestCase

  def test_function_call

    environment = Storage::Environment.new(true, Storage::ProbeMode_NONE, Storage::TargetMode_DIRECT)
    storage = Storage::Storage.new(environment)

    assert_equal(storage.default_mount_by(), Storage::MountByType_UUID)
    assert_raises(ArgumentError) { storage.default_mount_by("extra parameter") }
    assert_raises(NoMethodError) { storage.does_not_exist() }

    assert_equal(storage.public_send(:default_mount_by), Storage::MountByType_UUID)
    assert_raises(ArgumentError) { storage.public_send(:default_mount_by, "extra parameter") }
    assert_raises(NoMethodError) { storage.public_send(:does_not_exist) }

  end

end
