/* This file generated automatically. */ 
/* Do not modify. */ 
#include "udf.h" 
#include "prop.h" 
#include "dpm.h" 
extern DEFINE_SOURCE(direct_source_no1, c, t, dS, eqn);
extern DEFINE_DIFFUSIVITY(diffu_coef,c,t,i);
extern DEFINE_ON_DEMAND(python_udf_socket);
__declspec(dllexport) UDF_Data udf_data[] = { 
{"direct_source_no1", (void (*)(void))direct_source_no1, UDF_TYPE_SOURCE},
{"diffu_coef", (void (*)(void))diffu_coef, UDF_TYPE_DIFFUSIVITY},
{"python_udf_socket", (void (*)(void))python_udf_socket, UDF_TYPE_ON_DEMAND},
}; 
__declspec(dllexport) int n_udf_data = sizeof(udf_data)/sizeof(UDF_Data); 
#include "version.h" 
__declspec(dllexport) void UDF_Inquire_Release(int *major, int *minor, int *revision) 
{ 
*major = RampantReleaseMajor; 
*minor = RampantReleaseMinor; 
*revision = RampantReleaseRevision; 
} 
