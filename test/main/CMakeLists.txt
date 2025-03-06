idf_component_register(SRCS "test_ring.cc"
                       REQUIRES cmock esp_hw_support jesl
                       WHOLE_ARCHIVE)

target_compile_options(${COMPONENT_LIB} PUBLIC -fsanitize=address -fconcepts)
target_link_options(${COMPONENT_LIB} PUBLIC -fsanitize=address)
target_link_libraries(${COMPONENT_LIB} PUBLIC Catch2::Catch2WithMain)

idf_component_get_property(mqtt mqtt COMPONENT_LIB)
