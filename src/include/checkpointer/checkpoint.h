
#if ! defined CPPC_CHECKPOINTER_CHECKPOINT_H
#define CPPC_CHECKPOINTER_CHECKPOINT_H

#include <compressor/compressor.h>
#include <data/context.h>
#include <data/cppc_basic.h>
#include <data/cppc_file.h>
#include <data/memory_block.h>
#include <data/register.h>
#include <writer/writer.h>

#include <string>

using std::string;

using cppc::compressor::CompressorType;
using cppc::data::BasicUIntegerType;
using cppc::data::Context;
using cppc::data::FileMap;
using cppc::data::BlockMap;
using cppc::data::Register;
using cppc::data::UIntegerType;
using cppc::writer::WriterType;

namespace cppc {
  namespace checkpointer {

    typedef BasicUIntegerType BasicCheckpointCode;
    typedef UIntegerType CheckpointCode;

    class Checkpoint {

    public:
      Checkpoint( MemoryBlock::MemoryType = MemoryBlock::fragmentSize );
      Checkpoint( WriterType, CompressorType, Context * );
      Checkpoint( const Checkpoint & );

      ~Checkpoint();

	  void rehash();
      bool populate( CheckpointCode, string& ); // Called before checkpointing to fill all checkpoint data
      void depopulate(); // Called after checkpointing to free unused memory
      void unpack();
      void update( Checkpoint * );
	  
      inline void setWriterType( WriterType wt ) { writerType = wt; }
      inline void setCompressorType( CompressorType ct ) { compressorType = ct; }
      inline void setCheckpointCode( CheckpointCode cc ) { checkpointCode = cc; }
      inline void setContext( Context * c ) { context = c; }
      inline void setFiles( FileMap * e_files ) { fileMap = e_files; }
      inline void setMemBlocks( BlockMap * e_blocks ) { memBlocks = e_blocks; }
      inline void setPath( string path ) { this->path = path; }
      inline void setFullCheckpoint( bool full ) { this->isFull = full; }

      inline WriterType getWriterType() { return writerType; }
      inline CompressorType getCompressorType() { return compressorType; }
      inline CheckpointCode getCheckpointCode() { return checkpointCode; }
      inline Context * getContext() { return context; }
      inline BlockMap * getMemBlocks() { return memBlocks; }
      inline FileMap * getFiles() { return fileMap; }
      inline string getPath() { return path; }
      inline bool isFullCheckpoint() { return isFull; }
      inline MemoryBlock::MemoryType getFragmentSize() { return fragmentSize; }

      inline void addressesToIndependentOffsets() {
        this->addressesToIndependentOffsets( this->context );
      }

      inline void independentOffsetsToAddresses() {
        this->independentOffsetsToAddresses( this->context );
      }

    private:
      Checkpoint & operator=( const Checkpoint & );

      void addressesToIndependentOffsets( Context * );
      void addressesToIndependentOffsets( Register *, BlockMap * );
      void independentOffsetsToAddresses( Context * );
      void independentOffsetsToAddresses( Register *, BlockMap * );

      WriterType writerType;//
      CompressorType compressorType;
      CheckpointCode checkpointCode;//检查点序号
      Context * context;//上下文
      FileMap * fileMap;//文件map
      BlockMap * memBlocks;//内存块
      string path;//地址
      bool isFull;//是否完全检查点
      MemoryBlock::MemoryType fragmentSize;//分块的尺寸
    };

  }
}

#endif
