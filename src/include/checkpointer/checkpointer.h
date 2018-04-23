

#if ! defined CPPC_CHECKPOINTER_CHECKPOINTER_H
#define CPPC_CHECKPOINTER_CHECKPOINTER_H

#include <checkpointer/checkpoint.h>
#include <data/register.h>
#include <util/configuration/configuration_manager.h>

//FIXME: In C++11 this should be implemented using std::thread.
#include <pthread.h>

using cppc::data::Register;
using cppc::util::configuration::ParameterKeyType;

namespace cppc {
  namespace checkpointer {

    class Checkpointer {

    public:
      Checkpointer();
      Checkpointer( Checkpoint * );
      ~Checkpointer();

      //Check data correctness检查数据正确性
      static bool integrityTest( string );
      static bool isFullCheckpoint( string );

      //Read file contents into memory for incremental restart
	  //读取文件内容到内存中去，目的是为了增量重启
      CheckpointCode readCheckpoint( string );

	  //主操作
      void checkpoint();

      // Recovers a single variable
	  //恢复单个变量
      void * partialRestart( const string &, unsigned int );
		
		//返回检查点指针
      inline Checkpoint * getCheckpoint() { return chkpt; }
      //是否更多重启？？？
	  bool moreRestart( CheckpointCode& );

      // Wait for last thread before allowing end
	  //在允许结束之前等待最后一个进程
      static void Shutdown();

    private:
      Checkpointer(const Checkpointer&);
      Checkpointer& operator=(const Checkpointer&);

      // Members
	  //检查点指针
      Checkpoint * chkpt; // Checkpoint to write
      //上一个执行的线程
	  static pthread_t * last_thread; // Last executed thread (only in multithreaded execution)

	  //线程函数
      // Friend multithread function
      friend void * thread_run( void * );
    };
  }
}

#endif
