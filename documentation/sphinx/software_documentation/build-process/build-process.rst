.. include:: ../../macros.rst

.. _SOFTWARE_DOCUMENTATION_BUILD_PROCESS:

=============
Build Process
=============

|foxbms| uses ``waf The meta build system`` for building binaries and the
documentation.

For detailed information on |waf| see `waf.io <https://waf.io/>`_. A short
introcution to |waf| is given at
`waf.io/apidocs/tutorial <https://waf.io/apidocs/tutorial.html>`_.
The more detailed version of how to use |waf| is found at
`waf.io/book <https://waf.io/book/>`_.

General
-------

Where to find the toolchain?
++++++++++++++++++++++++++++

The |waf| toolchain is located in the directory ``foxbms\tools``, in the binary
``waf``. This archive is automatically unpacked in a directory named
something like ``waf-{{X}-{{some-hash-value}}`` containing the waf
library, where ``{{X}}`` is the dash-separated version number of ``waf``. It is
unpacked into ``foxbms\tools``. It is generally assumed that all commands are
run from directory ``foxbms``. Therefore |waf| has to be always called by
``python tools\waf some-command`` where ``some-command`` is an argument defined
in the |wscript|.

Additional build tools are located in ``foxbms\tools\waftools``. These are
the tooles needed for building the documentation, i.e., ``doxygen`` and
``sphinx``.

Where are the build steps described?
++++++++++++++++++++++++++++++++++++
The build process is described in files named |wscript|, that can be found
nearly everywhere inside the different directories inside the |foxbms| project.
Later in this documentation this is explained in detail.

General
+++++++

|waf| needs always to be run form the top level of the repository and the path
to waf must be given relative to that directory. This path is ``tools\waf``.

..  code-block::    console
    :name: wafgeneral
    :caption: How to call waf

    cd path\to\foxbms
    python tools\waf {{some-command}}


What commands can be used?
++++++++++++++++++++++++++
To get an overview of support commands run ``--help`` or ``-h`` on the |waf|
binary:

..  code-block::    console
    :name: wafhelp
    :caption: How to call help on waf

    python tools\waf --help

This is the output in |foxbms| version |version|

.. literalinclude:: help.txt
    :language: console
    :name: outputofwafhelp
    :caption: Waf help in |foxbms|

The ``configure`` command
+++++++++++++++++++++++++


Before building any binaries or documentation is possible, the project needs to
be configured. A successfull ``configure`` command and its ouput is shown below:

..  code-block::    console
    :name: configureawafproject
    :caption: Configuration of a the project

    python tools\waf configure
    (...)
    'configure' finished successfully (0.340s)

The ``build`` commands
++++++++++++++++++++++


After the project has been configured, a build can be triggered and it is
generally exectued by the ``build`` commands. As |foxbms| requries building
variants, one has to use e.g., ``build_primary`` in order to build binaries
for the primary MCU.

..  code-block::    console
    :name: buildtheproject
    :caption: Example of a wrong and a correct build command.

    python tools\waf build
    Waf: Entering directory `.\foxbms\build'
    A build variant must be specified, run 'python tools\waf --help'
    python tools\waf build_primary
    (...)
    'build_primary' finished successfully (8.800s)

The possible ``build`` commands, the definition of the and corresponding targets
and the targets itself is listet below:

- Primary MCU
    ..  code-block::    console
        :name: buildprimarytar
        :caption: Build primary binaries

        python tools\waf build_primary

    The targets are defined at:

    -   ``foxbms\embedded-software\mcu-primary\src\application\wscript``
    -   ``foxbms\embedded-software\mcu-common\src\engine\wscript``
    -   ``foxbms\embedded-software\mcu-common\src\module\wscript``
    -   ``foxbms\embedded-software\mcu-primary\src\engine\wscript``
    -   ``foxbms\embedded-software\mcu-primary\src\module\wscript``
    -   ``foxbms\embedded-software\mcu-freertos\wscript``
    -   ``foxbms\embedded-software\mcu-hal\STM32F4xx_HAL_Driver\wscript``
    -   ``foxbms\embedded-software\mcu-primary\src\general\wscript``

    .. literalinclude:: list_primary.txt
        :language:  console
        :name: primary
        :caption: Primary targets

- Secondary MCU
    ..  code-block::    console
        :name: buildsecondarytar
        :caption: Build secondary binaries

        python tools\waf build_secondary

    The targets are defined at:

    -   ``foxbms\embedded-software\mcu-secondary\src\application\wscript``
    -   ``foxbms\embedded-software\mcu-common\src\engine\wscript``
    -   ``foxbms\embedded-software\mcu-common\src\module\wscript``
    -   ``foxbms\embedded-software\mcu-secondary\src\engine\wscript``
    -   ``foxbms\embedded-software\mcu-secondary\src\module\wscript``
    -   ``foxbms\embedded-software\mcu-freertos\wscript``
    -   ``foxbms\embedded-software\mcu-hal\STM32F4xx_HAL_Driver\wscript``
    -   ``foxbms\embedded-software\mcu-secondary\src\general\wscript``


    .. literalinclude:: list_secondary.txt
        :language:  console
        :name: secondary
        :caption: Secondary targets

- General documentation
    The general documenation is build by

    ..  code-block::    console
        :name: buildsphinxdocumentation
        :caption: Build the general |foxbms| documentation

        python tools\waf sphinx

- API documentation
    The API documentation is build using the ``doxygen_{{variant}}``, therefore

    ..  code-block::    console
        :name: builddoxydocumentation
        :caption: Build the general |foxbms| documentation

        python tools\waf doxygen_primary
        python tools\waf doxygen_secondary

- Cleaning
    It is also possible to clean the binaries and Doxygen documentation.
    This step is performed by the ``clean`` command.

    As seen from ``--help`` the possible ``clean`` commands are

    -   ``clean_all``
    -   ``clean_libs``
    -   ``clean_primary``
    -   ``clean_primary_bare``
    -   ``clean_secondary``
    -   ``clean_secondary_bare``
    -   ``distclean``

    Each command cleans the specified option, except for ``distclean``.
    However it is possible to make a complete clean by ``distclean``. After
    ``distclean`` the entire build directory and all lock files etc. are
    deleted and the project needs to be configured again.
    Cleaning the general sphinx documentation alone is currently not supported,
    but it can be achivied by running ``distclean``.

Targets
-------

As stated above the targets and sub targets of the build process are shown by
``list_x`` where ``x`` is the specified target. The main target is the
``*.elf.unpatched`` file. The final targets are build afterwards. After
successfully linking the map file is generated.

These logging files are found in ``build`` and ``build\{{target}}``.
Additional to the ``*.elf.unpatched`` and ``*.elf`` files a ``*.hex`` and two
``*.bin`` files of the binary are generated. The ``*.bin`` files are separated
into the flash and the flashheader. The size of each object/binary is written
to a log file.

The targets are build as follows (final targets are filled gray):

..  graphviz::
    :caption: |foxbms| build process targets
    :name: foxbms_build_process_targets

        digraph {
            rankdir=TB;
            graph [fontname = "monospace"];
            node [fontname = "monospace"];
            edge [fontname = "monospace"];
            "foxbms_flash.bin" [style=filled];
            "foxbms_flashheader.bin" [style=filled];
            "foxbms.hex" [style=filled];
            "foxbms.elf" [style=filled];
            "foxbms.elf.unpatched" -> "foxbms_flash.bin" [label="objcopy"];
            "foxbms.elf.unpatched" -> "foxbms_flashheader.bin.unpatchted" [label="objcopy"];
            "foxbms_flash.bin" -> "checksum.yml" [label="tsk_cal_chksum"];
            "foxbms_flashheader.bin.unpatchted" -> "checksum.yml" [label="tsk_cal_chksum"];
            "checksum.yml" -> "foxbms.elf" [label="objdump"];
            "foxbms.elf" -> "foxbms.hex" [label="objcopy"];
            "foxbms.elf" -> "foxbms_flashheader.bin" [label="objcopy"];
     }

Build Process
-------------

..  note::
    For testing the following explanations it is assumed that
    ``python tools\waf configure`` has been run.

This sections gives an overview how the build process is defined. All features
are generally defined in the top |wscript| located at
|mainwscript|.

The minimum functions that are needed to be defined a build in |waf| are:

- ``configure`` and
- ``build``.

As the toolchain needs more targets the following functions need to be
implemented: ``doxygen`` and ``sphinx``.

Furthermore the following features are needed:

- for calculating the checksum based on the ``*.elf.unpatched`` file the
  class ``tsk_cal_chksum`` and for creating the ``*.elf`` file the
  ``tsk_wrt_chksum`` class and for adding these features the function
  ``add_chksum_task``,
- for stripping the debug symbols in release mode the class ``strip`` and the
  function ``add_strip_task``,
- for creating a ``hex`` file from the ``elf`` file the class
  ``hexgen``  and the function ``add_hexgen_task``,
- for generating ``bin`` files from ``elf`` files the classes
  ``tsk_binflashheadergen``, ``tsk_binflashgen`` and the function
  ``add_bingen_task`` and the class ``tsk_binflashheaderpatch`` and the
  function ``add_patch_bin_task``,
- for generating size information of the objects and binaries the class
  ``size`` and the function ``process_sizes``,
- for copying the libraries build by ``build_libs`` into the correct
  directories the class ``copy_libs`` and the function ``add_copy_libs``,
- for compiling assembler files ``*.s`` the class ``Sasm`` and the function
  ``asm_hook``.

For implementation details see the |wscript| itself.

Some of these functionalities require scripts from ``foxbms\tools``.

.. image:: build-process.png
    :name: build_process_overview
    :alt: Overview of the build process
    :width: 2000 px

General Documentation
+++++++++++++++++++++
This build target uses the function ``def sphinx(bld)``. Since this
definition of a function called ``sphinx``, it is accepted as command to waf.

This general documentation is generated by running

..  code-block::    console
    :name: sphinx_function
    :caption: Generate general |foxbms| documentation

    python tools\waf sphinx

The implementation details of the ``sphinx`` command can be found in
``foxbms\tools\waftools\sphinx_build.py``.

Primary and Secondary Binaries and Doxygen Documentation
++++++++++++++++++++++++++++++++++++++++++++++++++++++++

In order to have different build *variants*, these variants have to be defined.
This is done at the top of the main |wscript| at |mainwscript|. The
variants have to be defined for the binary build and Doxygen documentation.

.. literalinclude:: variants.txt
    :language: python
    :name: variantimplementation
    :caption: Implementation of the variant build

In the function build and doxygen the build variant is checked, the the correct
sources are selected. If no build variant is specified, an error message is
displayed, telling to specify a variant. This is generally implemented something
like this:

.. code-block:: python
    :name: ensurevariantonbuild
    :caption: Implementation to ensure a variant build

    def build(bld):
        import sys
        import logging
        from waflib import Logs
        if not bld.variant:
            bld.fatal('A {} variant must be specified, run \'{} {} --help\'\
    '.format(bld.cmd, sys.executable, sys.argv[0]))

        bld.env.__sw_dir = os.path.normpath('embedded-software')

        src_dir = os.path.normpath('mcu-{}'.format(bld.variant))
        ldscript = os.path.join(bld.env.__sw_dir, src_dir, 'src', bld.env.ldscript_filename)

For doxygen it is implemented very similar:

.. code-block:: python
    :name: ensurevariantondoxygen
    :caption: Implementation to ensure a variant doxgen API documentation

    def doxygen(bld):
        import sys
        import logging
        from waflib import Logs

        if not bld.variant:
            bld.fatal('A build variant must be specified, run \'{} {} --help\'\
    '.format(sys.executable, sys.argv[0]))

        if not bld.env.DOXYGEN:
            bld.fatal('Doxygen was not configured. Run \'{} {} --help\'\
    '.format(sys.executable, sys.argv[0]))

        _docbuilddir = os.path.normpath(bld.bldnode.abspath())
        doxygen_conf_dir = os.path.join('documentation', 'doxygen')
        os.makedirs(_docbuilddir, exist_ok=True)
        conf_file = 'doxygen-{}.conf'.format(bld.variant)
        doxygenconf = os.path.join(doxygen_conf_dir, conf_file)

wscripts
--------

As mentioned above, the build process is described in |wscript|\ s, which are
itself valid python scripts.
The top is |mainwscript| which defines the functions needed for the
build, e.g., ``configure``, ``build`` etc.

From the top |wscript| the other |wscript| s are called recursive by
``bld.recurse(..)``.

To get a detailed view on the single build steps, see these files.

.. |wscript|        replace:: ``wscript``
.. |mainwscript|    replace:: ``foxbms\wscript``


Building and Linking with a Library
-----------------------------------

The toolchain enables to build a library and then links against the library.
It is possible to build and link multiple libraries into the binaries.

The ``wscript`` in ``embedded-software\libs`` lists the libraries to be build.
Libraries that should be build have to be listed here. Based on the example
library ``testlib`` it is shown how to include a library in |foxbms|.

.. note::
    In fact the ``libs`` directory contains two test libraries
    (``foxbms-user-lib`` and ``my-foxbms-library``) in order to show how
    multiple libraries can be used. The first example shows how to build one
    single library and in the second example it is shown, how to build and use
    more than one library.

**General Setup**

The ``wscript`` in ``embedded-software\libs`` lists in the function
``bld.recurse(...)`` the directories containing the sources for the to be build
library (see line 11 in :numref:`libs_wscript`).

.. literalinclude:: ./../../../../embedded-software/libs/wscript
   :language: python
   :caption: The ``wscript`` in ``embedded-software\libs``
   :name: libs_wscript
   :linenos:
   :lines: 44-
   :emphasize-lines: 11

..  note::
    For every additional library that should be build, the directory
    containing the library must be added to this line, e.g., if the library
    sources are in a directory called ``advancedalgorithms``
    this lines needs to look like this:

        .. code-block:: python
            :caption: Adding the library source directory ``advancedalgorithms``
            :name: add_libs_to_wscript
            :linenos:

            def build(bld):
                bld.recurse('testlib myfoxbmslibrary advancedalgorithms')

The actual build of the library is defined in the ``wscript`` in
``embedded-software\libs\testlib``. All source and header files have to be in
the library directory, for this example these are are ``testlib.c`` and
``testlib.h``. The library is then build by the ``wscript`` in
``embedded-software\libs\testlib``.

:numref:`wscript_for_testlib` explained in detail:

 -  All source files that should be build have to be listed in the ``srcs`` list
    (see line 5-6.).
 -  The name of the library is set to ``foxbms-user-lib`` (see line 11).

    ..  note::
        For later on further expanding the ``advanced-algorithms`` example, the
        library name ``my-advanced-algorithm`` is assumed.

.. literalinclude:: ./../../../../embedded-software/libs/testlib/wscript
   :caption: wscript
   :name: wscript_for_testlib
   :language: python
   :linenos:
   :lines: 43-
   :emphasize-lines: 5-6,11,14-

- The object files (``*.o``) and the library (``*.a``) are found in
  ``build\libs\embedded-software\libs\testlib\``.
- The libraries (the ``*.a``\ -files) are copied in
  ``build\lib\*.a``. When building the default dummy libraries these are
  ``build\lib\libfoxbms-user-lib.a`` and ``build\lib\libmy-foxbms-library.a``.
  The *lib*-prefix is generated automatically. This task is generated by the
  ``copy_lib`` feature (see line 14).
- The headers are copied to ``build\include`` (see line 15-17).

.. warning::
    **The header names for all library headers are checked for uniqueness.**
    Header files with the same name recursively inside the ``libs`` directory
    will lead to a build error. This check needs to be performed, as all
    headers get copied to include directory at ``build\include``.

**The Library**

The library declaration of ``super_function(uint8_t a, uint8_t b)`` is in ``testlib.h``:

.. literalinclude:: ./../../../../embedded-software/libs/testlib/testlib.h
   :caption: testlib.h
   :language: c
   :linenos:
   :lines: 53-
   :emphasize-lines: 13

The library defines a function ``super_function(uint8_t a, uint8_t b)`` in
``testlib.c``:

.. literalinclude:: ./../../../../embedded-software/libs/testlib/testlib.c
   :caption: testlib.c
   :language: c
   :linenos:
   :lines: 65-67

**Building**

#.  Build the library (or libraries):

    ..  code-block::    console
        :name: builthelibs
        :caption: Build the libraries

        python tools\waf build_libs

    Now all libraries are present in ``build\libs`` and the headers are in
    ``build\include``.

#.  Configure the |foxbms| project to work with a library, in the first example
    it is the ``foxbms-user-lib`` library.

    ..  code-block::    console
        :name: configwithlib
        :caption: Configuration with library useage

        python tools\waf configure --libs=foxbms-user-lib

.. note::
    For including the hypothetical ``my-advanced-algorithm`` library the command
    would be:

    ..  code-block::    console

        python tools\waf configure --libs=my-advanced-algorithm

#.  Include the headers needed for the functions in the sources and use the functions
    as needed.

    ..  code-block::    c
        :name: includecfunctionfromlibrary
        :caption: Include header and use a function from the library
        :linenos:
        :emphasize-lines: 3,11

        /*================== Includes =============================================*/
        /* some other includes  */
        #include "testlib.h"

        /*================== Function Prototypes ==================================*/

        /*================== Function Implementations =============================*/
        int main(void) {
            uint16_t a = 0;
            /* Use the function super_function from the library */
            a = super_function(2,2);
            /* other code */
        }

#.  Build the |foxbms| binary as usual.

    ..  code-block::    console
        :name: builthebinarywithlib
        :caption: Build the |foxbms| binary

        python tools\waf build_primary

**Building with Multiple Libraries**

A project may want to use multiple libraries. For this example the two provided
dummy libraries are assumed (``foxbms-user-lib`` and ``my-foxbms-library``).

#.  The project is configured to work with both libraries. The library names are
    given as command line argument separated by comma (**no additional \
    whitespace**).

    ..  code-block::    console
        :name: configwithmultiplelib
        :caption: Configuration with multiple library useage

        python tools\waf configure --libs=foxbms-user-lib,my-foxbms-library

#.  Build the library (or libraries):

    ..  code-block::    console
        :name: builthelibs2
        :caption: Build the libraries

        python tools\waf build_libs

#.  Include the headers needed for the functions in the sources and use the functions
    as needed.

    ..  code-block::    c
        :name: includecfunctionsfromlibraries
        :caption: Include header and use a function from the library
        :linenos:
        :emphasize-lines: 3,4,11,15

        /*================== Includes =============================================*/
        /* some other includes  */
        #include "testlib.h"
        #include "myfoxbmsalgorithms.h"

        /*================== Function Prototypes ==================================*/

        /*================== Function Implementations =============================*/
        int main(void) {
            uint16_t a = 0;
            /* Use the function super_function from the library foxbms-user-lib */
            a = super_function(2,2);
            uint16_t b = 0;
            /* Use the function another_super_function from the library my-foxbms-library */
            a = another_super_function(2,2);
            /* other code */
        }

#.  Build the |foxbms| binary as usual.

    ..  code-block::    console
        :name: builthebinarywithlib2
        :caption: Build the |foxbms| binary

        python tools\waf build_primary

Building the Test
-----------------

.. note::

    The test builds described in this section are not mandatory. They can be
    used as a simple check that the software architecture is kept (see
    :ref:`foxbms_software_architecture`).

In order to verify that low level drivers (i.e., the drivers in
``embedded-software\mcu-common\driver``) do not relay on higher level modules
(e.g., FreeRTOS, database, etc.) two tests are included. These can be build by

    ..  code-block::    console
        :name: build_bare_build
        :caption: Build bare tests for primary and secondary mcu

        python tools\waf configure
        python tools\waf configure build_primary_bare
        python tools\waf configure build_secondary_bare
