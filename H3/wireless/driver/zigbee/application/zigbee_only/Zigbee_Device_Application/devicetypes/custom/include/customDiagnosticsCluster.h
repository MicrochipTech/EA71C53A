#ifndef _CUSTOMDIAGNOSTICSCLUSTER_H
#define _CUSTOMDIAGNOSTICSCLUSTER_H
/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl/include/zclDiagnosticsCluster.h>
/******************************************************************************
                    Defines section
******************************************************************************/
/******************************************************************************
                    Types section
******************************************************************************/
/******************************************************************************
                    External variables section
******************************************************************************/
extern ZCL_DiagnosticsClusterServerAttributes_t customDiagnosticsClusterServerAttributes;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Diagnostics cluster
******************************************************************************/
void customDiagnosticsClusterInit(void);

#endif // _CUSTOMDIAGNOSTICSCLUSTER_H