# Define la librería (estática por defecto)
add_library(utils
    utils.cpp
)

# Agrega los directorios de headers para quienes usen la librería
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
