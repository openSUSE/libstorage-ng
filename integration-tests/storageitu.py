
# storage integration test utils

# TODO can this be made available as storage.itu?

from storage import *
from os import system


class MyCheckCallbacks(CheckCallbacks):

    def __init__(self):
        super(MyCheckCallbacks, self).__init__()

    def error(self, message):
        print("error '%s'" % message)


def check(storage):

    my_check_callbacks = MyCheckCallbacks()

    storage.check(my_check_callbacks)


def red(s):
    return "\033[0;31m" + s + "\033[00m"

def green(s):
    return "\033[0;32m" + s + "\033[00m"


def colorize_action(message, action):
    if is_delete(action):
        return red(message)
    return message


def colorize_compound_action(message, compound_action):
    if compound_action.is_delete():
        return red(message)
    return message


class MyCommitCallbacks(CommitCallbacksV2):

    def __init__(self):
        super(MyCommitCallbacks, self).__init__()

    def begin_action(self, action):
        self.action = action

    def end_action(self, action):
        self.action = None

    def message(self, message):
        print("message '%s'" % colorize_action(message, self.action))

    def error(self, message, what):
        print("error '%s' '%s'" % (message, what))
        return False


def commit(storage, skip_save_graphs = True, skip_print_actiongraph = True,
           skip_print_used_features = True, skip_commit = False):

    if not skip_save_graphs:
        probed = storage.get_probed()
        probed.save("probed.xml")
        probed.write_graphviz("probed.gv", get_debug_devicegraph_style_callbacks(), View_ALL)
        system("/usr/bin/dot -Tsvg < probed.gv > probed.svg")

        staging = storage.get_staging()
        staging.save("staging.xml")
        staging.write_graphviz("staging.gv", get_debug_devicegraph_style_callbacks(), View_ALL)
        system("/usr/bin/dot -Tsvg < staging.gv > staging.svg")

    commit_options = CommitOptions(False)
    my_commit_callbacks = MyCommitCallbacks()

    actiongraph = storage.calculate_actiongraph()

    if not skip_print_used_features:

        probed = storage.get_probed()
        print("used features (required) of probed devicegraph:",
              get_used_features_names(probed.used_features(UsedFeaturesDependencyType_REQUIRED)))
        print("used features (suggested) of probed devicegraph:",
              get_used_features_names(probed.used_features(UsedFeaturesDependencyType_SUGGESTED)))

        staging = storage.get_staging()
        print("used features (required) of staging devicegraph:",
              get_used_features_names(staging.used_features(UsedFeaturesDependencyType_REQUIRED)))
        print("used features (suggested) of staging devicegraph:",
              get_used_features_names(staging.used_features(UsedFeaturesDependencyType_SUGGESTED)))

        print("used features of actiongraph:",
              get_used_features_names(actiongraph.used_features()))

        print()

    if not skip_print_actiongraph:
        actiongraph.print_graph()

    print("Detailed:")
    for action in actiongraph.get_commit_actions():
        print(colorize_action(get_string(actiongraph, action), action))
    print()

    print("Condensed:")
    for compound_action in actiongraph.get_compound_actions():
        print(colorize_compound_action(compound_action.sentence(), compound_action))
    print()

    if not skip_save_graphs:
        actiongraph.write_graphviz("action.gv", get_debug_actiongraph_style_callbacks())
        system("/usr/bin/dot -Tsvg < action.gv > action.svg")

    if not skip_commit:
        try:
            storage.commit(commit_options, my_commit_callbacks)
        except Exception as exception:
            print(exception.what())
