#!/usr/bin/env python
# -*- coding: utf-8 -*-

# @copyright &copy; 2010 - 2020, Fraunhofer-Gesellschaft zur Foerderung der
#   angewandten Forschung e.V. All rights reserved.
#
# BSD 3-Clause License
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1.  Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
# 3.  Neither the name of the copyright holder nor the names of its
#     contributors may be used to endorse or promote products derived from this
#     software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# We kindly request you to use one or more of the following phrases to refer to
# foxBMS in your hardware, software, documentation or advertising materials:
#
# &Prime;This product uses parts of foxBMS&reg;&Prime;
#
# &Prime;This product includes parts of foxBMS&reg;&Prime;
#
# &Prime;This product is derived from foxBMS&reg;&Prime;

import os
import sys
import datetime
import posixpath
import re
import yaml
import jinja2
# try to use FullLoader (PyYAML5.1+ and fall back to normale Loader)
try:
    from yaml import FullLoader as YAMLLoader
except ImportError:
    from yaml import Loader as YAMLLoader

from waflib import Utils, Options, Errors, Logs
from waflib import Task, TaskGen
from waflib.Tools.compiler_c import c_compiler
from waflib.Tools.c import c
from waflib.Tools import c_preproc


# overwrite c task to use absolute paths for the input file
class c(Task.Task):  # noqa: F811
    run_str = '${CC} ${ARCH_ST:ARCH} ${CFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CC_SRC_F}${SRC[0].abspath()} ${CC_TGT_F}${TGT[0].abspath()} ${CPPFLAGS}'
    vars = ['CCDEPS']
    ext_in = ['.h']
    scan = c_preproc.scan


out = 'build'
variants = ['primary', 'secondary', 'libs', 'primary_bare', 'secondary_bare']
from waflib.Build import BuildContext, CleanContext, ListContext, StepContext  # noqa: E402
for x in variants:
    for y in (BuildContext,
              CleanContext,
              ListContext,
              StepContext):
        name = y.__name__.replace('Context', '').lower()

        class tmp_1(y):
            if name == 'build':
                __doc__ = '''executes the {} of {}'''.format(name, x)
            elif name == 'clean':
                __doc__ = '''cleans the project {}'''.format(x)
            elif name == 'list':
                __doc__ = '''lists the targets to execute for {}'''.format(x)
            elif name == 'step':
                __doc__ = '''executes tasks in a step-by-step fashion, ''' \
                          '''for debugging of {}'''.format(x)
            cmd = name + '_' + x
            variant = x

    dox = 'doxygen'

    class tmp_2(BuildContext):
        __doc__ = '''creates the {} documentation of {}'''.format(dox, x)
        cmd = dox + '_' + x
        fun = dox
        variant = x


def options(opt):
    opt.load('compiler_c')
    opt.load('python')
    opt.load(['doxygen', 'sphinx_build', 'cpplint', 'flake8', 'cppcheck'],
             tooldir=os.path.join('tools', 'waftools'))
    opt.add_option('-t', '--target', action='store', default='debug',
                   help='build target: debug (default)/release', dest='target')
    opt.add_option('-l', '--libs', action='store', default='',
                   help='name of the library to be used')
    for k in ('--targets',
              '--out',
              '--top',
              '--prefix',
              '--destdir',
              '--bindir',
              '--libdir',
              '--msvc_version',
              '--msvc_targets',
              '--no-msvc-lazy',
              '--force',
              '--check-c-compiler'):
        option = opt.parser.get_option(k)
        if option:
            opt.parser.remove_option(k)

    mctx = waflib.Context.classes
    mctx.remove(waflib.Build.InstallContext)
    mctx.remove(waflib.Build.UninstallContext)


