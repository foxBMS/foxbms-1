.. include:: ../../macros.rst

.. _HARDWARE_DESIGN_RESOURCES:

================
Design Resources
================

The hardware design packages for the |BMS-Master|, the |BMS-Extension|, the |BMS-Interface| and the |BMS-Slaves| are available in the common `GitHub repository <https://github.com/foxBMS/foxbms-1>`_. The packages include:

* Altium Designer Source Files

    * schematics
    * layout
    * active bill of materials

* Assembly files

    * BOM in Excel format
    * 3D model of PCB in step format
    * schematics in PDF format

* Fabrication files

    * PCB manufacturing data in ODB++ format
    * PCB layer stack in PDF format

.. note::
    To open the schematic and layout files, please use `Altium Designer <https://www.altium.com/altium-designer/de>`_.


.. note::
    To manufacture the printed circuit boards, the BOM (Microsoft Excel file) and the ODB++ files in each corresponding folder should be used. Before sending the board layout to a PCB manufacturer, the layout files must be checked against the design rules provided by this manufacturer, since some board layout settings may depend on its specific design rules and may cause violations (e.g., pad layout).
