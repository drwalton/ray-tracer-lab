cmake_minimum_required(VERSION 3.20)

project(direct_ray_tracer LANGUAGES CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenMP)

add_library(tgaimage
    3rdParty/tgaimage/tgaimage.cpp
    3rdParty/tgaimage/tgaimage.h
)

set(ENTITIES_SOURCE_GROUP
    Entity.hpp
    Renderable.hpp
    Scene.hpp
    Sphere.hpp
    Plane.hpp
    Triangle.hpp
    Mesh.hpp
    AABBMesh.hpp
)

set(LIGHTS_SOURCE_GROUP
    Light.hpp
    PointLight.hpp
    DirectionalLight.hpp
)

set(SHADERS_SOURCE_GROUP
    Shader.hpp
    LambertianShader.hpp
    TexturedLambertianShader.hpp
    PhongShader.hpp
    MirrorShader.hpp
    TexCoordTestShader.hpp
)

source_group("Header Files\\Entities" FILES ${ENTITIES_SOURCE_GROUP})
source_group("Header Files\\Lights" FILES ${LIGHTS_SOURCE_GROUP})
source_group("Header Files\\Shaders" FILES ${SHADERS_SOURCE_GROUP})

add_executable(main
    main.cpp

    GeomUtil.hpp

    Ray.hpp
    HitInfo.hpp
    Camera.hpp

    Model.cpp
    Model.hpp

    BitMasks.hpp

    ${ENTITIES_SOURCE_GROUP}
    ${LIGHTS_SOURCE_GROUP}
    ${SHADERS_SOURCE_GROUP}
)


if(OpenMP_CXX_FOUND)
    target_link_libraries(main PUBLIC OpenMP::OpenMP_CXX tgaimage)
else()
    target_link_libraries(main tgaimage)
endif()

include_directories(3rdParty/tgaimage)
include_directories(3rdParty/eigen-3.4.0)
