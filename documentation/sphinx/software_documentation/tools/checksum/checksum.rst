.. include:: ../../../macros.rst



.. _software_documentation_checksum_tool:

=============
Checksum Tool
=============

.. highlight:: python

This section of the documentation shows the software build process and the
integration of the checksum into that process in |foxbms|. To setup and
enabling/disabling the checksum verification on the microcontroller, see
:ref:`CHECKSUM`.


Module Files
~~~~~~~~~~~~

Source:

 - ``tools\checksum\chksum.py``

Configuration:

 - ``tools\checksum\chksum.yml``

Procedure
~~~~~~~~~

Checksum is an after-build process.

.. code:: bash

    python tools\waf-1.8.12 configure
    python tools\waf-1.8.12 build_primary
    python tools\waf-1.8.12 build_secondary

Principle
~~~~~~~~~

#. The step

    .. code:: bash

        python tools\waf-1.8.12 configure

    configures waf as needed to build.

#. The step

    .. code:: bash

        python tools\waf-1.8.12 build

    builds the files

     - ``foxbms.elf``
     - ``foxbms.hex``
     - ``foxbms_flash.bin``
     - ``foxbms_flashheader.bin``

    The ``chksum`` featured task is perfmored at next as defined in the main wscript. The steps are of this task are:

        #. Reading the ``foxbms.hex`` file and calculates the checksum. The checksum is written back into the ``foxbms.hex`` file by the checksum script.
        #. Calling the GDB debugger and replaces the initial ``ver_sw_validation.Checksum_u32`` in ``foxbms.elf`` with the correct checksum.
        #. Calling the ``objcopy`` to regenerate the ``foxbms_flashheader.bin`` of ``foxbms.elf``.

Related Modules
~~~~~~~~~~~~~~~
The checksum tool is related on the ``Checksum Module`` configuration (see :ref:`CHECKSUM`).
