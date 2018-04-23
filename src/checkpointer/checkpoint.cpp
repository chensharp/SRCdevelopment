
#include <checkpointer/checkpoint.h>
#include <data/call_image.h>
#include <data/cppc_file.h>
#include <data/memory_block.h>

#include <cassert>
#include <cstring>

using cppc::data::CallImage;
using cppc::data::CppcFile;
using cppc::data::RegisterVector;

namespace cppc {
  namespace checkpointer {


    Checkpoint::Checkpoint( MemoryBlock::MemoryType fs )
      : writerType( 0 ), compressorType( 0 ), checkpointCode( 0 ),
      context( 0 ), fileMap( new FileMap() ), memBlocks( new BlockMap() ),
      path( "" ), isFull( true ), fragmentSize( fs ) {}

    Checkpoint::Checkpoint( WriterType wt, CompressorType ct, Context * c )
    : writerType( wt ), compressorType( ct ), checkpointCode( -1 ), context( c ),
      fileMap( new FileMap() ), memBlocks( new BlockMap() ), path( "" ), isFull( true ),
      fragmentSize( MemoryBlock::fragmentSize ) {}

    // This function is invoked to create a new skeleton checkpoint from the old one upon checkpointing.
    // Memory splitting is responsibility of the checkpointer
    Checkpoint::Checkpoint( const Checkpoint & chkpt )
      : writerType( chkpt.writerType ), compressorType( chkpt.compressorType ),
      checkpointCode( chkpt.checkpointCode ), context( chkpt.context ),
      fileMap( new FileMap() ), memBlocks( chkpt.memBlocks ), path( "" ), isFull( true ),
      fragmentSize( MemoryBlock::fragmentSize ) {
      for( FileMap::const_iterator i = chkpt.fileMap->begin(); i != chkpt.fileMap->end(); ++i ) {
        fileMap->insert( FileMap::value_type( i->first, new CppcFile( *i->second ) ) );
      }
    }

    Checkpoint & Checkpoint::operator=( const Checkpoint & rhs ) {

      if( this == &rhs ) {
        return *this;
      }

      writerType = rhs.writerType;
      compressorType = rhs.compressorType;
      checkpointCode = rhs.checkpointCode;
      fragmentSize = rhs.fragmentSize;

      // Memory deleting is responsability of the checkpointer
      // Memory splitting is responsability of the checkpointer
      context = rhs.context;

      fileMap = rhs.fileMap;

      path = rhs.path;

      return *this;
    }

    Checkpoint::~Checkpoint() {
      // Do not delete memory blocks or context. These are created/erased by
      // populate() / depopulate()

      for( FileMap::iterator i = fileMap->begin(); i != fileMap->end(); ++i ) {
        delete i->second;
      }

      delete fileMap;
    }

    void Checkpoint::rehash() {
      for( BlockMap::const_iterator i = this->memBlocks->begin(); i != this->memBlocks->end(); ++i ) {
        MemoryBlock * b = i->second;
        b->prepareForCheckpoint( isFull );
      }
    }

	//增加
    bool Checkpoint::populate( CheckpointCode code, string & path ) {
      this->checkpointCode = code;
      this->path = path;//传入地址、code数据

      // Set "checkpoint here" in context
      Context * saved = context;//复制当前context
      saved->setCheckpointHere( true );

      // Clone context hierarchy.
      while( context->getParent() != 0 ) {
        context = context->getParent();
      }

      this->context = context->clone();
      this->addressesToIndependentOffsets();

      // Set "checkpoint here" to false again in the saved context (used by the cloned checkpoint which is now the skeleton checkpoint)
      saved->setCheckpointHere( false );

      // Rehash memory. If scheduled for dumping, create a clone of the blockmap. Else, remove blockmap.
      BlockMap * newBlocks = new BlockMap();
      for( BlockMap::const_iterator i = this->memBlocks->begin(); i != this->memBlocks->end(); ++i ) {
        MemoryBlock * newBlock = i->second->deepClone();
        newBlocks->insert( BlockMap::value_type( newBlock->getCode(), newBlock ) );
      }

      this->memBlocks = newBlocks;

      return this->isFull;
    }

	//减少
    void Checkpoint::depopulate() {
      for( BlockMap::iterator i = memBlocks->begin(); i != memBlocks->end(); ++i ) {
        MemoryBlock * b = i->second;
        unsigned char * copy = reinterpret_cast<unsigned char *>( b->getInitAddr() );
        delete [] copy;
        delete b;
      }

      delete memBlocks;
      delete context;

      // The FileMap will be deleted when deleting the checkpoint object.
    }

