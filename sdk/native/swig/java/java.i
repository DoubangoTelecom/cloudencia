/* File : java.i 
* http://www.swig.org/Doc1.3/Java.html
*/

// http://www.swig.org/Doc1.3/Java.html#enumerations
%include "enums.swg"
%javaconst(1);


//================== Mapping void* (Java to C) as ByteBuffer
%typemap(jni) void * "jbyteArray"
%typemap(jtype) void * "java.nio.ByteBuffer"
%typemap(jstype) void * "java.nio.ByteBuffer"
%typemap(javain) void * "$javainput"
%typemap(javaout) void * { return $jnicall; }

// (From Java to C)
%typemap(in) void * %{ 
	$1 = jenv->GetDirectBufferAddress($input); 
%}

// (From C to Java)
//%typemap(out) void * %{
//	$result = $1; 
//%}
%typemap(javadirectorin) void * "$jniinput"
//==================


%include ../cloudencia.i