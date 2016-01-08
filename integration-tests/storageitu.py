#!/usr/bin/python

# storage integration test utils

# TODO can this be made available as storage.itu?

from storage import *

class MyCommitCallbacks(CommitCallbacks):

    def __init__(self):
        super(MyCommitCallbacks, self).__init__()

    def message(self, message):
        print "message '%s'" % message

    def error(self, message, what):
        print "error '%s' '%s'" % (message, what)
        return False


def commit(storage):

    my_commit_callbacks = MyCommitCallbacks()

    storage.calculate_actiongraph()
    storage.commit(my_commit_callbacks)

