.. include:: ../../macros.rst

.. _HARDWARE_DOCUMENTATION_SPECIFICATIONS:

==============
Specifications
==============

The following specifications must be met to ensure a safe and optimal work with the |foxbms| hardware.

----------
BMS-Master
----------

^^^^^^^^^^^^^^^^^^
Electrical Ratings
^^^^^^^^^^^^^^^^^^

==================================================  =======     =======     =======     ====
Description                                         Minimum     Typical     Maximum     Unit
==================================================  =======     =======     =======     ====
Supply Voltage DC                                   10          --          26          V
Contactor Continuous Current                        --          --          4           A
Contactor Feedback Supply Voltage                   --          5.0         --          V
Analog Input                                        --          --          3.3         V
Isolated Contacts Continuous Current                --          --          4           A
Interlock Circuit Sink Current                      --          10          --          mA
Idle Supply Current at 12V supply                   --          150         --          mA
Idle Supply Current at 24V supply                   --          110         --          mA
==================================================  =======     =======     =======     ====

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Mechanical Dimensions (BMS-Master PCB only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

==================================================  =======     =======
Description                                         Value       Unit
==================================================  =======     =======
Width                                               120         mm
Length                                              160         mm
Height                                              15          mm
Weight                                              122         g
==================================================  =======     =======

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Mechanical Dimensions (BMS-Master Unit in housing)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The BMS-Master Unit consists of BMS-Master board, BMS-Interface board and BMS-Extension board and is mounted in an aluminum housing.

==================================================  =======     =======
Description                                         Value       Unit
==================================================  =======     =======
Width                                               124         mm
Length                                              164         mm
Height                                              49.2        mm
Weight                                              785         g
==================================================  =======     =======

-----------------
BMS-Slave 12-Cell
-----------------

^^^^^^^^^^^^^^^^^^
Electrical Ratings
^^^^^^^^^^^^^^^^^^

The current consumption from the module has been measured at 43.2V module voltage, which
is equivalent to a cell-voltage of 3.6V per cell. No sense lines have been connected from
this measurment (as the impact of cell voltage sensing is neglectable on the current
consumption).

The DC supply current has been measured with a voltage of 12V and no Vbat or cells connected.

=========================================================== =======     =======     =======     ====
Description                                                 Minimum     Typical     Maximum     Unit
=========================================================== =======     =======     =======     ====
Battery Module Voltage                                      11          --          60          V
Single Battery Cell Voltage                                 0           --          5           V
Temperature Sensor Inputs                                   --          10k         --          Ω
Analog Inputs (pin headers)                                 0           --          5           V
Digital Inputs/Outputs (pin headers)                        0           --          5           V
External DC Supply (HW version 2.1.0 and above)             8           12          24          V
Current consumption: Primary in sleep, Secondary in sleep   --          9.1µ        --          A
Current consumption: Primary active, Secondary in sleep     --          11.64m      --          A
Current consumption: Primary active, Secondary active       --          22.33m      --          A
DC supply current: Primary in sleep, Secondary in sleep     --          4.3m        --          A
DC supply current: Primary active, Secondary in sleep       --          73.6m       --          A
DC supply current: Primary active, Secondary active         --          144.3m      --          A
=========================================================== =======     =======     =======     ====

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Mechanical Dimensions (PCB only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

==================================================  =======     =======
Description                                         Value       Unit
==================================================  =======     =======
Width                                               100         mm
Length                                              160         mm
Height                                              15          mm
Weight                                              83          g
==================================================  =======     =======

-----------------
BMS-Slave 18-Cell
-----------------

^^^^^^^^^^^^^^^^^^
Electrical Ratings
^^^^^^^^^^^^^^^^^^

The current consumption from the module has been measured at 64.8V module voltage, which
is equivalent to a cell-voltage of 3.6V per cell. No sense lines have been connected from
this measurment (as the impact of cell voltage sensing is neglectable on the current
consumption).

The DC supply current has been measured with a voltage of 12V and no Vbat or cells connected.

=========================================================== =======     =======     =======     ====
Description                                                 Minimum     Typical     Maximum     Unit
=========================================================== =======     =======     =======     ====
Battery Module Voltage                                      16          --          90          V
Single Battery Cell Voltage                                 0           --          5           V
Temperature Sensor Inputs                                   --          10k         --          Ω
Analog Inputs (pin headers)                                 0           --          5           V
Digital Inputs/Outputs (pin headers)                        0           --          5           V
External DC Supply                                          8           12          24          V
Current consumption: Primary in sleep, Secondary in sleep   --          13.35µ      --          A
Current consumption: Primary active, Secondary in sleep     --          11.66m      --          A
Current consumption: Primary active, Secondary active       --          22.35m      --          A
DC supply current: Primary in sleep, Secondary in sleep     --          3.3m        --          A
DC supply current: Primary active, Secondary in sleep       --          112.0m      --          A
DC supply current: Primary active, Secondary active         --          211.6m      --          A
=========================================================== =======     =======     =======     ====

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Mechanical Dimensions (PCB only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

==================================================  =======     =======
Description                                         Value       Unit
==================================================  =======     =======
Width                                               100         mm
Length                                              160         mm
Height                                              15          mm
Weight                                              88          g
==================================================  =======     =======
