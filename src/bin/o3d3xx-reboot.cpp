/*
 * Copyright (C) 2014 Love Park Robotics, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distribted on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include "o3d3xx.h"

//
// Soft reboot of the camera
//

int main(int argc, const char** argv)
{
  std::string camera_ip;
  uint32_t xmlrpc_port;
  std::string password;

  try
    {
      o3d3xx::CmdLineOpts opts("o3d3xx Reboot");
      if (! opts.Parse(argc, argv, &camera_ip, &xmlrpc_port, &password))
        {
          return 0;
        }

      o3d3xx::Camera::Ptr cam =
        std::make_shared<o3d3xx::Camera>(camera_ip, xmlrpc_port, password);

      cam->Reboot();
    }
  catch (const std::exception& e)
    {
      std::cerr << "Failed to reboot camera: " << e.what() << std::endl;
      return 1;
    }

  return 0;
}
