#!/usr/bin/ruby

require 'storage'
require 'open3'

$host = "localhost"

def run_command(name)
  cmd = "ssh -l root #{$host} #{name}"
  stdin, stdout, stderr, wait_thr = Open3.popen3(cmd)

  ret = Storage::RemoteCommand.new

  stdout.read.each_line { |line| ret.stdout << line.rstrip }
  stderr.read.each_line { |line| ret.stderr << line.rstrip }

  ret.exit_code = wait_thr.value.to_i

  return ret
end

class MyRemoteCallbacks < Storage::RemoteCallbacks
  def command(name)
    puts "command #{name}"
    command = run_command(name)
  end

  def file(name)
    puts "file #{name}"
    command = run_command("cat '#{name}'")
    file = Storage::RemoteFile.new(command.stdout)
  end
end

def doit
  my_remote_callbacks = MyRemoteCallbacks.new
  Storage.remote_callbacks = my_remote_callbacks
  environment = Storage::Environment.new(true)
  storage = Storage::Storage.new(environment)
  probed = storage.probed()
  puts probed
end

doit
