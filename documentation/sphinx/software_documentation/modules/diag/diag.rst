.. include:: ../../../macros.rst

.. _DIAGNOSIS:

=========
Diagnosis
=========

.. highlight:: C

The |mod_uart| is part of the ``Engine`` layer.

The diagnose module is responsible for error handling, error reporting and
system monitoring.

Module Files
~~~~~~~~~~~~

Driver:
 - ``embedded-software\mcu-common\src\engine\diag\diag.c`` (:ref:`diagc`)
 - ``embedded-software\mcu-common\src\engine\diag\diag.h`` (:ref:`diagh`)

Driver Configuration:
 - ``embedded-software\mcu-primary\src\engine\config\diag_cfg.c``      (:ref:`diagcfgprimaryc`)
 - ``embedded-software\mcu-primary\src\engine\config\diag_cfg.h``      (:ref:`diagcfgprimaryh`)
 - ``embedded-software\mcu-primary\src\engine\config\diag_id_cfg.h``   (:ref:`diagidcfgprimaryh`)
 - ``embedded-software\mcu-secondary\src\engine\config\diag_cfg.c`` (:ref:`diagcfgsecondaryc`)
 - ``embedded-software\mcu-secondary\src\engine\config\diag_cfg.h`` (:ref:`diagcfgsecondaryh`)
 - ``embedded-software\mcu-secondary\src\engine\config\diag_id_cfg.h`` (:ref:`diagidcfgsecondaryh`)

Description
~~~~~~~~~~~

The |mod_diag| consists of 2 independent main parts, diagnosis handling and
system monitoring. When configured, reported errors are logged into the global
diagnosis memory and a callback function can be triggered.

The handler counts errors and calls a callback function when the configured
error threshold is exceeded. The callback function is called again only when
the error counter go back to zero after the error threshold was reached.

The initialization of the diagnosis module has to be done during start-up
after the diagnosis memory is available (e.g. after Backup-SRAM is accessible).
The Backup-SRAM Flag ``DIAG_DATA_IS_VALID`` indicates the data validity in
diagnosis memory.

Two types of handling are defined:

- the general handler with debounce filter and thresholds for entering and
  exiting error state
- the contactor handler which counts all switching actions and reports when
  contactors are opened while the current flowing through the battery is above
  the configured threshold.

Usage
~~~~~

Diagnosis Handling
------------------

For using the diagnosis handler for a specific check in a module, a free
diagnosis id has to be defined in ``diag_cfg.h`` and included as an additional
diagnosis channel in ``diag_cfg.c``:

.. code-block:: C

   diag_cfg.h:
   #define     DIAG_ISOMETER_ERROR                 DIAG_ID_37           // Device error, invalid measurement result

   diag_cfg.c:
   DIAG_CH_CFG_s  diag_ch_cfg[]= {
     ...
     {DIAG_ISOMETER_ERROR, DIAG_ERROR_SENSITIVITY_MID,  DIAG_RECORDING_ENABLED, DIAG_ENABLED, callbackfunction},
     ...
   };

Where error counting is needed, the diagnosis handler has to be called in the
following way:

.. code-block:: C

     if( <error detected>)
     {
          retVal = DIAG_Handler(DIAG_ISOMETER_ERROR, DIAG_EVENT_OK, 0, 0);
          if (retVAl != DIAG_HANDLER_RETURN_OK)
          {
               /* here implement local (directly) diagnosis handling */
          }
     }
     else
     {
          DIAG_Handler(DIAG_ISOMETER_ERROR, DIAG_EVENT_NOK, 0, 0);
     }


The callback function

.. code-block:: C

   /**
    * @brief  dummy callback function of diagnosis events
    */
   void callbackfunction(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
       /* Dummy function -> empty */
   }

is called when the error threshold is reached or when the counter goes back to
zero after the threshold was reached. Typically, an error flag is set or unset
in the callback function. This database entry is updated periodically in the
1ms engine task.

System Monitoring
-----------------

For using the system monitor for a specific task or function, a free monitoring
channel ID has to be defined in ``diag_cfg.h``:

.. code-block:: C

   typedef enum {
       DIAG_SYSMON_DATABASE_ID         = 0,   /*!< diag entry for database               */
       DIAG_SYSMON_SYS_ID              = 1,   /*!< diag entry for sys                    */
       DIAG_SYSMON_BMS_ID              = 2,   /*!< diag entry for bms                    */
       DIAG_SYSMON_CONT_ID             = 3,   /*!< diag entry for contactors             */
       DIAG_SYSMON_ILCK_ID             = 4,   /*!< diag entry for contactors             */
       DIAG_SYSMON_LTC_ID              = 5,   /*!< diag entry for ltc                    */
       DIAG_SYSMON_ISOGUARD_ID         = 6,   /*!< diag entry for ioguard                */
       DIAG_SYSMON_CANS_ID             = 7,   /*!< diag entry for can                    */
       DIAG_SYSMON_APPL_CYCLIC_1ms     = 8,   /*!< diag entry for application 10ms task  */
       DIAG_SYSMON_APPL_CYCLIC_10ms    = 9,   /*!< diag entry for application 10ms task  */
       DIAG_SYSMON_APPL_CYCLIC_100ms   = 10,  /*!< diag entry for application 100ms task */
       DIAG_SYSMON_MODULE_ID_MAX       = 11   /*!< end marker do not delete              */
   } DIAG_SYSMON_MODULE_ID_e;

and a new channel configured in  ``diag_cfg.c``:

.. code-block:: C

   DIAG_SYSMON_CH_CFG_s  diag_sysmon_ch_cfg[] = {
      ...
      {DIAG_SYSMON_ISOGUARD_ID,   DIAG_SYSMON_CYCLICTASK,  400, DIAG_RECORDING_ENABLED, DIAG_ENABLED, callbackfunction},
      ...
   };

In this example, a timeout of 400 ms is defined. In the corresponding task or
cyclic called function, a notification to the system monitor has to be done
within the configured timeout by passing the state value, here 0 (ok),

Example:

.. code-block:: C

   my_isoguardfunction() {
      ...
      DIAG_SysMonNotify(DIAG_SYSMON_ISOGUARD_ID,0);
      ...
   }
