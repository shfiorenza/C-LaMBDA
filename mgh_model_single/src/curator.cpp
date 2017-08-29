#include "master_header.h"
#include "curator.h"

Curator::Curator(){
}

void Curator::Initialize(system_parameters *parameters, 
						 system_properties *properties){

	parameters_ = parameters;
	properties_ = properties;
}

void Curator::OutputSimDetails(){

	int n_steps = parameters_->n_steps;
	double k_on = parameters_->k_on;
	double c_motor = parameters_->c_motor;
	double k_off = parameters_->k_off;
	double switch_rate = parameters_->switch_rate;
	double motor_speed = parameters_->motor_speed;
	double delta_t = parameters_->delta_t;
	double p_bind = k_on*c_motor*delta_t;
	double p_unbind = k_off*delta_t;
	double p_switch = switch_rate*delta_t;
	double p_step = motor_speed*delta_t;
    printf("Total simulation duration: %g seconds\n", delta_t*n_steps);
    printf("Timestep duration: %g seconds\n\n", delta_t);
    printf("Motor binding frequency: %g per timestep\n", p_bind);
    printf("Motor unbinding frequency: %g per timestep\n", p_unbind);
    printf("Motor switching frequency: %g per timestep\n", p_switch);
    printf("Motor stepping frequency: %g sites per timestep\n\n", p_step);
    fflush(stdout);
}

void Curator::PrintMicrotubules(){

    int n_mts = parameters_->n_microtubules;
    int mt_length = parameters_->length_of_microtubule;
    for(int i_mt = 0; i_mt < n_mts; i_mt++){
		Microtubule *mt = &properties_->microtubules.mt_list_[i_mt];
        for(int i_site = 0; i_site < mt_length; i_site++){
            if(mt->lattice_[i_site].occupied_ == false)
                printf("=");
			else if(mt->lattice_[i_site].xlink_ != nullptr)
				printf("X");
            else if(mt->lattice_[i_site].motor_ != nullptr)
                printf("M");
        }
		printf(" %i\n", mt->polarity_);
    }   
	printf("\n");
}

void Curator::OutputData(FILE *occupancy_file, FILE *ID_file){
	
	int n_mts = parameters_->n_microtubules;
	int mt_length = parameters_->length_of_microtubule;
	for(int i_mt = 0; i_mt < n_mts; i_mt++){
		Microtubule *mt = &properties_->microtubules.mt_list_[i_mt];
		int occupancy_array[mt_length];
		int *occupancy_ptr = occupancy_array;
		int ID_array[mt_length];
		int *ID_ptr = ID_array;
		for(int i_site = 0; i_site < mt_length; i_site++){
			Tubulin *site = &mt->lattice_[i_site];
			// If unoccupied, save occupancy of '0' and ID of -1 (null)
			if(site->occupied_ == false){
				occupancy_array[i_site] = 0;
				ID_array[i_site] = -1;
			}
			// If occupied by xlink, save occupancy of '1' and ID of the xlink
			else if(site->xlink_ != nullptr){
				occupancy_array[i_site] = 1;
//				ID_array[i_site] = site->xlink_->ID_;
			}
			// If occupied by motor, save occupancy of '2' and ID of motor
			else if(site->motor_ != nullptr){
				occupancy_array[i_site] = 2;
				ID_array[i_site] = site->motor_->ID_;
			}
		}
		// Write the data to respective files one microtubule at a time
		fwrite(occupancy_ptr, sizeof(int), mt_length, occupancy_file);
		fwrite(ID_ptr, sizeof(int), mt_length, ID_file);
	}	
}