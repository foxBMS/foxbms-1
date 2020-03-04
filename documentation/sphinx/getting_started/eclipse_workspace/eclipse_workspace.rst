.. include:: ../../macros.rst

.. _GETTING_STARTED_ECLIPSE_WORKSPACE:

====================================
Eclipse Workspace Setup and Flashing
====================================

In this section, it is shown how to set up an Eclipse workspace to work with
the |foxBMS| sources. Within the workspace, it is possible to flash the
compiled sources on the |Master|.

Setting an Eclipse Workspace
============================

..  warning::
    Every part of this setup instruction must be read carefully and it must be
    followed step by step, including details. If this is not the case, the
    Eclipse Workspace will not work.

..  warning::
    Only |foxconda_name| (see :ref:`GETTING_STARTED_FOXCONDA`) is supported.
    The support for older versions of |foxconda_old| has been dropped.


In the following, it is described which requirements have to be fulfilled and
what steps have to be made to setup an Eclipse workspace for working with the
|foxbms| sources.

..  note::
    The following setup instructions use two variables which have to be adapted
    to the user's settings. The example below shows how to adapt these two
    variables.

    -   **Variable 1**: Path to the project directory

        -   The project directory is |setup_dir|. The path to the project is
            therefore |setup_path| (e.g., |setup_dir_example|,
            |setup_dir_example2|, etc.). This means, during this setup
            procedure where ever the variable |setup_path| occurs it needs to
            be replaced with actual project directory on the machine (e.g.,
            |setup_dir_example| or |setup_dir_example2|, etc.).

        -   The path to the project directory must not contain spaces.


    -   **Variable 2**: Path to |foxconda_name| installation

            -   The preferred installation path of |foxconda_name| is
                |foxconda_default_windows_path|. This path will be used during
                the setup. If this is changed, it is up to the reader to change
                it at time to the appropriate path. It is strongly recommended
                to install foxconda to |foxconda_default_windows_path|.

            -   If |foxconda_name| is installed into
                |foxconda_default_windows_path| some steps of these
                instructions can be skipped. These steps are indicated.


Requirements
------------

The following requirements have to be fulfilled to be able to set up an Eclipse
Workspace for working with the |foxbms| sources.

-   |foxconda_name| is installed.

-   |foxbms| source files are downloaded to |setup_path|. Getting the source
    files is described in section ":ref:`getting_started_build`".

-   `Eclipse CDT <https://www.eclipse.org/cdt/>`_ , in version Oxygen.1.Release
    (4.7.1a). Eclipse CDT Oxygen.1.Release (4.7.1a) can be downloaded form
    `eclipse.org <https://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/oxygenr>`_.
    After downloading the ``.zip file`` and extracting it, ``eclipse.exe`` must
    be started to launch ``Eclipse CDT``.

-   The ``PyDev`` (`PyDev project website <https://www.pydev.org/>`_) plugin
    can be installed into ``Eclipse CDT``. The plugin can be installed from
    the ``Eclipse Marketplace``.

    .. _eclipse_marketplace:
    .. figure:: eclipse-marketplace.png

            ``Eclipse Marketplace``

    - Installing ``PyDev``

            .. _pydev_plugin:
            .. figure:: eclipse-marketplace-pydev.png

                ``PyDev`` Eclipse plugin

        - Click |ECLIPSEINSTALL| to install the plugin.

How to Setup the Eclipse Workspace
----------------------------------

The setup process is divided into following parts:

 #. :ref:`creating_the_workspace`
 #. :ref:`configuring_the_python_interpreter`
 #. :ref:`importing_the_project`
 #. :ref:`configuring_the_project_paths`
 #. :ref:`configuring_the_project_includes`
 #. :ref:`testing_the_project_setup`

The order of these steps must **not** be changed.

.. _creating_the_workspace:

Creating the Workspace
++++++++++++++++++++++

The Eclipse Workspace will be called ``.ws``.