def configure(conf):
    if ' ' in conf.path.abspath():
        conf.fatal(f'path to foxbms must not contain spaces'
                   f' (current path: {conf.path.abspath()}).')

    conf.load('python')
    conf.check_python_version((3, 6))

    # Setup the whole toolchain (compiler, interpreter etc.)
    print('Compiler toolchain:')
    pref = 'arm-none-eabi-'  # prefix for all gcc related tools
    exe_extension = ''
    if sys.platform.startswith('win'):
        conf.env.jinja2_newline = '\r\n'
        exe_extension = '.exe'
    else:
        conf.env.jinja2_newline = '\n'
    conf.env.CC = pref + 'gcc' + exe_extension
    conf.env.AR = pref + 'ar' + exe_extension
    conf.env.LINK_CC = pref + 'g++' + exe_extension
    gcc_tools = 'cpp ranlib as strip objcopy objdump size gdb'.split()
    for k in reversed(sorted(gcc_tools, key=len)):
        conf.find_program(pref + k, var=k.upper(), mandatory=True)
    for key in c_compiler:  # force only using gcc
        c_compiler[key] = ['gcc']
    conf.load('compiler_c')
    conf.load(['doxygen', 'sphinx_build', 'cpplint', 'flake8', 'cppcheck'])
    print('General tools:')
    conf.find_program('python', var='PYTHON', mandatory=True)
    conf.find_program('git', var='GIT', mandatory=False)

    # define configuration files etc.
    # parsing the version info based on the general documentation
    conf.env.sphinx_doc_dir = os.path.join('documentation', 'sphinx')
    to_posix = (conf.env.sphinx_doc_dir).split(os.sep)
    conf.env.sphinx_doc_dir_posix = posixpath.join(*(to_posix))
    conf.env.sphinx_conf_path = os.path.join(conf.env.sphinx_doc_dir,
                                             'conf.py')
    version_info_file = os.path.join(conf.env.sphinx_doc_dir, 'macros.rst')
    with open(version_info_file, 'r', encoding='UTF-8') as f:
        txt = f.read()
    rgx = r'\.\.[ ]\|version\|[ ]replace::[ ]``(\d{1,}\.\d{1,}\.\d{1,})``'
    tmp_version = re.search(rgx, txt)
    try:
        conf.env.version = tmp_version.group(1)
    except AttributeError:
        err_msg = 'Could not find a version info in {}.\
'.format(version_info_file)
        conf.fatal(err_msg)

    conf.env.appname = 'foxbms'
    conf.env.appname_prefix = conf.env.appname
    conf.env.vendor = 'Fraunhofer IISB'
    conf.env.version_primary = conf.env.version
    conf.env.version_secondary = conf.env.version

    # Setup the compiler and link flags
    with open('compiler-flags.yml', 'r') as stream:
        try:
            compiler_flags = yaml.load(stream, Loader=YAMLLoader)
        except yaml.YAMLError as exc:
            conf.fatal(exc)
    cflags = compiler_flags['CFLAGS']
    conf.env.append_value('CFLAGS', [x for x in cflags if type(x) == str])
    for x in cflags:
        if type(x) is dict:
            add_flag = 'CFLAGS_' + list(x.keys())[0]
            conf.env.append_value(add_flag, list())
            if list(x.values())[0] is None:
                continue
            conf.env.append_value(add_flag, *(x.values()))
    conf.env.ASMFLAGS = compiler_flags['ASMFLAGS']
    conf.env.LINKFLAGS = compiler_flags['LINKFLAGS']
    conf.env.XLINKER = compiler_flags['XLINKER']

    # get HAL version and floating point version based on compiler define and
    # check if cpu and floating point version  are fitting together
    cpu = None
    floating_point_version = None
    for _cflag in conf.env.CFLAGS:
        if 'mcpu' in _cflag:
            cdef, cpu = _cflag.split('=')
        if 'mfpu' in _cflag:
            cdef, floating_point_version = _cflag.split('=')
    if not cpu:
        conf.fatal('cflag \'mcpu\' missing.')
    if not floating_point_version:
        conf.fatal('cflag \'mfpu\' missing.')
    if cpu == 'cortex-m4':
        conf.env.CPU_MAJOR = 'STM32F4xx'
        if floating_point_version != 'fpv4-sp-d16':
            conf.fatal('floating point unit flag not compatible with cpu')
    else:
        conf.fatal(f'cpu \'{cpu}\' is not supported')

    utcnow = datetime.datetime.utcnow()
    utcnow = ''.join(utcnow.isoformat('-').split('.')
                     [0].replace(':', '-').split('-'))
    conf.env.timestamp = utcnow

    conf.define('BUILD_APPNAME_PREFIX', conf.env.appname_prefix)
    for x in variants:
        conf.define(
            ('BUILD_APPNAME_{}'.format(x)).upper(),
            '{}_{}'.format(conf.env.appname_prefix, x)[:14],
            comment='Define is trimmed to max. 14 characters'.format(x))
    conf.define('BUILD_VERSION_PRIMARY', conf.env.version_primary)
    conf.define('BUILD_VERSION_SECONDARY', conf.env.version_secondary)

    conf.env.target = conf.options.target

    env_debug = conf.env.derive()
    env_debug.detach()
    env_release = conf.env.derive()
    env_release.detach()

    # configuration for debug
    conf.setenv('debug', env_debug)
    conf.define('RELEASE', 1)
    conf.undefine('DEBUG')
    conf.env.CFLAGS += ['-g', '-O0']

    # configuration for release
    conf.setenv('release', env_release)
    conf.env.CFLAGS += ['-O2']

    if conf.options.target == 'release':
        conf.setenv('', env_release)
    else:
        conf.setenv('', env_debug)

    env_release.store(os.path.join(out, 'env-store.log'))

    config_dir = 'config'
    conf.path.get_bld().make_node(config_dir).mkdir()
    conf.confdir = conf.path.get_bld().find_node(config_dir)

    _cmd = [Utils.subst_vars('${CC}', conf.env), '-dM', '-E', '-']
    std_out, std_err = conf.cmd_and_log(_cmd, output=0, input='\n'.encode())
    std_out = '/* WARNING: DO NOT EDIT */\n' \
              '/* INTERNAL GCC MARCOS */\n' \
              '/* FOR INFORMATION ONLY */\n' \
              '\n' \
              '{}'.format(std_out)
    conf.confdir.make_node('gcc_builtin_macros.h').write(std_out)

    header_file_name = conf.env.appname_prefix + 'config.h'
    header_file_path = os.path.join(config_dir, header_file_name),
    def_guard = header_file_name.upper().replace('.H', '_H_')
    conf.write_config_header(header_file_path, guard=def_guard)
    print('---')
    print('Vendor:              {}'.format(conf.env.vendor))
    print('Appname prefix:      {}'.format(conf.env.appname_prefix))
    print('Applications:        {}'.format(', '.join(variants)))
    print('Version primary:     {}'.format(conf.env.version_primary))
    print('Version secondary:   {}'.format(conf.env.version_secondary))
    print('---')
    print('Config header:       {}'.format(conf.env.cfg_files))
    print('Build configuration: {}'.format(conf.env.target))
    print('---')
    conf.path.get_bld().make_node('cflags.log').write('\n'.join(conf.env.CFLAGS) + '\n')
    conf.path.get_bld().make_node('cflags-foxbms.log').write('\n'.join(conf.env.CFLAGS_foxbms) + '\n')
    conf.path.get_bld().make_node('cflags-freertos.log').write('\n'.join(conf.env.CFLAGS_freertos) + '\n')
    conf.path.get_bld().make_node('hal.log').write('\n'.join(conf.env.CFLAGS_hal) + '\n')
    conf.path.get_bld().make_node('asmflags.log').write('\n'.join(conf.env.ASMFLAGS) + '\n')
    conf.path.get_bld().make_node('linkflags.log').write('\n'.join(conf.env.LINKFLAGS) + '\n')
    conf.path.get_bld().make_node('xlinker.log').write('\n'.join(conf.env.XLINKER) + '\n')

    lib_dir = conf.path.get_bld().make_node('lib')
    lib_dir.mkdir()
    conf.env.append_value('LIBPATH', lib_dir.abspath())
    conf.env.LIB_DIR_LIBS = lib_dir.abspath()
    print(f'Additional Library directory:   {lib_dir.abspath()}')

    inc_dir = conf.path.get_bld().make_node('include')
    inc_dir.mkdir()
    conf.env.append_value('INCLUDES', inc_dir.abspath())
    conf.env.INCLUDE_DIR_LIBS = inc_dir.abspath()
    print(f'Additional Include directory:   {inc_dir.abspath()}')

    if conf.options.libs:
        conf.env.USER_DEFINED_LIBS = conf.options.libs
        print(f'Using library:                  {conf.options.libs}')
    else:
        conf.env.USER_DEFINED_LIBS = None

    # calculate expected binary size from flasheader credentials
    conf.env.flash_begin_adr = 0x080FFF48 & 0x00ffffff
    conf.env.flash_header_adr = 0x080FFF00 & 0x00ffffff
    conf.env.flash_end_adr = 0x080FFF4C & 0x00ffffff

    # cppcheck configuration
    if conf.env.CPPCHECK:
        cppcheck_dir = conf.path.get_bld().make_node('cppcheck')
        cppcheck_dir.mkdir()
        templateLoader = jinja2.FileSystemLoader(searchpath=os.path.join('tools', 'cppcheck'))
        templateEnv = jinja2.Environment(loader=templateLoader, newline_sequence=conf.env.jinja2_newline)
        template = templateEnv.get_template('cppcheck.template')
        outputText = template.render(
            bld_dir='.',
            src_dir='../../embedded-software',
            inc_dirs=['../../embedded-software/libs',
                      '../../embedded-software/mcu-common',
                      '../../embedded-software/mcu-primary',
                      '../../embedded-software/mcu-secondary',
                      '../../build/primary/embedded-software/mcu-primary/src/general',
                      '../../build/secondary/embedded-software/mcu-secondary/src/general'],
            addons=['threadsafety', 'y2038', 'cert', 'misra'])

        cppcheck_cfg = cppcheck_dir.make_node('cppcheck.cppcheck')
        cppcheck_cfg.write(outputText)
        conf.env.cppcheck_dir = cppcheck_dir.abspath()
        conf.env.cppcheck_cfg = cppcheck_cfg.abspath()
        print(f'---\ncppcheck configuration:         {conf.env.cppcheck_cfg}')

    print('---\ngit information:')
    try:
        (std_out, std_err) = conf.cmd_and_log([conf.env.GIT[0], 'config', '--get', 'remote.origin.url'], output=waflib.Context.BOTH)
    except Errors.WafError as e:
        Logs.warn('--> directory is not a git repository')
        t = ('NOREMOTE', [True])
    else:
        t = (std_out.strip(), [False])

    conf.env.append_value('GIT_REPO_PATH', t[0])
    conf.env.append_value('GIT_DIRTY_ALWAYS', t[1])
    print(f'repository path:                {conf.env.GIT_REPO_PATH[0]}')
    if conf.env.GIT_DIRTY_ALWAYS[0]:
        print(f'no remote:                      {conf.env.GIT_DIRTY_ALWAYS[0]}')
    conf.env.FILE_TEMPLATE_C = conf.path.find_node('tools/styleguide/file-templates/template.c.jinja2').read()
    conf.env.FILE_TEMPLATE_H = conf.path.find_node('tools/styleguide/file-templates/template.h.jinja2').read()


