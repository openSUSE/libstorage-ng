#!/usr/bin/ruby

require 'storage'

environment = Storage::Environment.new(true)

storage = Storage::Storage.new(environment)
storage.probe()

probed = storage.probed()

# class to define own style
class MyDevicegraphStyleCallbacks < Storage::DevicegraphStyleCallbacks
  def graph()
    attrs = Storage::MapStringString.new()
    attrs['bgcolor'] = 'gray90'
    return attrs
  end

  def nodes
    attrs = Storage::MapStringString.new()
    attrs['shape'] = 'rectangle'
    attrs['style'] = 'filled'
    return attrs
  end

  def node(device)
    attrs = Storage::MapStringString.new()
    attrs['label'] = device.displayname
    attrs['fillcolor'] = Storage.disk?(device) ? 'red' : 'yellow'
    attrs['style'] = 'filled,rounded' if Storage.mount_point?(device)
    return attrs
  end

  def edges()
    attrs = Storage::MapStringString.new()
    return attrs
  end

  def edge(holder)
    attrs = Storage::MapStringString.new()
    attrs['style'] = Storage.subdevice?(holder) ? 'solid' : 'dashed'
    return attrs
  end
end

my_style_callbacks = MyDevicegraphStyleCallbacks.new()

probed.write_graphviz('devicegraph.gv', my_style_callbacks)

system('/usr/bin/dot -Tsvg < devicegraph.gv > devicegraph.svg')
system('/usr/bin/display devicegraph.svg')
