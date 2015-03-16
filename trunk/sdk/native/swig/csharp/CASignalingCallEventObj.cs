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

public class CASignalingCallEventObj : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal CASignalingCallEventObj(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(CASignalingCallEventObj obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~CASignalingCallEventObj() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          clWRAPPINVOKE.delete_CASignalingCallEventObj(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public CASignalingCallEventObj(CASignalingCallEvent obj) : this(clWRAPPINVOKE.new_CASignalingCallEventObj__SWIG_0(CASignalingCallEvent.getCPtr(obj)), true) {
  }

  public CASignalingCallEventObj(CASignalingCallEventObj obj) : this(clWRAPPINVOKE.new_CASignalingCallEventObj__SWIG_1(CASignalingCallEventObj.getCPtr(obj)), true) {
    if (clWRAPPINVOKE.SWIGPendingException.Pending) throw clWRAPPINVOKE.SWIGPendingException.Retrieve();
  }

  public CASignalingCallEvent unWrap() {
    IntPtr cPtr = clWRAPPINVOKE.CASignalingCallEventObj_unWrap(swigCPtr);
    CASignalingCallEvent ret = (cPtr == IntPtr.Zero) ? null : new CASignalingCallEvent(cPtr, false);
    return ret;
  }

}

}