def build(bld):

    import sys
    import logging
    from waflib import Logs
    if not bld.variant:
        bld.fatal(f'A {bld.cmd} variant must be specified, run \'{sys.executable} {sys.argv[0]} --help\'')

    log_file = os.path.join(out, 'build_' + bld.variant + '.log')
    bld.logger = Logs.make_logger(log_file, out)
    hdlr = logging.StreamHandler(sys.stdout)
    formatter = logging.Formatter('%(message)s')
    hdlr.setFormatter(formatter)
    bld.logger.addHandler(hdlr)

    if bld.variant in ('primary', 'secondary'):
        bld.add_pre_fun(repostate)

    bld.env.es_dir = os.path.normpath('embedded-software')
    if bld.variant == 'libs':
        src_dir = os.path.normpath('{}'.format(bld.variant))
        if bld.cmd.startswith('clean'):
            for x in bld.path.ant_glob(f'{out}/lib/**/*.a {out}/include/**/*.h'):
                x.delete()
    else:
        # for bare build we *basically* have the same build, therefore the
        # source folder stays the same, but *_bare builds do not include
        # FreeRTOS
        if bld.variant.endswith('_bare'):
            src_dir = os.path.normpath(
                'mcu-{}'.format(bld.variant.replace('_bare', '')))
            bld.env.FreeRTOS_dirs = ''  # no FreeRTOS in bare build
        else:
            src_dir = os.path.normpath('mcu-{}'.format(bld.variant))
            bld.env.FreeRTOS_dirs = ' '.join([
                os.path.join(bld.top_dir, bld.env.es_dir, 'mcu-freertos', 'Source'),
                os.path.join(bld.top_dir, bld.env.es_dir, 'mcu-freertos', 'Source', 'include'),
                os.path.join(bld.top_dir, bld.env.es_dir, 'mcu-freertos', 'Source', 'portable', 'GCC', 'ARM_CM4F')])

        bld.env.mcu_dir = src_dir

        bld.env.common_dir = os.path.normpath('mcu-common')
        bld.env.hal_dirs = ' '.join([
            os.path.join(bld.top_dir, bld.env.es_dir, 'mcu-hal', 'CMSIS', 'Device', 'ST', bld.env.CPU_MAJOR, 'Include'),
            os.path.join(bld.top_dir, bld.env.es_dir, 'mcu-hal', 'CMSIS', 'Include'),
            os.path.join(bld.top_dir, bld.env.es_dir, 'mcu-hal', bld.env.CPU_MAJOR + '_HAL_Driver', 'Inc')])
        t = os.path.dirname(bld.env.cfg_files[0])
        bld.env.append_value('INCLUDES', t)
    bld.recurse(os.path.join(bld.env.es_dir, src_dir))


