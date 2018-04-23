
#if ! defined CPPC_CONTROLLER_ACTIONS_CHECKPOINT_ACTION_H
#define CPPC_CONTROLLER_ACTIONS_CHECKPOINT_ACTION_H

#include <checkpointer/checkpoint.h>
#include <controller/controller.h>
#include <data/context.h>
#include <data/memory_block.h>
#include <data/register.h>

#include <pthread.h>

using cppc::data::MemoryBlock;

using cppc::controller::Controller;
using cppc::checkpointer::CheckpointCode;
using cppc::data::Context;
using cppc::data::Register;

namespace cppc {
  namespace controller {
    namespace actions {

      class CheckpointAction {

      public:
        CheckpointAction( CheckpointCode, Controller::ControllerState & );
        ~CheckpointAction();

        void execute();

      private:
        CheckpointAction( const CheckpointAction & );
        CheckpointAction & operator=( const CheckpointAction & );

        void Do_checkpoint( bool = true );

        CheckpointCode c_code;
        Controller::ControllerState & controllerState;
      };

    }
  }
}

#endif
