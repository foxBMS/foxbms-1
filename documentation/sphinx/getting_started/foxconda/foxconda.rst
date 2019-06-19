.. include:: ../../macros.rst

.. _GETTING_STARTED_FOXCONDA:

======================
Installing |foxconda3|
======================

The |foxbms| embedded software consists of a program written in C. Before it
can be flashed and run on the microcontroller units (i.e., |MCU0| and |MCU1|
located on the |BMS-Master|), it must be compiled to generate a binary file
called firmware.

Different softwares (i.e., the software toolchain) are needed for this
compilation step. They are based on a Python environment. All the needed
software, including the Python environment, are contained in a Python
distribution called |foxconda3|. It will be installed in the next steps.

This section shows how to set up the development environment, |foxconda3| (the
software toolchain).

In the section :ref:`getting_started_eclipse_workspace`, it is shown how the
compiled sources are flashed on the |Master|.

--------------------------------------------------------------------
Installation of the Needed Software via the |foxconda3| Distribution
--------------------------------------------------------------------

The first step is to install the software environment needed to compile and
flash the |foxbms| sources. First, the |foxconda3| installer must be downloaded
from the `server <https://iisb-foxbms.iisb.fraunhofer.de/foxbms/>`_ containing
the |foxconda3| installers. For Windows, the installer is run by executing
|installer archive|. The installer version with the highest version and build
number must always be used.

..  warning::
    -   Setting the installation directory from ``C:\foxconda3`` to
        something other will create a lot extra effort when setting up the
        Eclipse workspace. It is highly advised against changing the default
        installation directory.
    -   When installing |foxconda3| to another directory, |foxconda3| must not
        be installed into a directory containing whitespace (e.g.
        ``C:\Program Files``)


The following figures from :numref:`fig. %s <condainstaller1>` to
:numref:`fig. %s <condainstaller8>` will show the installation process.

#.  The startpage of the installer

    .. _condainstaller1:
    .. figure:: ./condainstaller1.png

        Installer start page

#.  The license terms of the installer

    .. _condainstaller2:
    .. figure:: ./condainstaller2.png

        Installer license terms

#.  The installation type |foxconda3| binaries. Select
    ``Just Me (recommended)``.

    .. _condainstaller3:
    .. figure:: ./condainstaller3.png

        Installation type of |foxconda3|

#.  The installation path for |foxconda3|. Set the path to ``C:\foxconda3``.

    ..  warning::

            Changing the default installation directory from ``C:\foxconda3``
            to something other will create a lot extra effort when setting up
            the Eclipse workspace. This is not advised.

    .. _condainstaller4:
    .. figure:: ./condainstaller4.png

        Installation directory

#.  Advanced options of the installation. **Unselect**
    ``Register Anaconda as my default Python 3.6``

    .. _condainstaller5:
    .. figure:: ./condainstaller5.png

        Advanced Options

#.  The installation process

    .. _condainstaller6:
    .. figure:: ./condainstaller6.png

        Installation process

#.  The installation process is completed. Click ``Next >``.

    .. _condainstaller7:
    .. figure:: ./condainstaller7.png

        Completed installation step

#.  The finish of the installation process. Click ``Finish``.

    .. _condainstaller8:
    .. figure:: ./condainstaller8.png

        Installation finish

From the command line, the convenience terminal environment can be used by
executing the ``Anaconda Prompt``:

- Go the the start menu
- Type ``Anaconda Prompt``
- Enter

It will open a CMD window with a ``PATH`` environment ready to work with
|foxbms|.

The prompt can also be started manually by opening a CMD window followed by
following command:

``C:\foxconda3\Scripts\activate``

-----
Proxy
-----

Some ``conda`` commands (e.g., ``conda install``) will not work behind a proxy
by default. The following steps configure ``conda`` to work behind a proxy:

#.  The ``.condarc`` is created by running the following command in the
    ``Anaconda Prompt``

    ..  code-block::    console
        :name: condaconfig
        :caption: conda config command

        conda config

    The ``.condarc`` file is created in the user home
    directory (e.g., ``C:\users\username\.condarc``)

#. Open the ``.condarc`` with a text editor and add the proxy information:

    ..  code-block::    console
        :name: condaproxyconfig
        :caption: conda proxy configuration

        proxy_servers:
          http: http://my-http-proxy-server:Port
          https: https://my-https-proxy-server:Port

Details on proxy configuration are found in the official ``conda``
documentation at
`Configure conda for use behind a proxy server (proxy_servers) <https://conda.io/projects/conda/en/latest/user-guide/configuration/use-condarc.html#configure-conda-for-use-behind-a-proxy-server-proxy-servers>`_.

More detailed information on conda configuration is also found in the
`conda configuration documentation <https://conda.io/projects/conda/en/latest/user-guide/configuration/index.html#>`_.

-------------------------------------------
Software Related Frequently Asked Questions
-------------------------------------------

Where are the Sources?
----------------------

The sources are available on `GitHub <https://github.com/foxBMS/>`_.

What Libraries and Programs Must be Installed?
----------------------------------------------

None, because they are installed with the |foxconda3| installer.
