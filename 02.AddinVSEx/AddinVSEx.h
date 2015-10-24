

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Mar 06 17:39:57 2015
 */
/* Compiler settings for AddinVSEx.idl:
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

#ifndef __AddinVSEx_h__
#define __AddinVSEx_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAtlEdit_FWD_DEFINED__
#define __IAtlEdit_FWD_DEFINED__
typedef interface IAtlEdit IAtlEdit;
#endif 	/* __IAtlEdit_FWD_DEFINED__ */


#ifndef __CAtlEdit_FWD_DEFINED__
#define __CAtlEdit_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAtlEdit CAtlEdit;
#else
typedef struct CAtlEdit CAtlEdit;
#endif /* __cplusplus */

#endif 	/* __CAtlEdit_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IAtlEdit_INTERFACE_DEFINED__
#define __IAtlEdit_INTERFACE_DEFINED__

/* interface IAtlEdit */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IAtlEdit;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2D7E0F90-3215-11D0-BF28-0000E8D0D146")
    IAtlEdit : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE setParam( 
            /* [in] */ int id,
            /* [in] */ const CHAR *param) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAtlEditVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAtlEdit * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAtlEdit * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAtlEdit * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAtlEdit * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAtlEdit * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAtlEdit * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAtlEdit * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *setParam )( 
            IAtlEdit * This,
            /* [in] */ int id,
            /* [in] */ const CHAR *param);
        
        END_INTERFACE
    } IAtlEditVtbl;

    interface IAtlEdit
    {
        CONST_VTBL struct IAtlEditVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAtlEdit_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAtlEdit_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAtlEdit_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAtlEdit_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAtlEdit_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAtlEdit_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAtlEdit_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAtlEdit_setParam(This,id,param)	\
    ( (This)->lpVtbl -> setParam(This,id,param) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAtlEdit_INTERFACE_DEFINED__ */



#ifndef __AddinVSExLib_LIBRARY_DEFINED__
#define __AddinVSExLib_LIBRARY_DEFINED__

/* library AddinVSExLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AddinVSExLib;

EXTERN_C const CLSID CLSID_CAtlEdit;

#ifdef __cplusplus

class DECLSPEC_UUID("2D7E0F8F-3215-11D0-BF28-0000E8D0D146")
CAtlEdit;
#endif
#endif /* __AddinVSExLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


