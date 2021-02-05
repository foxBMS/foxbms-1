.. include:: ../../macros.rst

.. _GENERAL_INFORMATION_MONITORED_PARAMETERS:

====================
Monitored parameters
====================

    This section describes the parameters monitored by |foxbms|. The
    parameters are monitored periodically, using the |mod_diag|. A counter
    is dedicated to each parameter. Each time an error condition is detected,
    the corresponding counter is increased, otherwise it is decreased. If the
    threshold configured for the parameter is exceeded by the counter, an error
    occured and the BMS takes an appropriate action. This mechanism
    avoids false error detections, e.g. in case of short current peaks. As the
    parameters are monitored periodically via the periodic tasks, the counter
    threshold corresponds to the **response time** between error happening
    and error detection. A default **response time** of 100ms is selected for
    overcurrent events as a trade-off between peak detection and safety. It
    must be adapted to the application. |foxbms| provides three different error
    levels. The first and lowest error level is called **Maximum Operating
    Limit** (MOL), the second error level is called **Recommended Safety Limit**
    (RSL) and the third and highest error level is called **Maximum Safety
    Limit** (MSL). The first two error levels indicate that a parameter is
    reaching the limits of the recommended operating area and counter measures
    should be initiated to prevent an unwanted opening of the contactors. A
    violation of a **Maximum Safety Limit** means the safety of the system and
    the persons cannot be guaranted anymore and leads to the opening of the
    contactors. The BMS state machine consequently switches to the **Error
    State** to prevent a further closing of the contactors. The **Error State**
    can only be left if the cause of the error has been removed and the
    **Standby State** is requested. After that the BMS is back in normal
    operational mode. All warning and error flags are available on CAN messages,
    allowing the system to react accordingly to prevent hazardous situations.
    The used nomenclature in the following is, that a warning is only raised for
    information purpose but has no impact on the state of the BMS. In contrast, an
    error flag will subsequently force the BMS into  **Error State** and thus
    lead to an opening of the contactors).

------------
Battery Cell
------------

Parameter: Cell voltage
-----------------------


+---------------------+------------------+-------------------+----------------------+---------------------------------+
| Error condition     | Response time    | Cause             | Counter measures     | Actions of the BMS              |
+=====================+==================+===================+======================+=================================+
| Cell voltage >      | 100 ms           | Overcharging      | - Reduce/stop charge | - Set warning flag at violation |
| overvoltage limit   |                  | (derating error,  |   current            |   of MOL (2550mV)               |
|                     |                  | charger defect)   | - Discharge battery  | - Set warning flag at violation |
|                     |                  |                   |                      |   of RSL (2600mV)               |
|                     |                  |                   |                      | - Set error flag at violation   |
|                     |                  |                   |                      |   of MSL (2650mV) and switch to |
|                     |                  |                   |                      |   **Error State**               |
+---------------------+------------------+-------------------+----------------------+---------------------------------+
| Cell voltage <      | 100 ms           | Deep-discharge    | - Reduce/stop        | - Set warning flag at violation |
| undervoltage limit  |                  | (derating error,  |   discharge current  |   of MOL (1700mV)               |
|                     |                  | high standby/     | - Charge battery     | - Set warning flag at violation |
|                     |                  | leakage current)  |                      |   of RSL (1750mV) and           |
|                     |                  |                   |                      | - Set error flag at violation   |
|                     |                  |                   |                      |   of MSL (1780mV) and switch to |
|                     |                  |                   |                      |   **Error State**               |
|                     |                  |                   |                      | - Set error flag that deep-     |
|                     |                  |                   |                      |   discharge (below MSL) was     |
|                     |                  |                   |                      |   detected (stored in           |
|                     |                  |                   |                      |   nonvolatile memory) to        |
|                     |                  |                   |                      |   prevent closing of contactors |
|                     |                  |                   |                      |   until defect cell is replaced |
|                     |                  |                   |                      |   and reset command is received |
|                     |                  |                   |                      |   over CAN                      |
+---------------------+------------------+-------------------+----------------------+---------------------------------+

Parameter: Cell temperature
---------------------------

