#!/usr/bin/ruby

require 'test/unit'
require 'storage'


$my_logger_called = false


class MyLogger < Storage::Logger

  def write(level, component, filename, line, function, content)

    puts "my-logger " + content

    $my_logger_called = true

  end

end


class TestLogger < Test::Unit::TestCase

  def test_logger

    @my_logger = MyLogger.new
    Storage::logger = @my_logger

    GC.start

    # Just test that our logger object is called and not already garbage
    # collected.

    environment = Storage::Environment.new(true, Storage::ProbeMode_NONE, Storage::TargetMode_DIRECT)
    storage = Storage::Storage.new(environment)

    assert($my_logger_called)

  end

end
