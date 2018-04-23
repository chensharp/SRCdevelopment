#include <checkpointer/checkpointer.h>
#include <data/call_image.h>
#include <data/memory_block.h>
#include <data/register.h>
#include <writer/writer.h>
#include <writer/writer_factory.h>
#include <util/filemanager/file_system_manager.h>

#include <cassert>
#include <cstring>
#include <fstream>

using std::ifstream;

using cppc::data::CallImage;
using cppc::data::MemoryBlock;
using cppc::data::Register;
using cppc::data::RegisterVector;
using cppc::util::configuration::ConfigurationManager;
using cppc::util::configuration::ParameterValueType;
using cppc::writer::Writer;
using cppc::writer::WriterFactory;
using cppc::util::filesystem::FileSystemManager;

namespace cppc {
  namespace checkpointer {

  //赋初值
    void * thread_run( void * );
    pthread_t *Checkpointer::last_thread = NULL;

    Checkpointer::Checkpointer()
      : chkpt( new Checkpoint() ) {}

    Checkpointer::Checkpointer( Checkpoint * c )
      : chkpt( c ) {}

    Checkpointer::~Checkpointer() {
      delete chkpt;
    }

    Checkpointer::Checkpointer( const Checkpointer & c_check )
      : chkpt( c_check.chkpt ) {}

    Checkpointer& Checkpointer::operator=(const Checkpointer &c_check) {

      if(this == &c_check) {
        return(*this);
      }

      chkpt = c_check.chkpt;

      return *this;
    }

	//检查点执行
    void Checkpointer::checkpoint() {
		//不能为空
      assert( chkpt->getContext() != 0 );
      // Wait for last thread to exit (it's no good to have multiple
      // checkpoints being written to memory at once)
	  //等待上一个线程结束，不能多个线程他同时写内存，会冲突
      if( last_thread != NULL ) {//
        pthread_join( *last_thread, NULL );//以阻塞的方式等待上一线程结束
      } else {
        last_thread = new pthread_t;
      }
		//创建线程
      pthread_create( last_thread, 0, &thread_run, this );
    }

	//文件完整性检查
    bool Checkpointer::integrityTest( string path ) {
      WriterType writerType;

      // 检查文件是否存在
      if( !FileSystemManager::fileExists( path ) ) {
        return false;
      }
      // 文件大小为空也失败
      if( FileSystemManager::getFileSize( path ) == 0 ) {
        return false;
      }

	  //文件第一个byte包含格式。。
      // The first byte of the file must contain the writer type
      ifstream file( path.c_str(), std::ios::in );
      assert( file.is_open() );
      file.read( reinterpret_cast<char *>( writerType.getValue() ) ,
        writerType.getSize() );
      file.close();

      // Try to obtain the writer to read the file
      Writer * w = WriterFactory::instance().getWriter( writerType.getStaticValue() );
      if( !w ) {
        return false;
      }

      bool ret = w->testCheckpoint( path );//

      delete w;
      return ret;
    }

    bool Checkpointer::isFullCheckpoint( string path ) {
      // The second byte of the file must contain whether this is a full checkpoint
      ifstream file( path.c_str(), std::ios::in );
      assert( file.is_open() );
      file.seekg( 1 );//设置文件指针的偏移位置，读第二个位置
      char full;
      file.read( &full, 1 );

      return full;
    }

	//读检查点
    CheckpointCode Checkpointer::readCheckpoint( string path ) {
      WriterType writerType;

      ifstream file( path.c_str(), std::ios::in );
      assert( file.is_open() );
      file.read( reinterpret_cast<char *>( writerType.getValue() ),
        WriterType::staticSize() );

      file.close();

      this->chkpt = WriterFactory::instance().getWriter( writerType.getStaticValue() )->readCheckpoint( path );
      return chkpt->getCheckpointCode();
    }

	//该函数供恢复模式里实现
	//从context里恢复一个变量，输入变量名和长度，成功了返回首地址
    void * Checkpointer::partialRestart( const string & varName, unsigned int bytes ) {

      Context * context = chkpt->getContext();
      if( context == 0 ) {
        return 0;
      }

      Register * r = context->getRegister( varName );
      if( r == 0 ) {
        // Register not found in this context
        return 0;
      }

      context->removeRegister( varName );

      MemoryBlock::MemoryType bytesOrig =
        reinterpret_cast<MemoryBlock::MemoryType>( r->endAddress() ) -
        reinterpret_cast<MemoryBlock::MemoryType>( r->initAddress() );

      assert( bytes == bytesOrig );

      return r->initAddress();
    }

    bool Checkpointer::moreRestart( CheckpointCode & code ) {
      return( !chkpt->getContext()->getCheckpointHere() ||
        (code.getStaticValue() != chkpt->getCheckpointCode().getStaticValue()) );
    }

	//线程执行函数
    void *thread_run( void *e_checkpointer ) {

      Checkpointer * checkpointer = static_cast<Checkpointer *>(
        e_checkpointer );

      Checkpoint * checkpoint = checkpointer->chkpt;

      Writer * w = WriterFactory::instance().getWriter(
        checkpoint->getWriterType() );

      w->write( checkpoint );//内存数据保存在checkpoint里，写入到文件

      checkpoint->depopulate();//删除该检查点，
      delete w;
      delete checkpointer;

      pthread_exit( NULL );
      return 0;
    }

    void Checkpointer::Shutdown() {
      if (last_thread != NULL ) {//非空则等待线程结束
        pthread_join( *last_thread, NULL );
      }
    }
  }
}
