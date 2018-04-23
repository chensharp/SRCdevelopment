
#include <controller/controller.h>
#include <controller/actions/checkpoint_action.h>
#include <controller/actions/controller_helper.h>
#include <data/call_image.h>
#include <util/filemanager/file_system_manager.h>

#include <data/register.h>

using cppc::data::CallImage;
using cppc::data::Register;
using cppc::data::RegisterVector;

using cppc::checkpointer::Checkpoint;
using cppc::controller::Controller;
using cppc::util::filesystem::FileSystemManager;

namespace cppc {
  namespace controller {
    namespace actions {

      CheckpointAction::CheckpointAction( CheckpointCode
        e_code,Controller::ControllerState & c_state )
          : c_code( e_code ), controllerState( c_state ) {}

      CheckpointAction::CheckpointAction( const CheckpointAction & ca )
        : c_code( ca.c_code ), controllerState( ca.controllerState ) {}

      CheckpointAction & CheckpointAction::operator=(
        const CheckpointAction & rhs ) {

        if( this == &rhs ) {
          return *this;
        }

        c_code = rhs.c_code;
        controllerState = rhs.controllerState;

        return *this;
      }

      CheckpointAction::~CheckpointAction() {}

      void CheckpointAction::execute() {
        static unsigned int restart_epoch = 0;

        if( ControllerHelper::getRestartParameter() ) {
          if( !controllerState.checkpointer->moreRestart( c_code ) ) {

            ControllerHelper::setRestartParameter( false );
            restart_epoch = controllerState.epoch;
            Checkpoint * chkpt = controllerState.checkpointer->getCheckpoint();
            // No full depopulation is possible, since memory blocks have been freed when registering variables.
            // Note that not all mem blocks are deleted, only those corresponding to static memory. For this reason,
            // we just delete here the memblocks list, and not its contents.
            delete chkpt->getMemBlocks();
            delete chkpt->getContext();
            delete controllerState.checkpointer;
          }
        } else {
          if( ++controllerState.touchedCheckpoints % ControllerHelper::getFrequencyParameter() == 0 ) {
            Do_checkpoint( (controllerState.epoch-restart_epoch) % ControllerHelper::getFullFrequencyParameter() == 0 );
            ++controllerState.epoch;
          } else {
            if( ControllerHelper::getCheckpointOnFirstTouchParameter() &&
              ( c_code > controllerState.lastCheckpointTouched ) ) {

              Do_checkpoint();
              controllerState.lastCheckpointTouched = c_code;
              controllerState.epoch++;
            }
          }
        }
      }

      void CheckpointAction::Do_checkpoint( bool full ) {
        // Build the checkpoint file name
		//生成当前检查点存储目录
        string f_name = ControllerHelper::getChkptPath( controllerState.localDir, controllerState.epoch );

		//复制当前检查点，操作复制后的检查点指针
        // Create a copy of the current checkpoint skeleton, assign it as new checkpoint skeleton
        Checkpoint * chkpt = controllerState.checkpointSkeleton;
        // Rehash needs to come before the new checkpoint skeleton is created, to achieve persistence of current hash codes
        chkpt->setFullCheckpoint( full );
        chkpt->rehash();
        controllerState.checkpointSkeleton = new Checkpoint( *chkpt );

		//复制自身检查点数据,以使程序能够继续执行下去
        // Populate the skeleton. This duplicates the memory, so that the application may continue execution.
        full = chkpt->populate( c_code, f_name );

        // Checkpoint
        controllerState.checkpointer = new Checkpointer( chkpt );
        controllerState.checkpointer->checkpoint();

        if( full ) {
          // If the list is already full, remove the oldest recovery set
          controllerState.recoverySets.push_back( controllerState.epoch );
          if( controllerState.recoverySets.size() > ControllerHelper::getRecoverySetsParameter() ) {
            for( CheckpointCode i = controllerState.recoverySets[0]; i < controllerState.recoverySets[1]; ++i ) {
              string path = ControllerHelper::getChkptPath( controllerState.localDir, i );
              FileSystemManager::removeFile( path );
            }

            controllerState.recoverySets.erase( controllerState.recoverySets.begin() );
          }
        }
      }

    }
  }
}
