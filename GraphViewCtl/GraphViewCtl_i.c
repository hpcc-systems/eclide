

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Aug 10 10:24:00 2011
 */
/* Compiler settings for GraphViewCtl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IUniqueID,0x429DC998,0x0CED,0x449C,0xA2,0x37,0x73,0x98,0x10,0x99,0x0F,0x12);


MIDL_DEFINE_GUID(IID, IID_ICommon,0x52590230,0x99FB,0x46F6,0x9B,0xBD,0x58,0xB6,0x43,0xEA,0x18,0xF8);


MIDL_DEFINE_GUID(IID, IID_IVertex,0x49055A2B,0x68B5,0x4C77,0xA3,0x66,0x7C,0xBC,0x49,0xE0,0x1D,0x2A);


MIDL_DEFINE_GUID(IID, IID_IEdge,0x95F0BCB6,0xAA9F,0x4554,0xB9,0xCB,0x58,0x9E,0x66,0xB2,0x43,0x5D);


MIDL_DEFINE_GUID(IID, IID_ISubgraph,0xEF1EA17B,0xC0E1,0x4e2e,0xA9,0xB2,0x14,0xB5,0x12,0x0E,0x82,0xFF);


MIDL_DEFINE_GUID(IID, IID_IHourglass,0x99CF7B85,0xC2CE,0x41E3,0xB6,0xC4,0xE8,0xE5,0xC4,0xC9,0x2B,0x85);


MIDL_DEFINE_GUID(IID, IID_IMetaDataProperties,0x359453BC,0x9763,0x428D,0x9F,0x7E,0x7D,0x58,0xB4,0xB0,0xA4,0x54);


MIDL_DEFINE_GUID(IID, IID_IVertices,0xF4EA0FF6,0x4005,0x4cf6,0x86,0x71,0x7F,0x5C,0xDD,0xFF,0xD0,0xC5);


MIDL_DEFINE_GUID(IID, IID_IGraphViewControl,0xAEF18F28,0xD5BA,0x42EB,0x87,0xA5,0x7A,0x4E,0xC7,0xAC,0x80,0x32);


MIDL_DEFINE_GUID(IID, LIBID_GraphViewControlLib,0xB7729748,0xC0E5,0x45B5,0xA2,0x9D,0xB8,0x9A,0xD5,0xC9,0xFE,0x81);


MIDL_DEFINE_GUID(IID, DIID_DSeisintGraphControlEvents,0x5E48FEE6,0x3244,0x44EF,0xB6,0xC0,0x94,0x15,0x67,0x1E,0x2A,0x13);


MIDL_DEFINE_GUID(CLSID, CLSID_GraphViewControl,0xF1EA17CB,0xF7BD,0x4108,0xA7,0x42,0x1B,0xC7,0x77,0x43,0x83,0xFF);


MIDL_DEFINE_GUID(CLSID, CLSID_Hourglass,0xB067BDCD,0xB1A6,0x4790,0x82,0xB1,0xD8,0x43,0xB7,0x80,0xBF,0x15);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



