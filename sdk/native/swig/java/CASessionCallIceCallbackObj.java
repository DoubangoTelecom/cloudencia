/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public class CASessionCallIceCallbackObj {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected CASessionCallIceCallbackObj(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CASessionCallIceCallbackObj obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        clWRAPJNI.delete_CASessionCallIceCallbackObj(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public CASessionCallIceCallbackObj(CASessionCallIceCallback obj) {
    this(clWRAPJNI.new_CASessionCallIceCallbackObj__SWIG_0(CASessionCallIceCallback.getCPtr(obj), obj), true);
  }

  public CASessionCallIceCallbackObj(CASessionCallIceCallbackObj obj) {
    this(clWRAPJNI.new_CASessionCallIceCallbackObj__SWIG_1(CASessionCallIceCallbackObj.getCPtr(obj), obj), true);
  }

  public CASessionCallIceCallback unWrap() {
    long cPtr = clWRAPJNI.CASessionCallIceCallbackObj_unWrap(swigCPtr, this);
    return (cPtr == 0) ? null : new CASessionCallIceCallback(cPtr, false);
  }

}
