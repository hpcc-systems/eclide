// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the COMMS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// COMMS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef SEISINT_LIBEXPORTS
#  define COMMS_API
#else
#  ifdef COMMS_EXPORTS
#	 define COMMS_API __declspec(dllexport)
#  else
#    define COMMS_API __declspec(dllimport)
#  endif
#endif // SEISINT_LIBEXPORTS

#include <global.h> //clib

extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_IP; //(SectionLabel(_T("Server"), _T("ServerIP")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_SSL; //(SectionLabel(_T("Server"), _T("SSL")), false);
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_ADVANCED; //(SectionLabel(_T("Server"), _T("Advanced")), false);

extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_ATTRIBUTE; //(SectionLabel(_T("Server"), _T("Attribute")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_TOPOLOGY; //(SectionLabel(_T("Server"), _T("Topology")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_WORKUNIT; //(SectionLabel(_T("Server"), _T("Workunit")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_ACCOUNT; //(SectionLabel(_T("Server"), _T("Account")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_SMC; //(SectionLabel(_T("Server"), _T("SMC")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_FILESPRAY; //(SectionLabel(_T("Server"), _T("FileSpray")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_DFU; //(SectionLabel(_T("Server"), _T("DFU")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_SERVER_ANALYSIS;
extern COMMS_API const SectionLabelDefault GLOBAL_ACTION_CLASSIFY;
extern COMMS_API const SectionLabelDefault GLOBAL_ACTION_RESOLVE;
extern COMMS_API const SectionLabelDefault GLOBAL_ACTION_ENHANCE;

extern COMMS_API const SectionLabelDefault GLOBAL_CLUSTER; //(SectionLabel(_T("Server"), _T("Cluster")), _T("hthor"));
extern COMMS_API const SectionLabelDefault GLOBAL_GROUP; //(SectionLabel(_T("Server"), _T("Cluster")), _T("hthor"));
extern COMMS_API const SectionLabelDefault GLOBAL_QUEUE; //(SectionLabel(_T("Server"), _T("Queue")), _T(""));
extern COMMS_API const SectionLabelDefault GLOBAL_WORKUNIT_RESULTLIMIT; //(SectionLabel(_T("Workunit"), _T("ResultLimit")), 100);

COMMS_API bool VerifyCommsServerConfig(); //if false, force them to prefs
// only needed in static build
void CommsInitialize();

COMMS_API void ReleaseAllSingletons(); 
