/* File : tinyWRAP.i */
%module(directors="1") clWRAP
%include "typemaps.i"
%include <stdint.i>
%include <std_string.i>

%include "cloudencia_enums.i"

#define CA_DISABLE_WARNINGS_BEGIN(nn)
#define CA_DISABLE_WARNINGS_END()
#define CLOUDENCIA_API
#define CA_INLINE

%nodefaultctor;
%include "../cloudencia/include/cloudencia/ca_obj.h"
%clearnodefaultctor;

%template(CASignalingObj) CAObjWrapper<CASignaling* >;
%template(CASignalingCallbackObj) CAObjWrapper<CASignalingCallback* >;
%template(CASignalingEventObj) CAObjWrapper<CASignalingEvent* >;
%template(CASignalingCallEventObj) CAObjWrapper<CASignalingCallEvent* >;

%nodefaultctor;
%include "../cloudencia/include/cloudencia/ca_engine.h"
%include "../cloudencia/include/cloudencia/ca_signaling.h"
%clearnodefaultctor;