def repostate(bld):
    Logs.info('Adding git information...')
    file_name = 'gitinfo_cfg'
    gitinfo_dir = os.path.join(bld.env.es_dir, bld.env.mcu_dir, 'src', 'general')
    bld.path.get_bld().make_node(gitinfo_dir).mkdir()
    bld.env.git_infoc = bld.path.get_bld().make_node(os.path.join(gitinfo_dir, f'{file_name}.c'))
    bld.env.git_infoh = bld.path.get_bld().make_node(os.path.join(gitinfo_dir, f'{file_name}.h'))
    templatec = jinja2.Environment(loader=jinja2.BaseLoader, keep_trailing_newline=True, newline_sequence=bld.env.jinja2_newline).from_string(bld.env.FILE_TEMPLATE_C)
    templateh = jinja2.Environment(loader=jinja2.BaseLoader, keep_trailing_newline=True, newline_sequence=bld.env.jinja2_newline).from_string(bld.env.FILE_TEMPLATE_H)
    _date = datetime.datetime.today().strftime('%d.%m.%Y')

    if bld.env.GIT_REPO_PATH[0] == "NOREMOTE":
        bld.env.GIT_COMMIT_ID = 'NOVALIDCOMMIT'
        bld.env.CLEAN, bld.env.DIRTY = (0, 1)
        bld.env.GIT_STATUS = 'GIT_DIRTY_STARTUP'
    else:
        try:
            (std_out, std_err) = bld.cmd_and_log([bld.env.GIT[0], 'rev-parse', 'HEAD'], output=waflib.Context.BOTH)
        except Errors.WafError as e:
            Logs.error(e)
            bld.env.GIT_COMMIT_ID = 'NOVALIDCOMMIT'
        else:
            bld.env.GIT_COMMIT_ID = std_out.strip()

        try:
            (std_out, std_err) = bld.cmd_and_log([bld.env.GIT[0], 'status'], output=waflib.Context.BOTH)
        except Errors.WafError as e:
            std_out = None
            Logs.error(e)
        else:
            if "nothing to commit, working tree clean" in std_out:
                bld.env.CLEAN, bld.env.DIRTY = (1, 0)
                bld.env.GIT_STATUS = 'GIT_CLEAN_STARTUP'
            else:
                bld.env.CLEAN, bld.env.DIRTY = (0, 1)
                bld.env.GIT_STATUS = 'GIT_DIRTY_STARTUP'
    print(f'clean: {bld.env.CLEAN} \ndirty: {bld.env.DIRTY}')

    # header file
    brief = 'Contains information about the repository state'
    macros = []
    defs = ['''\
typedef enum {
    GIT_CLEAN_STARTUP = 0,
    GIT_DIRTY_STARTUP = 1,
} GIT_STARTUP_BIT_e;''',
            f'''\
typedef struct {{
    STD_RETURN_TYPE_e clean_repo_build;
    STD_RETURN_TYPE_e allow_startup;
}} GIT_STATUS_s;''',
            f'''\
typedef struct {{
    char repo_url[{len(bld.env.GIT_REPO_PATH[0])+1}];
    char commit_id[{len(bld.env.GIT_COMMIT_ID)+1}];
    uint8_t always_dirty; /* if there is no remote, this is always set */
    GIT_STARTUP_BIT_e git_startup_bit; /* set dependent on git status */
}} GIT_ValidStruct_s;''']

    externvars = [
        'extern GIT_STATUS_s git_status;',
        'extern const GIT_ValidStruct_s git_validation;']
    externfunsproto = ['extern STD_RETURN_TYPE_e GIT_checkStartup(void);']
    txt_git_info_h = templateh.render(
        filename=os.path.splitext(file_name)[0],
        add_author_info='(autogenerated)',
        filecreation=_date,
        ingroup='GIT_INFO',
        prefix='GIT',
        brief=brief,
        details='',
        includes=['general.h'],
        macros=macros,
        defs=defs,
        externvars=externvars,
        externfunsproto=externfunsproto)
    Logs.info(f'Created {bld.env.git_infoh.relpath()}')
    bld.env.git_infoh.write(txt_git_info_h)

    # implementation file
    startupfun = ['''\
STD_RETURN_TYPE_e GIT_checkStartup() {
    STD_RETURN_TYPE_e retval = E_NOT_OK;
    if (git_validation.git_startup_bit == GIT_CLEAN_STARTUP &&
        git_validation.always_dirty == 0) {
        retval = E_OK;
    }
    return retval;
}''']
    externvars = [
        '''GIT_STATUS_s git_status = {0xFF, 0xFF};''',
        f'''\
extern const GIT_ValidStruct_s git_validation = {{
    "{bld.env.GIT_REPO_PATH[0]}", /* remote repository path */
    "{bld.env.GIT_COMMIT_ID}", /* last commit hash that could be retrivied */
    {int(bld.env.GIT_DIRTY_ALWAYS[0])}, /* repository has a valid remote */
    {bld.env.GIT_STATUS}, /* is the repository dirty? */
}};
''']
    txt_git_info_c = templatec.render(
        filename=os.path.splitext(file_name)[0],
        add_author_info='(autogenerated)',
        inc_files=[],
        filecreation=_date,
        ingroup='GIT_INFO',
        prefix='GIT',
        brief='Contains information about the repository state',
        externvars=externvars,
        externfunsimpl=startupfun,
        details='')
    bld.env.git_infoc.write(txt_git_info_c)
    Logs.info(f'Created {bld.env.git_infoc.relpath()}')
    Logs.info('done...')


