# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT

set(RGL_HASH 06b6e02b09d995b42886b4c7d8accf79bf6f922a)
set(RGL_TAG v0.11.0)

set(RGL_SO_URL https://github.com/RobotecAI/RobotecGPULidar/releases/download/${RGL_TAG}/libRobotecGPULidar.so)
set(RGL_API_URL https://raw.githubusercontent.com/RobotecAI/RobotecGPULidar/${RGL_HASH}/include/rgl/api/core.h)

set(RGL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3-rdParty/RobotecGPULidar)
set(RGL_INCLUDE_DIR ${RGL_LIB_DIR}/include)

set(RGL_SO_PATH ${RGL_LIB_DIR}/libRobotecGPULidar.so)
set(RGL_API_PATH ${RGL_LIB_DIR}/include/rgl/api/core.h)

file(DOWNLOAD
        ${RGL_SO_URL}
        ${RGL_SO_PATH}
    )

file(DOWNLOAD
        ${RGL_API_URL}
        ${RGL_API_PATH}
    )