#.  Start ``Eclipse`` and click ``File`` -> ``Switch Workspace`` -> ``other``
    and configure ``path\to\foxbms\.ws`` as workspace, see
    :numref:`Fig. %s <switch_workspace>`.

    .. _switch_workspace:
    .. figure:: switch-workspace.png
        :width: 80 %

        Selecting the correct workspace location

#. Click |ECLIPSELAUNCH|

|OK| The |foxbms| Eclipse Workspace is now successfully created. |OK|


.. _DEFAULT_ENCODING_AND_NEWLINE_DELIMITER:

Configuring Default Encoding and Newline Delimiter
++++++++++++++++++++++++++++++++++++++++++++++++++

#.  Open ``Window`` -> ``Preferences`` -> ``General`` -> ``Workspace``

#.  Set ``Text file encoding`` to ``Other: UTF-8`` and
    ``New text file line delimiter`` and select ``Other: Windows``

    ..  _SET_DEFAULT_ENCODING_AND_NEWLINE_DELIMITER:
    ..  figure:: eclipse-default-file-encoding-and-newline-delimiter.png

        Setting the default file encoding and the line delimiter

#.  Click |ECLIPSEAPPLYANDCLOSE|

.. _configuring_the_python_interpreter:

Configuring the Python Interpreter
++++++++++++++++++++++++++++++++++

#.  Open ``Window`` -> ``Preferences`` -> ``PyDev`` -> ``Interpreters`` ->
    ``Python Interpreter``.

    ..  _select_python_interpreter:
    ..  figure:: select-python-interpreter-1.png

        Selecting the python interpreter

#.  Click |ECLIPSENEW|.
    In the window ``Select Interpreter`` use

    #.  |foxconda_name| for the ``Interpreter Name``

    #.  |foxconda_pythonexe_path| for ``Interpreter Executable``.

    ..  _select_python_interpreter_2:
    ..  figure:: select-python-interpreter-2.png
        :width: 80 %

        Choosing the python interpreter name and executable

#.  Click |ECLIPSEOK|

#.  In menu ``Selection needed`` do not change anything and click |ECLIPSEOK|

    ..  _python_selection_needed:
    ..  figure:: python-selection-needed.png
        :width: 80 %

        Adding the ``PYTHONPATH``

#.  Click |ECLIPSEAPPLYANDCLOSE|

    ..  _python_configuration_done:
    ..  figure:: python-configuration-done.png
        :width: 80 %

        Configuration done

|OK| |foxconda_name| is now successfully selected as Python interpreter. |OK|


.. _importing_the_project:

Importing the Project
+++++++++++++++++++++

#.  Import the project via ``File`` -> ``Import``

    .. _eclipse_import.png:
    .. figure:: eclipse-import.png

        Select ``Import``


#.  Click ``General`` -> ``Existing Projects into Workspace``.

    ..  _setup_project_existing_projects_into_workspace.png:
    ..  figure:: setup-project-existing-projects-into-workspace.png

        Select projects import by ``Existing Projects into Workspace``

#.  Click |ECLIPSENEXT|

#.  Chose ``Select archive file:`` and use
    ``path\to\foxbms\tools\eclipse\foxbms-eclipse-project.zip``
    This will list the |foxbms| projects.

    .. _setup-project-import-projects.png:
    .. figure:: setup-project-import-projects.png

        Select all projects to be imported

    .. note::
        Always import all projects that appear, as the number of projects might
        increase with future updates and this help might not be up-to-date.

#.  Click |ECLIPSEFINISH| to complete the project import process.

|OK| The |foxbms| projects are now successfully imported. |OK|

.. _configuring_the_project_paths:

Configuring the Project PATHs
+++++++++++++++++++++++++++++

..  note::
    If |foxconda_name| was installed in the default installation path
    (|foxconda_default_windows_path|), this section can be skipped.

For this setup: Change the path |foxconda_default_windows_path| to the path
where |foxconda_name| was installed to.

#.  Select the ``Primary`` project and click right and select ``Properties``.

    ..  _eclipse_project_properties.png:
    ..  figure:: eclipse-project-properties.png

        Eclipse project properties