def doxygen(bld):
    import sys
    import logging
    from waflib import Logs

    if not bld.variant:
        bld.fatal(f'A {bld.cmd} variant must be specified, run \'{sys.executable} {sys.argv[0]} --help\'')

    if not bld.env.DOXYGEN:
        bld.fatal(f'Doxygen was not configured. Run \'{sys.executable} {sys.argv[0]} --help\'')

    _docbuilddir = os.path.normpath(bld.bldnode.abspath())
    doxygen_conf_dir = os.path.join('documentation', 'doxygen')
    os.makedirs(_docbuilddir, exist_ok=True)
    conf_file = 'doxygen-{}.conf'.format(bld.variant)
    doxygenconf = os.path.join(doxygen_conf_dir, conf_file)

    log_file = os.path.join(
        bld.bldnode.abspath(), 'doxygen_' + bld.variant + '.log')
    bld.logger = Logs.make_logger(log_file, out)
    hdlr = logging.StreamHandler(sys.stdout)
    formatter = logging.Formatter('%(message)s')
    hdlr.setFormatter(formatter)
    bld.logger.addHandler(hdlr)

    bld(features='doxygen', doxyfile=doxygenconf)


def flake8(bld):
    bld(features='flake8')


def cppcheck(bld):

    class tsk_cppcheck(Task.Task):

        def scan(self):
            nodes = bld.path.ant_glob('**/*.c **/*.h')
            return (nodes, [])

        def run(self):
            prg = Utils.subst_vars('${CPPCHECK}', bld.env)
            cfg = Utils.subst_vars('${cppcheck_cfg}', bld.env)
            cmd = [prg, f'--project={cfg}']
            if bld.env.CPPCHECK_ERROR_EXITCODE:
                opt = [f'--error-exitcode={bld.env.CPPCHECK_ERROR_EXITCODE}']
                cmd.extend(opt)
            std_out, std_err = self.generator.bld.cmd_and_log(cmd, output=waflib.Context.BOTH)
            if std_err:
                Logs.error(std_err)
            if not std_err or bld.env.CPPCHECK_ERROR_EXITCODE == 0:
                self.outputs[0].write(std_out)

    @TaskGen.feature('run_cppcheck')
    def add_cppcheck(self):
        self.create_task('tsk_cppcheck', tgt=self.path.find_or_declare('cppcheck').make_node('cppcheck.out'))

    bld(features='run_cppcheck')


def cpplint(bld):
    from waflib import Logs

    class tsk_cpplint(Task.Task):

        def keyword(self):
            return 'Linting'

        def scan(self):
            node = self.inputs[0]
            return ([node], [])

        def run(self):
            cmd = [Utils.subst_vars('${CPPLINT}', bld.env)] + self.generator.env.cpplint_options + [self.inputs[0].abspath()]
            Logs.debug(' '.join(cmd))
            proc = Utils.subprocess.Popen(cmd, stdout=Utils.subprocess.PIPE, stderr=Utils.subprocess.PIPE, shell=True)
            std_out, std_err = proc.communicate()
            std_out, std_err = std_out.decode(), std_err.decode()
            if proc.returncode:
                Logs.error(std_err)
                self.outputs[0].change_ext('.error').write(std_err)
            else:
                self.outputs[0].change_ext('.error').delete()
                self.outputs[0].write(std_out)

    def set_out_dir(bld, src_node, out_dir='cpplint', ext='.cpplint'):
        my_bld_node = bld.bldnode.make_node(out_dir)
        rp = src_node.get_bld().relpath()
        bld_target = my_bld_node.make_node(rp + ext)

        return bld_target

    @TaskGen.feature('run_cpplint')
    def add_cpplint(self):
        srcs = bld.path.ant_glob(self.env.cpplint_src, excl=self.env.cpplint_excl)
        for src_file in srcs:
            tgt = set_out_dir(bld, src_file)
            self.create_task('tsk_cpplint', src=src_file, tgt=tgt)

    with open(bld.env.CPPLINT_CONF, 'r') as stream:
        try:
            cpplint_conf = yaml.load(stream, Loader=YAMLLoader)
        except yaml.YAMLError as exc:
            bld.fatal(exc)
    src = cpplint_conf['files']['include']
    filters = cpplint_conf['filter']
    excl_tmp = cpplint_conf['files']['exclude']
    excl = []
    for x in excl_tmp:
        if not x.startswith('**/'):
            excl.append('**/' + x)
        else:
            excl.append(x)

    bld.env.cpplint_options = ['--output={}'.format(cpplint_conf['output'])]
    bld.env.cpplint_options += ['--linelength={}'.format(cpplint_conf['linelength'])]
    bld.env.cpplint_options += ['--filter=' + ','.join(filters)]
    bld.env.cpplint_src = src
    bld.env.cpplint_excl = excl
    bld(features='run_cpplint')


