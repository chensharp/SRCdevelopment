#if ! defined CPPC_CONTROLLER_CONTROLLER_H
#define CPPC_CONTROLLER_CONTROLLER_H

#include <checkpointer/checkpoint.h>
#include <checkpointer/checkpointer.h>
#include <data/context.h>
#include <data/cppc_file.h>
#include <data/memory_block.h>
#include <util/singleton/singleton.h>

#include <string>

using std::string;

using cppc::checkpointer::Checkpoint;
using cppc::checkpointer::CheckpointCode;
using cppc::checkpointer::Checkpointer;
using cppc::data::BlockMap;
using cppc::data::Context;
using cppc::data::CppcFile;
using cppc::data::FileMap;
using cppc::util::singleton::Singleton;

namespace cppc {
  namespace controller {

    class Controller:public Singleton<Controller> {

    public :
      typedef struct {
        
		string localDir;
		string appName;
		
        bool initConfig;
        bool initState;
        
		CheckpointCode lastCheckpointTouched;
        vector<CheckpointCode> recoverySets;
		
        unsigned int storedCheckpoints;
        unsigned int epoch;
        unsigned int touchedCheckpoints;
        
		Checkpointer * checkpointer;//指向一个检查点管理器
        
		string * loopVarName;
		
        DataType::DataTypeIdentifier loopVarType;
		
        Checkpoint * checkpointSkeleton;//检查点骨架
      } ControllerState;

      // System initialization and shutdown
      int CPPC_Init_configuration( int *, char *** );
      int CPPC_Init_state();
      int CPPC_Shutdown();

      // Data registration
      void * CPPC_Register( void *, int nelems, DataType::DataTypeIdentifier,
        string, bool ) const;
      void CPPC_Unregister( const string & ) const;

      // File registration
      CppcFile * CPPC_Register_descriptor( CppcFile::FileCode, void *,
        CppcFile::DescriptorType, string );
      void CPPC_Unregister_descriptor( void * );
      void CPPC_Update_descriptor( CppcFile::FileCode, void *,
        CppcFile::DescriptorType, string );
      void CPPC_Offset_set( CppcFile::FileCode, CppcFile::FileOffset );

      // Context management
      void CPPC_Context_push( const string &, unsigned int );
      void CPPC_Context_pop();
      void CPPC_Add_loop_index( const string &, DataType::DataTypeIdentifier );
      void CPPC_Set_loop_index( void * );
      void CPPC_Remove_loop_index();

      // Call images management
      void CPPC_Create_call_image( string, unsigned int );
      void * CPPC_Register_for_call_image( void *, int nelems,
        DataType::DataTypeIdentifier, string, bool );
      void CPPC_Commit_call_image();

      // Checkpointing
      void CPPC_Do_checkpoint( CheckpointCode ) const;

      // Conditional jump to next relevant block on restart
      bool CPPC_Jump_next() const;

    private:
      Controller();
      Controller(const Controller&);
      ~Controller();

      // Members
      ControllerState state;

      friend class Singleton<Controller>;
    };
  }
}

#endif
