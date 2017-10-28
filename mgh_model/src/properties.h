#ifndef _SYSTEM_PROPERTIES
#define _SYSTEM_PROPERTIES
#include "curator.h"
#include "rng_management.h"
#include "microtubule_management.h"
#include "kinesin_management.h"
#include "associated_protein_management.h"

typedef struct system_properties{

	Curator wallace;
	RandomNumberManagement gsl; 
	MicrotubuleManagement microtubules;
	KinesinManagement kinesin4; 
	AssociatedProteinManagement prc1; 
	
	int current_step_;
	FILE *occupancy_file_, *motor_ID_file_, *xlink_ID_file_;
	
} system_properties;
#endif