#.  Goto ``C/C++ Build`` -> ``Environment``. Select ``[All configurations]``

    ..  _eclipse_project_properties_c_cpp_environment.png:
    ..  figure:: eclipse-project-properties-c-cpp-environment.png

        ``C/C++ Build`` project properties

        #. Click |ECLIPSEADD| and

            #.  use ``FOXCONDA3`` for ``Name``

            #.  use
                ``C:\foxconda3;``
                ``C:\foxconda3\bin;``
                ``C:\foxconda3\Scripts;``
                ``C:\foxconda3\Library\bin;``
                ``C:\foxconda3\Lib;``
                for ``Value``.

                .. warning::
                    -   The ``Value`` property  must not include spaces.
                    -   Do not add a backslash (``\``) at the end of the paths.
                    -   These paths need to be in a single line.

            #.  Check ``Add to all configurations``

            #.  Click |ECLIPSEOK|

        #. Click |ECLIPSEADD| and

            #.  Use ``PATH`` for ``Name``
            #.  Leave ``Value`` empty
            #.  Click |ECLIPSEOK|

        #. Select the ``PATH`` entry and click |ECLIPSEEDIT|

            #.  Delete all entries in ``Value``
            #.  Add ``${FOXCONDA3};`` to ``Value``
            #.  Click |ECLIPSEOK|

    The result should look like this:

    ..  _project_path_adding.png:
    ..  figure:: project-path-adding.png

        ``Environment`` settings for the project.

#.  Repeat these steps for the following projects:

    - ``documentation``
    - ``foxbms``
    - ``secondary``

|OK| The |foxbms| projects now include the correct environment settings. |OK|

.. _configuring_the_project_includes:

Configuring the Project Includes
++++++++++++++++++++++++++++++++

..  note::
    If |foxconda_name| was installed in the default installation path
    (|foxconda_default_windows_path|), this section can be skipped.

For this setup: Change the path |foxconda_default_windows_path| to the path
where |foxconda_name| was installed to.

#.  Select the ``primary`` project and click right. Goto ``C/C++ General`` ->
    ``Paths and Symbols`` and switch to Tab ``Includes``.

#.  Click |ECLIPSEADD| for ``Assembly``, ``GNU C`` and ``GNU C++`` and
    configure the following two paths as ``Includes``

    #. ``C:\foxconda3\Library\arm-none-eabi\include``

    #. ``C:\foxconda3\Library\lib\gcc\arm-none-eabi\4.9.3\include``

    ..  _include_header.png:
    ..  figure:: include-header.png

        Adding the gcc-includes to a project

    #. Click |ECLIPSEAPPLYANDCLOSE|

#.  Repeat this procedure for the ``secondary`` project.

#.  Restart Eclipse

|OK| The |foxbms| projects are now successfully configured. |OK|

.. _testing_the_project_setup:

Testing the Project Setup
+++++++++++++++++++++++++

#. ``foxbms`` project

    #.  Select the ``foxbms`` project and click |ECLIPSEHAMMER| button and
        select ``1 configure``. The project is now configured and all options
        (building documentation and binaries) can now be used.

        ..  code-block:: console

            'configure' finished successfully (0.450s)

    #.  Select the ``foxbms`` project and click |ECLIPSEHAMMER| button and
        select ``2 help``. The project help for building is printed.

#.  ``documentation`` project

    #.  Select the ``documentation`` project and click on the dropdown menu of
        the |ECLIPSEHAMMER| button and select ``1 sphinx`` to build the general
        |foxbms| documentation.

        ..  code-block:: console

            'sphinx' finished successfully (0.105s)

    #. The ``Documentation`` project is now successfully tested.