+------------------------+---------------+-----------------------+-------------------------+--------------------------------------------------+
| Error condition        | Response time | Cause                 | Counter measures        | Actions of the BMS                               |
+========================+===============+=======================+=========================+==================================================+
| Cell temperature >     | 500 ms        | - Current too high    | - Reduce/stop charge    | - Set warning flag at violation of MOL (35 °C)   |
| overtemperature        |               | - Ambient temperature |   current               | - Set warning flag at violation of RSL (40 °C)   |
| limit charge           |               | - Cooling defect      | - Increase cooling      | - Set error flag at violation of MSL (45 °C)     |
|                        |               |                       |                         |   and switch to **Error State**                  |
+------------------------+---------------+-----------------------+-------------------------+--------------------------------------------------+
| Cell temperature >     | 500 ms        | - Current too high    | - Reduce/stop discharge | - Set warning flag at violation of MOL (55 °C)   |
| overtemperature        |               | - Ambient temperature |   current               | - Set warning flag at violation of RSL (50 °C)   |
| limit discharge        |               | - Cooling defect      | - Increase cooling      | - Set error flag at violation of MSL (45 °C)     |
|                        |               |                       |                         |   and switch to **Error State**                  |
+------------------------+---------------+-----------------------+-------------------------+--------------------------------------------------+
| Cell temperature <     | 500 ms        | Ambient temperature   | - Reduce/stop charge    | - Set warning flag at violation of MOL (-10 °C)  |
| undertemperature       |               |                       |   current               | - Set warning flag at violation of RSL (-15 °C)  |
| limit charge           |               |                       | - Activate heating      | - Set error flag at violation of MSL (-20 °C)    |
|                        |               |                       |                         |   and switch to **Error State**                  |
+------------------------+---------------+-----------------------+-------------------------+--------------------------------------------------+
| Cell temperature <     | 500 ms        | Ambient temperature   | - Reduce/stop discharge | - Set warning flag at violation of MOL (-10 °C)  |
| undertemperature       |               |                       |   current               | - Set warning flag at violation of RSL (-15 °C)  |
| limit discharge        |               |                       | - Activate heating      | - Set error flag at violation of MSL (-20 °C)    |
|                        |               |                       |                         |   and switch to **Error State**                  |
+------------------------+---------------+-----------------------+-------------------------+--------------------------------------------------+

Parameter: Cell current
-----------------------

+---------------------------+---------------+------------------+----------------------------+-----------------------------------------------+
| Error condition           | Response time | Cause            | Counter measures           | Actions of the BMS                            |
+===========================+===============+==================+============================+===============================================+
| Cell current >            | 100 ms        | - Derating error | Reduce charge current      | - Set warning flag at violation of MOL (170A) |
| maximum                   |               | - Charger defect |                            | - Set warning flag at violation of RSL (175A) |
| charge current            |               |                  |                            | - Set error flag at violation of MSL (180A)   |
|                           |               |                  |                            |   and switch to **Error State**               |
+---------------------------+---------------+------------------+----------------------------+-----------------------------------------------+
| Cell current >            | 100 ms        | - Derating error | Reduce discharge current   | - Set warning flag at violation of MOL (170A) |
| maximum discharge current |               | - Short circuit  |                            | - Set warning flag at violation of RSL (175A) |
|                           |               |                  |                            | - Set error flag at violation of MSL (180A)   |
|                           |               |                  |                            |   and switch to **Error State**               |
+---------------------------+---------------+------------------+----------------------------+-----------------------------------------------+

------
System
------

Parameter: Battery system current
---------------------------------

+----------------------+------------------+-----------------------------+------------------+-------------------------------------------------------+
| Error condition      | Response time    | Cause                       | Counter measures | Actions of the BMS                                    |
+======================+==================+=============================+==================+=======================================================+
| Current > precharge  | no response time | Precharge resistor defect   | --               | - Set error flag that current is too                  |
| current limit        |                  |                             |                  |   high (50mA) and abort precharge                     |
|                      |                  |                             |                  |   process                                             |
|                      |                  |                             |                  | - Do not retry and switch to **Error State**          |
+----------------------+------------------+-----------------------------+------------------+-------------------------------------------------------+
| Contactors           | no response time | System failure detected     | --               | Set warning flag that contactors were operated under  |
| opened/closed        |                  |                             |                  | current (10A)                                         |
| under current        |                  |                             |                  |                                                       |
+----------------------+------------------+-----------------------------+------------------+-------------------------------------------------------+

