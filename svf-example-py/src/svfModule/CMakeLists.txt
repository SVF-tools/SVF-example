project(svfModule)
llvm_map_components_to_libnames(llvm_libs bitwriter core ipo irreader instcombine instrumentation target linker analysis scalaropts support )
file (GLOB SOURCES
   *.cpp
)
ADD_LIBRARY(svfModule SHARED ${SOURCES})

target_link_libraries(svfModule ${SVF_LIB} ${llvm_libs})
target_link_libraries(svfModule ${Z3_LIBRARIES})
set_target_properties(svfModule PROPERTIES
                       RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )