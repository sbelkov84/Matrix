cmake_minimum_required(VERSION 3.2)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(Matrix VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(Matrix VERSION 0.0.1)
endif()


configure_file(version.h.in version.h)

add_subdirectory("${PROJECT_SOURCE_DIR}/googletest" "googletest")

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

add_executable(Matrix main.cpp)
add_library(matrix_lib matrix.h)
add_executable(test_matrix test_matrix.cpp)

set_target_properties(Matrix matrix_lib test_matrix
    PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    LINKER_LANGUAGE CXX
)

target_link_libraries(Matrix PRIVATE
    matrix_lib 
)
target_link_libraries(test_matrix PRIVATE
     matrix_lib gtest gmock gtest_main
)

if (MSVC)
    target_compile_options(Matrix PRIVATE
        /W4
    )
    target_compile_options(matrix_lib PRIVATE
        /W4
    )
    target_compile_options(test_matrix PRIVATE
       /W4
    )
else ()
    target_compile_options(Matrix PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(matrix_lib PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_matrix PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()


install(TARGETS Matrix RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(test_matrix test_matrix)