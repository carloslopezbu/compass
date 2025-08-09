# Define la librería (estática por defecto)
add_library(evolutive
    evolutive.cpp
)

# Agrega los directorios de headers para quienes usen la librería
target_include_directories(evolutive PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