Parameter: HV measurement
-------------------------

+----------------------------------------------+------------------+------------------------------+---------------------+-----------------------------------------+
| Error condition                              | Response time    | Cause                        | Counter measures    | Actions of the BMS                      |
+==============================================+==================+==============================+=====================+=========================================+
| Voltage difference(Vbat - Vdc,link) >        | No response time | - DC link short circuit      | - Check contactor   | - Set error flag that voltage           |
| precharge voltage limit                      |                  | - Precharge contactor defect | - DC link capacitor |   difference is too high (5000mV)       |
|                                              |                  |                              |                     |   and abort precharge process           |
|                                              |                  |                              |                     | - Do not retry and switch to            |
|                                              |                  |                              |                     |   **Error State**                       |
+----------------------------------------------+------------------+------------------------------+---------------------+-----------------------------------------+
| Fuse tripped (Vbat - Vfuse) >                | 100 ms           | Fuse tripped                 | Replace fuse        | Set error flag that tripped fuse is     |
| limit                                        |                  |                              |                     | detected (10000mV) and switch to        |
|                                              |                  |                              |                     | **Error State**                         |
+----------------------------------------------+------------------+------------------------------+---------------------+-----------------------------------------+
| Voltage difference                           | 100 ms           | Precharge contactor defect   | Check contactor     | Measurement only done when contactors   |
| (Vfuse - Vdc,link) < limit                   |                  |                              |                     | are open. Set error flag that defect    |
|                                              |                  |                              |                     | precharge contactor is detected         |
|                                              |                  |                              |                     | (2000mV), switch to **Error State**     |
+----------------------------------------------+------------------+------------------------------+---------------------+-----------------------------------------+

------------------
Contactor feedback
------------------

+----------------------------+---------------+-------------------------+-------------------+-----------------------------------------------+
| Error condition            | Response time | Cause                   | Counter measures  | Actions of the BMS                            |
+============================+===============+=========================+===================+===============================================+
| Contactor feedback         | 100 ms        | - Contactor defect      | Check contactor   | Set error flag that measured                  |
| unequal to requested state |               | - Wiring error/defecter |                   | contactor feedback is different from          |
|                            |               |                         |                   | requested state and switch to **Error State** |
+----------------------------+---------------+-------------------------+-------------------+-----------------------------------------------+

-----------------
CAN communication
-----------------

+-------------------------+---------------+-----------------------+------------------------+----------------------------------------+
| Error condition         | Response time | Cause                 | Counter measures       | Actions of the BMS                     |
+=========================+===============+=======================+========================+========================================+
| No CAN staterequest     | 1000 ms       | - VCU defect          | - Check VCU            | Set error flag if state request        |
| message (ID: 0x120) on  |               | - Wiring error/defect | - Check wiring         | message (0x108) is not received in an  |
| CAN0 received           |               |                       |                        | interval of 100ms +/- 5ms. Switch to   |
|                         |               |                       |                        | **Error State**.                       |
+-------------------------+---------------+-----------------------+------------------------+----------------------------------------+
| No CAN currentsensor    | 1000 ms       | - Current sensor      | - Check Current Sensor | Set error flag if for 200ms no new     |
| messages on             |               |   defect              | - Check wiring         | current sensor message received and    |
| CAN0 received           |               | - Wiring              |                        | switch to **Error State**              |
|                         |               |   error/defect        |                        |                                        |
+-------------------------+---------------+-----------------------+------------------------+----------------------------------------+

-------------------
Slave communication
-------------------

+-----------------+---------------+----------------+----------------------+------------------------------------------+
| Error condition | Response time | Cause          | Counter measures     | Actions of the BMS                       |
+=================+===============+================+======================+==========================================+
| Daisy-chain     | 5 ms          | - Slave defect | - Check daisy-chain  | Set error flag that daisy-chain          |
| communication   |               | - Wiring       |   connection         | communication fails and switch to        |
| not working     |               |   error/defect | - Check Slaves       | **Error State**                          |
+-----------------+---------------+----------------+----------------------+------------------------------------------+

---------------
Open wire check
---------------

