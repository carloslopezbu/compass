# Define la librería (estática por defecto)
add_library(trayectory
    trayectory.cpp
)

# Agrega los directorios de headers para quienes usen la librería
target_include_directories(trayectory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
