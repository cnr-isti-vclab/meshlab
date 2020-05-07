#!/usr/bin/env python3
# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

# Be sure to install cmake-format first to get best results:
# pip3 install cmake-format

from pathlib import Path

from jinja2 import Environment, FileSystemLoader
from jinja2.utils import Markup

try:
    from cmake_format.__main__ import process_file as cf_process_file
    from cmake_format.configuration import Configuration as CF_Configuration
    HAVE_CMAKE_FORMAT = True
except:
    print("cmake-format not found... generated files might look gross. Do pip3 install cmake-format")
    HAVE_CMAKE_FORMAT = False


class CMaker:
    def __init__(self, format_config=None):
        self.root = Path(__file__).parent.resolve()
        self.template_dir = self.root / 'templates'
        self.env = Environment(keep_trailing_newline=True,
                               autoescape=False,
                               loader=FileSystemLoader([str(self.template_dir)]))
        self.default_template_name = 'CMakeLists.template.cmake'
        self.template = self.env.get_template(self.default_template_name)
        self.format_config = format_config

    def handle_dir(self, d, recurse=True):
        # print(d)
        if recurse:
            glob = d.rglob
        else:
            glob = d.glob
        data = {
            "name": d.stem,
        }
        sources = list(sorted(x.relative_to(d) for x in glob("*.cpp")
                              if not x.name.startswith('moc_') and not x.name.startswith('qrc_')))
        if sources:
            data['sources'] = sources

        headers = list(sorted(x.relative_to(d) for x in glob("*.h")
                              if not x.name.startswith('ui_') and not x.name.startswith('moc_')))
        if headers:
            data["headers"] = headers

        resources = list(sorted(x.relative_to(d) for x in glob("*.qrc")))
        if resources:
            data["resources"] = resources

        ui = list(sorted(x.relative_to(d) for x in glob("*.ui")))
        if ui:
            data["ui"] = ui

        data["xml"] = list(sorted(x.relative_to(d) for x in glob("*.xml")))
        if len(data['xml']) not in (0, 1):
            print(data["xml"])
            raise RuntimeError("Expected 0 or 1 xml files for plugin %s, got %d" % (
                data["name"], len(data['xml'])))

        custom_template_name = d.name + '.cmake'
        if (self.template_dir / custom_template_name).exists():
            print(d, "has a custom template")
            template = self.env.get_template(custom_template_name)
            data["template"] = "{} (custom for this directory)".format(
                custom_template_name)
        else:
            template = self.template
            data["template"] = "{} (shared with all other directories)".format(
                self.default_template_name)
            data["assumed_custom_template_name"] = custom_template_name
        output = template.render(data)
        if self.format_config:
            output = cf_process_file(self.format_config, output)
            # Handle both original 0.6.0 and newer - tested with 0.6.10dev3
            if isinstance(output, tuple):
                output = output[0]
        with open(d / 'CMakeLists.txt', 'w', encoding='utf-8') as fp:
            fp.write(output)

    def run(self):
        self.handle_dir(self.root / 'common', False)
        self.handle_dir(self.root / 'meshlab')
        self.handle_dir(self.root / 'meshlabserver')
        plugins_dirs = (
            self.root / 'meshlabplugins',
            self.root / 'plugins_experimental',
            self.root / 'plugins_unsupported',
            # self.root / 'sampleplugins'
        )
        for plugins_dir in plugins_dirs:
            if not plugins_dir.exists():
                continue
            for d in plugins_dir.iterdir():
                if d.is_dir() and d.name not in ("fgt", "external", "shaders"):
                    self.handle_dir(d)


if __name__ == "__main__":
    config = None
    if HAVE_CMAKE_FORMAT:
        #config = get_default_cmake_format_config(enable_markup=False)
        config = CF_Configuration(enable_markup=False, tab_size=4)

    app = CMaker(config)
    app.run()
