#!/usr/bin/python3

# requirements: something to probe


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(True)

storage = Storage(environment)

storage.probe()

probed = storage.get_probed()


class MyDevicegraphStyleCallbacks(DevicegraphStyleCallbacks):

    def graph(self):
        attrs = MapStringString()
        attrs["bgcolor"] = "gray50:gray70"
        attrs["gradientangle"] = "90"
        return attrs

    def nodes(self):
        attrs = MapStringString()
        attrs["shape"] = "rectangle"
        attrs["style"] = "filled"
        attrs["gradientangle"] = "90"
        return attrs

    def node(self, device):
        attrs = MapStringString()
        attrs["label"] = self.escape(device.get_displayname())
        if is_disk(device):
            attrs["color"] = "coral4"
            attrs["fillcolor"] = "coral3:coral"
            if to_disk(device).is_rotational():
                attrs["image"] = "hdd.svg"
            else:
                attrs["image"] = "ssd.svg"
        elif is_partition(device):
            attrs["color"] = "deepskyblue4"
            attrs["fillcolor"] = "deepskyblue3:deepskyblue"
            attrs["style"] = "filled,dashed"
        elif is_partition_table(device):
            attrs["color"] = "darkolivegreen4"
            attrs["fillcolor"] = "darkolivegreen3:darkolivegreen1"
            attrs["shape"] = "ellipse"
        elif is_filesystem(device):
            attrs["color"] = "aquamarine4"
            attrs["fillcolor"] = "aquamarine3:aquamarine1"
            attrs["style"] = "filled,diagonals"
            if is_nfs(device):
                attrs["image"] = "nfs.svg"
        elif is_mount_point(device):
            attrs["color"] = "gold4"
            attrs["fillcolor"] = "gold3:gold1"
            attrs["style"] = "filled,rounded"
        return attrs

    def edges(self):
        attrs = MapStringString()
        attrs["color"] = "gray20"
        return attrs

    def edge(self, holder):
        attrs = MapStringString()
        if is_subdevice(holder):
            attrs["style"] = "solid"
            attrs["arrowhead"] = "empty"
        else:
            attrs["style"] = "dashed"
            attrs["arrowhead"] = "vee"
        return attrs


my_style_callbacks = MyDevicegraphStyleCallbacks()

probed.write_graphviz("devicegraph.gv", my_style_callbacks)

system("/usr/bin/dot -Tsvg < devicegraph.gv > devicegraph.svg")
system("/usr/bin/display devicegraph.svg")
