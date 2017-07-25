#!/usr/bin/python

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
        for node2 in node1.getElementsByTagName("ref"):
            exception_names.append("storage::" + node2.childNodes[0].data)

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
                        if not 'functions' in classes.node[compound_name]:
                            classes.node[compound_name]['functions'] = tmp
                        else:
                            classes.node[compound_name]['functions'] += tmp

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
