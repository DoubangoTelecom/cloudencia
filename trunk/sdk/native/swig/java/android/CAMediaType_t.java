/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public enum CAMediaType_t {
  CAMediaType_None(0x00),
  CAMediaType_Audio((0x01 << 0)),
  CAMediaType_Video((0x01 << 1)),
  CAMediaType_ScreenCast((0x01 << 2)),
  CAMediaType_AudioVideo((CAMediaType_Audio|CAMediaType_Video)),
  CAMediaType_All(0xFF);

  public final int swigValue() {
    return swigValue;
  }

  public static CAMediaType_t swigToEnum(int swigValue) {
    CAMediaType_t[] swigValues = CAMediaType_t.class.getEnumConstants();
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (CAMediaType_t swigEnum : swigValues)
      if (swigEnum.swigValue == swigValue)
        return swigEnum;
    throw new IllegalArgumentException("No enum " + CAMediaType_t.class + " with value " + swigValue);
  }

  @SuppressWarnings("unused")
  private CAMediaType_t() {
    this.swigValue = SwigNext.next++;
  }

  @SuppressWarnings("unused")
  private CAMediaType_t(int swigValue) {
    this.swigValue = swigValue;
    SwigNext.next = swigValue+1;
  }

  @SuppressWarnings("unused")
  private CAMediaType_t(CAMediaType_t swigEnum) {
    this.swigValue = swigEnum.swigValue;
    SwigNext.next = this.swigValue+1;
  }

  private final int swigValue;

  private static class SwigNext {
    private static int next = 0;
  }
}