+-----------------+------------------+-----------------------+-----------------------+-----------------------------------------------+
| Error condition | Response time    | Cause                 | Counter measures      | Actions of the BMS                            |
+=================+==================+=======================+=======================+===============================================+
| Cell voltage    | No response time | Wiring error/defect   | Check voltage sense   | Default behavior: Set warning flag that open  |
| open-wire       |                  |                       | wiring                | wire is detected (configurable to error flag) |
| check detects   |                  |                       |                       |                                               |
| error           |                  |                       |                       |                                               |
+-----------------+------------------+-----------------------+-----------------------+-----------------------------------------------+

------
Master
------

Parameter: MCU die temperature
------------------------------

+-----------------------+---------------+-----------------------+------------------+------------------------------------------------+
| Error condition       | Response time | Cause                 | Counter measures | Actions of the BMS                             |
+=======================+===============+=======================+==================+================================================+
| MCU die temperature > | 1000 ms       | Ambient temperature   | Reduce thermal   | Set warning flag that maximum junction         |
| maximum junction      |               |                       | stress           | temperature is violated                        |
| temperature           |               |                       |                  | (T > 105 °C, T < -40 °C)                       |
+-----------------------+---------------+-----------------------+------------------+------------------------------------------------+

Parameter: Coin cell voltage
----------------------------

+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
| Error condition      | Response time | Cause                   | Counter measures       | Actions of the BMS                       |
+======================+===============+=========================+========================+==========================================+
| Coin cell voltage <  | 1000 ms       | Coin cell discharged    | Replace coin cell if   | Set warning flag that minimum coin       |
| minimum cell voltage |               | at end of life          | voltage is below       | cell voltage threshold is violated to    |
| limit                |               |                         | threshold              | prevent loss of data (2200mV). Typical   |
|                      |               |                         |                        | lifetime of a coin cell is 2-3 years.    |
+----------------------+---------------+-------------------------+------------------------+------------------------------------------+

---------------
Software checks
---------------

Plausibility check
------------------

+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
| Error condition      | Response time | Cause                   | Counter measures       | Actions of the BMS                       |
+======================+===============+=========================+========================+==========================================+
| Measurement          | 1000 ms       | - Daisy-chain error     | - Check Slaves         | Set error flag that measurement          |
| timestamps (I,       |               | - CAN1 error            | - Check current sensor | timestamps are not updated periodically  |
| V, T) not upated     |               |                         | - Check wiring         | (200ms) and switch to **Error State**    |
+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
| Cell temperature <   | 100 ms        | T-Sensor short circuit  | - Check temperature    | - Set measurement value invalid because  |
| below measurement    |               |                         |   sensor wiring        |   measured temperature is below          |
| range                |               |                         |                        |   measurement range (T <-50 °C)          |
|                      |               |                         |                        | - Set temperature measurement warning    |
|                      |               |                         |                        |   flag                                   |
+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
| Cell temperature     | 100 ms        | T-Sensor disconnected   | - Check temperature    | - Set measurement value invalid because  |
| > above              |               |                         |   sensor wiring        |   measured temperature is above          |
| measurement          |               |                         |                        |   measurement range (Tabove > 125 °C)    |
| range                |               |                         |                        | - Set temperature measurement warning    |
| range                |               |                         |                        |   flag                                   |
+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
| Cell voltage >       | 300 ms        | - Slave defect          | - Replace defective    | - Set measurement value invalid because  |
| above measurement    |               | - Wiring error          |   part                 |   individual cell voltage > measurement  |
| range                |               |                         |                        |   range (5000mV)                         |
|                      |               |                         |                        | - Set voltage measurement warning flag   |
+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
| Cell voltage         | 300 ms        | - Slave defect          | --                     | - Set measurement value invalid because  |
| deviation from       |               | - Wiring error          |                        |   individual cell voltage deviation      |
| average cell         |               | - Cell defect           |                        |   from average cell voltage is too large |
| voltage too          |               |                         |                        |   (+/-1000mV)                            |
| large                |               |                         |                        | - Set voltage measurement warning flag   |
+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
| Difference between   | 100 ms        | - Slave defect          | - Check Slaves         | Set warning flag that deviation between  |
| pack voltage and     |               | - Wiring error          | - Check wiring         | battery pack voltage measurement of LTC  |
| LTC / current sensor |               | - Current sensor defect | - Check curent sensor  | and current sensor is too high (3000 mV) |
| measurement too high |               |                         |                        |                                          |
+----------------------+---------------+-------------------------+------------------------+------------------------------------------+
