=============
CAN data base
=============

This directory contains the *data base CAN* (dbc) describing the CAN
messages that are issued by this version of foxBMS.

It also contains a dbf-file which is the equivalent database-format
for the free software tool BUSMASTER.

Usage of BUSMASTER
------------------

The manual of BUSMASTER can be accessed over the Help-menu in the software.
It is rather extensive and a worthy read.


Converting DBC to DBF
^^^^^^^^^^^^^^^^^^^^^

In order to convert dbc-files to dbf-files, use the builtin converter in
BUSMASTER. You can find it in Tools/Format Converter/OtherConverters. Select
the conversion type 'DBC to DBF Conversion'.
Select then the appropriate input and output files.

The logging functions of BUSMASTER are in the CAN-menu.
If you load the example from this directory, the interface and DBF-file will
already be connected. Otherwise select the PEAK-interface in 'Driver Selection'
and associate the DBF file with the 'Database'-menu.

Under 'Signal Graph' you can configure plots that can use decoded ('physical')
data. The plots are limited to 10 entries.
Signal Watch can show you decoded signal values.
With the logging menu you can select a logfile into which the messages
will be saved. When selecting the 'Simulation'-driver, you can replay those
messages inside of the software.