#.  ``primary`` project

    #.  Select the ``primary`` project and click on the dropdown menu of the
        |ECLIPSEHAMMER| button and select ``1 build_primary`` to build the
        |foxbms| binaries of the primary mcu.

        The binary generation was successful, if the Eclipse console puts the
        following line at the bottom:

        ..  code-block:: console

            'build_primary' finished successfully (1.340s)

    #.  Select the ``primary`` project and click on the dropdown menu of the
        |ECLIPSEHAMMER| button and select ``2 doxygen_primary`` to build the
        |foxbms| embedded documentation of the primary mcu.

        The primary mcu dopxygen documentation generation was successful, if
        the Eclipse console puts the following line at the bottom:

        ..  code-block:: console

            'doxygen_primary' finished successfully (46.906s)

    #.  Select the ``primary`` project, right click the project and select
        ``Clean Project``

        The cleaning of the primary mcu binaries and doxygen documentation was
        successful, if the Eclipse console puts the following line at the
        bottom:

        ..  code-block:: console

            'clean_primary' finished successfully (1.065s)

    #. The ``primary`` project is now successfully tested.

#.  ``secondary`` project

    #.  Select the ``secondary`` project and repeat all steps of the
        ``primary`` project.

    #. The ``secondary`` project is now successfully tested.

|OK| The |foxbms| Eclipse Workspace is now successfully tested. |OK|

..  warning::

    To test flashing in the next step, binaries of the primary and secondary
    mcu need to be build again.

.. note::

    If problems occur while using the Eclipse workspace, it is possible to use
    a VS Code setup. For details on |foxbms| and VS Code see
    `here <https://github.com/foxBMS/foxbms/tree/v1.6.4/tools/vscode>`_.

    **However, VS Code is not actively supported and should only be used in**
    **cases where the Eclipse setup does not work.**


Optional Settings
-----------------

When working with the build scripts (``wscript``) it is more convenient to have
python syntax highlighting. The following has to be configured to achieve it:

#.  Click ``Window`` -> ``Preferences``

#. Select ``General`` -> ``Editors`` -> ``File Associations``

#.  At the entry ``File types`` click |ECLIPSEADD|

    ..  _syntax-highlighting-wscript-1.png:
    ..  figure:: syntax-highlighting-wscript-1.png

        ``File Associations`` settings

#.  Define the file type ``wscript`` and click |ECLIPSEOK|

    ..  _syntax-highlighting-wscript-2.png:
    ..  figure:: syntax-highlighting-wscript-2.png

        ``File type definition``

#.  Select the ``Python Editor`` as and click |ECLIPSEOK|

    #.  ..  _syntax-highlighting-wscript-3.png:
        ..  figure:: syntax-highlighting-wscript-3.png

        ``Editor Selection`` setting

#.  Finish the configuration by clicking |ECLIPSEAPPLYANDCLOSE|

    ..  _syntax-highlighting-wscript-4.png:
    ..  figure:: syntax-highlighting-wscript-4.png

        Final file association setting for ``wscript``


Flashing |foxbms|
=================

This section describes how to supply the |master| in order to flash both |MCU0|
and |MCU1| with the firmware produced by compiling the C-code source files.

This section shows how to connect the different parts of the |foxbms| platform.


Convention for Connector Numbering
----------------------------------

:numref:`Fig. %s <connector_convention_flash>` presents the convention for the
connector numbering. It is used throughout the documentation.

.. _connector_convention_flash:
.. figure:: connector_viewing_direction.png
    :width: 100 %

    Supply connector pin out, receptable - rear view, header - front view
    (image source: MOLEX)

There are two types of connectors:

 * Header
 * Receptable, plugged into the header

The numbering shown on the left in
:numref:`fig. %s <connector_convention_flash>` is always valid when viewing in
the direction indicated by the arrow with the indication ``viewing direction``.
This must be taken into account when crimping the receptables.

Hardware Setup of |master| and |slaves|
---------------------------------------

The foxBMS system can be mounted in a metal housing, as shown in
:numref:`fig. %s <housing_flash>`.

.. _housing_flash:
.. figure:: foxbms_housing.jpg
    :width: 100 %

    |master| housing

