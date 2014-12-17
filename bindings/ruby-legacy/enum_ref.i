
%define OUTPUT_TYPEMAP(type, converter, convtype)
%typemap(in,numinputs=0) type *OUTPUT($*1_ltype temp), type &OUTPUT($*1_ltype temp) "$1 = &temp;";
%typemap(argout, fragment="output_helper") type *OUTPUT, type &OUTPUT {
   VALUE o = converter(convtype (*$1));
   $result = output_helper($result, o);
   }
%enddef
