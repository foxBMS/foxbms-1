.. include:: ../../macros.rst

.. _GETTING_STARTED_BUILD:

==============================================
Building the foxBMS Software and Documentation
==============================================


Requirements
------------

The build process of |foxbms| heavily depends on Python, for example, for
code generation purposes. |foxbms| hence comes with its own Python
distribution, called |foxconda3|, powered by
`Anaconda <https://continuum.io>`_.
These build instructions assume that |foxconda3| was successfully installed and
that the ``PATH`` environment has been adjusted accordingly. For further
information refer to the |foxconda3| documentation
(:ref:`getting_started_foxconda`).

Obtaining the Sources
---------------------

Download or clone the |foxbms| repository from
`github.com/foxBMS/ <https://github.com/foxBMS/foxbms>`_.

..  warning::

    Do not change directory names or the structure inside ``foxbms``. If this
    is changed most, if not all, ``wscript``\ s, have to be heavily adapted and
    this can get very complex extremely fast.

Building the Binaries and Documentation
---------------------------------------

.. note::
    All commands described here must be run in the ``Anaconda Prompt`` provided
    by |foxconda3|.

|foxbms| targets can be build using the command line or using the |foxbms|
Eclipse workspace. This section describes the build from command line. Details
on building the binaries using the Eclipse Workspace can be found in
":ref:`getting_started_eclipse_workspace`".

The tool for building targets is found in ``foxbms\tools`` and is called
``waf``. A help is displayed by running ``python tools\waf -h``.

In the |foxbms| project, several targets can be built. The output is stored
in a subdirectory of ``\build\``.

-   To be able to build binaries and documentation, the project needs to be
    configured once:

    ..  code-block:: console

        python tools\waf configure

This will create a directory ``build`` including the configuration files.

- Primary MCU:

 - Doxygen documentation

  - This target is built with ``python tools\waf doxygen_primary``.
  - The output directory is ``build\primary\doxygen``.
  - The main document of the software documentation is found at
    ``build\primary\doxygen\html\index.html``.

 - Binaries

  .. note::
    The output files where the filenames end with ``.unpatched`` are
    intermediate build results which can not run on the hardware.

  - This target is built with ``python tools\waf build_primary``.
  - The output directory is ``build\primary\embedded-software``.

  - The files generated in the directory
    ``build\primary\embedded-software\mcu-primary\src\general`` are:

    - ``foxbms_primary.elf``,
    - ``foxbms_primary_flash.bin``,
    - ``foxbms_primary_flashheader.bin`` and
    - ``foxbms_primary.hex``.

  - The primary mcu binaries and documentation are cleaned by running
    ``python tools\waf clean_primary``.

- Secondary MCU:

 - Doxygen documentation

  - This target is built with ``python tools\waf doxygen_secondary``.
  - The output directory is ``build\secondary\doxygen``.
  - The main document of the software documentation is found at
    ``build\secondary\doxygen\html\index.html``.

 - Binaries

   - This target is built with ``python tools\waf build_secondary``.
   - The output directory is ``build\secondary\embedded-software``.
   - The files generated in the directory
     ``build\secondary\embedded-software\mcu-secondary\src\general`` are:

     - ``foxbms_secondary.elf``,
     - ``foxbms_secondary_flash.bin``,
     - ``foxbms_secondary_flashheader.bin`` and
     - ``foxbms_secondary.hex``.

  - The secondary mcu binaries and documentation are cleaned by running
    ``python tools\waf clean_secondary``.

- General documentation (``sphinx`` documentation):

  - This target is built with ``python tools\waf sphinx``.
  - The output directory is ``build\sphinx``.
  - The main document of the software documentation is found in ``build\documentation\index.html``.
