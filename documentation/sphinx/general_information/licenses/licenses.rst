.. include:: ../../macros.rst
.. include:: <isonum.txt>

.. _GENERAL_INFORMATION_LICENSES:

========
Licenses
========

.. highlight:: none


|foxbms| Software License
~~~~~~~~~~~~~~~~~~~~~~~~~

|copy| 2010-2020, Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V. All rights reserved.

The |foxbms| embedded software and computer software are licensed under the BSD 3-Clause License.

-------------------------------------------------------------------------------

.. raw:: html

    <h2 style="font-family:monospace;"> BSD 3-Clause License </h2>

    <p style="font-family:monospace;"> Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: </p>

    <ol style="font-family:monospace;">
        <li> Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. </li>
        <li> Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. </li>
        <li> Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. </li>
    </ol>


    <p style="font-family:monospace;"> THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. </p>


-------------------------------------------------------------------------------


.. important::
    We kindly request you to use one or more of the following phrases to refer to |foxbms| in your hardware, software, documentation or advertising materials:

    .. parsed-literal::

        This product uses parts of |foxbms|\ |reg|
        This product includes parts of |foxbms|\ |reg|
        This product is derived from |foxbms|\ |reg|


|foxbms| Hardware and Documentation License
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

|copy| 2010-2020, Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V. All rights reserved.



The |foxbms| hardware and documentation are licensed under the Creative Commons Attribution 4.0 International (CC BY 4.0) License. To view a copy of this license, visit `CC BY 4.0 <https://creativecommons.org/licenses/by/4.0/>`_.



.. important::
    We kindly request you to use one or more of the following phrases to refer to |foxbms| in your hardware, software, documentation or advertising materials:

    .. parsed-literal::

        This product uses parts of |foxbms|\ |reg|
        This product includes parts of |foxbms|\ |reg|
        This product is derived from |foxbms|\ |reg|


|cc1|

.. |cc1| image:: cc_large.png
   :width: 20 %


Third Party Licenses
~~~~~~~~~~~~~~~~~~~~

This is a summary of the third party tools used by foxBMS and their licenses.

.. note::
    This is not legal advice.

.. csv-table:: Third Party Licenses
   :file: ./third-party-licenses.csv
   :header-rows: 1
   :delim: ;

.. [1]  depends on the installed version.

.. [2]  GPL, GPL2, LGPL2, LGPL2.1, LGPL3, Expat License, GCC RUNTIME LIBRARY EXCEPTION

.. [3]  see package (<foxconda-installdir>\\Library\\share\\doc\\gcc-arm-none-eabi\\license.txt)

.. [4]  ``matplotlib`` is not distributed with the |foxconda3| installer.
        However the
        `GUI <https://github.com/foxBMS/foxbms/tree/master/tools/gui>`_
        requires ``matplotlib``. As the package needs to be installed
        manually by the user, no version information can be given here.

.. [5]  This package is not distributed by the |foxbms| team, and manually
        installed by the user. Therefore the |foxbms| team does cannot provide
        version information.

.. [6]  This package is not distributed by the |foxbms| team, and manually
        installed by the user. Therefore the |foxbms| team does cannot provide
        version information.

.. [7]  see `HAL source files <https://github.com/foxBMS/foxbms/tree/master/embedded-software/mcu-hal/CMSIS>`_

.. [8]  see `waf <https://github.com/foxBMS/foxbms/blob/master/tools/waf>`_ and
        `waftools <https://github.com/foxBMS/foxbms/tree/master/tools/waftools>`_
