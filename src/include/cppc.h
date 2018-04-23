
#include <data/cppc_types.h>
#include <util/filemanager/unix_descriptor_types.h>

#define CPPC_STATIC 0
#define CPPC_DYNAMIC 1

// Library initialization
int CPPC_Init_configuration( int *, char *** );
int CPPC_Init_state();

// Context management: loops and heap
void CPPC_Add_loop_index( char *, CPPC_Datatype );
void CPPC_Context_pop();
void CPPC_Context_push( char *, unsigned int );
void CPPC_Remove_loop_index();
void CPPC_Set_loop_index( void * );

// Call image management: non-portable state
void CPPC_Commit_call_image();
void CPPC_Create_call_image( char *, unsigned int );
void * CPPC_Register_for_call_image( void *, int, CPPC_Datatype, char *,
  unsigned char );

// Checkpoint dumping
void CPPC_Do_checkpoint(unsigned char);

// Restart state check
int CPPC_Jump_next();

// Registers management: variables and files
void *CPPC_Register( void *, int, CPPC_Datatype, char *, unsigned char );
void * CPPC_Register_descriptor( int, void *, CPPC_DescriptorType, char * );
void CPPC_Unregister_descriptor( void * );
void CPPC_Unregister( char * );

// Library shutdown
void CPPC_Shutdown();
