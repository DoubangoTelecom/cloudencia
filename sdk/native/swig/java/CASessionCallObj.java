/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public class CASessionCallObj {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected CASessionCallObj(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CASessionCallObj obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        clWRAPJNI.delete_CASessionCallObj(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public CASessionCallObj(CASessionCall obj) {
    this(clWRAPJNI.new_CASessionCallObj__SWIG_0(CASessionCall.getCPtr(obj), obj), true);
  }

  public CASessionCallObj(CASessionCallObj obj) {
    this(clWRAPJNI.new_CASessionCallObj__SWIG_1(CASessionCallObj.getCPtr(obj), obj), true);
  }

  public CASessionCall unWrap() {
    long cPtr = clWRAPJNI.CASessionCallObj_unWrap(swigCPtr, this);
    return (cPtr == 0) ? null : new CASessionCall(cPtr, false);
  }

}