def sphinx(bld):
    import sys
    import logging
    from waflib import Logs
    if not bld.env.SPHINX_BUILD:
        bld.fatal('ERROR: cannot build documentation (\'sphinx-build\' is not'
                  'found in PATH)')
    log_file = os.path.join(bld.bldnode.abspath(), 'sphinx.log')
    bld.logger = Logs.make_logger(log_file, out)
    hdlr = logging.StreamHandler(sys.stdout)
    formatter = logging.Formatter('%(message)s')
    hdlr.setFormatter(formatter)
    bld.logger.addHandler(hdlr)
    bld(features='sphinx',
        config=bld.path.find_node(bld.env.sphinx_conf_path),
        outdir='documentation',
        version=bld.env.version,
        )


def clean_all(bld):
    """cleans all parts of the project"""
    from waflib import Options
    commands_after = Options.commands
    Options.commands = ['clean_primary', 'clean_secondary',
                        'clean_primary_bare', 'clean_secondary_bare',
                        'clean_libs']
    Options.commands += commands_after


def build_all(bld):
    """builds all parts of the project (binaries and documentation)"""
    from waflib import Options
    commands_after = Options.commands
    Options.commands = []
    if bld.options.libs:
        Options.commands = ['build_libs']
    Options.commands += ['build_primary', 'build_secondary',
                         'build_primary_bare', 'build_secondary_bare',
                         'doxygen_primary', 'doxygen_secondary',
                         'doxygen_primary_bare', 'doxygen_secondary_bare',
                         'sphinx']
    Options.commands += commands_after


def dist(conf):
    conf.base_name = 'foxbms'
    conf.algo = 'tar.gz'
    conf.excl = out
    conf.excl += ' .ws **/tools/waf*.*.**-* .lock-*'
    conf.excl += ' **/.git **/.gitignore **/.gitattributes '
    conf.excl += ' **/*.tar.bz2 **/*.tar.gz **/*.pyc '


def distcheck_cmd(self):
    import shlex
    cfg = []
    if Options.options.distcheck_args:
        cfg = shlex.split(Options.options.distcheck_args)
    else:
        cfg = [x for x in sys.argv if x.startswith('-')]
    cmd = [sys.executable, sys.argv[0], 'configure', 'build_primary', 'build_secondary', 'doxygen_primary', 'doxygen_secondary', 'sphinx'] + cfg
    return cmd


def check_cmd(self):
    import tarfile
    with tarfile.open(self.get_arch_name())as t:
        for x in t:
            t.extract(x)
    cmd = self.make_distcheck_cmd()
    ret = Utils.subprocess.Popen(cmd, cwd=self.get_base_name()).wait()
    if ret:
        raise Errors.WafError('distcheck failed with code % r' % ret)


def distcheck(conf):
    """creates tar.bz form the source directory and tries to run a build"""
    from waflib import Scripting
    Scripting.DistCheck.make_distcheck_cmd = distcheck_cmd
    Scripting.DistCheck.check = check_cmd
    conf.base_name = 'foxbms'
    conf.excl = out
    conf.excl += ' .ws **/tools/waf*.*.**-* .lock-*'
    conf.excl += ' **/.git **/.gitignore **/.gitattributes '
    conf.excl += ' **/*.tar.bz2 **/*.tar.gz **/*.pyc '


class tsk_cal_chksum(Task.Task):
    def keyword(self):
        return 'Calculating checksum'
    after = ['tsk_binflashheadergen', 'tsk_binflashgen']
    color = 'RED'

    def run(self):
        import binascii
        import struct

        with open(self.inputs[1].abspath(), 'rb') as pheader_buffer_file:
            pheader_buffer_file.seek(self.generator.env.flash_begin_adr ^ self.generator.env.flash_header_adr)
            checksum_start_address = struct.unpack('i', pheader_buffer_file.read(4))[0]
            pheader_buffer_file.seek(self.generator.env.flash_end_adr ^ self.generator.env.flash_header_adr)
            checksum_end_address = struct.unpack('i', pheader_buffer_file.read(4))[0]
            chksum_calc_length = checksum_end_address - checksum_start_address + 1

        prg_txt = self.inputs[0].read('rb')
        if not chksum_calc_length == len(prg_txt):
            self.generator.fatal('Checksum calculation error')

        checksum = binascii.crc32(prg_txt, 0) & 0xFFFFFFFF
        output = f'checksum: 0x{checksum:X}'
        self.outputs[0].write(output + '\n')
        print(output)


