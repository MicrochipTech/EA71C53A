/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_trcbps_handler.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_TRCBPS_HANDLER_H
#define _APP_TRCBPS_HANDLER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "ble_trcbps/ble_trcbps.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:
    void APP_TrcbpsEvtHandler(BLE_TRCBPS_Event_T *p_event)

  Summary:
     BLE application Transparent credit based profile server event handler.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_TrcbpsEvtHandler(BLE_TRCBPS_Event_T *p_event);

#endif /* _APP_TRCBPS_HANDLER_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

