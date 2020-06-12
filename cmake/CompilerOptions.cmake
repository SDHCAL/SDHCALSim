#
# Copyright (C) 2018 by George Cave - gcave@stablecoder.ca
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

if(ENABLE_ALL_WARNINGS)
  if(CMAKE_COMPILER_IS_GNUCC
     OR CMAKE_COMPILER_IS_GNUCXX
     OR CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
     OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
    # GCC/Clang
    add_compile_options(-Wall -Wextra)
  elseif(MSVC)
    # MSVC
    add_compile_options(/W4)
  endif()
endif()

if(ENABLE_EFFECTIVE_CXX)
  if(CMAKE_COMPILER_IS_GNUCC
     OR CMAKE_COMPILER_IS_GNUCXX
     OR CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
     OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
  endif()
endif()

if(GENERATE_DEPENDENCY_DATA)
  if(CMAKE_COMPILER_IS_GNUCC
     OR CMAKE_COMPILER_IS_GNUCXX
     OR CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
     OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
    add_compile_options(-MD)
  else()
    message(
      WARNING "Cannot generate header dependency on non GCC/Clang compilers.")
  endif()
endif()
