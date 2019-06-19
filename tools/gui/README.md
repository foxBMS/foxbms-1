# foxBMS GUI

The script ``foxbms_interface.py`` is used to visualize the data from foxBMS
(e.g., voltages, temperatures current, errors).

It was written in a modular way, so that the only reference to the driver
of the CAN adapter is in the function ``receive_send_can_adapter()``.
Throughout the rest of the code, instances of the class can_message() are
used to manipulate CAN frames (ID, DLC, data).

The interface needs ``matplotlib``. Within the foxconda environment, this
can be done by using the command

```sh
C:\foxconda3\Scripts\activate
conda install matplotlib
```

The default version was configured to work with USB CAN-adapter from
PEAK (information found on the [peak-system.com](https://www.peak-system.com)
website). Two external elements are then needed in addition to the interface
script:

* PCAN-adapter DLL
* Python-wrapper for PCAN, named ``PCANBasic.py``

These files are redistributed in the foxBMS repository.
Before using them, you have to read the file ``readme.txt`` which gives
the terms of use.

The redistributed DLL is the 64 bit version.

The frame ``AdapterSelectionFrame()`` was used to implement the choice of the
CAN-adapter to use. It was configured to write with the PCAN-adapters.
In case another adapter is used, the frame has to be modified accordingly.
