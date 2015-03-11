##### CSharp
echo "--->CSharp...<---"
swig -c++ -csharp -namespace org.doubango.clWRAP -outdir csharp -o csharp/clWRAP_wrap.cc csharp/csharp.i

##### Objective-C
#echo "--->Objective-C...<---"
#swig -c++ -objc -outdir objc -o -objc/clWRAP_wrap.cc -objc/-objc.i


##### Java
echo "--->Java...<---"
swig -c++ -java -package org.doubango.clWRAP -outdir java -o java/clWRAP_wrap.cc java/java.i
echo "Java(Google Dalvik)..."
echo "Google Android special tasks"
swig -noexcept -c++ -java -package org.doubango.clWRAP -outdir java/android -o java/android/clWRAP_wrap.cc java/java.i
sed -i 's/dynamic_cast/static_cast/g' java/android/clWRAP_wrap.cc

##### Python
echo "--->Python...<---"
swig -c++ -python -outdir python -o python/clWRAP_wrap.cc python/python.i

##### Perl
echo "--->Perl...<---"
swig -c++ -perl -outdir perl -o Perl/clWRAP_wrap.cc perl/perl.i

##### Ruby
echo "--->Ruby...<---"
#swig -c++ -ruby -outdir Ruby -o Ruby/clWRAP_wrap.cc ruby/ruby.i