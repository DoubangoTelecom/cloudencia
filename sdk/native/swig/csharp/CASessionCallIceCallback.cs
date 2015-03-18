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

public class CASessionCallIceCallback : CAObj {
  private HandleRef swigCPtr;

  internal CASessionCallIceCallback(IntPtr cPtr, bool cMemoryOwn) : base(clWRAPPINVOKE.CASessionCallIceCallback_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(CASessionCallIceCallback obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~CASessionCallIceCallback() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          clWRAPPINVOKE.delete_CASessionCallIceCallback(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public virtual bool onStateChanged(CASessionCallObj oCall) {
    bool ret = clWRAPPINVOKE.CASessionCallIceCallback_onStateChanged(swigCPtr, CASessionCallObj.getCPtr(oCall));
    if (clWRAPPINVOKE.SWIGPendingException.Pending) throw clWRAPPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}