    void Checkpoint::addressesToIndependentOffsets( Context * c ) {
      RegisterVector * regs = c->getRegisters();
      if( regs != 0 ) {
        for( RegisterVector::iterator it = regs->begin(); it != regs->end(); ++it ) {
          this->addressesToIndependentOffsets( *it, this->memBlocks );
        }
      }

      // Process call images
      for( vector<CallImage*>::iterator it = c->getCallImages()->begin(); it != c->getCallImages()->end(); ++it ) {
        BlockMap * blocks = (*it)->getBlocks();
        for( vector<Register*>::iterator itP = (*it)->getParameters()->begin(); itP != (*it)->getParameters()->end(); ++itP ) {
          this->addressesToIndependentOffsets( *itP, blocks );
        }
      }

      // Process subcontexts
      for( vector<Context*>::iterator it = c->getSubcontexts()->begin(); it != c->getSubcontexts()->end(); ++it ) {
        this->addressesToIndependentOffsets( *it );
      }
    }

	//地址转为独立地址
    void Checkpoint::addressesToIndependentOffsets( Register * r, BlockMap * blocks ) {
      BlockMap::iterator it = blocks->find( r->getCode() );//取出块
      assert( it != blocks->end() );//不能为空
      MemoryBlock * b = it->second;//取出block
      MemoryBlock::MemoryType regInit =
        reinterpret_cast<MemoryBlock::MemoryType>( r->initAddress() );
      MemoryBlock::MemoryType regEnd =
        reinterpret_cast<MemoryBlock::MemoryType>( r->endAddress() );
      MemoryBlock::MemoryType initOffset = regInit - b->getInitAddr();
      MemoryBlock::MemoryType endOffset = regEnd - b->getInitAddr();

      r->setInitAddress( reinterpret_cast<void *>( initOffset ) );
      r->setEndAddress( reinterpret_cast<void *>( endOffset ) );
    }

    void Checkpoint::independentOffsetsToAddresses( Context * c ) {
      RegisterVector * regs = c->getRegisters();
      if( regs != 0 ) {
        for( RegisterVector::iterator it = regs->begin(); it != regs->end(); ++it ) {
          this->independentOffsetsToAddresses( *it, this->memBlocks );
        }
      }

      // Process call images
      vector<CallImage*> * callIms = c->getCallImages();
      for( vector<CallImage*>::iterator it = callIms->begin(); it != callIms->end(); ++it ) {
        CallImage * callIm = *it;
        BlockMap * blocks = callIm->getBlocks();
        regs = callIm->getParameters();
        for( RegisterVector::iterator itP = regs->begin(); itP != regs->end(); ++itP ) {
          this->independentOffsetsToAddresses( *itP, blocks );
        }
      }

      // Process subcontexts
      vector<Context*> * subcontexts = c->getSubcontexts();
      for( vector<Context*>::iterator it = subcontexts->begin(); it != subcontexts->end(); ++it ) {
        this->independentOffsetsToAddresses( *it );
      }
    }

    void Checkpoint::independentOffsetsToAddresses( Register * r, BlockMap * blocks ) {
      BlockMap::iterator it = blocks->find( r->getCode() );
      assert( it != blocks->end() );
      MemoryBlock * b = it->second;
      MemoryBlock::MemoryType initOffset =
          reinterpret_cast<MemoryBlock::MemoryType>( r->initAddress() );
      MemoryBlock::MemoryType endOffset =
          reinterpret_cast<MemoryBlock::MemoryType>( r->endAddress() );
      MemoryBlock::MemoryType regInit = b->getInitAddr() + initOffset;
      MemoryBlock::MemoryType regEnd = b->getInitAddr() + endOffset;

      r->setInitAddress( reinterpret_cast<void *>( regInit ) );
      r->setEndAddress( reinterpret_cast<void *>( regEnd ) );
    }

    void Checkpoint::unpack() {
      // This function unpacks zero-blocks that may be contained in full checkpoints.
      assert( this->isFull );

      for( BlockMap::iterator i = this->memBlocks->begin(); i != this->memBlocks->end(); ++i ) {
        i->second->unpack( this->fragmentSize );
      }
    }

    void Checkpoint::update( Checkpoint * rhs ) {
      // Update the context tree
      context->update( rhs->context, *memBlocks, *rhs->memBlocks, rhs->fragmentSize );

      // Update the files map
      delete fileMap;
      fileMap = new FileMap();
      for( FileMap::iterator i = rhs->fileMap->begin(); i != rhs->fileMap->end(); ++i ) {
        fileMap->insert( FileMap::value_type( i->second->get_file_code(), new CppcFile( *i->second ) ) );
      }

      // Update the checkpoint code
      this->checkpointCode = rhs->checkpointCode;
    }

  }
}