class tsk_wrt_chksum(Task.Task):
    def keyword(self):
        return 'Writing checksum'
    after = ['tsk_cal_chksum']
    color = 'RED'

    def run(self):
        import shutil
        import struct

        checksum_struct_name = 'ver_sw_validation'
        checksum_position_in_struct = 0x10
        date_position_in_struct = 0xA0
        time_position_in_struct = 0xAC

        checksum_data = yaml.load(self.inputs[1].read(), Loader=yaml.Loader)
        checksum = hex(checksum_data['checksum'])

        if checksum.endswith('L'):
            checksum = checksum[:-1]
        cmd = Utils.subst_vars('${OBJDUMP} --section=.flashheader -h', self.env)
        cmd += f' {self.inputs[0].abspath()}'
        std_out, std_err = self.generator.bld.cmd_and_log(cmd, output=waflib.Context.BOTH)
        sectionAttributes = dict(zip(std_out.splitlines()[4].split(), std_out.splitlines()[5].split()))

        cmd = Utils.subst_vars('${OBJDUMP} --section=.flashheader -t', self.env)
        cmd += f' {self.inputs[0].abspath()}'
        std_out, std_err = self.generator.bld.cmd_and_log(cmd, output=waflib.Context.BOTH)
        std_out = [line for line in std_out.splitlines() if checksum_struct_name in line]
        symbolAttributes = std_out[0].split()

        positionInSection = int(symbolAttributes[0], 16) - int(sectionAttributes['LMA'], 16)

        shutil.copy(self.inputs[0].abspath(), self.outputs[0].abspath())

        # calculate offset of checksum in ELF file
        offset = int(sectionAttributes['File'], 16) + positionInSection + checksum_position_in_struct
        # create single bytes from checksum string
        bytes = struct.pack('I', int(checksum, 16))
        # write checksum bytes to calculated offset in ELF file
        with open(self.outputs[0].abspath(), 'r+b') as fh:
            fh.seek(offset)
            fh.write(bytes)

        # calculate offset of date in ELF file
        offset = int(sectionAttributes['File'], 16) + positionInSection + date_position_in_struct
        # create single bytes from date string
        d = datetime.datetime.now()
        bytes = d.strftime('%b %d %Y').encode()
        # write date bytes to calculated offset in ELF file
        with open(self.outputs[0].abspath(), 'r+b') as fh:
            fh.seek(offset)
            fh.write(bytes)

        # calculate offset of time in ELF file
        offset = int(sectionAttributes['File'], 16) + positionInSection + time_position_in_struct
        # create single bytes from time string
        bytes = d.strftime('%H:%M:%S').encode()
        # write time bytes to calculated offset in ELF file
        with open(self.outputs[0].abspath(), 'r+b') as fh:
            fh.seek(offset)
            fh.write(bytes)


@TaskGen.feature('chksum')
@TaskGen.before('add_hexgen_task')
@TaskGen.after('apply_link', 'add_bingen_task')
def add_chksum_task(self):
    try:
        link_task = self.link_task
        binflashgen = self.binflashgen_task
        binflashheadergen = self.binflashheadergen_task
    except AttributeError:
        return
    self.cal_chksum_task = self.create_task('tsk_cal_chksum',
                                            src=[binflashgen.outputs[0],
                                                 binflashheadergen.outputs[0]],
                                            tgt=self.path.get_bld().make_node('checksum.yml'))
    self.wrt_chksum_task = self.create_task('tsk_wrt_chksum',
                                            src=[link_task.outputs[0], self.cal_chksum_task.outputs[0]],
                                            tgt=link_task.outputs[0].change_ext(''))


class strip(Task.Task):
    after = ['tsk_binflashheaderpatch']
    run_str = '${STRIP} ${SRC} -o ${TGT}'
    color = 'BLUE'


@TaskGen.feature('strip')
@TaskGen.after('add_chksum_task')
@TaskGen.after('add_bingen_task')
def add_strip_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.create_task('strip',
                     src=link_task.outputs[0],
                     tgt=link_task.outputs[0].change_ext('_nd.elf'))


class tsk_hexgen(Task.Task):
    def keyword(self):
        return 'Creating hex file'
    after = ['tsk_wrt_chksum']
    run_str = '${OBJCOPY} -R .ext_sdramsect_bss -R .bkp_ramsect -O ihex ${SRC} ${TGT}'
    color = 'CYAN'


@TaskGen.feature('hexgen')
@TaskGen.after('add_chksum_task')
def add_hexgen_task(self):
    try:
        wrt_chksum_task = self.wrt_chksum_task
    except AttributeError:
        return
    self.hexgen = self.create_task('tsk_hexgen',
                                   src=wrt_chksum_task.outputs[0],
                                   tgt=wrt_chksum_task.outputs[0].change_ext('.hex'))


class tsk_binflashheaderpatch(Task.Task):
    def keyword(self):
        return 'Patching bin flashheader'
    after = ['tsk_wrt_chksum']
    run_str = '${OBJCOPY} -j .flashheader -O binary ${SRC} ${TGT}'
    color = 'RED'


class tsk_binflashheadergen(Task.Task):
    def keyword(self):
        return 'Creating bin flashheader'
    run_str = '${OBJCOPY} -j .flashheader -O binary ${SRC} ${TGT}'
    color = 'RED'


