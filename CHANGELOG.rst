=========
Changelog
=========

**Release 1.6.4**

Software:

* Toolchain:

  * none

* Bugfixes:

  * Previous timestamp in database entries is now set with the timestamp value
    from database, not from passed structure. (``database.c``)
  * Removed double-buffering feature for database as no concurrent read/write
    access to database with current software architecture is possible.
    (``database.c/.h``)
  * Fixed compile error if ``BS_SEPARATE_POWERLINES`` was set to 0.
    (``contactor_cfg.h``)
  * Fixed check of current direction. Current value was compared
    with enum value instead of define for resting current limit.
    (``batterysystem_cfg.c``)
  * Fixed initialization of variable ``ltc_balancing_feedback.value[]`` in
    ``LTC_Initialize_Database()``. It was iterated over ``BS_NR_OF_BAT_CELLS``
    elements instead of BS_NR_OF_MODULES elements. (``ltc.c``)
  * added missing includes to wscripts for building common drivers and modules.

* Enhancements:

  * Endianness of transmitted/received CAN data can now be configured. If data
    is received/transmitted as big-endian (motorola) it will be converted
    directly before transmission from little-endian to big-endian and vice
    versa after receiving data. The native endianness of the MCU (STM32F429) is
    little-endian (intel). (``cansignal.c``, ``cansignal_cfg.c/h``)
  * Removed unnecessary distinction between getter and setter callback
    functions for cansignals as this brought no advantage. (``cansignal.c``,
    ``cansignal_cfg.c/h``)
  * Simplified diagnosis module
  * COM test mode duration to use the extended set of commands is now
    automatically extended if valid command is received (``com.c``)
  * If the number of inputs configured in the variable ``ltc_voltage_input_used``
    in ``ltc_cfg.c`` does not match ``BS_NR_OF_BAT_CELLS_PER_MODULE`` in
    ``batterysystem_cfg.h``, the first LTC measurement cycle is stoppped and the
    ``SYS`` module goes in the error state. A corresponding error flag
    (``ltc_config_error``) has been added to the error table and is
    transmitted per CAN.
  * SOC is now automatically recalibrated via LUT while battery system is at
    rest. (``soc.c``)

Hardware:

* none

Documentation:

* Fixed error in pinout description for RS485 and isolated GPIOs
  (``connectors.rst``)
* Added section about monitored parameters by foxBMS to the sphinx
  documentation (``monitored_parameters.rst``)
* Fixed error that ``CR`` instead of ``LF`` shoould be used for communication
  over UART with BMS

------------------------------------------------------------------------------

**Release 1.6.3**

Software:

* Toolchain:

  * the flash process sometimes did not work correctly for the secondary MCU.
    The robustness of the `_ack` function was generally improved and flashing
    both, primary and secondary, MCU should no longer fail.
  * if foxBMS was not developed inside a git repository, building the binaries
    failed. For these cases building now succeeds and the binary is still
    runnable. This requires the define ``BUILD_ALLOW_DIRTY_STARTUP`` to be set
    to ``1`` (in ``general.h`` for both, primary and secondary MCU). Defining
    ``BUILD_ALLOW_DIRTY_STARTUP`` to ``1`` is the default setting.

* Bugfixes:

  * minimum, maximum and average SOC were identically if Coulomb counting
    feature of current sensors was used (``sox.c/h``)
  * recommended safety limit flag for undertemperature in charge direction was
    was never set. (``diag_cfg.c``)
  * diagnosis system monitoring error occured if ``ISOGUARD``-module was
    disabled (``isoguard.c``)
  * current sensor detection failed always if triggered mode was selected, as
    CAN TX messages were activated after check for current sensor (``sys.c``)

* Enhancements:

  * changed file structure to allow type definitions to be used as return
    values for static function prototypes (``cansignal_cfg.c``)

Hardware:

* none

Documentation:

* none

------------------------------------------------------------------------------

**Release 1.6.2**

Software:

* Toolchain:

  * added cpplint configuration file
  * added cppcheck configuration file
  * added flake8 configuration file
  * added busmaster project file

* Bugfixes:

  * cell voltages for module 4 for cells 12 and upwards were not transmitted
    via CAN (``cansignal_cfg.c``)
  * ISO_MeasureInsulation() did not correctly utilize DIAG_SysMonNotify(),
    which could lead to initialization errors (``isoguard.c``)
  * cell voltages and temperatures are now depicted in foxBMS-GUI if more than
    eight modules are selected (``foxbms_interface.py``)
  * compiling primary or secondary MCU binaries without COM module enabled led
    to a compile error
  * SPI chipselect pin for FPGA extension board was erroneously set in EEPROM
    module (``eepr.c``)

* Enhancements:

  * source code cleanup of interlock module (``interlock.c/h``)

Hardware:

* none

Documentation:

* fixed pinout of temperature sensor connectors X201 and X202 for 18-cell Slave
  ``v1.1.3 and above``

------------------------------------------------------------------------------

**Release 1.6.1**

Software:

* Toolchain:

  * none

* Bugfixes:

  * fixed the extension of the startup scripts (``*.S`` to ``*.s``)

* Enhancements:

  * none

Hardware:

* none

Documentation:

* none

------------------------------------------------------------------------------

**Release 1.6.0**

Software:

* Toolchain:

  * Updated to ``waf-2.0.15`` (from ``waf-2.0.14``)
  * ``size`` is now implemented as a waf-feature to speed up build times
  * foxBMS custom waf tasks displayed wrong information in the terminal about
    the running processes (e.g., ``Compiling`` instead of
    ``Creating hex file``)
  * fixed a task order constraint when building the elf file. For details see
    the updated build documentation (see section *Build Process*).
  * added a simple test, that verifies that low level drivers do not relay on
    higher level modules (e.g., FreeRTOS, database, etc.). A project can be
    tested by running ``python tools\waf build_primary_bare`` or
    ``python tools\waf build_secondary_bare`` (see section *Build Process*).
  * removed unnecessary ``run_always`` attribute to reduce build time
  * rewrote the build of libraries. Now libraries can be build independent from
    the project configuration. Libraries can then later be added to the project
    as needed with the ``configure --libs=...`` command (see section
    *Build Process*).
  * fixed ``clean_libs`` command as it did not remove all ``build_libs``
    artifacts
  * if an include directory does not exist, an error is raised
  * if a directory is included more than once, an error is raised
  * some build attributes of ``bld.env`` inside the ``wscript`` s haven been
    renamed (``__inc_hal`` to ``hal_dirs``, ``__inc_FreeRTOS`` to
    ``FreeRTOS_dirs``, ``__bld_common`` to ``common_dir``, ``__sw_dir`` to
    ``es_dir`` and ``__bld_project`` to ``mcu_dir``
  * an error is raised if the path to the foxBMS project directory contains
    whitespace
  * an error is raised if a comparison between signed and unsigned integers
    is used
  * ensured compatibility with PyYAML v5.1 by supplying a Loader-parameter
    to calls of yaml.load()
  * fixed generation of .hex file to only contain flash content
  * raise the ``switch``-warning to error in GCC
  * add a configuration for cppcheck to the repository
  * raise the ``type-limits``-warning to error in GCC
  * raise the ``double-promotion``-warning to error in GCC

* Bugfixes:

  * in the function ``LTC_RX_PECCheck()``, the LTC PEC (packet error code)
    check of the last module in the daisy-chain was overriding the PEC check
    of all preceding modules. If the PEC of the last module was correct, all
    other PECs were detected as correct, even if some errors were
    present (``ltc.c``)
  * fixed compile error, when setting define ``MEAS_TEST_CELL_SOF_LIMITS``
    to ``TRUE`` (``bms.c``)
  * some variables used to store database content at the beginning of the
    ``BMS_Trigger()`` function were defined as local variables. This could lead
    to a stack overflow if a high number of modules was configured. These
    variables have been defined as static to solve this issue (``bms.c``)
  * the ``DIAG_GeneralHandler`` has been removed. The ``DIAG_Handler`` must be
    used for all diagnosis instead, except for the diagnosis of the contactors,
    which is handled by ``DIAG_ContHandler``.
  * ``DIAG_Handler`` returned wrong value if it was called when an error has
    already been detected (``diag.c``)
  * If more cell voltages or temperatures were defined for CAN transmission
    than for measurement in the battery system, during transmission, the array
    boundaries of the local variables storing database entries were violated.
    now boundary violations are checked and default values are sent for CAN
    signals not corresponding to existing measurements. (``cansignal_cfg.c/h``)
  * fixed incorrect array offset mapping CAN1 RX messages to respective CAN1 RX
    signals (``cansignal.c``)
  * moved fuse state error handling from ``CONT`` module to ``BMS`` module to
    avoid ``CONT`` state machine switching into error state without ``BMS``
    state machine transitioning into error state
  * fixed an implicit cast to uint that prevented working protection of the
    battery cells against overdischarge and -charge below zero degrees
    (``bms.c``)
  * moved checksum feature to main ``wscript``
  * added initialization state flags to contactor-, BMS- and
    balancing-statemachine in order to fix race-conditions between these
    statemachines and the sys-statemachine
    (``sys.c/h``, ``bal.c/h``, ``bms.c/h``, ``contactor.c/h``)
  * fixed overlapping signals in dbc file for CAN message CAN_Cell_voltage_M2_0
  * fixed function ``BMS_CheckCurrent()``. If contactors opened in case of
    over-current, error flags remained set in spite of the current being
    back to zero.
  * allow for current thresholds in ``BMS_CheckCurrent()`` differing from the
    cell-limits and adaptable to both charge and normal powerline
    (``bms.c``, ``contactor.c/h``)

* Enhancements:

  * reimplemented UART COM Decoder as a non-realtime background task for easier
    use
  * removed direct register access in UART module to improve portability
    (``uart.c/h``)
  * added plausibility module to check pack voltage (``plausibility.c/h``)
  * added plausibility module to check cell voltage and cell temperature
    (``slaveplausibility.c/h``)
  * the Python wrapper and DLL needed to run the graphical user interface
    with a CAN-adapter from the company Peak are now redistributed with
    foxBMS. Before using them, the conditions in the file ``readme.txt`` in
    ``tools\gui`` must be read and accepted.
  * updated STM32 HAL to version 1.7.4 and CMSIS to version 2.6.2
  * rewrote CAN driver to work with new HAL CAN module introduced in
    HAL version 1.7.0 (``can.c/h``, ``can_cfg.c/h``)
  * updated FreeRTOS to version 10.2.0 and adapted FreeRTOSConfig.h accordingly
  * added dedicated datasheet files for EPCOS B57251V5103J060, EPCOS
    B57861S0103F045 and Vishay NTCALUG01A103G NTC sensors to calculate cell
    temperatures using either a look-up table or polynomial approximation
  * the diagnosis modules of primary and secondary were unified and moved to
    mcu-common
  * error and safe operating area flags are now written periodically (1ms) to
    prevent erroneous database operations due to concurrency effects
  * moved #define to configure current sensor response timeout from
    ``cansignal_cfg.h`` to ``batterysystem_cfg.h``
  * moved #define to select if current sensor is in cyclic or triggered mode
    from ``can_cfg.h`` to ``batterysystem_cfg.h``
  * added simple diag-function that allows to track the call period
    of system tasks
  * moved linker scripts from ``src`` to ``general\config\STM32F4xx``
  * moved FreeRTOS configuration headers from ``src\general\config`` to
    ``src\general\config\FreeRTOS``
  * removed ``MCU_0_`` and ``MCU_1_`` from the pin defines in ``io_mcu_cfg``
    to increase the readability of the drivers
  * added deep-discharge flag that gets set if the deep-discharge voltage limit
    is violated. Flag is stored in non-volatile backup SRAM and can only be
    reset with CAN debug message. This prevents closing the contactors
    before the affected cell has been replaced
  * added support for FreeRTOS runtime stats. The stats can be accessed by the
    new 'printstats' command in the COM module.
  * added state transition functions for ltc-statemachine to reduce
    code size (``ltc.c``)
  * added stack overflow-handler that can be used for debugging and detecting
    stack overflows during development
  * disabled dynamic allocation for operating system, removed heap-implementation
    and switched to static allocation for operating system components
  * information about the git repository from which the binaries are built is
    included in the binaries. The define ``BUILD_ALLOW_DIRTY_STARTUP`` has been
    added in ``general.h`` to allow or disallow the startup of the BMS in case
    of a non clean repository.
  * set error flag if current flows in spite of all contactors being open
    (``bms.c``)
  * added support for TCA6408A port expander in ``LTC`` module (write output
    pins and read input pins)
  * added decoding for up to 18 cell voltages per module in foxBMS interface

Hardware:

* removed version number from hardware file names

* Slave 12-cell v2.1.7

  * EMI layout improvements (targeting UN ECE R10 Revision 5)
  * added RC filters on NTC sensor inputs
  * replaced linear regulators for LTC6811 5V supply with DC/DC converters
  * added circuit for switching off 5V DC/DC converters in LTC sleep mode, thus
    reducing the current consumption to less than 20µA

* Interface LTC6820 v1.9.4

  * replaced connectors J500 and J501 with TE 534206-4 due to clearance issues
    in component placement

Documentation:

* added missing unit information for some CAN signals in section
  ``Communicating with foxBMS``
* added a section on how to configure ``conda`` to work behind a proxy.

------------------------------------------------------------------------------

**Release 1.5.5**

Software:

* Toolchain:

  * Updated to ``waf-2.0.14`` (from ``waf-2.0.13``)

* Bugfixes:

  * fixed UART frame error due to floating RX pin by enabling pull-up in the
    MCU
  * fixed reading wrong entry from database when checking battery system
    current against SOF limits (``bms.c``)
  * the flag SPI transmit_ongoing was reset incorrectly after SPI dummy byte
    was transmitted. This lead to invalid measured cell voltages if the
    daisy-chain was too long (i.e., more than 10 BMS-Slaves in the daisy-chain)
  * enabled simultaneous measurement of lithium-coin-cell V_bat and MCU
    temperature in ADC module
  * fixed error calculating MCU temperature in ADC module
  * balancing threshold for voltage-based balancing was set in the wrong place:
    it is now set in the function BAL_Activate_Balancing_Voltage (``bal.c``)

* Enhancements:

  * database entries are initialized with 0 to prevent undefined data if
    entries are read before valid values are written into the database
    (``database.c``)

Hardware:

* BMS-Slave 18-cell v1.1.5

  * EMI layout improvements (targeting UN ECE R10 Revision 5)
  * adapted component variations to simplify the management of component
    variations in Altium Designer
  * replaced DC/DC converter power inductor to comply with AEC-Q
  * added circuit for switching off DC/DC converters in LTC sleep mode, thus
    reducing the current consumption to less than 20µA
  * added pull-ups on GPIOs 6-9 of the LTCs (open-drain outputs) to enable them
    to be used as digital I/O

Documentation:

* BMS-Interface: fixed pin 11 in the pinout of the connectors for version 1.2.0
  and above
* Updated BMS-Slave 18-cell hardware documentation for version 1.1.5
* Updated year in copyright
* Fixed some wrong @file attributes in doxygen comments

------------------------------------------------------------------------------

**Release 1.5.4**

Software:

* Toolchain:

  * Added a Python script that implements a graphical user interface
    to communicate with foxBMS. The instructions in the README.md file
    supplied with the script must be followed.
  * Removed obsolete ``build.py`` wrapper
  * Updated to ``waf-2.0.13`` (from ``waf-2.0.12``)
  * Fixed a build error when using the ``build_all`` option

* Bugfixes:

  * In BMS module, wait time between error request to contactors and open
    request to interlock was increased. Otherwise, interlock opened before
    contactors were open. If this is the case, both contactors open at the
    same time without any delay between first and second contactor.
  * Fixed error in reading of interlock feedback.

* Enhancements:

  * none

Hardware:

* none

Documentation:

* none

------------------------------------------------------------------------------

**Release 1.5.3**

Software:

* Toolchain:

  * raised compiler warning ``[-Werror=comment]`` to error level
  * write compiler macros to header file for improved eclipse support

* Bugfixes:

  * fixed a bug, that caused the mcu temperature for primary and secondary mcu
    to be never updated.
  * fixed a bug, that caused the coin cell voltage of the primary mcu to be
    never updated.
  * rewrite of struct ``DIAG_RETURNTYPE_e``. The enumeration had
    non-consecutive numbering and potentially dangerous typo in duplicate enum
    (``DIAG_HANDLER_RETURN_ERR_OCCURED = 2`` and
    ``DIAG_HANDLER_RETURN_ERR_OCCURRED = 4``).
  * fixed a bug, that diagnosis entry for a voltage violation of the maximimum
    safety limit wrote to wrong database entry.
  * NVRAM module was compiled twice for primary mcu. Once it was compiled by
    mcu-common module and once again in mcu-primary module). Now compiled only
    once by mcu-common module as on mcu-secondary.

