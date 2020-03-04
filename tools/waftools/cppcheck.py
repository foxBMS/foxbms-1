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
from waflib import Build, Context, Options, Utils, Errors, Logs


def options(opt):
    opt.add_option('--cppcheck-error-exitcode', type='int', default=-1,
                   action='store', dest='CPPCHECK_ERROR_EXITCODE',
                   help='cppcheck configuration file (default: number of errors n)')


def configure(conf):
    print('cppcheck:')
    if Utils.unversioned_sys_platform()=='win32':
        # default installtion path on Windows
        path_list = [os.path.join('C:', os.sep, 'Program Files', 'Cppcheck')]
    else:
        path_list = []
    conf.find_program('cppcheck', var='CPPCHECK', mandatory=False,
                      path_list=path_list)
    if conf.env.CPPCHECK:
        try:
            (std_out, std_err) = conf.cmd_and_log([conf.env.CPPCHECK[0], '--version'], output=Context.BOTH)
        except Errors.WafError as e:
            conf.fatal('Could not determine Cppcheck version')
        else:
            cppcheck_version = std_out.strip()
            cppcheck_version = cppcheck_version.split(' ')[1]
            cppcheck_version_major, cppcheck_version_minor = cppcheck_version.split('.')
            min_ver = (1, 87)
        if (int(cppcheck_version_major), int(cppcheck_version_minor)) < min_ver:
            Logs.warn(f'Install Cppcheck version {".".join([str(i) for i in min_ver])} or greater (Installed version is {cppcheck_version}).')
            Logs.warn(f'Skipping Cppcheck configuration')
            conf.env.CPPCHECK = None

    if conf.options.CPPCHECK_ERROR_EXITCODE == -1:
        conf.env.CPPCHECK_ERROR_EXITCODE = ''
    else:
        conf.env.CPPCHECK_ERROR_EXITCODE = conf.options.CPPCHECK_ERROR_EXITCODE

from waflib.Build import BuildContext
class cppcheck(BuildContext):
    __doc__ = '''configures cppcheck'''
    cmd = 'cppcheck'
    fun = 'cppcheck'