Connectors are available, shown in
:numref:`Fig. %s <foxbmsfront_quickstart_flash>`, which presents all the
connectors of the |master|.

.. _foxbmsfront_quickstart_flash:
.. figure:: ../../hardware_documentation/casing/foxbms-frontplate-rotated.png
    :width: 100 %

    Front view of the |master| indicating the location of each header

For this section on flashing, only the connector ``Supply`` is needed.

Supply of the |master|
----------------------

The first step is to supply the |master|, which works with supply voltages
between 12V and 24V DC. To supply the |master|, a connector must be prepared
for the ``Supply`` connector as shown in
:numref:`table %s <master_supply_connector>`, which describes the different
pins used.

.. _sixcon_view:
.. figure:: connector_6pin.png
    :width: 10 %

.. _master_supply_connector:

.. table:: |BMS-Master| Supply Connector

   ====   =============    ============    ============
   Pin    Signal           Input/Output    Description
   ====   =============    ============    ============
   1      SUPPLY_EXT_2     Input           12 - 24V
   2      SUPPLY_EXT_2     Input           12 - 24V
   3      GND_EXT_2        Input           GND
   4      SUPPLY_EXT_0     Input           12 - 24V
   5      GND_EXT_0        Input           GND
   6      GND_EXT_2        Input           GND
   ====   =============    ============    ============

The supply is separated as follows:

 -  The microcontrollers and the isolation devices are supplied through the
    pins ``SUPPLY_EXT_0`` and ``GND_EXT_0``
 -  The contactors and the interlock are supplied through the pins
    ``SUPPLY_EXT_2`` and ``GND_EXT_2``

To power up the |master|, plug in the supply connector and apply a voltage
between 12V and 24V. ``SUPPLY_EXT_0`` / ``GND_EXT_0`` and ``SUPPLY_EXT_2`` /
``GND_EXT_2`` may be connected to the same source for this initial test. At
this point, the |master| should draw approximately 150mA at 12V or 110mA at
24V.

Primary and Secondary MCU
-------------------------

The |BMS-Master| has two MCUs: primary (|MCU0|) and secondary (|MCU1|). The
|MCU1| is present to ensure redundant safety especially when used in research
and development prototyping.

First, the primary MCU will be flashed.

In order to program the primary MCU, the mini USB jack indicated as
``Prim. USB`` in :numref:`fig. %s <foxbmsfront_quickstart_flash>` must be used
to connect the |master| to a PC. The |master| can be connected to a PC
immediately. When connecting |master| for the first time, the required drivers
will install automatically.

.. note::

      Before the connection is made between the |master| and the computer for
      the first time, the computer must be connected to the internet, because
      the operating system might look for drivers on the internet. It this
      fails, administrator rights are needed to install the driver.

In case of problems by the installation of the drivers, administrator rights
might be needed. Once the hardware is supplied with the appropriate voltage,
the |foxbms| binaries can be flashed.

The same procedure must be made for secondary: the mini USB jack indicated as
``Sec. USB`` in :numref:`fig. %s <foxbmsfront_quickstart_flash>` must be used
to connect the |master| to a PC. As for the primary MCU, the PC must be
connected to the internet before the connection is made with the |master| for
the first time.

Flashing the Compiled Sources for the Primary MCU
-------------------------------------------------

In the chapter :ref:`getting_started_build`, the |foxbms| sources have been
compiled and the generated binary is ready to be flashed.
The |MCU0| will be flashed first.

These binaries are needed:

 1. a binary file with the header
 2. a binary file with the main code

Unless the build process has been altered or different binaries should be
flashed, there is no need to change the default file locations. For this guide,
the default file locations are used.

The device must be connected to the computer with the USB cable. The |MCU0|
USB-connector must be used. The ``primary`` project must be select. Clicking on
the dropdown menu of the |ECLIPSEHAMMER| button and selecting
``3 flash_primary`` allows flashing the |foxbms| binaries of the primary mcu.