* Enhancements:

  * debug printing is replaced by ``printf`` for easier and more versatile
    usage
  * added additional basic math macros (e.g., LN10, PI etc.) in ``foxmath.h``
  * Fuse state is now monitored. Fuse can be placed in NORMAL and/or CHARGE
    path. Added flag to ``CAN0_MSG_SystemState_2`` message
  * added support to build and link multiple libraries
  * added warning flag if MCU die temperature is outside of operating range to
    ``CAN0_MSG_SystemState_2`` message
  * added warning flag to replace coin cell if measured coin cell voltage is
    low to ``CAN0_MSG_SystemState_2`` message
  * added daisy-chain communication error flags to ``CAN0_MSG_SystemState_2``
    message
  * added error flag if an open voltage sense wire is detected

Hardware:

* none

Documentation:

* updated library build documentation
* updated .dbc file

------------------------------------------------------------------------------

**Release 1.5.2**

Software:

* Toolchain:

  * updated to ``waf-2.0.12`` (from ``waf-2.0.11``)

* Bugfixes:

  * fixed bug that delay after SPI wake-up byte was not long enough

* Enhancements:

  * increased CPU clock frequency from 168MHz to 180MHz
  * increased SPI bitrate from 656.25kHz to 703.125kHz
  * added CAN boot message with SW-version and flash checksum (0x101)
  * CAN messages are now always sent, even if system error was detected
  * foxBMS SW-version requestable via CAN (request ID: 0x777, response ID: 0x101)
  * added insulation error flag to ``DATA_BLOCK_ERRORSTATE_s``
  * configurable behavior if contactors should be open on insulation error or not
  * separate configurable precharging for charge/discharge path possible

