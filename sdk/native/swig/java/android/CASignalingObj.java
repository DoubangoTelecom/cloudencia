/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public class CASignalingObj {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected CASignalingObj(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CASignalingObj obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        clWRAPJNI.delete_CASignalingObj(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public CASignalingObj(CASignaling obj) {
    this(clWRAPJNI.new_CASignalingObj__SWIG_0(CASignaling.getCPtr(obj), obj), true);
  }

  public CASignalingObj(CASignalingObj obj) {
    this(clWRAPJNI.new_CASignalingObj__SWIG_1(CASignalingObj.getCPtr(obj), obj), true);
  }

  public CASignaling unWrap() {
    long cPtr = clWRAPJNI.CASignalingObj_unWrap(swigCPtr, this);
    return (cPtr == 0) ? null : new CASignaling(cPtr, false);
  }

}
