APPNAME = 'timezero-shell'
VERSION = '0.1.0'

top = '.'
out = 'build'


def options(opt):
	opt.load('compiler_c')
	opt.add_option('--debug', action='store_true', default=False, help='Enable debug mode')
	opt.add_option('--enable-cli-colors', action='store_true', default=False, help='Use colors for logs')


def need(ctx, name, version=None, lib_store='', require=True):
	if version is None:
		ctx.check_cfg(
			package         = name,
			args            = '--cflags --libs',
			uselib_store    = lib_store,
			mandatory       = require,
		)
	else:
		ctx.check_cfg(
			package         = name,
			args            = '--cflags --libs',
			atleast_version = version,
			uselib_store    = lib_store,
			mandatory       = require,
		)


def configure(conf):
	conf.load('compiler_c')
	#conf.load('gnu_dirs intltool')

	conf.define('GETTEXT_PACKAGE', APPNAME)
	conf.define('VERSION', VERSION)
	if conf.options.enable_cli_colors:
		conf.define('ENABLE_CLI_COLORS', 1)

	need(conf, 'gtk+-2.0', '2.16.0', 'GTK')
	need(conf, 'webkit-1.0', None, 'WEBKIT')
	# need(conf, 'libnotify', None, 'NOTIFY', False)
	need(conf, 'json', None, 'JSON')

	conf.env.append_unique('CFLAGS', '-std=gnu99')

	if conf.options.debug:
		conf.define('DEBUG', 1)
		conf.env.prepend_value('CFLAGS', [
			'-O0',
			'-g',
			'-Wall',
			'-Wextra',
			'-fstack-protector',
			'-Wno-unused-parameter'
		])
	else:
		conf.env.prepend_value('CFLAGS', [
			'-O3',
			'-Wno-unused-parameter',
			'-Wno-unused-function'
		])

	conf.write_config_header('config.h')


def build(bld):
	bld.program(
		target = APPNAME,
		source = bld.path.ant_glob('src/**/*.c'),
		features = 'c',
		uselib = 'GTK JSON WEBKIT',
		lib = ''
	)

# vim: set noet fenc= ff=unix sts=0 sw=4 ts=4 ft=python :