Hardware:

* Master v1.0.6

  * adapted CAN filter circuit for improved fault tolerance at short of CAN_L to GND or CAN_H to supply

* Extension v1.0.5

  * adapted CAN filter circuit for improved fault tolerance at short of CAN_L to GND or CAN_H to supply

Documentation:

* updated instruction for flashing primary MCU
* updated FAQ section

------------------------------------------------------------------------------

**Release 1.5.1**

Software:

* Toolchain:

  * toolchain compatible with POSIX operating systems
  * updated to ``waf-2.0.11`` (from ``waf-2.0.10``)
  * fixed missing files in eclipse workspace (CHANGELOG.rst and compiler-flags.yml)

* Bugfixes:

  * fixed bug updating BKPSRAM values to EEPROM: BKPSRAM checksum was calculated wrong

* Enhancements:

  * modules CONTACTOR, INTERLOCK and ISOGUARD can be disabled if not needed
  * selected new EEPROM M95M02 as default EEPROM (equipped on foxBMS-Master since v1.0.5)

Hardware:

* none

Documentation:

* added a section on how to build and include a library
* removed references to directory ``foxbms-setup``, as it is now simply called ``foxbms``
* removed references to script ``bootstrap.py``, as this script is no longer used

------------------------------------------------------------------------------

