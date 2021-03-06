/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.doubango.clWRAP;

public class clWRAPJNI {
  public final static native void delete_CAObj(long jarg1);
  public final static native String CAObj_getObjectId(long jarg1, CAObj jarg1_);
  public final static native int CAObj_takeRef(long jarg1, CAObj jarg1_);
  public final static native int CAObj_releaseRef(long jarg1, CAObj jarg1_);
  public final static native long new_CASignalingObj__SWIG_0(long jarg1, CASignaling jarg1_);
  public final static native long new_CASignalingObj__SWIG_1(long jarg1, CASignalingObj jarg1_);
  public final static native void delete_CASignalingObj(long jarg1);
  public final static native long CASignalingObj_unWrap(long jarg1, CASignalingObj jarg1_);
  public final static native long new_CASignalingCallbackObj__SWIG_0(long jarg1, CASignalingCallback jarg1_);
  public final static native long new_CASignalingCallbackObj__SWIG_1(long jarg1, CASignalingCallbackObj jarg1_);
  public final static native void delete_CASignalingCallbackObj(long jarg1);
  public final static native long CASignalingCallbackObj_unWrap(long jarg1, CASignalingCallbackObj jarg1_);
  public final static native long new_CASignalingEventObj__SWIG_0(long jarg1, CASignalingEvent jarg1_);
  public final static native long new_CASignalingEventObj__SWIG_1(long jarg1, CASignalingEventObj jarg1_);
  public final static native void delete_CASignalingEventObj(long jarg1);
  public final static native long CASignalingEventObj_unWrap(long jarg1, CASignalingEventObj jarg1_);
  public final static native long new_CASignalingCallEventObj__SWIG_0(long jarg1, CASignalingCallEvent jarg1_);
  public final static native long new_CASignalingCallEventObj__SWIG_1(long jarg1, CASignalingCallEventObj jarg1_);
  public final static native void delete_CASignalingCallEventObj(long jarg1);
  public final static native long CASignalingCallEventObj_unWrap(long jarg1, CASignalingCallEventObj jarg1_);
  public final static native long new_CASessionCallObj__SWIG_0(long jarg1, CASessionCall jarg1_);
  public final static native long new_CASessionCallObj__SWIG_1(long jarg1, CASessionCallObj jarg1_);
  public final static native void delete_CASessionCallObj(long jarg1);
  public final static native long CASessionCallObj_unWrap(long jarg1, CASessionCallObj jarg1_);
  public final static native long new_CASessionCallIceCallbackObj__SWIG_0(long jarg1, CASessionCallIceCallback jarg1_);
  public final static native long new_CASessionCallIceCallbackObj__SWIG_1(long jarg1, CASessionCallIceCallbackObj jarg1_);
  public final static native void delete_CASessionCallIceCallbackObj(long jarg1);
  public final static native long CASessionCallIceCallbackObj_unWrap(long jarg1, CASessionCallIceCallbackObj jarg1_);
  public final static native void delete_CAEngine(long jarg1);
  public final static native String CAEngine_getObjectId(long jarg1, CAEngine jarg1_);
  public final static native boolean CAEngine_init();
  public final static native boolean CAEngine_deInit();
  public final static native boolean CAEngine_isInitialized();
  public final static native boolean CAEngine_setDebugLevel(int jarg1);
  public final static native boolean CAEngine_setSSLCertificates__SWIG_0(String jarg1, String jarg2, String jarg3, boolean jarg4);
  public final static native boolean CAEngine_setSSLCertificates__SWIG_1(String jarg1, String jarg2, String jarg3);
  public final static native boolean CAEngine_setVideoPrefSize(String jarg1);
  public final static native boolean CAEngine_setVideoFps(int jarg1);
  public final static native boolean CAEngine_setVideoBandwidthUpMax(int jarg1);
  public final static native boolean CAEngine_setVideoBandwidthDownMax(int jarg1);
  public final static native boolean CAEngine_setVideoMotionRank(int jarg1);
  public final static native boolean CAEngine_setVideoCongestionCtrlEnabled(boolean jarg1);
  public final static native boolean CAEngine_setVideoJbEnabled(boolean jarg1);
  public final static native boolean CAEngine_setVideoAvpfTail(int jarg1, int jarg2);
  public final static native boolean CAEngine_setVideoZeroArtifactsEnabled(boolean jarg1);
  public final static native boolean CAEngine_setAudioEchoSuppEnabled(boolean jarg1);
  public final static native boolean CAEngine_setAudioEchoTail(int jarg1);
  public final static native boolean CAEngine_addNattIceServer__SWIG_0(String jarg1, String jarg2, int jarg3, boolean jarg4, boolean jarg5, String jarg6, String jarg7);
  public final static native boolean CAEngine_addNattIceServer__SWIG_1(String jarg1, String jarg2, int jarg3, boolean jarg4, boolean jarg5, String jarg6);
  public final static native boolean CAEngine_addNattIceServer__SWIG_2(String jarg1, String jarg2, int jarg3, boolean jarg4, boolean jarg5);
  public final static native boolean CAEngine_addNattIceServer__SWIG_3(String jarg1, String jarg2, int jarg3, boolean jarg4);
  public final static native boolean CAEngine_addNattIceServer__SWIG_4(String jarg1, String jarg2, int jarg3);
  public final static native boolean CAEngine_clearNattIceServers();
  public final static native boolean CAEngine_setNattIceStunEnabled(boolean jarg1);
  public final static native boolean CAEngine_setNattIceTurnEnabled(boolean jarg1);
  public final static native long new_CASignalingEvent__SWIG_0(int jarg1, String jarg2, java.nio.ByteBuffer jarg3, long jarg4);
  public final static native long new_CASignalingEvent__SWIG_1(int jarg1, String jarg2, java.nio.ByteBuffer jarg3);
  public final static native long new_CASignalingEvent__SWIG_2(int jarg1, String jarg2);
  public final static native void delete_CASignalingEvent(long jarg1);
  public final static native String CASignalingEvent_getObjectId(long jarg1, CASignalingEvent jarg1_);
  public final static native int CASignalingEvent_getType(long jarg1, CASignalingEvent jarg1_);
  public final static native String CASignalingEvent_getDescription(long jarg1, CASignalingEvent jarg1_);
  public final static native java.nio.ByteBuffer CASignalingEvent_getDataPtr(long jarg1, CASignalingEvent jarg1_);
  public final static native long CASignalingEvent_getDataSize(long jarg1, CASignalingEvent jarg1_);
  public final static native long new_CASignalingCallEvent(String jarg1);
  public final static native void delete_CASignalingCallEvent(long jarg1);
  public final static native String CASignalingCallEvent_getObjectId(long jarg1, CASignalingCallEvent jarg1_);
  public final static native String CASignalingCallEvent_getType(long jarg1, CASignalingCallEvent jarg1_);
  public final static native String CASignalingCallEvent_getFrom(long jarg1, CASignalingCallEvent jarg1_);
  public final static native String CASignalingCallEvent_getTo(long jarg1, CASignalingCallEvent jarg1_);
  public final static native String CASignalingCallEvent_getCallId(long jarg1, CASignalingCallEvent jarg1_);
  public final static native String CASignalingCallEvent_getTransacId(long jarg1, CASignalingCallEvent jarg1_);
  public final static native String CASignalingCallEvent_getSdp(long jarg1, CASignalingCallEvent jarg1_);
  public final static native void delete_CASignalingCallback(long jarg1);
  public final static native boolean CASignalingCallback_onEventNet(long jarg1, CASignalingCallback jarg1_, long jarg2, CASignalingEventObj jarg2_);
  public final static native boolean CASignalingCallback_onEventCall(long jarg1, CASignalingCallback jarg1_, long jarg2, CASignalingCallEventObj jarg2_);
  public final static native void delete_CASignaling(long jarg1);
  public final static native String CASignaling_getObjectId(long jarg1, CASignaling jarg1_);
  public final static native boolean CASignaling_setCallback(long jarg1, CASignaling jarg1_, long jarg2, CASignalingCallbackObj jarg2_);
  public final static native boolean CASignaling_isConnected(long jarg1, CASignaling jarg1_);
  public final static native boolean CASignaling_isReady(long jarg1, CASignaling jarg1_);
  public final static native boolean CASignaling_connect(long jarg1, CASignaling jarg1_);
  public final static native boolean CASignaling_sendData(long jarg1, CASignaling jarg1_, java.nio.ByteBuffer jarg2, long jarg3);
  public final static native boolean CASignaling_disConnect(long jarg1, CASignaling jarg1_);
  public final static native long CASignaling_newObj__SWIG_0(String jarg1, String jarg2, String jarg3, String jarg4, int jarg5);
  public final static native long CASignaling_newObj__SWIG_1(String jarg1, String jarg2, String jarg3, String jarg4);
  public final static native long CASignaling_newObj__SWIG_2(String jarg1, String jarg2, String jarg3);
  public final static native String CASignaling_getCredUserId(long jarg1, CASignaling jarg1_);
  public final static native String CASignaling_getCredPassword(long jarg1, CASignaling jarg1_);
  public final static native void delete_CASession(long jarg1);
  public final static native String CASession_getObjectId(long jarg1, CASession jarg1_);
  public final static native int CASession_getType(long jarg1, CASession jarg1_);
  public final static native boolean CASessionCallIceCallback_onStateChanged(long jarg1, CASessionCallIceCallback jarg1_, long jarg2, CASessionCallObj jarg2_);
  public final static native void delete_CASessionCallIceCallback(long jarg1);
  public final static native void delete_CASessionCall(long jarg1);
  public final static native String CASessionCall_getObjectId(long jarg1, CASessionCall jarg1_);
  public final static native boolean CASessionCall_setIceCallback(long jarg1, CASessionCall jarg1_, long jarg2, CASessionCallIceCallbackObj jarg2_);
  public final static native boolean CASessionCall_setVideoDisplays__SWIG_0(long jarg1, CASessionCall jarg1_, int jarg2, long jarg3, long jarg4);
  public final static native boolean CASessionCall_setVideoDisplays__SWIG_1(long jarg1, CASessionCall jarg1_, int jarg2, long jarg3);
  public final static native boolean CASessionCall_setVideoDisplays__SWIG_2(long jarg1, CASessionCall jarg1_, int jarg2);
  public final static native boolean CASessionCall_call(long jarg1, CASessionCall jarg1_, int jarg2, String jarg3);
  public final static native boolean CASessionCall_acceptEvent(long jarg1, CASessionCall jarg1_, long jarg2, CASignalingCallEventObj jarg2_);
  public final static native boolean CASessionCall_rejectEvent(long jarg1, CASignalingObj jarg1_, long jarg2, CASignalingCallEventObj jarg2_);
  public final static native boolean CASessionCall_setMute__SWIG_0(long jarg1, CASessionCall jarg1_, boolean jarg2, int jarg3);
  public final static native boolean CASessionCall_setMute__SWIG_1(long jarg1, CASessionCall jarg1_, boolean jarg2);
  public final static native boolean CASessionCall_hangup(long jarg1, CASessionCall jarg1_);
  public final static native boolean CASessionCall_setVideoFps__SWIG_0(long jarg1, CASessionCall jarg1_, int jarg2, int jarg3);
  public final static native boolean CASessionCall_setVideoFps__SWIG_1(long jarg1, CASessionCall jarg1_, int jarg2);
  public final static native boolean CASessionCall_setVideoBandwidthUploadMax__SWIG_0(long jarg1, CASessionCall jarg1_, int jarg2, int jarg3);
  public final static native boolean CASessionCall_setVideoBandwidthUploadMax__SWIG_1(long jarg1, CASessionCall jarg1_, int jarg2);
  public final static native boolean CASessionCall_setVideoBandwidthDownloadMax__SWIG_0(long jarg1, CASessionCall jarg1_, int jarg2, int jarg3);
  public final static native boolean CASessionCall_setVideoBandwidthDownloadMax__SWIG_1(long jarg1, CASessionCall jarg1_, int jarg2);
  public final static native String CASessionCall_getCallId(long jarg1, CASessionCall jarg1_);
  public final static native int CASessionCall_getMediaType(long jarg1, CASessionCall jarg1_);
  public final static native int CASessionCall_getIceState(long jarg1, CASessionCall jarg1_);
  public final static native long CASessionCall_newObj__SWIG_0(long jarg1, CASignalingObj jarg1_);
  public final static native long CASessionCall_newObj__SWIG_1(long jarg1, CASignalingObj jarg1_, long jarg2, CASignalingCallEventObj jarg2_);
  public final static native long CAEngine_SWIGUpcast(long jarg1);
  public final static native long CASignalingEvent_SWIGUpcast(long jarg1);
  public final static native long CASignalingCallEvent_SWIGUpcast(long jarg1);
  public final static native long CASignalingCallback_SWIGUpcast(long jarg1);
  public final static native long CASignaling_SWIGUpcast(long jarg1);
  public final static native long CASession_SWIGUpcast(long jarg1);
  public final static native long CASessionCallIceCallback_SWIGUpcast(long jarg1);
  public final static native long CASessionCall_SWIGUpcast(long jarg1);
}
