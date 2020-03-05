#!/usr/bin/env python3

from pathlib import Path

from jinja2 import Environment, FileSystemLoader
from jinja2.utils import Markup

try:
    # You need to add:
    #
    # from .configuration import Configuration
    # from .__main__ import process_file
    #
    # to your [~/.local/lib/python3.7/site-packages/]cmake_format/__init__.py
    from cmake_format import process_file as cf_process_file, Configuration as CF_Configuration
    HAVE_CMAKE_FORMAT = True
except:
    HAVE_CMAKE_FORMAT = False


class CMaker:
    def __init__(self, format_config=None):
        self.root = Path(__file__).parent.resolve()
        self.template_dir = self.root / 'templates'
        self.env = Environment(keep_trailing_newline=True,
                               autoescape=False,
                               loader=FileSystemLoader([str(self.template_dir)]))
        self.template = self.env.get_template('CMakeLists.template.cmake')
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
        assert(len(data['xml']) in (0, 1))

        custom_template_name = d.name + '.cmake'
        if (self.template_dir / custom_template_name).exists():
            print(d, "has a custom template")
            template = self.env.get_template(custom_template_name)
        else:
            template = self.template
        output = template.render(data)
        if self.format_config:
            output = cf_process_file(self.format_config, output)
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
            self.root / 'sampleplugins'
        )
        for plugins_dir in plugins_dirs:
            if not plugins_dir.exists():
                continue
            for d in plugins_dir.iterdir():
                if d.is_dir() and d.name != "fgt":
                    self.handle_dir(d)


if __name__ == "__main__":
    config = None
    if HAVE_CMAKE_FORMAT:
        #config = get_default_cmake_format_config(enable_markup=False)
        config = CF_Configuration(enable_markup=False, tab_size=4)

    app = CMaker(config)
    app.run()
