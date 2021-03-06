//    This file is part of CPPC.
//
//    CPPC is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    CPPC is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with CPPC; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



#if ! defined CPPC_CONTROLLER_ACTIONS_REGISTER_DESCRIPTOR_ACTION_H
#define CPPC_CONTROLLER_ACTIONS_REGISTER_DESCRIPTOR_ACTION_H

#include <cppc.h>
#include <controller/controller.h>
#include <data/cppc_file.h>

#include <string>

using cppc::controller::Controller;
using cppc::data::CppcFile;

using std::string;

namespace cppc {
  namespace controller {
    namespace actions {

      class RegisterDescriptorAction {

      public:
        RegisterDescriptorAction( CppcFile::FileCode, void *,
          CppcFile::DescriptorType, string, Controller::ControllerState & );
        ~RegisterDescriptorAction();

        CppcFile * execute();

      private:
        RegisterDescriptorAction( const RegisterDescriptorAction & );
        RegisterDescriptorAction & operator=(
          const RegisterDescriptorAction & );

        CppcFile::FileCode code;
        void * descriptor;
        CppcFile::DescriptorType type;
        string path;
        Controller::ControllerState & controllerState;
      };

    }
  }
}

#endif
