#!/usr/bin/ruby

require 'test/unit'
require 'tmpdir'
require 'storage'


# Destructors of wrapped objects can be run by the garbage collector (since SWIG 4.5 even
# while sweeping) and they can log, e.g. the destructor of Storage logs if the temporary
# directory cannot be removed. The logger is implemented in Ruby and Ruby code must not be run
# during the garbage collection.

class MyLogger < Storage::Logger

  attr_reader :lines

  def initialize
    super
    @lines = 0
  end

  def write(level, component, filename, line, function, content)
    @lines += 1
  end

end


class TestLoggerGc < Test::Unit::TestCase

  def new_storage
    environment = Storage::Environment.new(true, Storage::ProbeMode_NONE, Storage::TargetMode_DIRECT)
    Storage::Storage.new(environment)
  end

  def test_logging_in_destructor_during_gc

    Dir.mktmpdir do |tmpdir|

      old_tmpdir = ENV['TMPDIR']
      ENV['TMPDIR'] = tmpdir

      begin

        logger = MyLogger.new
        Storage::logger = logger

        # The storage objects are kept in a list so that the garbage collector cannot destroy any of
        # them before all temporary directories are removed.
        storages = Array.new(20) { new_storage }

        # The temporary directories of the storage objects are removed so that their destructors
        # log an error.
        Dir.children(tmpdir).each { |name| Dir.rmdir(File.join(tmpdir, name)) }

        storages.clear

        # The storage objects are destroyed by the garbage collector.
        GC.start

        # Logging still works after the garbage collection.

        assert_not_nil(Storage::logger)

        lines = logger.lines
        storage = new_storage
        assert(logger.lines > lines)

      ensure

        Storage::logger = nil
        ENV['TMPDIR'] = old_tmpdir

      end

    end

  end

end
