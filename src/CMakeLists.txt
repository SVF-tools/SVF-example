# Define the actual runtime executable
add_llvm_executable(svf-example svf-ex.cpp)
target_link_libraries(svf-example PUBLIC ${llvm_libs})
target_link_libraries(svf-example PUBLIC z3 SvfCore SvfLLVM)
