###########################################################
# PlusIrbis project
# Alexey Mironov, 2018-2020
###########################################################

# network client library
project(irbis)

include(../common/irbis.cmake)

add_library(${PROJECT_NAME}
    STATIC
    ${HeaderFiles}
    ${CppFiles}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    -DPLUSIRBIS_LIBRARY
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${ARTIFACTS})
