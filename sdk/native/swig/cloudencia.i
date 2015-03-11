/* File : tinyWRAP.i */
%module(directors="1") clWRAP
%include "typemaps.i"
%include <stdint.i>
%include <std_string.i>

#define CA_DISABLE_WARNINGS_BEGIN(nn)
#define CA_DISABLE_WARNINGS_END()
#define CLOUDENCIA_API
#define CA_INLINE

%nodefaultctor;
%include "../cloudencia/include/cloudencia/ca_obj.h"
%include "../cloudencia/include/cloudencia/ca_engine.h"
%clearnodefaultctor;