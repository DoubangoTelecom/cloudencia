/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public class CASignalingCallback extends CAObj {
  private long swigCPtr;

  protected CASignalingCallback(long cPtr, boolean cMemoryOwn) {
    super(clWRAPJNI.CASignalingCallback_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CASignalingCallback obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        clWRAPJNI.delete_CASignalingCallback(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public boolean onEventNet(CASignalingEventObj e) {
    return clWRAPJNI.CASignalingCallback_onEventNet(swigCPtr, this, CASignalingEventObj.getCPtr(e), e);
  }

  public boolean onEventCall(CASignalingCallEventObj e) {
    return clWRAPJNI.CASignalingCallback_onEventCall(swigCPtr, this, CASignalingCallEventObj.getCPtr(e), e);
  }

}