**Release 1.5.0**

* **foxBMS has been migrated from Python 2.7 to Python 3.6. The foxconda3 installer is found at https://iisb-foxbms.iisb.fraunhofer.de/foxbms/. foxconda3 must be installed to C:\foxconda3.**
* **EEPROM addresses on the BMS-Master were changed. Previous saved EEPROM data will be lost with new update.**
* **introduction of an improved software structure to differentiate between hardware-dependent and hardware-independent software layers**

Software:

* Toolchain:

  * switched to monolithic repository structure to simplify the versioning
  * raised compiler warning ``[-Wimplicit-function-declaration]`` to error level
  * avoid ``shell=True`` in python subprocess
  * updated python checksum script
  * updated to ``waf-2.0.10`` and renamed the waf binary to simply ``waf``

* Bugfixes:

  * fixed bug passing *mV* instead of *V* to function ``LTC_Convert_MuxVoltages_to_Temperatures()``
  * typedef ``DATA_BLOCK_ID_TYPE_e`` starts at 0x00 instead of 0x01 (renamed DATA_BLOCK_1 to DATA_BLOCK_00) for consistency
  * fixed bug in ltc module: wrote wrong values to database when using filtered mode for measuring cell voltages and temperatures
  * #define ``CONT_PRECHARGE_VOLTAGE_THRESHOLD`` used *V* instead of *mV*
  * fixed bug in function ``CAN_WakeUp()``: wrong HAL function call was corrected
  * fixed bug in diag module: did not evaluated diagnostic errors with ``DIAG_ERROR_SENSITIVITY_HIGH``


* Enhancements:

  * adapted wscripts to new restructured software architecture
  * added timestamp to MCU backup SRAM and external EEPROM entries
  * added three alarm levels (maximum operating limit, recommended safety limit, maximum safety limit)
  * enhanced voltage based balancing algorithm
  * updated .dbc file
  * added measure AllGPIO state to ltc module
  * added CAN message for pack voltage (CAN-ID: 0x1F0)
  * added algorithm module to enable future advanced algorithms
  * increased FreeRTOS heap size from 15kByte to 20kByte
  * increased stack size of 100ms application task from 512bytes to 1024bytes
  * increased size of CAN TX message buffer from 16 to 24 messages
  * added calculation of moving average values (1s, 5s, 10s, 30s and 60s) for current and power
  * database timestamp are now automatically written on DB_Write - no need to manually update timestamps anymore
  * added native matlab datatypes support
  * cleanup of ASCII conversion functions (uint to ASCII, hex to ASCII, int to ASCII)
  * added nvramhandler to automatically update non-volatile memory (i.e., external EEPROM on BMS-Master)
  * renamed various structs, variables and functions for an improved code understanding and increased readability

Hardware:

* added hardware changelogs
* ported hardware PCB design files to Altium Designer format (AutoDesk Eagle files no longer supported)
* updated hardware PCB designs: Master V1.0.5, Extension V1.0.4, Interface 1.9.3, Slave 12-cell (LTC6811-1) V2.1.5, Slave 18-cell (LTC6813-1) V1.1.3

Documentation:

* added foxbms styleguide
* fixed spelling errors
* added documentation of software architecture
* added documentation of algorithm module
* added documentation of nvramhandler
* updated isoguard documentation

------------------------------------------------------------------------------

**Release 1.1.0**

foxbms-setup(v1.0.1):

* updated build scripts
* updated waf script
* updated README.md

mcu-common(v1.1.0):

* updated license header
* seperated database entries to prevent concurrent read/write requests to the database
* updated wscripts to build specific files only for primary/secondary
* moved sdram from common repository to primary repository
* renamed database functions to ``DB_WriteBlock()`` and ``DB_ReadBlock()``
* There was a compile error when CAN0 and CAN1 are deactivated
* updated README.md

mcu-freertos(v1.1.0):

* updated license header
* updated wscripts to build specific files only for primary/secondary
* moved sdram from common repository to primary repository
* updated README.md

mcu-hal(v1.0.1):

