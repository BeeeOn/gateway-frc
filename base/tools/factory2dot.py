#! /usr/bin/env python3

import sys
import re
import logging
import configparser
import xml.etree.ElementTree as xml

class FactoryConfigInterpolation(configparser.Interpolation):
	def __init__(self, factory):
		self.factory = factory

	def before_get(self, parser, section, option, value, defaults):
		return self.factory.interpolate(value)

"""
Collection of instance definitions and application configurations that
together create the whole runtime. It analysis which instances are used
and allows to generate a graph for the graphviz dot program.
"""
class Factory:
	"""
	Arguments:
	- sources is a list of file input streams of XML factory files
	- ini is a list of file input streams of INI configuration files
	- groups is a list of file input streams of INI group visualization settings
	- unused defines whether unused instances should be shown
	"""
	def __init__(self, sources, ini, groups, unused):
		self.root = None
		self.unused = unused
		self.groups = configparser.ConfigParser()
		self.config = configparser.ConfigParser(
				interpolation = FactoryConfigInterpolation(self))
		self.interpolation_cache = {}

		self._configDefaults()

		if ini is not None:
			logging.info("loading INI file '%s'" % ini.name)
			self.config.read_file(ini)

		if groups is not None:
			logging.info("loading groups INI file '%s'" % groups.name)
			self.groups.read_file(groups)

		for source in sources:
			logging.info("loading factory file '%s'" % source.name)

			dom = xml.parse(source)
			if self.root is None:
				self.root = dom
			else:
				self._merge(dom)

	def _configDefaults(self):
		# TODO: better value, e.g. from argparse
		self.config['application'] = {}
		self.config['application']['configDir'] = './'

	def _merge(self, dom):
		for factory in dom.getroot():
			self.root.getroot().append(factory)

	"""
	Replaces placeholders in format ${...} with a configuration entry in the given string
	and returns the interpolated result value.
	"""
	def interpolate(self, value):
		result = value

		if value in self.interpolation_cache:
			return self.interpolation_cache[value]

		logging.debug("interpolating: '%s'" % value)

		for i in range(configparser.MAX_INTERPOLATION_DEPTH):
			if result.find("${") < 0:
				self.interpolation_cache[value] = result
				return result

			changes = 0

			for item in self.config.items():
				logging.debug("section '%s'" % item[0])

				for leaf in item[1]:
					leaf_value = self.config.get(item[0], leaf, raw = True)
					logging.debug("key-value pair '%s' = '%s'" % (leaf, leaf_value))

					if item[0] != "DEFAULT":
						name = "${" + item[0] + "." + leaf + "}"
					else:
						name = "${" + leaf + "}"

					logging.debug("apply to '%s'" % result)

					replaced = result.replace(name, leaf_value)
					if replaced != result:
						logging.debug("key '%s' replaced" % name)
						changes += 1

					result = replaced

			logging.debug("applied %u changes" % changes)

			if changes == 0:
				break

		logging.warning("reached interpolation depth limit for '%s'" % (value))
		return result

	"""
	Generates graph node settings based on the group information. A given name (usually a class name)
	fits into a group when it matches the group's pattern. The first group that matches is used.
	It applies properties: color, fillcolor and fontcolor according to graphviz definitions, see
	http://www.graphviz.org/doc/info/colors.html for color names.
	"""
	def _apply_groups(self, output, name):
		logging.debug("gather group properties for '%s'" % name)

		for item in self.groups.items():
			if item[0] == "DEFAULT":
				continue

			if "pattern" not in item[1]:
				logging.warning("no 'pattern' property for group '%s'" % item[0])
				continue

			pattern = item[1]['pattern']

			if name.find(pattern) < 0:
				continue

			logging.debug("pattern '%s' matches name '%s'" % (pattern, name))

			if "color" in item[1]:
				output.write('    color="%s"\n' % item[1]["color"])
			if "fillcolor" in item[1]:
				output.write('    style="filled" fillcolor="%s"\n' % item[1]["fillcolor"])
			if "fontcolor" in item[1]:
				output.write('    fontcolor="%s"\n' % item[1]["fontcolor"])

	"""
	Extract a short class name from the full class name written in the C++ format
	(Ns1::Ns2::MyClass).
	"""
	def _class_name(self, el):
		name = el.get("class")

		delim = name.rfind("::")
		if delim >= 0:
			return name[delim + 2:]

		return name

	"""
	An instance is considered to be unused when it does not refer to any existing
	instance and there is no reference to the instance. Also, the early initialized
	instances are always considered to be used.
	"""
	def _is_unused(self, el):
		if self.unused:
			return False

		if "init" in el.attrib and el.get('init') == 'early':
			return False

		name = self.interpolate(el.get('name'))

		for child in el:
			if "ref" in child.attrib:
				return False

		for other in self.root.findall('.//*[@ref]'):
			ref = self.interpolate(other.get('ref'))

			if ref == name:
				return False

		return True

	"""
	Generation graph node definitions. Return all instances that were skipped
	as being considered as unused.
	"""
	def _definitions(self, output):
		logging.info("generating graph node definitions")

		skipped = []

		for el in self.root.findall('.//instance'):
			name = self.interpolate(el.get('name'))

			if self._is_unused(el):
				logging.warning("skipping unused '%s'" % el.get('name'))
				skipped.append(name)
				continue

			output.write('  "' + name + '" [\n')
			output.write('    label="<top> ')
			output.write(name)
			output.write('\\nclass ' + self._class_name(el))

			offset = 0
			for child in el:
				if child.tag in ('add', 'set'):
					child_name = self.interpolate(child.get('name'))

					if 'ref' in child.attrib:
						id = child_name + str(offset)
					else:
						id = child_name

					output.write(' | <' + id + '> ' + child_name)
					offset += 1

			output.write('"\n')
			output.write('    shape="record"\n')

			self._apply_groups(output, el.get('class'))

			output.write('  ];\n')

		return skipped

	"""
	References to other instances can go via aliases. This method traverses all the aliases
	on the way and returns the name of the target instance at the end of the alias chain.
	"""
	def _solve_alias(self, ref, parent, visited):
		name = self.interpolate(ref)

		logging.info("solving instance name %s (%s) for parent '%s'" % (ref, name, parent))

		if name in visited:
			raise Exception("detected alias loop for: %s (%s) %s" % (ref, name, str(visited)))

		for el in self.root.findall(".//instance[@name]"):
			target = self.interpolate(el.get('name'))
			if target != name:
				continue

			return target

		for el in self.root.findall(".//alias[@name]"):
			target = self.interpolate(el.get('name'))
			if target != name:
				continue

			visited.append(name)
			return self._solve_alias(el.get('ref'), parent, visited)

		raise Exception("no such instance found for %s (%s)" % (ref, name))

	"""
	Generate graph edges among nodes based on the relationships defined via the ref
	XML attribute.
	"""
	def _dependencies(self, output, skipped):
		logging.info("generating graph node dependencies")

		for el in self.root.findall('.//instance'):
			name = self.interpolate(el.get('name'))

			if name in skipped:
				continue

			offset = 0
			for child in el:
				if 'ref' not in child.attrib:
					offset += 1
					continue

				child_name = self.interpolate(child.get('name'))

				target = self._solve_alias(child.get("ref"), name, [])
				output.write('  "' + name + '":')
				output.write(child_name + str(offset))
				output.write(' -> ')
				output.write('"' + target + '":top;\n')
				offset += 1

	def process(self, output):
		logging.info("generating graph header")

		output.write('digraph factory {\n')
		output.write('  graph [rankdir="LR"];\n')

		skipped = self._definitions(output)
		self._dependencies(output, skipped)

		output.write('}\n')

		logging.info("generation successful")

if __name__ == '__main__':
	import argparse

	parser = argparse.ArgumentParser()
	parser.add_argument('sources', metavar='source',
			type=argparse.FileType('r'), nargs='+',
			help='source files to process at once')
	parser.add_argument('--ini', metavar='file', dest='ini',
			type=argparse.FileType('r'),
			help='ini files containing configuration')
	parser.add_argument('--groups', metavar='file', dest='groups',
			type=argparse.FileType('r'),
			help='ini files containing class groups properties')
	parser.add_argument('--unused', dest='unused', action='store_true',
			help='include unused instances (excluded as default)')
	parser.add_argument('--debug', dest='debug', action='store_true',
			help='raise logging level to DEBUG')
	parser.add_argument('--quiet', dest='quiet', action='store_true',
			help='raise logging level to CRITICAL')

	args = parser.parse_args()

	if args.debug:
		logging.basicConfig(level = logging.DEBUG)
	elif args.quiet:
		logging.basicConfig(level = logging.CRITICAL)
	else:
		logging.basicConfig(level = logging.INFO)

	factory = Factory(args.sources, args.ini, args.groups, args.unused)
	factory.process(sys.stdout)