class tsk_binflashgen(Task.Task):
    def keyword(self):
        return 'Creating bin flash'
    run_str = '${OBJCOPY} -R .ext_sdramsect_bss -R .bkp_ramsect -R .flashheader -O binary ${SRC} ${TGT}'
    color = 'RED'


@TaskGen.feature('bingen')
@TaskGen.before('add_chksum_task')
@TaskGen.after('apply_link')
def add_bingen_task(self):
    try:
        link_task = self.link_task
    except AttributeError:
        return
    self.binflashgen_task = self.create_task('tsk_binflashgen', src=link_task.outputs[0], tgt=link_task.outputs[0].change_ext('_flash.bin', '.elf.unpatched'))
    self.binflashheadergen_task = self.create_task('tsk_binflashheadergen', src=link_task.outputs[0], tgt=link_task.outputs[0].change_ext('_flashheader.bin.unpatched', '.elf.unpatched'))


@TaskGen.feature('binpatch')
@TaskGen.after('add_chksum_task')
@TaskGen.after('apply_link')
def add_patch_bin_task(self):
    try:
        wrt_chksum_task = self.wrt_chksum_task
    except AttributeError:
        return
    self.binflashheaderpatch_task = self.create_task('tsk_binflashheaderpatch', src=wrt_chksum_task.outputs[0], tgt=wrt_chksum_task.outputs[0].change_ext('_flashheader.bin'))


import waflib.Tools.asm  # noqa: E402 import before redefining
from waflib.TaskGen import extension  # noqa: E402


class Sasm(Task.Task):
    color = 'BLUE'
    run_str = '${CC} ${ASMFLAGS} ${CPPPATH_ST:INCPATHS} -o ${TGT} ${SRC[0].abspath()}'


@extension('.s')
def asm_hook(self, node):
    name = 'Sasm'
    out = node.change_ext('.o')
    task = self.create_task(name, node, out)
    try:
        self.compiled_tasks.append(task)
    except AttributeError:
        self.compiled_tasks = [task]
    return task


class size(Task.Task):
    def keyword(self):
        return 'Calculating size'
    before = ['tsk_cal_chksum']
    color = 'BLUE'

    def run(self):
        cmd = Utils.subst_vars('${SIZE}', self.env) + f' {self.inputs[0].abspath()}'
        x = self.outputs[0].path_from(self.generator.path)
        out, err = self.generator.bld.cmd_and_log(cmd, output=waflib.Context.BOTH, quiet=waflib.Context.STDOUT)
        self.generator.path.make_node(x).write(out)
        if err:
            Logs.error(err)


@TaskGen.feature('size')
@TaskGen.after('apply_link')
def process_sizes(self):
    if getattr(self, 'link_task', None) is None:
        return

    objects_to_size = []
    objects_to_size.extend(self.link_task.inputs)
    objects_to_size.extend(self.link_task.outputs)

    for node in objects_to_size:
        out = node.change_ext('.size.log')
        self.create_task('size', node, out)


class tsk_check_includes(Task.Task):
    before = ['size']
    color = 'PINK'

    def run(self):
        import os
        import collections
        err_msg = f'{self.inputs[0].abspath()} introduces the following errors:\n'
        err_ctn_missing = 0
        err_ctn_duplicates = 0
        incs = self.generator.bld.env.INCLUDES + [x if os.path.isabs(x) else os.path.join(self.generator.path.abspath(), x) for x in self.generator.includes]
        Logs.debug('\n'.join(incs))
        for x in incs:
            if not os.path.isdir(x):
                err_ctn_missing += 1
                if err_ctn_missing == 1:
                    err_msg += 'The following include directories do not exist:\n'
                err_msg += f'{x}\n'
        if not (sorted(incs) == sorted(list(set(incs)))):
            err_ctn_duplicates += 1
            if err_ctn_duplicates == 1:
                err_msg += 'There are duplicate includes:\n'
            duplicates = [item for item, count in collections.Counter(incs).items() if count > 1]
            for p in duplicates:
                err_msg += f'{p}\n'
        if (err_ctn_missing + err_ctn_duplicates):
            Logs.error(err_msg)
            self.generator.bld.fatal('There are include errors.')
        else:
            self.outputs[0].write(f'wscript: "{self.inputs[0]}"\n')
            if incs:
                self.outputs[0].write('includes:\n  - "', 'a')
                self.outputs[0].write('"\n  - "'.join(incs) + '"\n', 'a+')
            else:
                self.outputs[0].write('includes:\n', 'a+')


@TaskGen.feature('check_includes')
@TaskGen.before('process_rule')
def add_check_includes(self):
    src = self.path.make_node('wscript')
    tgt = src.change_ext('.includes.yml')
    self.create_task('tsk_check_includes', src=src, tgt=tgt)


class copy_libs(Task.Task):
    def keyword(self):
        return 'Copying'

    def run(self):
        import shutil
        shutil.copyfile(self.inputs[0].abspath(), self.outputs[0].abspath())


@TaskGen.feature('copy_libs')
@TaskGen.after('apply_link')
def add_copy_libs(self):
    if getattr(self, 'link_task', None) is None:
        return

    for src in self.link_task.outputs:
        tgt = os.path.normpath(src.abspath())
        tgt = os.path.join(self.env.LIB_DIR_LIBS, os.path.basename(tgt))
        tgt = self.path.find_or_declare(tgt)
        self.create_task('copy_libs', src=src, tgt=tgt)
