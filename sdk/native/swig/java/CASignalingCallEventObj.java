/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public class CASignalingCallEventObj {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected CASignalingCallEventObj(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CASignalingCallEventObj obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        clWRAPJNI.delete_CASignalingCallEventObj(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public CASignalingCallEventObj(CASignalingCallEvent obj) {
    this(clWRAPJNI.new_CASignalingCallEventObj__SWIG_0(CASignalingCallEvent.getCPtr(obj), obj), true);
  }

  public CASignalingCallEventObj(CASignalingCallEventObj obj) {
    this(clWRAPJNI.new_CASignalingCallEventObj__SWIG_1(CASignalingCallEventObj.getCPtr(obj), obj), true);
  }

  public CASignalingCallEvent unWrap() {
    long cPtr = clWRAPJNI.CASignalingCallEventObj_unWrap(swigCPtr, this);
    return (cPtr == 0) ? null : new CASignalingCallEvent(cPtr, false);
  }

}
