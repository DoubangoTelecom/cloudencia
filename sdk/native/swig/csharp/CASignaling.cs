/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace org.doubango.clWRAP {

using System;
using System.Runtime.InteropServices;

public class CASignaling : CAObj {
  private HandleRef swigCPtr;

  internal CASignaling(IntPtr cPtr, bool cMemoryOwn) : base(clWRAPPINVOKE.CASignaling_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(CASignaling obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~CASignaling() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          clWRAPPINVOKE.delete_CASignaling(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public override string getObjectId() {
    string ret = clWRAPPINVOKE.CASignaling_getObjectId(swigCPtr);
    return ret;
  }

  public bool setCallback(CASignalingCallbackObj oCallback) {
    bool ret = clWRAPPINVOKE.CASignaling_setCallback(swigCPtr, CASignalingCallbackObj.getCPtr(oCallback));
    if (clWRAPPINVOKE.SWIGPendingException.Pending) throw clWRAPPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool isConnected() {
    bool ret = clWRAPPINVOKE.CASignaling_isConnected(swigCPtr);
    return ret;
  }

  public bool isReady() {
    bool ret = clWRAPPINVOKE.CASignaling_isReady(swigCPtr);
    return ret;
  }

  public bool connect() {
    bool ret = clWRAPPINVOKE.CASignaling_connect(swigCPtr);
    return ret;
  }

  public bool sendData(IntPtr pcData, uint nDataSize) {
    bool ret = clWRAPPINVOKE.CASignaling_sendData(swigCPtr, pcData, nDataSize);
    return ret;
  }

  public bool disConnect() {
    bool ret = clWRAPPINVOKE.CASignaling_disConnect(swigCPtr);
    return ret;
  }

  public static CASignalingObj newObj(string strConnectionUri, string strCredUserId, string strCredPassword, string pcLocalIP, ushort nLocalPort) {
    CASignalingObj ret = new CASignalingObj(clWRAPPINVOKE.CASignaling_newObj__SWIG_0(strConnectionUri, strCredUserId, strCredPassword, pcLocalIP, nLocalPort), true);
    if (clWRAPPINVOKE.SWIGPendingException.Pending) throw clWRAPPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static CASignalingObj newObj(string strConnectionUri, string strCredUserId, string strCredPassword, string pcLocalIP) {
    CASignalingObj ret = new CASignalingObj(clWRAPPINVOKE.CASignaling_newObj__SWIG_1(strConnectionUri, strCredUserId, strCredPassword, pcLocalIP), true);
    if (clWRAPPINVOKE.SWIGPendingException.Pending) throw clWRAPPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static CASignalingObj newObj(string strConnectionUri, string strCredUserId, string strCredPassword) {
    CASignalingObj ret = new CASignalingObj(clWRAPPINVOKE.CASignaling_newObj__SWIG_2(strConnectionUri, strCredUserId, strCredPassword), true);
    if (clWRAPPINVOKE.SWIGPendingException.Pending) throw clWRAPPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public virtual string getCredUserId() {
    string ret = clWRAPPINVOKE.CASignaling_getCredUserId(swigCPtr);
    return ret;
  }

  public virtual string getCredPassword() {
    string ret = clWRAPPINVOKE.CASignaling_getCredPassword(swigCPtr);
    return ret;
  }

}

}