* updated license header
* updated README.md

mcu-primary(v1.1.0):

* uses now wafs feature of variant builds
* baudrate of CAN0 and CAN1 can now be set independently
* the setup of the tasks in engine and application layer is now consistent
* updated license header
* fixed a bug in contactor module to write unnecessary often into the database which caused a high cpuload
* seperated database entries to prevent concurrent read/write requests to the database
* added support of external SDRAM using keyword ``MEM_EXT_SDRAM``
* moved sdram from common repository to primary repository
* fixed a bug that closed the interlock for a short period of time after restart even if no CAN message was received to switch to STANDBY state
* renamed database functions to ``DB_WriteBlock()`` and ``DB_ReadBlock()``
* updated README.md

mcu-secondary(v1.1.0):

* uses now wafs feature of variant builds
* the setup of the tasks in engine and application layer is now consistent
* updated license header
* seperated database entries to prevent concurrent read/write requests to the database
* renamed database functions to ``DB_WriteBlock()`` and ``DB_ReadBlock()``
* deleted unused code
* updated README.md

tools(v1.0.2):

* Updated waf
* Updated copyright
* Updated the Eclipse Project
* Updated checksum tool from gdb-based to object-copy-based toolchain
* updated README.md

documentation(v1.0.2):

* updated documentation for the build process
* updated FAQ section
* updated copyright
* updated README.md

------------------------------------------------------------------------------

**Release 1.0.1**

* updated build scripts
* updated waf script

------------------------------------------------------------------------------

**Release 1.0.0**

* renamed repository from ``foxBMS-setup`` to ``foxbms-setup``.
* Removed update functionallity
* Moved arm-none-eabi-size call as post function in build process
* added a ``.config.yaml`` file which includes a list of repositories which are
  boostrapped and their bootstrap location.

------------------------------------------------------------------------------

**Release 0.5.2**

Release notes:
We fixed a bug in the ltc driver, leading to a non-functional temperature
sensing for foxBMS Slave Hardware version 1.xx. The slave version is
configuration for the primary MCU in foxBMS-primary\src\module\config\ltc_cfg.h
by the define SLAVE_BOARD_VERSION and for the secondary MCU in
foxBMS-secondary\src\module\config\ltc_cfg.h by the define SLAVE_BOARD_VERSION.

* Set SLAVE_BOARD_VERSION to "1" if you are using version 1.xx of the foxBMS
  Slave.
* Set SLAVE_BOARD_VERSION to "2" if you are using version 2.xx of the foxBMS
  Slave. Version 2.xx is the default configuration.

Changelog:

* foxBMS-primary

  * fixed LTC temperature sensing bug

* foxBMS-secondary

  * fixed LTC temperature sensing bug

------------------------------------------------------------------------------

**Release 0.5.1**

* foxBMS-setup

  * added parameter '-u', '--update' to bootstrap.py for updating the setup
    repository.

* foxBMS-primary

  * updates for waf 1.9.13 support
  * updated module/EEPROM and migrated to module/nvmram
  * minor code adaptations and cleanup

* foxBMS-secondary

  * support for waf 1.9.13
  * minor code adaptations and cleanup

* foxbMS-tools

  * updated waf from version 1.8.12 to version 1.9.13

------------------------------------------------------------------------------

**Release 0.5.0**

A new project structure is now used by foxBMS. The documentation is no more
contained in the embedded software sources and has its own repository. FreeRTOS
and hal have their own repository, too.
A central repository called foxBMS-setup is now used. It contains
several scripts:


* bootstrap.py gets all the repositories needed to work with foxBMS
* build.py is used to compile binaries and to generate the documentation
* clean.py is used to removed the generated binaries and documentation

Release notes:

* New project structure
* Added support for external (SPI) EEPROM on the BMS-Master
* Redesign of can and cansignal module to simplify the usage
* Added support for triggered and cyclic current measurement of Isabellenhütte
  current sensor (IVT)
* Current sensor now functions by default in non-triggered modus (no
  reprogramming needed for the sensor)
* Updated and restructured complete documentation
* Restructured file and folder structure for the documentation
* Added safety and risk analysis section
* Cleaning up of non-used files in the documentation
* Consistency check and correction of the naming and wording used
* Addition of the source files (e.g., Microsoft Visio diagrams) used to
  generate the figures in the documentation
* Reformatted the licenses text formatting (no changes in the licenses
  content)
* Updated the battery junction box (BJB) section with up-to-date components
  and parameters

------------------------------------------------------------------------------

**Release 0.4.4**

The checksum tool is now automatically called when building binaries.
Therefore the command
``python tools/waf-1.8.12 configure build chksum``
is NOT longer supported. The command to build binaries with checksum support is
``python tools/waf-1.8.12 configure build``
This is the build command used in foxBMS FrontDesk, that is, FrontDesk software
is compatible with this change and now supports automatic checksum builds.

