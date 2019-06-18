

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __GraphViewCtl_h__
#define __GraphViewCtl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUniqueID_FWD_DEFINED__
#define __IUniqueID_FWD_DEFINED__
typedef interface IUniqueID IUniqueID;
#endif 	/* __IUniqueID_FWD_DEFINED__ */


#ifndef __ICommon_FWD_DEFINED__
#define __ICommon_FWD_DEFINED__
typedef interface ICommon ICommon;
#endif 	/* __ICommon_FWD_DEFINED__ */


#ifndef __IVertex_FWD_DEFINED__
#define __IVertex_FWD_DEFINED__
typedef interface IVertex IVertex;
#endif 	/* __IVertex_FWD_DEFINED__ */


#ifndef __IEdge_FWD_DEFINED__
#define __IEdge_FWD_DEFINED__
typedef interface IEdge IEdge;
#endif 	/* __IEdge_FWD_DEFINED__ */


#ifndef __ISubgraph_FWD_DEFINED__
#define __ISubgraph_FWD_DEFINED__
typedef interface ISubgraph ISubgraph;
#endif 	/* __ISubgraph_FWD_DEFINED__ */


#ifndef __IHourglass_FWD_DEFINED__
#define __IHourglass_FWD_DEFINED__
typedef interface IHourglass IHourglass;
#endif 	/* __IHourglass_FWD_DEFINED__ */


#ifndef __IMetaDataProperties_FWD_DEFINED__
#define __IMetaDataProperties_FWD_DEFINED__
typedef interface IMetaDataProperties IMetaDataProperties;
#endif 	/* __IMetaDataProperties_FWD_DEFINED__ */


#ifndef __IVertices_FWD_DEFINED__
#define __IVertices_FWD_DEFINED__
typedef interface IVertices IVertices;
#endif 	/* __IVertices_FWD_DEFINED__ */


#ifndef __IGraphViewControl_FWD_DEFINED__
#define __IGraphViewControl_FWD_DEFINED__
typedef interface IGraphViewControl IGraphViewControl;
#endif 	/* __IGraphViewControl_FWD_DEFINED__ */


#ifndef __DSeisintGraphControlEvents_FWD_DEFINED__
#define __DSeisintGraphControlEvents_FWD_DEFINED__
typedef interface DSeisintGraphControlEvents DSeisintGraphControlEvents;
#endif 	/* __DSeisintGraphControlEvents_FWD_DEFINED__ */


#ifndef __GraphViewControl_FWD_DEFINED__
#define __GraphViewControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class GraphViewControl GraphViewControl;
#else
typedef struct GraphViewControl GraphViewControl;
#endif /* __cplusplus */

#endif 	/* __GraphViewControl_FWD_DEFINED__ */


#ifndef __Hourglass_FWD_DEFINED__
#define __Hourglass_FWD_DEFINED__

#ifdef __cplusplus
typedef class Hourglass Hourglass;
#else
typedef struct Hourglass Hourglass;
#endif /* __cplusplus */

#endif 	/* __Hourglass_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "mshtml.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_GraphViewCtl_0000_0000 */
/* [local] */ 

typedef /* [helpstring] */ 
enum LayoutType
    {	LayoutSpring	= 0,
    LayoutCircle	= ( LayoutSpring + 1 ) ,
    LayoutNeato	= ( LayoutCircle + 1 ) ,
    LayoutTwoPi	= ( LayoutNeato + 1 ) ,
    LayoutDot	= ( LayoutTwoPi + 1 ) ,
    LayoutCirco	= ( LayoutDot + 1 ) ,
    LayoutNone	= ( LayoutCirco + 1 ) ,
    LayoutFDR	= ( LayoutNone + 1 ) ,
    LayoutBoostSpring	= ( LayoutFDR + 1 ) ,
    LayoutLee	= ( LayoutBoostSpring + 1 ) ,
    LayoutSplines	= ( LayoutLee + 1 ) 
    } 	LayoutType;

typedef /* [helpstring] */ 
enum ImagePosition
    {	PositionNorthWest	= 0,
    PositionNorth	= ( PositionNorthWest + 1 ) ,
    PositionNorthEast	= ( PositionNorth + 1 ) ,
    PositionWest	= ( PositionNorthEast + 1 ) ,
    PositionCenter	= ( PositionWest + 1 ) ,
    PositionEast	= ( PositionCenter + 1 ) ,
    PositionSouthWest	= ( PositionEast + 1 ) ,
    PositionSouth	= ( PositionSouthWest + 1 ) ,
    PositionSouthEast	= ( PositionSouth + 1 ) 
    } 	ImagePosition;

typedef /* [helpstring] */ 
enum MouseModes
    {	MouseModeGrab	= 0,
    MouseModeSelection	= ( MouseModeGrab + 1 ) ,
    MouseModeDrawLinks	= ( MouseModeSelection + 1 ) 
    } 	MouseModes;

typedef /* [helpstring] */ 
enum DISPID_DSeisintGraphControlEvents
    {	DISPID_EdgeContextMenu	= 1,
    DISPID_EdgeDoubleClick	= ( DISPID_EdgeContextMenu + 1 ) ,
    DISPID_GraphContextMenu	= ( DISPID_EdgeDoubleClick + 1 ) ,
    DISPID_GraphDoubleClick	= ( DISPID_GraphContextMenu + 1 ) ,
    DISPID_VertexContextMenu	= ( DISPID_GraphDoubleClick + 1 ) ,
    DISPID_VertexDoubleClick	= ( DISPID_VertexContextMenu + 1 ) ,
    DISPID_VertexSingleClick	= ( DISPID_VertexDoubleClick + 1 ) ,
    DISPID_VertexMouseOver	= ( DISPID_VertexSingleClick + 1 ) ,
    DISPID_GraphMouseOver	= ( DISPID_VertexMouseOver + 1 ) ,
    DISPID_MetaDataLoaded	= ( DISPID_GraphMouseOver + 1 ) ,
    DISPID_LayoutFinished	= ( DISPID_MetaDataLoaded + 1 ) ,
    DISPID_SubgraphMinClick	= ( DISPID_LayoutFinished + 1 ) 
    } 	DISPID_DSeisintGraphControlEvents;

typedef /* [helpstring] */ 
enum AppStyle
    {	AppStyleRelavint	= 0,
    AppStyleGrETL	= ( AppStyleRelavint + 1 ) ,
    AppStyleQBGraph	= ( AppStyleGrETL + 1 ) 
    } 	AppStyle;



extern RPC_IF_HANDLE __MIDL_itf_GraphViewCtl_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_GraphViewCtl_0000_0000_v0_0_s_ifspec;

#ifndef __IUniqueID_INTERFACE_DEFINED__
#define __IUniqueID_INTERFACE_DEFINED__

