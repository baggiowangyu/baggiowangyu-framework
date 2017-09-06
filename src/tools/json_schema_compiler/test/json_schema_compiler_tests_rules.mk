OutDirCygwin:=$(shell cygpath -u "$(OutDir)")
IntDirCygwin:=$(shell cygpath -u "$(IntDir)")
all: $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\arrays.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\choices.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\functions_on_types.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\functions_as_parameters.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\objects.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\additional_properties.h $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\enums.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\crossref.h $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\any.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\callbacks.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\simple_api.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\idl_object_types.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\idl_basics.cc
	mkdir -p `cygpath -u "$(OutDir)obj\global_intermediate\tools\json_schema_compiler\test"`

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\arrays.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\arrays.h: ..\..\..\tools\json_schema_compiler\json_schema.py arrays.json ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "arrays.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\choices.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\choices.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h choices.json ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "choices.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\functions_on_types.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\functions_on_types.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py functions_on_types.json ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "functions_on_types.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\functions_as_parameters.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\functions_as_parameters.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py functions_as_parameters.json ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "functions_as_parameters.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\objects.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\objects.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py objects.json ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "objects.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\additional_properties.h $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\additional_properties.cc: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py additional_properties.json ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "additional_properties.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\enums.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\enums.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py enums.json ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "enums.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\crossref.h $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\crossref.cc: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h crossref.json ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "crossref.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\any.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\any.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py any.json ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "any.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\callbacks.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\callbacks.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py callbacks.json ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "callbacks.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\simple_api.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\simple_api.h: ..\..\..\tools\json_schema_compiler\json_schema.py ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py simple_api.json ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "simple_api.json" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\idl_object_types.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\idl_object_types.h: ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\idl_schema.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py idl_object_types.idl ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "idl_object_types.idl" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

$(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\idl_basics.cc $(OutDirCygwin)obj\global_intermediate\tools\json_schema_compiler\test\idl_basics.h: ..\..\..\tools\json_schema_compiler\cpp_generator.py ..\..\..\tools\json_schema_compiler\idl_schema.py ..\..\..\tools\json_schema_compiler\compiler.py ..\..\..\tools\json_schema_compiler\util.h idl_basics.idl ..\..\..\tools\json_schema_compiler\cpp_util.py ..\..\..\tools\json_schema_compiler\cc_generator.py ..\..\..\tools\json_schema_compiler\util_cc_helper.py ..\..\..\tools\json_schema_compiler\util.cc ..\..\..\tools\json_schema_compiler\h_generator.py ..\..\..\tools\json_schema_compiler\model.py ..\..\..\tools\json_schema_compiler\cpp_type_generator.py ..\..\..\tools\json_schema_compiler\code.py
	"python" "../../../tools/json_schema_compiler/compiler.py" "idl_basics.idl" "--root=../../.." "--destdir=$(OutDir)obj/global_intermediate" "--namespace=test::api" "--generator=cpp"

