# foxBMS GUI

The script foxbms_interface.py is used to visualize the data from foxBMS
(e.g., voltages, temperatures current, errors).

It was written in a modular way, so that the only reference to the driver
of the CAN adapter is in the function receive_send_can_adapter().
Throughout the rest of the code, instances of the class can_message() are
used to manipulate CAN frames (ID, DLC, data).

The interface needs ``matplotlib``. Within the foxconda environment, this
can be done by using the command

```sh
C:\foxconda3\Scripts\activate
conda install matplotlib
```

The default version was configured to work with USB CAN-adapter from
PEAK (information found on the [peak-system.com](https://www.peak-system.com) website).
Two eternal elements are then needed in addition to the interface script:

* PCAN-adapter dll, found at [https://www.peak-system.com/Software-APIs.305.0.html](https://www.peak-system.com/Software-APIs.305.0.html)
* Python-wrapper for PCAN, named ``PCANBasic.py``, found at [https://github.com/rberkow/python-can/blob/master/can/interfaces/PCANBasic.py](https://github.com/rberkow/python-can/blob/master/can/interfaces/PCANBasic.py)

The dll and the wrapper must simply be copied in the working directory
where the file ``foxbms_interface.py`` is run.

The frame AdapterSelectionFrame() was used to implement the choice of the
CAN-adapter to use. It was configured to write with the PCAN-adapters.
In case another adapter is used,  the frame has to be modified accordingly.