/* interface IUniqueID */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUniqueID;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("429DC998-0CED-449C-A237-739810990F12")
    IUniqueID : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Category( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Identity( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUniqueIDVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUniqueID * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUniqueID * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUniqueID * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUniqueID * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUniqueID * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUniqueID * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUniqueID * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Category )( 
            IUniqueID * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Identity )( 
            IUniqueID * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IUniqueIDVtbl;

    interface IUniqueID
    {
        CONST_VTBL struct IUniqueIDVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUniqueID_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUniqueID_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUniqueID_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUniqueID_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUniqueID_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUniqueID_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUniqueID_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUniqueID_get_Category(This,pVal)	\
    ( (This)->lpVtbl -> get_Category(This,pVal) ) 

#define IUniqueID_get_Identity(This,pVal)	\
    ( (This)->lpVtbl -> get_Identity(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUniqueID_INTERFACE_DEFINED__ */


#ifndef __ICommon_INTERFACE_DEFINED__
#define __ICommon_INTERFACE_DEFINED__

/* interface ICommon */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICommon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("52590230-99FB-46F6-9BBD-58B643EA18F8")
    ICommon : public IUniqueID
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Label( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ long key,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetProperty( 
            /* [in] */ long key,
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetImage( 
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos = PositionWest,
            /* [defaultvalue][in] */ long width = 0,
            /* [defaultvalue][in] */ long height = 0,
            /* [defaultvalue][in] */ long marginX = 0,
            /* [defaultvalue][in] */ long marginY = 0,
            /* [defaultvalue][in] */ long overlay = 0) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetImageVisible( 
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetImageVisible( 
            /* [in] */ ImagePosition position,
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetHtmlElement( 
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos = PositionWest,
            /* [defaultvalue][in] */ long width = 0,
            /* [defaultvalue][in] */ long height = 0,
            /* [defaultvalue][in] */ long marginX = 0,
            /* [defaultvalue][in] */ long marginY = 0,
            /* [defaultvalue][in] */ long overlay = 0) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICommonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommon * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommon * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommon * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICommon * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICommon * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICommon * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICommon * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Category )( 
            ICommon * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Identity )( 
            ICommon * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Label )( 
            ICommon * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Label )( 
            ICommon * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            ICommon * This,
            /* [in] */ long key,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            ICommon * This,
            /* [in] */ long key,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            ICommon * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            ICommon * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            ICommon * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImageVisible )( 
            ICommon * This,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetImageVisible )( 
            ICommon * This,
            /* [in] */ ImagePosition position,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetHtmlElement )( 
            ICommon * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        END_INTERFACE
    } ICommonVtbl;

    interface ICommon
    {
        CONST_VTBL struct ICommonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommon_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommon_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommon_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommon_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICommon_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICommon_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICommon_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICommon_get_Category(This,pVal)	\
    ( (This)->lpVtbl -> get_Category(This,pVal) ) 

#define ICommon_get_Identity(This,pVal)	\
    ( (This)->lpVtbl -> get_Identity(This,pVal) ) 


#define ICommon_get_Label(This,pVal)	\
    ( (This)->lpVtbl -> get_Label(This,pVal) ) 

#define ICommon_put_Label(This,newVal)	\
    ( (This)->lpVtbl -> put_Label(This,newVal) ) 

#define ICommon_GetProperty(This,key,pVal)	\
    ( (This)->lpVtbl -> GetProperty(This,key,pVal) ) 

#define ICommon_SetProperty(This,key,newVal)	\
    ( (This)->lpVtbl -> SetProperty(This,key,newVal) ) 

#define ICommon_put_Visible(This,newVal)	\
    ( (This)->lpVtbl -> put_Visible(This,newVal) ) 

#define ICommon_get_Visible(This,pVal)	\
    ( (This)->lpVtbl -> get_Visible(This,pVal) ) 

#define ICommon_SetImage(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetImage(This,element,pos,width,height,marginX,marginY,overlay) ) 

#define ICommon_SetImageVisible(This,position,newVal)	\
    ( (This)->lpVtbl -> SetImageVisible(This,position,newVal) ) 

#define ICommon_GetImageVisible(This,position,pVal)	\
    ( (This)->lpVtbl -> GetImageVisible(This,position,pVal) ) 

#define ICommon_SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICommon_INTERFACE_DEFINED__ */


#ifndef __IVertex_INTERFACE_DEFINED__
#define __IVertex_INTERFACE_DEFINED__

/* interface IVertex */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVertex;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49055A2B-68B5-4C77-A366-7CBC49E01D2A")
    IVertex : public ICommon
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetLinkedVertexVisibilityByEdgeCat( 
            /* [in] */ long edgeCategory,
            /* [in] */ BOOL visible) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetLinkedVertexVisibilityByVertexCat( 
            /* [in] */ long Category,
            /* [in] */ BOOL visible) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVisibleLinkedVerticesCountByEdgeCat( 
            /* [in] */ long edgeCategory,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetHiddenLinkedVerticesCountByEdgeCat( 
            /* [in] */ long edgeCategory,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVisibleLinkedVerticesCountByVertexCat( 
            /* [in] */ long Category,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetHiddenLinkedVerticesCountByVertexCat( 
            /* [in] */ long Category,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NormalBorderColor( 
            /* [retval][out] */ ULONG *Color) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_NormalBorderColor( 
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NormalFillColor( 
            /* [retval][out] */ ULONG *Color) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_NormalFillColor( 
            /* [in] */ ULONG Color) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVertexVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVertex * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVertex * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVertex * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVertex * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVertex * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVertex * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVertex * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Category )( 
            IVertex * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Identity )( 
            IVertex * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Label )( 
            IVertex * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Label )( 
            IVertex * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IVertex * This,
            /* [in] */ long key,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IVertex * This,
            /* [in] */ long key,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IVertex * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IVertex * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            IVertex * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImageVisible )( 
            IVertex * This,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetImageVisible )( 
            IVertex * This,
            /* [in] */ ImagePosition position,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetHtmlElement )( 
            IVertex * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetLinkedVertexVisibilityByEdgeCat )( 
            IVertex * This,
            /* [in] */ long edgeCategory,
            /* [in] */ BOOL visible);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetLinkedVertexVisibilityByVertexCat )( 
            IVertex * This,
            /* [in] */ long Category,
            /* [in] */ BOOL visible);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVisibleLinkedVerticesCountByEdgeCat )( 
            IVertex * This,
            /* [in] */ long edgeCategory,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetHiddenLinkedVerticesCountByEdgeCat )( 
            IVertex * This,
            /* [in] */ long edgeCategory,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVisibleLinkedVerticesCountByVertexCat )( 
            IVertex * This,
            /* [in] */ long Category,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetHiddenLinkedVerticesCountByVertexCat )( 
            IVertex * This,
            /* [in] */ long Category,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_X )( 
            IVertex * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Y )( 
            IVertex * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NormalBorderColor )( 
            IVertex * This,
            /* [retval][out] */ ULONG *Color);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_NormalBorderColor )( 
            IVertex * This,
            /* [in] */ ULONG Color);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NormalFillColor )( 
            IVertex * This,
            /* [retval][out] */ ULONG *Color);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_NormalFillColor )( 
            IVertex * This,
            /* [in] */ ULONG Color);
        
        END_INTERFACE
    } IVertexVtbl;

    interface IVertex
    {
        CONST_VTBL struct IVertexVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVertex_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVertex_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVertex_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVertex_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVertex_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVertex_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVertex_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVertex_get_Category(This,pVal)	\
    ( (This)->lpVtbl -> get_Category(This,pVal) ) 

#define IVertex_get_Identity(This,pVal)	\
    ( (This)->lpVtbl -> get_Identity(This,pVal) ) 


#define IVertex_get_Label(This,pVal)	\
    ( (This)->lpVtbl -> get_Label(This,pVal) ) 

#define IVertex_put_Label(This,newVal)	\
    ( (This)->lpVtbl -> put_Label(This,newVal) ) 

#define IVertex_GetProperty(This,key,pVal)	\
    ( (This)->lpVtbl -> GetProperty(This,key,pVal) ) 

#define IVertex_SetProperty(This,key,newVal)	\
    ( (This)->lpVtbl -> SetProperty(This,key,newVal) ) 

#define IVertex_put_Visible(This,newVal)	\
    ( (This)->lpVtbl -> put_Visible(This,newVal) ) 

#define IVertex_get_Visible(This,pVal)	\
    ( (This)->lpVtbl -> get_Visible(This,pVal) ) 

#define IVertex_SetImage(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetImage(This,element,pos,width,height,marginX,marginY,overlay) ) 

#define IVertex_SetImageVisible(This,position,newVal)	\
    ( (This)->lpVtbl -> SetImageVisible(This,position,newVal) ) 

#define IVertex_GetImageVisible(This,position,pVal)	\
    ( (This)->lpVtbl -> GetImageVisible(This,position,pVal) ) 

#define IVertex_SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay) ) 


#define IVertex_SetLinkedVertexVisibilityByEdgeCat(This,edgeCategory,visible)	\
    ( (This)->lpVtbl -> SetLinkedVertexVisibilityByEdgeCat(This,edgeCategory,visible) ) 

#define IVertex_SetLinkedVertexVisibilityByVertexCat(This,Category,visible)	\
    ( (This)->lpVtbl -> SetLinkedVertexVisibilityByVertexCat(This,Category,visible) ) 

#define IVertex_GetVisibleLinkedVerticesCountByEdgeCat(This,edgeCategory,pVal)	\
    ( (This)->lpVtbl -> GetVisibleLinkedVerticesCountByEdgeCat(This,edgeCategory,pVal) ) 

#define IVertex_GetHiddenLinkedVerticesCountByEdgeCat(This,edgeCategory,pVal)	\
    ( (This)->lpVtbl -> GetHiddenLinkedVerticesCountByEdgeCat(This,edgeCategory,pVal) ) 

#define IVertex_GetVisibleLinkedVerticesCountByVertexCat(This,Category,pVal)	\
    ( (This)->lpVtbl -> GetVisibleLinkedVerticesCountByVertexCat(This,Category,pVal) ) 

#define IVertex_GetHiddenLinkedVerticesCountByVertexCat(This,Category,pVal)	\
    ( (This)->lpVtbl -> GetHiddenLinkedVerticesCountByVertexCat(This,Category,pVal) ) 

#define IVertex_get_X(This,pVal)	\
    ( (This)->lpVtbl -> get_X(This,pVal) ) 

#define IVertex_get_Y(This,pVal)	\
    ( (This)->lpVtbl -> get_Y(This,pVal) ) 

#define IVertex_get_NormalBorderColor(This,Color)	\
    ( (This)->lpVtbl -> get_NormalBorderColor(This,Color) ) 

#define IVertex_put_NormalBorderColor(This,Color)	\
    ( (This)->lpVtbl -> put_NormalBorderColor(This,Color) ) 

#define IVertex_get_NormalFillColor(This,Color)	\
    ( (This)->lpVtbl -> get_NormalFillColor(This,Color) ) 

#define IVertex_put_NormalFillColor(This,Color)	\
    ( (This)->lpVtbl -> put_NormalFillColor(This,Color) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVertex_INTERFACE_DEFINED__ */


#ifndef __IEdge_INTERFACE_DEFINED__
#define __IEdge_INTERFACE_DEFINED__

/* interface IEdge */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IEdge;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("95F0BCB6-AA9F-4554-B9CB-589E66B2435D")
    IEdge : public ICommon
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Color( 
            /* [retval][out] */ OLE_COLOR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Color( 
            /* [in] */ OLE_COLOR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEdgeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEdge * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEdge * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEdge * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IEdge * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IEdge * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IEdge * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IEdge * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Category )( 
            IEdge * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Identity )( 
            IEdge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Label )( 
            IEdge * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Label )( 
            IEdge * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IEdge * This,
            /* [in] */ long key,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IEdge * This,
            /* [in] */ long key,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IEdge * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IEdge * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            IEdge * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImageVisible )( 
            IEdge * This,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetImageVisible )( 
            IEdge * This,
            /* [in] */ ImagePosition position,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetHtmlElement )( 
            IEdge * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Color )( 
            IEdge * This,
            /* [retval][out] */ OLE_COLOR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Color )( 
            IEdge * This,
            /* [in] */ OLE_COLOR newVal);
        
        END_INTERFACE
    } IEdgeVtbl;

    interface IEdge
    {
        CONST_VTBL struct IEdgeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEdge_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEdge_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEdge_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEdge_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IEdge_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IEdge_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IEdge_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IEdge_get_Category(This,pVal)	\
    ( (This)->lpVtbl -> get_Category(This,pVal) ) 

#define IEdge_get_Identity(This,pVal)	\
    ( (This)->lpVtbl -> get_Identity(This,pVal) ) 


#define IEdge_get_Label(This,pVal)	\
    ( (This)->lpVtbl -> get_Label(This,pVal) ) 

#define IEdge_put_Label(This,newVal)	\
    ( (This)->lpVtbl -> put_Label(This,newVal) ) 

#define IEdge_GetProperty(This,key,pVal)	\
    ( (This)->lpVtbl -> GetProperty(This,key,pVal) ) 

#define IEdge_SetProperty(This,key,newVal)	\
    ( (This)->lpVtbl -> SetProperty(This,key,newVal) ) 

#define IEdge_put_Visible(This,newVal)	\
    ( (This)->lpVtbl -> put_Visible(This,newVal) ) 

#define IEdge_get_Visible(This,pVal)	\
    ( (This)->lpVtbl -> get_Visible(This,pVal) ) 

#define IEdge_SetImage(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetImage(This,element,pos,width,height,marginX,marginY,overlay) ) 

#define IEdge_SetImageVisible(This,position,newVal)	\
    ( (This)->lpVtbl -> SetImageVisible(This,position,newVal) ) 

#define IEdge_GetImageVisible(This,position,pVal)	\
    ( (This)->lpVtbl -> GetImageVisible(This,position,pVal) ) 

#define IEdge_SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay) ) 


#define IEdge_get_Color(This,pVal)	\
    ( (This)->lpVtbl -> get_Color(This,pVal) ) 

#define IEdge_put_Color(This,newVal)	\
    ( (This)->lpVtbl -> put_Color(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEdge_INTERFACE_DEFINED__ */


#ifndef __ISubgraph_INTERFACE_DEFINED__
#define __ISubgraph_INTERFACE_DEFINED__

/* interface ISubgraph */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISubgraph;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF1EA17B-C0E1-4e2e-A9B2-14B5120E82FF")
    ISubgraph : public ICommon
    {
    public:
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Minimize( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Minimize( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISubgraphVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISubgraph * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISubgraph * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISubgraph * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISubgraph * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISubgraph * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISubgraph * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISubgraph * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Category )( 
            ISubgraph * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Identity )( 
            ISubgraph * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Label )( 
            ISubgraph * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Label )( 
            ISubgraph * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            ISubgraph * This,
            /* [in] */ long key,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            ISubgraph * This,
            /* [in] */ long key,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            ISubgraph * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            ISubgraph * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            ISubgraph * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetImageVisible )( 
            ISubgraph * This,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetImageVisible )( 
            ISubgraph * This,
            /* [in] */ ImagePosition position,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetHtmlElement )( 
            ISubgraph * This,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Minimize )( 
            ISubgraph * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Minimize )( 
            ISubgraph * This,
            /* [retval][out] */ BOOL *pVal);
        
        END_INTERFACE
    } ISubgraphVtbl;

    interface ISubgraph
    {
        CONST_VTBL struct ISubgraphVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISubgraph_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISubgraph_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISubgraph_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISubgraph_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISubgraph_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISubgraph_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISubgraph_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISubgraph_get_Category(This,pVal)	\
    ( (This)->lpVtbl -> get_Category(This,pVal) ) 

#define ISubgraph_get_Identity(This,pVal)	\
    ( (This)->lpVtbl -> get_Identity(This,pVal) ) 


#define ISubgraph_get_Label(This,pVal)	\
    ( (This)->lpVtbl -> get_Label(This,pVal) ) 

#define ISubgraph_put_Label(This,newVal)	\
    ( (This)->lpVtbl -> put_Label(This,newVal) ) 

#define ISubgraph_GetProperty(This,key,pVal)	\
    ( (This)->lpVtbl -> GetProperty(This,key,pVal) ) 

#define ISubgraph_SetProperty(This,key,newVal)	\
    ( (This)->lpVtbl -> SetProperty(This,key,newVal) ) 

#define ISubgraph_put_Visible(This,newVal)	\
    ( (This)->lpVtbl -> put_Visible(This,newVal) ) 

#define ISubgraph_get_Visible(This,pVal)	\
    ( (This)->lpVtbl -> get_Visible(This,pVal) ) 

#define ISubgraph_SetImage(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetImage(This,element,pos,width,height,marginX,marginY,overlay) ) 

#define ISubgraph_SetImageVisible(This,position,newVal)	\
    ( (This)->lpVtbl -> SetImageVisible(This,position,newVal) ) 

#define ISubgraph_GetImageVisible(This,position,pVal)	\
    ( (This)->lpVtbl -> GetImageVisible(This,position,pVal) ) 

#define ISubgraph_SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetHtmlElement(This,element,pos,width,height,marginX,marginY,overlay) ) 


#define ISubgraph_put_Minimize(This,newVal)	\
    ( (This)->lpVtbl -> put_Minimize(This,newVal) ) 

#define ISubgraph_get_Minimize(This,pVal)	\
    ( (This)->lpVtbl -> get_Minimize(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISubgraph_INTERFACE_DEFINED__ */


#ifndef __IHourglass_INTERFACE_DEFINED__
#define __IHourglass_INTERFACE_DEFINED__

/* interface IHourglass */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHourglass;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("99CF7B85-C2CE-41E3-B6C4-E8E5C4C92B85")
    IHourglass : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IHourglassVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHourglass * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHourglass * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHourglass * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHourglass * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHourglass * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHourglass * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHourglass * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IHourglassVtbl;

    interface IHourglass
    {
        CONST_VTBL struct IHourglassVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHourglass_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHourglass_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHourglass_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHourglass_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHourglass_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHourglass_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHourglass_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHourglass_INTERFACE_DEFINED__ */


#ifndef __IMetaDataProperties_INTERFACE_DEFINED__
#define __IMetaDataProperties_INTERFACE_DEFINED__

/* interface IMetaDataProperties */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IMetaDataProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("359453BC-9763-428D-9F7E-7D58B4B0A454")
    IMetaDataProperties : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVertexXmlName( 
            /* [in] */ long Category,
            /* [in] */ BSTR *retVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVertexXmlAnbIconFile( 
            /* [in] */ long Category,
            /* [in] */ BSTR *retVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexCategoryLabel( 
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryLabel) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexXmlData( 
            /* [in] */ long Category,
            /* [in] */ BSTR name,
            /* [in] */ BSTR anbIconFile) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexColors( 
            /* [in] */ long Category,
            /* [in] */ ULONG NormalFill,
            /* [in] */ ULONG NormalBorder,
            /* [in] */ ULONG SelectedFill,
            /* [in] */ ULONG SelectedBorder,
            /* [in] */ ULONG OverFill,
            /* [in] */ ULONG OverBorder) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexNormalBorderColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexNormalFillColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexSelectedBorderColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexSelectedFillColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexOverFillColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexOverBorderColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVerticesVisibleByEdgeCount( 
            /* [in] */ long edgeCount,
            /* [in] */ BOOL visible) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexImage( 
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos = PositionWest,
            /* [defaultvalue][in] */ long width = 0,
            /* [defaultvalue][in] */ long height = 0,
            /* [defaultvalue][in] */ long marginX = 0,
            /* [defaultvalue][in] */ long marginY = 0,
            /* [defaultvalue][in] */ long overlay = 0) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeColors( 
            /* [in] */ long Category,
            /* [in] */ ULONG Normal,
            /* [in] */ ULONG ShortestPath) = 0;
        
        virtual /* [helpstring][hidden] */ HRESULT STDMETHODCALLTYPE SetEdgeCategoryLabel( 
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryLabel) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeNormalColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeShortestPathColor( 
            /* [in] */ long Category,
            /* [in] */ ULONG Color) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeWidth( 
            /* [in] */ long Category,
            /* [in] */ short Width) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeXmlCategoryCode( 
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryCode) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeImage( 
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos = PositionWest,
            /* [defaultvalue][in] */ long width = 0,
            /* [defaultvalue][in] */ long height = 0,
            /* [defaultvalue][in] */ long marginX = 0,
            /* [defaultvalue][in] */ long marginY = 0,
            /* [defaultvalue][in] */ long overlay = 0) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVertexIconHtmlElement( 
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMetaDataPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMetaDataProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMetaDataProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMetaDataProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMetaDataProperties * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMetaDataProperties * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMetaDataProperties * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMetaDataProperties * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVertexXmlName )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ BSTR *retVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVertexXmlAnbIconFile )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ BSTR *retVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexCategoryLabel )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryLabel);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexXmlData )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ BSTR name,
            /* [in] */ BSTR anbIconFile);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexColors )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG NormalFill,
            /* [in] */ ULONG NormalBorder,
            /* [in] */ ULONG SelectedFill,
            /* [in] */ ULONG SelectedBorder,
            /* [in] */ ULONG OverFill,
            /* [in] */ ULONG OverBorder);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexNormalBorderColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexNormalFillColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexSelectedBorderColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexSelectedFillColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexOverFillColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexOverBorderColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVerticesVisibleByEdgeCount )( 
            IMetaDataProperties * This,
            /* [in] */ long edgeCount,
            /* [in] */ BOOL visible);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexImage )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeColors )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Normal,
            /* [in] */ ULONG ShortestPath);
        
        /* [helpstring][hidden] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeCategoryLabel )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryLabel);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeNormalColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeShortestPathColor )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeWidth )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ short Width);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeXmlCategoryCode )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryCode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeImage )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        HRESULT ( STDMETHODCALLTYPE *SetVertexIconHtmlElement )( 
            IMetaDataProperties * This,
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element);
        
        END_INTERFACE
    } IMetaDataPropertiesVtbl;

    interface IMetaDataProperties
    {
        CONST_VTBL struct IMetaDataPropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMetaDataProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMetaDataProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMetaDataProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMetaDataProperties_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMetaDataProperties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMetaDataProperties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMetaDataProperties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMetaDataProperties_GetVertexXmlName(This,Category,retVal)	\
    ( (This)->lpVtbl -> GetVertexXmlName(This,Category,retVal) ) 

#define IMetaDataProperties_GetVertexXmlAnbIconFile(This,Category,retVal)	\
    ( (This)->lpVtbl -> GetVertexXmlAnbIconFile(This,Category,retVal) ) 

#define IMetaDataProperties_SetVertexCategoryLabel(This,Category,CategoryLabel)	\
    ( (This)->lpVtbl -> SetVertexCategoryLabel(This,Category,CategoryLabel) ) 

#define IMetaDataProperties_SetVertexXmlData(This,Category,name,anbIconFile)	\
    ( (This)->lpVtbl -> SetVertexXmlData(This,Category,name,anbIconFile) ) 

#define IMetaDataProperties_SetVertexColors(This,Category,NormalFill,NormalBorder,SelectedFill,SelectedBorder,OverFill,OverBorder)	\
    ( (This)->lpVtbl -> SetVertexColors(This,Category,NormalFill,NormalBorder,SelectedFill,SelectedBorder,OverFill,OverBorder) ) 

#define IMetaDataProperties_SetVertexNormalBorderColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexNormalBorderColor(This,Category,Color) ) 

#define IMetaDataProperties_SetVertexNormalFillColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexNormalFillColor(This,Category,Color) ) 

#define IMetaDataProperties_SetVertexSelectedBorderColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexSelectedBorderColor(This,Category,Color) ) 

#define IMetaDataProperties_SetVertexSelectedFillColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexSelectedFillColor(This,Category,Color) ) 

#define IMetaDataProperties_SetVertexOverFillColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexOverFillColor(This,Category,Color) ) 

#define IMetaDataProperties_SetVertexOverBorderColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexOverBorderColor(This,Category,Color) ) 

#define IMetaDataProperties_SetVerticesVisibleByEdgeCount(This,edgeCount,visible)	\
    ( (This)->lpVtbl -> SetVerticesVisibleByEdgeCount(This,edgeCount,visible) ) 

#define IMetaDataProperties_SetVertexImage(This,Category,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetVertexImage(This,Category,element,pos,width,height,marginX,marginY,overlay) ) 

#define IMetaDataProperties_SetEdgeColors(This,Category,Normal,ShortestPath)	\
    ( (This)->lpVtbl -> SetEdgeColors(This,Category,Normal,ShortestPath) ) 

#define IMetaDataProperties_SetEdgeCategoryLabel(This,Category,CategoryLabel)	\
    ( (This)->lpVtbl -> SetEdgeCategoryLabel(This,Category,CategoryLabel) ) 

#define IMetaDataProperties_SetEdgeNormalColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetEdgeNormalColor(This,Category,Color) ) 

#define IMetaDataProperties_SetEdgeShortestPathColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetEdgeShortestPathColor(This,Category,Color) ) 

#define IMetaDataProperties_SetEdgeWidth(This,Category,Width)	\
    ( (This)->lpVtbl -> SetEdgeWidth(This,Category,Width) ) 

#define IMetaDataProperties_SetEdgeXmlCategoryCode(This,Category,CategoryCode)	\
    ( (This)->lpVtbl -> SetEdgeXmlCategoryCode(This,Category,CategoryCode) ) 

#define IMetaDataProperties_SetEdgeImage(This,Category,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetEdgeImage(This,Category,element,pos,width,height,marginX,marginY,overlay) ) 

#define IMetaDataProperties_SetVertexIconHtmlElement(This,Category,element)	\
    ( (This)->lpVtbl -> SetVertexIconHtmlElement(This,Category,element) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMetaDataProperties_INTERFACE_DEFINED__ */


#ifndef __IVertices_INTERFACE_DEFINED__
#define __IVertices_INTERFACE_DEFINED__

/* interface IVertices */
/* [nonextensible][unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IVertices;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F4EA0FF6-4005-4cf6-8671-7F5CDDFFD0C5")
    IVertices : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [restricted][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long lIndex,
            /* [retval][out] */ IVertex **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVerticesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVertices * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVertices * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVertices * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVertices * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVertices * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVertices * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVertices * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IVertices * This,
            /* [retval][out] */ long *pVal);
        
        /* [restricted][helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IVertices * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IVertices * This,
            /* [in] */ long lIndex,
            /* [retval][out] */ IVertex **pVal);
        
        END_INTERFACE
    } IVerticesVtbl;

    interface IVertices
    {
        CONST_VTBL struct IVerticesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVertices_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVertices_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVertices_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVertices_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVertices_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVertices_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVertices_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVertices_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IVertices_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define IVertices_get_Item(This,lIndex,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,lIndex,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVertices_INTERFACE_DEFINED__ */


#ifndef __IGraphViewControl_INTERFACE_DEFINED__
#define __IGraphViewControl_INTERFACE_DEFINED__

/* interface IGraphViewControl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IGraphViewControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AEF18F28-D5BA-42EB-87A5-7A4EC7AC8032")
    IGraphViewControl : public IMetaDataProperties
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddVertexSimple( 
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [defaultvalue][in] */ BSTR Label,
            /* [retval][out] */ IVertex **Vertex) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddVertex( 
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [defaultvalue][in] */ BSTR Label,
            /* [defaultvalue][in] */ long X,
            /* [defaultvalue][in] */ long Y,
            /* [retval][out] */ IVertex **Vertex) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddEdge( 
            /* [in] */ long Category,
            /* [in] */ IVertex *From,
            /* [in] */ IVertex *To,
            /* [defaultvalue][in] */ BSTR Label,
            /* [retval][out] */ IEdge **Edge) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVertex( 
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [retval][out] */ IVertex **Vertex) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetEdge( 
            /* [in] */ long Category,
            /* [in] */ IVertex *From,
            /* [in] */ IVertex *To,
            /* [retval][out] */ IEdge **Edge) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GenerateRandomData( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE HideCategory( 
            /* [in] */ long Category) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Recenter( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Redraw( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Rotation( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Rotation( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexVisible( 
            /* [in] */ long Category,
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetVertexImageVisible( 
            /* [in] */ long Category,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeVisible( 
            /* [in] */ long Category,
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetEdgeImageVisible( 
            /* [in] */ long Category,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShowCategory( 
            /* [in] */ long Category) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShowMenu( 
            /* [in] */ long x,
            /* [in] */ long y,
            /* [in] */ IDispatch *jsarrMenuStringNames,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShowHourglass( 
            /* [retval][out] */ IHourglass **pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Spacing( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Spacing( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StopLayout( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Layout( 
            /* [retval][out] */ LayoutType *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Layout( 
            /* [in] */ LayoutType newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MouseMode( 
            /* [retval][out] */ MouseModes *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MouseMode( 
            /* [in] */ MouseModes newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Zoom( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Zoom( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSelectedVertices( 
            /* [retval][out] */ IVertices **collection) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSelectedVertex( 
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [in] */ BOOL bSelected,
            /* [defaultvalue][in] */ BOOL bAppend = FALSE) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnsureVisible( 
            /* [in] */ LONG Category,
            /* [in] */ BSTR Identity) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ScaleToFit( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ScaleToFitSelected( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetRenderExtent( 
            /* [in] */ OLE_HANDLE hDC,
            /* [out] */ double *pX,
            /* [out] */ double *pY,
            /* [out] */ double *pWidth,
            /* [out] */ double *pHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RecenterSelected( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RecenterOnVertex( 
            long category,
            BSTR identity) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveVertex( 
            long category,
            BSTR identity) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RenderTo( 
            OLE_HANDLE hDC,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double PageX,
            /* [in] */ double PageY,
            /* [in] */ double PageWidth,
            /* [in] */ double PageHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RenderToClipboard( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Save( 
            BSTR filename) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveAs( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadXml( 
            /* [in] */ BSTR xml) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveXml( 
            /* [defaultvalue][in] */ BOOL version2,
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveXmlAs( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMeta( 
            /* [in] */ BSTR url) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetData( 
            /* [in] */ BSTR url,
            /* [in] */ LONG Category,
            /* [in] */ BSTR ID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE HideDisconnected( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE HideSingletons( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSplinesMode( 
            /* [in] */ LONG splines) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadXmlFile( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShowOverview( 
            /* [in] */ LONG show) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetOverviewOpacity( 
            /* [in] */ LONG opacity) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShowEdgeLabels( 
            /* [in] */ LONG show) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EmulateDecoration( 
            /* [in] */ BYTE inplace) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetGraphStyle( 
            /* [in] */ AppStyle style) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadXgmml( 
            /* [in] */ BSTR xgmml,
            /* [defaultvalue][in] */ LONG state = 0,
            /* [defaultvalue][in] */ BOOL minimizeIfLarge = TRUE) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE MergeXgmml( 
            /* [in] */ BSTR xgmml,
            /* [in] */ LONG state) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SelectFile( 
            /* [in] */ BSTR current,
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindVertices( 
            /* [in] */ BSTR searchstring,
            /* [defaultvalue][in] */ BOOL searchLabel,
            /* [defaultvalue][in] */ BOOL searchProperties,
            /* [retval][out] */ IVertices **collection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetApplicationZoomMode( 
            /* [in] */ LONG mode) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FindGraphItemByID( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGraphViewControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGraphViewControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGraphViewControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGraphViewControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGraphViewControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGraphViewControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGraphViewControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGraphViewControl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVertexXmlName )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR *retVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVertexXmlAnbIconFile )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR *retVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexCategoryLabel )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryLabel);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexXmlData )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR name,
            /* [in] */ BSTR anbIconFile);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexColors )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG NormalFill,
            /* [in] */ ULONG NormalBorder,
            /* [in] */ ULONG SelectedFill,
            /* [in] */ ULONG SelectedBorder,
            /* [in] */ ULONG OverFill,
            /* [in] */ ULONG OverBorder);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexNormalBorderColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexNormalFillColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexSelectedBorderColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexSelectedFillColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexOverFillColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexOverBorderColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVerticesVisibleByEdgeCount )( 
            IGraphViewControl * This,
            /* [in] */ long edgeCount,
            /* [in] */ BOOL visible);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexImage )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeColors )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Normal,
            /* [in] */ ULONG ShortestPath);
        
        /* [helpstring][hidden] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeCategoryLabel )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryLabel);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeNormalColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeShortestPathColor )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ULONG Color);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeWidth )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ short Width);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeXmlCategoryCode )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR CategoryCode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeImage )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element,
            /* [defaultvalue][in] */ ImagePosition pos,
            /* [defaultvalue][in] */ long width,
            /* [defaultvalue][in] */ long height,
            /* [defaultvalue][in] */ long marginX,
            /* [defaultvalue][in] */ long marginY,
            /* [defaultvalue][in] */ long overlay);
        
        HRESULT ( STDMETHODCALLTYPE *SetVertexIconHtmlElement )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ IHTMLElementRender *element);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddVertexSimple )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [defaultvalue][in] */ BSTR Label,
            /* [retval][out] */ IVertex **Vertex);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddVertex )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [defaultvalue][in] */ BSTR Label,
            /* [defaultvalue][in] */ long X,
            /* [defaultvalue][in] */ long Y,
            /* [retval][out] */ IVertex **Vertex);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddEdge )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ IVertex *From,
            /* [in] */ IVertex *To,
            /* [defaultvalue][in] */ BSTR Label,
            /* [retval][out] */ IEdge **Edge);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVertex )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [retval][out] */ IVertex **Vertex);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetEdge )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ IVertex *From,
            /* [in] */ IVertex *To,
            /* [retval][out] */ IEdge **Edge);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GenerateRandomData )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *HideCategory )( 
            IGraphViewControl * This,
            /* [in] */ long Category);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Recenter )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Redraw )( 
            IGraphViewControl * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Rotation )( 
            IGraphViewControl * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Rotation )( 
            IGraphViewControl * This,
            /* [in] */ long newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexVisible )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetVertexImageVisible )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeVisible )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetEdgeImageVisible )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ ImagePosition position,
            /* [in] */ BOOL newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowCategory )( 
            IGraphViewControl * This,
            /* [in] */ long Category);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowMenu )( 
            IGraphViewControl * This,
            /* [in] */ long x,
            /* [in] */ long y,
            /* [in] */ IDispatch *jsarrMenuStringNames,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowHourglass )( 
            IGraphViewControl * This,
            /* [retval][out] */ IHourglass **pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Spacing )( 
            IGraphViewControl * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Spacing )( 
            IGraphViewControl * This,
            /* [in] */ long newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *StopLayout )( 
            IGraphViewControl * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Layout )( 
            IGraphViewControl * This,
            /* [retval][out] */ LayoutType *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Layout )( 
            IGraphViewControl * This,
            /* [in] */ LayoutType newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MouseMode )( 
            IGraphViewControl * This,
            /* [retval][out] */ MouseModes *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MouseMode )( 
            IGraphViewControl * This,
            /* [in] */ MouseModes newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Zoom )( 
            IGraphViewControl * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Zoom )( 
            IGraphViewControl * This,
            /* [in] */ long newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSelectedVertices )( 
            IGraphViewControl * This,
            /* [retval][out] */ IVertices **collection);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetSelectedVertex )( 
            IGraphViewControl * This,
            /* [in] */ long Category,
            /* [in] */ BSTR Identity,
            /* [in] */ BOOL bSelected,
            /* [defaultvalue][in] */ BOOL bAppend);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnsureVisible )( 
            IGraphViewControl * This,
            /* [in] */ LONG Category,
            /* [in] */ BSTR Identity);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ScaleToFit )( 
            IGraphViewControl * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ScaleToFitSelected )( 
            IGraphViewControl * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetRenderExtent )( 
            IGraphViewControl * This,
            /* [in] */ OLE_HANDLE hDC,
            /* [out] */ double *pX,
            /* [out] */ double *pY,
            /* [out] */ double *pWidth,
            /* [out] */ double *pHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RecenterSelected )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RecenterOnVertex )( 
            IGraphViewControl * This,
            long category,
            BSTR identity);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RemoveVertex )( 
            IGraphViewControl * This,
            long category,
            BSTR identity);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RenderTo )( 
            IGraphViewControl * This,
            OLE_HANDLE hDC,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double PageX,
            /* [in] */ double PageY,
            /* [in] */ double PageWidth,
            /* [in] */ double PageHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RenderToClipboard )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IGraphViewControl * This,
            BSTR filename);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SaveAs )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadXml )( 
            IGraphViewControl * This,
            /* [in] */ BSTR xml);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SaveXml )( 
            IGraphViewControl * This,
            /* [defaultvalue][in] */ BOOL version2,
            /* [retval][out] */ BSTR *retVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SaveXmlAs )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetMeta )( 
            IGraphViewControl * This,
            /* [in] */ BSTR url);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetData )( 
            IGraphViewControl * This,
            /* [in] */ BSTR url,
            /* [in] */ LONG Category,
            /* [in] */ BSTR ID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *HideDisconnected )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *HideSingletons )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetSplinesMode )( 
            IGraphViewControl * This,
            /* [in] */ LONG splines);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadXmlFile )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowOverview )( 
            IGraphViewControl * This,
            /* [in] */ LONG show);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetOverviewOpacity )( 
            IGraphViewControl * This,
            /* [in] */ LONG opacity);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowEdgeLabels )( 
            IGraphViewControl * This,
            /* [in] */ LONG show);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EmulateDecoration )( 
            IGraphViewControl * This,
            /* [in] */ BYTE inplace);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetGraphStyle )( 
            IGraphViewControl * This,
            /* [in] */ AppStyle style);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadXgmml )( 
            IGraphViewControl * This,
            /* [in] */ BSTR xgmml,
            /* [defaultvalue][in] */ LONG state,
            /* [defaultvalue][in] */ BOOL minimizeIfLarge);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MergeXgmml )( 
            IGraphViewControl * This,
            /* [in] */ BSTR xgmml,
            /* [in] */ LONG state);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SelectFile )( 
            IGraphViewControl * This,
            /* [in] */ BSTR current,
            /* [retval][out] */ BSTR *retVal);
        
        HRESULT ( STDMETHODCALLTYPE *FindVertices )( 
            IGraphViewControl * This,
            /* [in] */ BSTR searchstring,
            /* [defaultvalue][in] */ BOOL searchLabel,
            /* [defaultvalue][in] */ BOOL searchProperties,
            /* [retval][out] */ IVertices **collection);
        
        HRESULT ( STDMETHODCALLTYPE *SetApplicationZoomMode )( 
            IGraphViewControl * This,
            /* [in] */ LONG mode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindGraphItemByID )( 
            IGraphViewControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            IGraphViewControl * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IGraphViewControlVtbl;

    interface IGraphViewControl
    {
        CONST_VTBL struct IGraphViewControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGraphViewControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGraphViewControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGraphViewControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGraphViewControl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGraphViewControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGraphViewControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGraphViewControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IGraphViewControl_GetVertexXmlName(This,Category,retVal)	\
    ( (This)->lpVtbl -> GetVertexXmlName(This,Category,retVal) ) 

#define IGraphViewControl_GetVertexXmlAnbIconFile(This,Category,retVal)	\
    ( (This)->lpVtbl -> GetVertexXmlAnbIconFile(This,Category,retVal) ) 

#define IGraphViewControl_SetVertexCategoryLabel(This,Category,CategoryLabel)	\
    ( (This)->lpVtbl -> SetVertexCategoryLabel(This,Category,CategoryLabel) ) 

#define IGraphViewControl_SetVertexXmlData(This,Category,name,anbIconFile)	\
    ( (This)->lpVtbl -> SetVertexXmlData(This,Category,name,anbIconFile) ) 

#define IGraphViewControl_SetVertexColors(This,Category,NormalFill,NormalBorder,SelectedFill,SelectedBorder,OverFill,OverBorder)	\
    ( (This)->lpVtbl -> SetVertexColors(This,Category,NormalFill,NormalBorder,SelectedFill,SelectedBorder,OverFill,OverBorder) ) 

#define IGraphViewControl_SetVertexNormalBorderColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexNormalBorderColor(This,Category,Color) ) 

#define IGraphViewControl_SetVertexNormalFillColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexNormalFillColor(This,Category,Color) ) 

#define IGraphViewControl_SetVertexSelectedBorderColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexSelectedBorderColor(This,Category,Color) ) 

#define IGraphViewControl_SetVertexSelectedFillColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexSelectedFillColor(This,Category,Color) ) 

#define IGraphViewControl_SetVertexOverFillColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexOverFillColor(This,Category,Color) ) 

#define IGraphViewControl_SetVertexOverBorderColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetVertexOverBorderColor(This,Category,Color) ) 

#define IGraphViewControl_SetVerticesVisibleByEdgeCount(This,edgeCount,visible)	\
    ( (This)->lpVtbl -> SetVerticesVisibleByEdgeCount(This,edgeCount,visible) ) 

#define IGraphViewControl_SetVertexImage(This,Category,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetVertexImage(This,Category,element,pos,width,height,marginX,marginY,overlay) ) 

#define IGraphViewControl_SetEdgeColors(This,Category,Normal,ShortestPath)	\
    ( (This)->lpVtbl -> SetEdgeColors(This,Category,Normal,ShortestPath) ) 

#define IGraphViewControl_SetEdgeCategoryLabel(This,Category,CategoryLabel)	\
    ( (This)->lpVtbl -> SetEdgeCategoryLabel(This,Category,CategoryLabel) ) 

#define IGraphViewControl_SetEdgeNormalColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetEdgeNormalColor(This,Category,Color) ) 

#define IGraphViewControl_SetEdgeShortestPathColor(This,Category,Color)	\
    ( (This)->lpVtbl -> SetEdgeShortestPathColor(This,Category,Color) ) 

#define IGraphViewControl_SetEdgeWidth(This,Category,Width)	\
    ( (This)->lpVtbl -> SetEdgeWidth(This,Category,Width) ) 

#define IGraphViewControl_SetEdgeXmlCategoryCode(This,Category,CategoryCode)	\
    ( (This)->lpVtbl -> SetEdgeXmlCategoryCode(This,Category,CategoryCode) ) 

#define IGraphViewControl_SetEdgeImage(This,Category,element,pos,width,height,marginX,marginY,overlay)	\
    ( (This)->lpVtbl -> SetEdgeImage(This,Category,element,pos,width,height,marginX,marginY,overlay) ) 

#define IGraphViewControl_SetVertexIconHtmlElement(This,Category,element)	\
    ( (This)->lpVtbl -> SetVertexIconHtmlElement(This,Category,element) ) 


#define IGraphViewControl_AddVertexSimple(This,Category,Identity,Label,Vertex)	\
    ( (This)->lpVtbl -> AddVertexSimple(This,Category,Identity,Label,Vertex) ) 

#define IGraphViewControl_AddVertex(This,Category,Identity,Label,X,Y,Vertex)	\
    ( (This)->lpVtbl -> AddVertex(This,Category,Identity,Label,X,Y,Vertex) ) 

#define IGraphViewControl_AddEdge(This,Category,From,To,Label,Edge)	\
    ( (This)->lpVtbl -> AddEdge(This,Category,From,To,Label,Edge) ) 

#define IGraphViewControl_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IGraphViewControl_GetVertex(This,Category,Identity,Vertex)	\
    ( (This)->lpVtbl -> GetVertex(This,Category,Identity,Vertex) ) 

#define IGraphViewControl_GetEdge(This,Category,From,To,Edge)	\
    ( (This)->lpVtbl -> GetEdge(This,Category,From,To,Edge) ) 

#define IGraphViewControl_GenerateRandomData(This)	\
    ( (This)->lpVtbl -> GenerateRandomData(This) ) 

#define IGraphViewControl_HideCategory(This,Category)	\
    ( (This)->lpVtbl -> HideCategory(This,Category) ) 

#define IGraphViewControl_Recenter(This)	\
    ( (This)->lpVtbl -> Recenter(This) ) 

#define IGraphViewControl_Redraw(This)	\
    ( (This)->lpVtbl -> Redraw(This) ) 

#define IGraphViewControl_get_Rotation(This,pVal)	\
    ( (This)->lpVtbl -> get_Rotation(This,pVal) ) 

#define IGraphViewControl_put_Rotation(This,newVal)	\
    ( (This)->lpVtbl -> put_Rotation(This,newVal) ) 

#define IGraphViewControl_SetVertexVisible(This,Category,newVal)	\
    ( (This)->lpVtbl -> SetVertexVisible(This,Category,newVal) ) 

#define IGraphViewControl_SetVertexImageVisible(This,Category,position,newVal)	\
    ( (This)->lpVtbl -> SetVertexImageVisible(This,Category,position,newVal) ) 

#define IGraphViewControl_SetEdgeVisible(This,Category,newVal)	\
    ( (This)->lpVtbl -> SetEdgeVisible(This,Category,newVal) ) 

#define IGraphViewControl_SetEdgeImageVisible(This,Category,position,newVal)	\
    ( (This)->lpVtbl -> SetEdgeImageVisible(This,Category,position,newVal) ) 

#define IGraphViewControl_ShowCategory(This,Category)	\
    ( (This)->lpVtbl -> ShowCategory(This,Category) ) 

#define IGraphViewControl_ShowMenu(This,x,y,jsarrMenuStringNames,pVal)	\
    ( (This)->lpVtbl -> ShowMenu(This,x,y,jsarrMenuStringNames,pVal) ) 

#define IGraphViewControl_ShowHourglass(This,pVal)	\
    ( (This)->lpVtbl -> ShowHourglass(This,pVal) ) 

#define IGraphViewControl_get_Spacing(This,pVal)	\
    ( (This)->lpVtbl -> get_Spacing(This,pVal) ) 

#define IGraphViewControl_put_Spacing(This,newVal)	\
    ( (This)->lpVtbl -> put_Spacing(This,newVal) ) 

#define IGraphViewControl_StopLayout(This)	\
    ( (This)->lpVtbl -> StopLayout(This) ) 

#define IGraphViewControl_get_Layout(This,pVal)	\
    ( (This)->lpVtbl -> get_Layout(This,pVal) ) 

#define IGraphViewControl_put_Layout(This,newVal)	\
    ( (This)->lpVtbl -> put_Layout(This,newVal) ) 

#define IGraphViewControl_get_MouseMode(This,pVal)	\
    ( (This)->lpVtbl -> get_MouseMode(This,pVal) ) 

#define IGraphViewControl_put_MouseMode(This,newVal)	\
    ( (This)->lpVtbl -> put_MouseMode(This,newVal) ) 

#define IGraphViewControl_get_Zoom(This,pVal)	\
    ( (This)->lpVtbl -> get_Zoom(This,pVal) ) 

#define IGraphViewControl_put_Zoom(This,newVal)	\
    ( (This)->lpVtbl -> put_Zoom(This,newVal) ) 

#define IGraphViewControl_GetSelectedVertices(This,collection)	\
    ( (This)->lpVtbl -> GetSelectedVertices(This,collection) ) 

#define IGraphViewControl_SetSelectedVertex(This,Category,Identity,bSelected,bAppend)	\
    ( (This)->lpVtbl -> SetSelectedVertex(This,Category,Identity,bSelected,bAppend) ) 

#define IGraphViewControl_EnsureVisible(This,Category,Identity)	\
    ( (This)->lpVtbl -> EnsureVisible(This,Category,Identity) ) 

#define IGraphViewControl_ScaleToFit(This,pVal)	\
    ( (This)->lpVtbl -> ScaleToFit(This,pVal) ) 

#define IGraphViewControl_ScaleToFitSelected(This,pVal)	\
    ( (This)->lpVtbl -> ScaleToFitSelected(This,pVal) ) 

#define IGraphViewControl_GetRenderExtent(This,hDC,pX,pY,pWidth,pHeight)	\
    ( (This)->lpVtbl -> GetRenderExtent(This,hDC,pX,pY,pWidth,pHeight) ) 

#define IGraphViewControl_RecenterSelected(This)	\
    ( (This)->lpVtbl -> RecenterSelected(This) ) 

#define IGraphViewControl_RecenterOnVertex(This,category,identity)	\
    ( (This)->lpVtbl -> RecenterOnVertex(This,category,identity) ) 

#define IGraphViewControl_RemoveVertex(This,category,identity)	\
    ( (This)->lpVtbl -> RemoveVertex(This,category,identity) ) 

#define IGraphViewControl_RenderTo(This,hDC,X,Y,PageX,PageY,PageWidth,PageHeight)	\
    ( (This)->lpVtbl -> RenderTo(This,hDC,X,Y,PageX,PageY,PageWidth,PageHeight) ) 

#define IGraphViewControl_RenderToClipboard(This)	\
    ( (This)->lpVtbl -> RenderToClipboard(This) ) 

#define IGraphViewControl_Save(This,filename)	\
    ( (This)->lpVtbl -> Save(This,filename) ) 

#define IGraphViewControl_SaveAs(This)	\
    ( (This)->lpVtbl -> SaveAs(This) ) 

#define IGraphViewControl_LoadXml(This,xml)	\
    ( (This)->lpVtbl -> LoadXml(This,xml) ) 

#define IGraphViewControl_SaveXml(This,version2,retVal)	\
    ( (This)->lpVtbl -> SaveXml(This,version2,retVal) ) 

#define IGraphViewControl_SaveXmlAs(This)	\
    ( (This)->lpVtbl -> SaveXmlAs(This) ) 

#define IGraphViewControl_GetMeta(This,url)	\
    ( (This)->lpVtbl -> GetMeta(This,url) ) 

#define IGraphViewControl_GetData(This,url,Category,ID)	\
    ( (This)->lpVtbl -> GetData(This,url,Category,ID) ) 

#define IGraphViewControl_HideDisconnected(This)	\
    ( (This)->lpVtbl -> HideDisconnected(This) ) 

#define IGraphViewControl_HideSingletons(This)	\
    ( (This)->lpVtbl -> HideSingletons(This) ) 

#define IGraphViewControl_SetSplinesMode(This,splines)	\
    ( (This)->lpVtbl -> SetSplinesMode(This,splines) ) 

#define IGraphViewControl_LoadXmlFile(This)	\
    ( (This)->lpVtbl -> LoadXmlFile(This) ) 

#define IGraphViewControl_ShowOverview(This,show)	\
    ( (This)->lpVtbl -> ShowOverview(This,show) ) 

#define IGraphViewControl_SetOverviewOpacity(This,opacity)	\
    ( (This)->lpVtbl -> SetOverviewOpacity(This,opacity) ) 

#define IGraphViewControl_ShowEdgeLabels(This,show)	\
    ( (This)->lpVtbl -> ShowEdgeLabels(This,show) ) 

#define IGraphViewControl_EmulateDecoration(This,inplace)	\
    ( (This)->lpVtbl -> EmulateDecoration(This,inplace) ) 

#define IGraphViewControl_SetGraphStyle(This,style)	\
    ( (This)->lpVtbl -> SetGraphStyle(This,style) ) 

#define IGraphViewControl_LoadXgmml(This,xgmml,state,minimizeIfLarge)	\
    ( (This)->lpVtbl -> LoadXgmml(This,xgmml,state,minimizeIfLarge) ) 

#define IGraphViewControl_MergeXgmml(This,xgmml,state)	\
    ( (This)->lpVtbl -> MergeXgmml(This,xgmml,state) ) 

#define IGraphViewControl_SelectFile(This,current,retVal)	\
    ( (This)->lpVtbl -> SelectFile(This,current,retVal) ) 

#define IGraphViewControl_FindVertices(This,searchstring,searchLabel,searchProperties,collection)	\
    ( (This)->lpVtbl -> FindVertices(This,searchstring,searchLabel,searchProperties,collection) ) 

#define IGraphViewControl_SetApplicationZoomMode(This,mode)	\
    ( (This)->lpVtbl -> SetApplicationZoomMode(This,mode) ) 

#define IGraphViewControl_FindGraphItemByID(This)	\
    ( (This)->lpVtbl -> FindGraphItemByID(This) ) 

#define IGraphViewControl_GetVersion(This,pVal)	\
    ( (This)->lpVtbl -> GetVersion(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGraphViewControl_INTERFACE_DEFINED__ */



#ifndef __GraphViewControlLib_LIBRARY_DEFINED__
#define __GraphViewControlLib_LIBRARY_DEFINED__

/* library GraphViewControlLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_GraphViewControlLib;

#ifndef __DSeisintGraphControlEvents_DISPINTERFACE_DEFINED__
#define __DSeisintGraphControlEvents_DISPINTERFACE_DEFINED__

/* dispinterface DSeisintGraphControlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DSeisintGraphControlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5E48FEE6-3244-44EF-B6C0-9415671E2A13")
    DSeisintGraphControlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DSeisintGraphControlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DSeisintGraphControlEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DSeisintGraphControlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DSeisintGraphControlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DSeisintGraphControlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DSeisintGraphControlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DSeisintGraphControlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DSeisintGraphControlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DSeisintGraphControlEventsVtbl;

    interface DSeisintGraphControlEvents
    {
        CONST_VTBL struct DSeisintGraphControlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DSeisintGraphControlEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define DSeisintGraphControlEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define DSeisintGraphControlEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define DSeisintGraphControlEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define DSeisintGraphControlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define DSeisintGraphControlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define DSeisintGraphControlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DSeisintGraphControlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_GraphViewControl;

#ifdef __cplusplus

class DECLSPEC_UUID("F1EA17CB-F7BD-4108-A742-1BC7774383FF")
GraphViewControl;
#endif

EXTERN_C const CLSID CLSID_Hourglass;

#ifdef __cplusplus

class DECLSPEC_UUID("B067BDCD-B1A6-4790-82B1-D843B780BF15")
Hourglass;
#endif
#endif /* __GraphViewControlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


