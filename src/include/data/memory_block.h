
#if ! defined CPPC_DATA_MEMORYBLOCK_H

#define CPPC_DATA_MEMORYBLOCK_H

#include <data/cppc_basic.h>
#include <data/cppc_types.h>

#include <openssl/sha.h>

#include <map>
#include <vector>

using std::map;
using std::vector;

namespace cppc {
  namespace data {

    class Register;
    typedef vector<Register *> RegisterVector;

    class MemoryBlock {
    public:
      typedef BasicUCharacterType BlockCode;
      typedef UCharacterType BlockCodeType;
      typedef BasicULongType MemoryType;
      typedef ULongType MemoryTypeType;
      typedef unsigned char SHA1Type[SHA_DIGEST_LENGTH];
      typedef struct {
        unsigned long fragmentCode;
        MemoryType fragmentSize;
        MemoryType fragmentAddress; // Used during checkpoint operation to indicate whether fragment is zeros
      } BlockFragment;
      const static MemoryType fragmentSize;

      MemoryBlock( MemoryType, MemoryType, DataType::DataTypeIdentifier, RegisterVector * = new RegisterVector() );
      MemoryBlock( BlockCode, MemoryType, MemoryType, DataType::DataTypeIdentifier, RegisterVector * = new RegisterVector() );
      ~MemoryBlock();

      MemoryBlock * deepClone(); // Clones this block, including the memory, and returns a copy with its memInit/memEnd modified.

      inline BlockCode getCode() { return code; }
      inline MemoryType getInitAddr() { return memInit; }
      inline MemoryType getEndAddr() { return memEnd; }
      inline DataType::DataTypeIdentifier getType() { return type; }
      inline RegisterVector * getRegisters() { return registers; }
      inline bool getFullStore() { return fullStore; }
      inline vector<BlockFragment> & getFragments() { return fragments; }

      inline void setInitAddr( MemoryType addr ) { memInit = addr; this->invalidateAll(); }
      inline void setEndAddr( MemoryType addr ) { memEnd = addr; this->invalidateAll(); }
      inline void setLimits( MemoryType lAddr, MemoryType rAddr ) { memInit = lAddr; memEnd = rAddr; this->invalidateAll(); }
      inline void setFragments( vector<BlockFragment> & f ) { fragments = f; fullStore = false; }

      // Calculate fragments that need to be dumped. Store them in the fragments variable.
      void prepareForCheckpoint( bool = true );
      void unpack( MemoryType );
      void update( MemoryBlock *, MemoryType );

      inline static void freeStaticMemory() { delete [] zeroHash; }

    private:
      MemoryBlock();
      MemoryBlock( const MemoryBlock & );
      MemoryBlock & operator=( const MemoryBlock & );

      void rehash( MemoryType, unsigned );

      inline void invalidateAll() { this->invalidateHashCodes(); this->invalidateFragments(); fullStore = true; }
      inline void invalidateHashCodes() { if( hashCodes ) { delete hashCodes; hashCodes = 0; } }
      inline void invalidateFragments() { fragments.clear(); }

      BlockCode code;
      MemoryType memInit;
      MemoryType memEnd;
      DataType::DataTypeIdentifier type;
      RegisterVector * registers;
      SHA1Type * hashCodes;
      vector<BlockFragment> fragments;
      bool fullStore;
      static BlockCode nextCode;
      static const unsigned char * zeroHash;
    };

    typedef map<MemoryBlock::BlockCode,MemoryBlock *> BlockMap;
  }
}

#endif