Release notes:

* Improved checksum-feature
* Updated copyright 2010 - 2017

------------------------------------------------------------------------------

**Release 0.4.3**

Starting from this version, a checksum mechanism was implemented in foxBMS. If
the checksum is active and it is not computed correctly, it will prevent the
flashed program from running. Details on deactivating the checksum can be found
in the Software FAQ, in How to use and deactivate the checksum.

Release notes:


* Important: Changed contactor configuration order in the software to match
  the labels on the front

  * Contactor 0: CONT_PLUS_MAIN
  * Contactor 1: CONT_PLUS_PRECHARGE
  * Contactor 2: CONT_MINUS_MAIN

* Fixed an bug which could cause an unintended closing of the contactors after
  recovering from error mode
* Increased stack size for the engine tasks to avoid stack overflow in some
  special conditions
* Added a note in the documentation to indicate the necessity to send a
  periodic CAN message to the BMS
* Fixed DLC of CAN message for the current sensor measurement
* Added checksum verification for the flashed binaries
* Updated linker script to allow integration of the checksum tool
* Activated debug without JTAG interface via USB

------------------------------------------------------------------------------

**Release 0.4.2**

Release notes:

* Removed schematic files from documentation, registration needed to obtain
  the files
* Added entries to the software FAQ

------------------------------------------------------------------------------

**Release 0.4.1**

Release notes:

* Corrected daisy chain connector pinout in quickstart guide
* Corrected code for contactors, to allow using contactors without feedback
* Corrected LTC code for reading balancing feedback
* Quickstart restructured, with mention of the necessity to generate the HTML
  documentation

------------------------------------------------------------------------------

**Release 0.4.0**

Beta version of foxBS that was supplied to selected partners for evaluation.

Release notes:

------------------------------------------------------------------------------

**foxBMS Hardware Change Log (deprecated)**

The hardware changelog is now included in the regular changelog (since version
1.5.0).

*foxBMS Master*

+--------+------------------------------------------------------------------------------------------------------+
| V1.0.6 | adapted CAN filter circuit for improved fault tolerance at short of CAN_L to GND or CAN_H to supply  |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.5 | schematic cleanup, improved fonts and sizes on PCB                                                   |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.4 | | introduced minor improvements to design                                                            |
|        | | replaced EEPROM with M95M02-DRMN6TP                                                                |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.3 | | ported schematics and layout to Altium Designer                                                    |
|        | | created hierarchical design                                                                        |
|        | | introduced minor improvements to design                                                            |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.2 | replaced ADuM14XX isolators by ADuM34XX                                                              |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.1 | added fuse protection on power supply input                                                          |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.0 | initial release                                                                                      |
+--------+------------------------------------------------------------------------------------------------------+

*foxBMS Extension*

+--------+------------------------------------------------------------------------------------------------------+
| V1.0.5 | adapted CAN filter circuit for improved fault tolerance at short of CAN_L to GND or CAN_H to supply  |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.4 | schematic cleanup, improved fonts and sizes on PCB                                                   |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.3 | | ported schematics and layout to Altium Designer                                                    |
|        | | created hierarchical design                                                                        |
|        | | introduced minor improvements to design                                                            |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.2 | replaced ADuM14XX isolators by ADuM34XX                                                              |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.1 | swapped input protection of isolated GPIOs                                                           |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.0 | initial release                                                                                      |
+--------+------------------------------------------------------------------------------------------------------+

*foxBMS Interface*

+--------+------------------------------------------------------------------------------------------------------+
| V1.9.4 | replaced connectors J500 and J501 with TE 534206-4 due to clearance issues in component placement    |
+--------+------------------------------------------------------------------------------------------------------+
| V1.9.3 | replace NAND-gate with SN74LVC00AQPWRQ1                                                              |
+--------+------------------------------------------------------------------------------------------------------+
| V1.9.2 | replace OR-gate with NAND-gate and add direction pin                                                 |
+--------+------------------------------------------------------------------------------------------------------+
| V1.9.1 | | rotate pinout of Daisy-Chain-Connectors in order to mirror Slave-Connectors                        |
|        | | add labels to Daisy-Chain-Connectors                                                               |
|        | | update with new layermarker                                                                        |
|        | | replace OR-gate with AEC-Q100 qualified COTS                                                       |
+--------+------------------------------------------------------------------------------------------------------+
| V1.9.0 | | update design for reverse isoSPI with second channel                                               |
|        | | port to Altium Designer                                                                            |
+--------+------------------------------------------------------------------------------------------------------+
| V1.1.0 | replaced isoSPI transformer HX1188 by HM2102                                                         |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.2 | modified connection of isoSPI transformer HX1188                                                     |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.1 | added fiducials                                                                                      |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.0 | initial release                                                                                      |
+--------+------------------------------------------------------------------------------------------------------+

