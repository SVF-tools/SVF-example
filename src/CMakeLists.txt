add_executable(svf-example svf-ex.cpp)
target_link_libraries(svf-example PUBLIC ${llvm_libs} SvfCore SvfLLVM z3)
