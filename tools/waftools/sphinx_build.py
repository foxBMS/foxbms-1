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

from waflib import Task, TaskGen, Logs, Utils


class sphinx_build(Task.Task):
    color = "BLUE"

    def keyword(self):
        return "Rendering sphinx"

    def __str__(self):
        return f"{self.inputs}"

    def run(self):
        # https://www.sphinx-doc.org/en/master/man/sphinx-build.html
        cmd = " ".join(
            [
                "${SPHINX_BUILD}",
                "-b", "html",
                "-c ${CONFDIR}",
                "-D ${VERSION}",
                "-D ${RELEASE}",
                "-D graphviz_dot=${DOT}",
                "${SRCDIR}",
                "${OUTDIR}",
            ]
        )
        cmd = Utils.subst_vars(cmd, self.env)
        Logs.info(cmd)
        proc = Utils.subprocess.Popen(
            cmd,
            stdout=Utils.subprocess.PIPE,
            stderr=Utils.subprocess.PIPE,
            cwd=self.generator.bld.path.abspath(),
        )

        std_out, std_err = proc.communicate()
        std_out = std_out.decode(errors='ignore')
        std_err = std_err.decode(errors='ignore')
        print(std_out)
        Logs.warn(std_err)
        return proc.returncode

@TaskGen.feature("sphinx")
@TaskGen.after_method("process_rule")
def tsk_sphinx(self):
    if (
        not getattr(self, "outdir", None)
        or not getattr(self, "config", None)
        or not getattr(self, "version", None)
    ):
        self.bld.fatal('"outdir", "config" and "version" are requried.')
    outdir = self.path.find_or_declare(self.outdir).get_bld()
    task = self.create_task("sphinx_build", self.config)
    task.env["SRCDIR"] = self.config.parent.abspath()
    task.env["VERSION"] = f"version={self.version}"
    task.env["RELEASE"] = f"release={self.version}"
    task.env["CONFDIR"] = self.config.parent.abspath()
    task.env["OUTDIR"] = outdir.abspath()


def configure(conf):
    print("Sphinx documentation tools:")
    conf.find_program("sphinx-build")
    conf.find_program("dot")


from waflib.Build import BuildContext
class sphinx(BuildContext):
    __doc__ = """creates the sphinx documentation of the project"""
    cmd = "sphinx"
    fun = "sphinx"