*foxBMS Slave 12-cell (LTC6811-1)*

+--------+------------------------------------------------------------------------------------------------------+
| V2.1.7 | | modified component designators to be compatible with 18-cell versions                              |
+--------+------------------------------------------------------------------------------------------------------+
| V2.1.6 | | EMI improvements (layout)                                                                          |
|        | | added RC filters on NTC sensor inputs                                                              |
|        | | added DC/DC converters for 5V LTC supplies                                                         |
+--------+------------------------------------------------------------------------------------------------------+
| V2.1.5 | | Replaced Opamps, Port Expanders and Optocouplers with AEC-Q100 compliant ones                      |
|        | | Modified silkscreen texts                                                                          |
+--------+------------------------------------------------------------------------------------------------------+
| V2.1.4 | | Primary software timer is now switched on by default                                               |
|        | | added layermarker on PCB                                                                           |
+--------+------------------------------------------------------------------------------------------------------+
| V2.1.3 | replaced EOL port expander with PCF8574                                                              |
+--------+------------------------------------------------------------------------------------------------------+
| V2.1.2 | | ported schematics and layout to Altium Designer                                                    |
|        | | created hierarchical design                                                                        |
|        | | introduced minor improvements to design                                                            |
+--------+------------------------------------------------------------------------------------------------------+
| V2.1.1 | improved isolation distances between external DC/DC converter supply and battery module signals      |
+--------+------------------------------------------------------------------------------------------------------+
| V2.1.0 | added DC/DC converter for external power supply                                                      |
+--------+------------------------------------------------------------------------------------------------------+
| V2.0.3 | fixed isoSPI transformer CMC issue                                                                   |
+--------+------------------------------------------------------------------------------------------------------+
| V2.0.2 | | replaced LTC1380 MUXs with ADG728 (400 kHz I2C)                                                    |
|        | | adjusted connection of 100 ohm resistors for V+/V_REG supply                                       |
|        | | reduced value of I2C pullups to 1k2                                                                |
+--------+------------------------------------------------------------------------------------------------------+
| V2.0.1 | | added missing cooling areas on bottom side, adjusted silk screen                                   |
|        | | enlarged PCB tracks, R201/202/301/302 other package                                                |
|        | | enlarged T201/301 cooling area                                                                     |
|        | | Replaced PCF8574 with PCA8574 (400 kHz I2C)                                                        |
+--------+------------------------------------------------------------------------------------------------------+
| V2.0.0 | initial release                                                                                      |
+--------+------------------------------------------------------------------------------------------------------+

*foxBMS Slave 18-cell (LTC6813-1)*

+--------+------------------------------------------------------------------------------------------------------+
| V1.1.5 | | EMI layout improvements                                                                            |
|        | | adapted component variants to other changes                                                        |
+--------+------------------------------------------------------------------------------------------------------+
| V1.1.4 | | replaced DC/DC converter power inductor with AEC-Q compliant one                                   |
|        | | added circuit for switching off DC/DC converters in LTC sleep mode                                 |
|        | | added pull-ups on all GPIOs of the LTCs                                                            |
+--------+------------------------------------------------------------------------------------------------------+
| V1.1.3 | schematic cleanup, improved fonts and sizes on PCB                                                   |
+--------+------------------------------------------------------------------------------------------------------+
| V1.1.2 | | replaced ACPL-247 with ACPL-217 optocoupler in order to be able to use automotive components       |
+--------+------------------------------------------------------------------------------------------------------+
| V1.1.1 | | replaced port expander with TCA6408APWR (automotive)                                               |
|        | | replaced analog buffer opamp with AD8628ARTZ-R2 (automotive)                                       |
|        | | replaced DC/DC buck controller with LM5161QPWPRQ1 (automotive)                                     |
+--------+------------------------------------------------------------------------------------------------------+
| V1.1.0 | | ported schematics and layout to Altium Designer                                                    |
|        | | created hierarchical design                                                                        |
|        | | introduced minor improvements to design                                                            |
|        | | replaced linear regulation (PNP transistor) for LTC power supply with DC/DC converters             |
|        | | improved isolation distances between external DC/DC converter supply and battery module signals    |
|        | | added 8-24 V isolated external power supply                                                        |
|        | | replaced I2C EEPROM 24AA02UID with M24M02-DR (ECC)                                                 |
|        | | replaced isoSPI transformers HX1188NL with HM2102NL                                                |
|        | | reduced balancing resistors from 2x 68 Ohm to 2x 130 Ohm due to shrinked cooling areas             |
|        | | added layermarker on PCB                                                                           |
|        | | Primary discharge timer is now switched on by default                                              |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.1 | replaced all LTC1380 MUXs with ADG728 MUXs                                                           |
+--------+------------------------------------------------------------------------------------------------------+
| V1.0.0 | initial release                                                                                      |
+--------+------------------------------------------------------------------------------------------------------+
