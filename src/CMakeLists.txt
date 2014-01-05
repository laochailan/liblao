find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)

set(SRCs log.c
		 config.c
		 resource/resource.c
		 resource/texture.c
		 resource/shader.c
		 resource/model.c
		 util/util.c
		 util/list.c
		 util/linalg.c
		 video/gl.c
		 video/vbo.c
		 video/ubo.c 
)

if(WIN32)
	set(SRCs ${SRCs} util/native_win.c)
else()
	set(SRCs ${SRCs} util/native_linux.c)
endif()

set(LIBs ${OPENGL_LIBRARIES}
		 ${PNG_LIBRARIES}
		 m
)

add_definitions(-std=c89)

include_directories(${OPENGL_INCLUDE_DIR} ${PNG_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
add_library(lao STATIC ${SRCs})
target_link_libraries(lao ${LIBs})

install(TARGETS lao ARCHIVE DESTINATION .)