.. note::

    Make sure that CAN0 connector is either disconnected or that there is no
    traffic on CAN0 bus while flashing ``primary`` MCU. If this is not the
    case, the internal bootloader of the controller will select a wrong boot
    source and the flashing fails.

The connection state can also be checked by watching the LEDs on the |master|
hardware: for a running board, the green power LED is on, and the two indicator
LEDs (green and red) are blinking alternately. If the device is connected and
being flashed, the power-on LED is on but the two indicator LEDs are off.

The LEDs for the |MCU0| must be used. In the :ref:`getting_started_cabling`, it
is explained how to find the |MCU0| indicator LEDs.

When the flashing is complete, the flashing windows disappears and the
indicator LEDs start to blink again.

Under Windows 7 (64 bit), it can happen that the COMPORT number is increased by
Windows, leading to problems during flashing. This can be reset in registry, by
setting ``ComDB`` to zero in the entry
``HKEY_LOCAL_MACHINE::SYSTEM::CURRENT_CONTROL_SET::CONTROL::COM NAME ARBITER``.
This way, all COM ports used are reset.

Flashing the Compiled Sources for the Secondary MCU
---------------------------------------------------

The same procedure as explained above must be followed for the |MCU1|. The
difference is that the |MCU1| USB-connector must be used. The ``secondary``
project must be selected before clicking on the dropdown menu of the
|ECLIPSEHAMMER| button. ``3 flash_secondary`` must then be selected to flash the
|foxbms| binaries of the secondary mcu.

With the supplied code, the |MCU1| checks the temperatures and voltages. If one
or more of the values are outside the limits, the |MCU1| opens the interlock
line (thus opening the contactors). The interlock line remains open until the
|MCU1| is reset.

.. note::

    Setting the limits is safety relevant and must be done with care.

.. note::

    **Working without configuring the right battery cell voltage limits is
    dangerous and should never be done when real batteries are connected, since
    they may burn and explode when overcharged or shorted.**

The next step shown in :ref:`getting_started_cabling` is to connect a |slave|
to perform voltage and temperature measurement.

Debugging the Primary and Secondary MCU
---------------------------------------

The following two debuggers can be used for debugging and have been tested with
|foxBMS|:

 *  `Segger J-Link Plus <https://www.segger.com/j-link-plus.html>`_, with the
    `19-Pin Cortex-M adapter <https://www.segger.com/jlink-adapters.html#CM_19pin>`_
    (needed to connect to foxBMS)
 *  `Lauterbach ÂµTrace Debugger for Cortex-M <http://www.lauterbach.com>`_


 .. |OK|                     image:: ok.png

.. |ECLIPSEADD|             image:: eclipse-add.png
.. |ECLIPSEAPPLY|           image:: eclipse-apply.png
.. |ECLIPSEAPPLYANDCLOSE|   image:: eclipse-apply-and-close.png
.. |ECLIPSEEDIT|            image:: eclipse-edit.png
.. |ECLIPSEFINISH|          image:: eclipse-finish.png
.. |ECLIPSEHAMMER|          image:: eclipse-hammer.png
.. |ECLIPSEINSTALL|         image:: eclipse-install.png
.. |ECLIPSELAUNCH|          image:: eclipse-launch.png
.. |ECLIPSENEW|             image:: eclipse-new.png
.. |ECLIPSENEXT|            image:: eclipse-next.png
.. |ECLIPSEOK|              image:: eclipse-ok.png

.. |setup_dir|                      replace:: ``foxbms``
.. |setup_path|                     replace:: ``path\to\foxbms``
.. |setup_dir_example|              replace:: ``C:\Users\username\Documents\foxbms``
.. |setup_dir_example2|              replace:: ``C:\projects\foxbms``
.. |foxconda_old|                   replace:: ``foxconda``
.. |foxconda_name|                  replace:: ``foxconda3``
.. |foxconda_path|                  replace:: ``C:\foxconda3``
.. |foxconda_pythonexe_path|        replace:: ``C:\foxconda3\python.exe``
.. |foxconda_default_windows_path|  replace:: ``C:\foxconda3``
