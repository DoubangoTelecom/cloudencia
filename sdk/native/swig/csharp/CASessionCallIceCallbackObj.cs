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

public class CASessionCallIceCallbackObj : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal CASessionCallIceCallbackObj(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(CASessionCallIceCallbackObj obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~CASessionCallIceCallbackObj() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          clWRAPPINVOKE.delete_CASessionCallIceCallbackObj(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public CASessionCallIceCallbackObj(CASessionCallIceCallback obj) : this(clWRAPPINVOKE.new_CASessionCallIceCallbackObj__SWIG_0(CASessionCallIceCallback.getCPtr(obj)), true) {
  }

  public CASessionCallIceCallbackObj(CASessionCallIceCallbackObj obj) : this(clWRAPPINVOKE.new_CASessionCallIceCallbackObj__SWIG_1(CASessionCallIceCallbackObj.getCPtr(obj)), true) {
    if (clWRAPPINVOKE.SWIGPendingException.Pending) throw clWRAPPINVOKE.SWIGPendingException.Retrieve();
  }

  public CASessionCallIceCallback unWrap() {
    IntPtr cPtr = clWRAPPINVOKE.CASessionCallIceCallbackObj_unWrap(swigCPtr);
    CASessionCallIceCallback ret = (cPtr == IntPtr.Zero) ? null : new CASessionCallIceCallback(cPtr, false);
    return ret;
  }

}

}
