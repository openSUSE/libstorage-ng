#!/usr/bin/python3

import sys, os
import glob
import xml.dom.minidom as minidom
import networkx


# all global classes

classes = networkx.DiGraph()


# all global functions

functions = []


class Function:

    def __init__(self, name, args_string, exception_names):
        self.name = name
        self.args_string = args_string
        self.exception_names = exception_names


def extract_exception_names(node):

    exception_names = []

    for node1 in node.getElementsByTagName("parametername"):

        child1 = node1.childNodes[0]
        if child1.nodeType == child1.TEXT_NODE:
            exception_names.append("storage::" + child1.data)

        for node2 in node1.getElementsByTagName("ref"):
            child2 = node2.childNodes[0]
            exception_names.append("storage::" + child2.data)

    return exception_names


def process_functions(compound_name, node):

    functions = []

    for node1 in node.getElementsByTagName("memberdef"):

        for node2 in node1.getElementsByTagName("name"):
            name = node2.childNodes[0].data

        for node2 in node1.getElementsByTagName("argsstring"):
            args_string = node2.childNodes[0].data
            if args_string.endswith("ST_DEPRECATED"):
                args_string = args_string[:-14]

        exception_names = []

        for node2 in node1.getElementsByTagName("detaileddescription"):

            for node3 in node1.getElementsByTagName("parameterlist"):
                kind = node3.getAttribute("kind")
                if kind == "exception":
                    exception_names = extract_exception_names(node3)

        functions.append(Function(compound_name + "::" + name, args_string, exception_names))

    return functions


def load_single_file(filename):

    global functions

    dom = minidom.parse(filename)

    for node1 in dom.getElementsByTagName("doxygen"):

        for node2 in node1.getElementsByTagName("compounddef"):

            compound_kind = node2.getAttribute("kind")

            if compound_kind == "class":

                for node3 in node1.getElementsByTagName("compoundname"):
                    compound_name = node3.childNodes[0].data
                    classes.add_node(compound_name)

                for node3 in node1.getElementsByTagName("derivedcompoundref"):
                    derived_compound_name = node3.childNodes[0].data
                    classes.add_edge(compound_name, derived_compound_name)

                for node3 in node1.getElementsByTagName("sectiondef"):
                    kind = node3.getAttribute("kind")
                    if kind in ["public-func", "public-static-func", "func"]:
                        tmp = process_functions(compound_name, node3)
                        if not 'functions' in classes.nodes[compound_name]:
                            classes.nodes[compound_name]['functions'] = tmp
                        else:
                            classes.nodes[compound_name]['functions'] += tmp

            if compound_kind == "namespace":

                for node3 in node1.getElementsByTagName("compoundname"):
                    compound_name = node3.childNodes[0].data

                for node3 in node1.getElementsByTagName("sectiondef"):
                    kind = node3.getAttribute("kind")
                    if kind in ["public-func", "public-static-func", "func"]:
                        tmp = process_functions(compound_name, node3)
                        functions += tmp



for filename in glob.glob("../doc/autodocs/xml/class*.xml"):
    load_single_file(filename)

for filename in glob.glob("../doc/autodocs/xml/namespace*.xml"):
    load_single_file(filename)

if not classes or not functions:
    print("no classes or no functions loaded", file=sys.stderr)
    print("please run make in doc directory", file=sys.stderr)
    sys.exit(1)



def check_exceptions_of_function(function):

    seen_exception_names = []

    for exception_name in function.exception_names:

        if not classes.has_node(exception_name):
            print("unknown exception for %s%s" % (function.name, function.args_string))
            sys.exit(1)

        for seen_exception_name in seen_exception_names:
            if exception_name in networkx.descendants(classes, seen_exception_name):
                print("wrong order of exceptions for %s%s" % (function.name, function.args_string))
                sys.exit(1)

        seen_exception_names.append(exception_name)



for function in functions:
    check_exceptions_of_function(function)

for classname in classes:
    for function in classes.nodes[classname].get('functions', []):
        check_exceptions_of_function(function)


if __name__ == '__main__':
    networkx.drawing.nx_agraph.write_dot(classes, "classes.gv")
    os.system("/usr/bin/dot -Tsvg < classes.gv > classes.svg")
    os.system("/usr/bin/display classes.svg")
