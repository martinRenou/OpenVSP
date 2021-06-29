IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "amd64")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
ENDIF()

IF( NOT ADEPT_CXX_COMPILER )
    SET( ADEPT_C_COMPILER ${CMAKE_C_COMPILER} )
	SET( ADEPT_CXX_COMPILER ${CMAKE_CXX_COMPILER} )
	SET( ADEPT_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
	SET( ADEPT_C_FLAGS ${CMAKE_C_FLAGS} )
ENDIF()

ExternalProject_Add( ADEPT2
	URL ${CMAKE_CURRENT_SOURCE_DIR}/Adept-2-709672a0b4b3.zip
	CMAKE_ARGS -DCMAKE_C_COMPILER=${ADEPT_C_COMPILER}
		-DCMAKE_CXX_COMPILER=${ADEPT_CXX_COMPILER}
		-DCMAKE_CXX_FLAGS=${ADEPT_CXX_FLAGS}
		-DCMAKE_C_FLAGS=${ADEPT_C_FLAGS}
		-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DBUILD_TESTING=FALSE
		-DADEPT_USE_OPENMP=FALSE
		-DADEPT_USE_LAPACK=FALSE
		-DADEPT_USE_BLAS=FALSE
		-DADEPT_USE_GSL=FALSE
)
ExternalProject_Get_Property( ADEPT2 INSTALL_DIR )
SET( ADEPT2_INSTALL_DIR ${INSTALL_DIR} )
