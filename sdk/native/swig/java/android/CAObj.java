/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public class CAObj {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected CAObj(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CAObj obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        clWRAPJNI.delete_CAObj(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public String getObjectId() {
    return clWRAPJNI.CAObj_getObjectId(swigCPtr, this);
  }

  public int takeRef() {
    return clWRAPJNI.CAObj_takeRef(swigCPtr, this);
  }

  public int releaseRef() {
    return clWRAPJNI.CAObj_releaseRef(swigCPtr, this);
  }

}
