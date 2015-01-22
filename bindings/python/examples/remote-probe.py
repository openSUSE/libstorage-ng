#!/usr/bin/python

from sys import argv, exit
from getopt import getopt, GetoptError
from subprocess import Popen, PIPE
from storage import Environment, Storage, PROBE_NORMAL, PROBE_NORMAL_WRITE_MOCKUP, TARGET_NORMAL
from storage import RemoteCallbacks, RemoteCommand, RemoteFile, set_remote_callbacks


host = "localhost"
save_mockup = False
save_devicegraph = False


def run_command(name):

    cmd = "ssh -l root %s %s" % (host, name)

    p = Popen(cmd, shell = True, stdout = PIPE, stderr = PIPE, close_fds = True)
    stdout, stderr = p.communicate()

    ret = RemoteCommand()

    for line in stdout.rstrip().split('\n'):
        ret.stdout.push_back(line)

    for line in stderr.rstrip().split('\n'):
        ret.stderr.push_back(line)

    ret.exitcode = p.returncode

    return ret


class MyRemoteCallbacks(RemoteCallbacks):

    def __init__(self):
        super(MyRemoteCallbacks, self).__init__()

    def get_command(self, name):
        print "command '%s'" % name
        command = run_command(name)
        return command

    def get_file(self, name):
        print "file '%s'" % name
        command = run_command("cat '%s'" % name)
        file = RemoteFile(command.stdout)
        return file


def doit():
    my_remote_callbacks = MyRemoteCallbacks()

    set_remote_callbacks(my_remote_callbacks)

    environment = Environment(True, PROBE_NORMAL_WRITE_MOCKUP if save_mockup else PROBE_NORMAL, TARGET_NORMAL)
    if save_mockup:
        environment.set_mockup_filename("remote-probe-mockup.xml");

    storage = Storage(environment)

    print

    probed = storage.get_probed()

    print probed

    if save_devicegraph:
        probed.save("remote-probe-devicegraph.xml");


def usage():
    print "usage: remote-probe.py [--host=name] [--save-mockup] [--save-devicegraph]"
    exit(1)

try:
    opts, args = getopt(argv[1:], "", ["host=", "save-mockup", "save-devicegraph"])

except GetoptError:
    usage()

if len(args) > 0:
    usage()

for o, a in opts:
    if o == "--host":
        host = a
    if o == "--save-mockup":
        save_mockup = True
    if o == "--save-devicegraph":
        save_devicegraph = True

doit()
