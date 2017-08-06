/* Kinetic Monte Carlo algorithms */

#include "master_header.h"

void motors_bind(system_parameters *parameters, microtubule *mt_array, std::vector<motor> &motor_list, std::vector<motor*> &bound_list, std::vector<tubulin*> &unbound_list, gsl_rng *rng){

	int length_of_microtubule = parameters->length_of_microtubule;
	int n_microtubules = parameters->n_microtubules;

	// Check to make sure at least one unbound site exists
	if(unbound_list.empty() != true){
	
		int unbound_index, site_coord, mt_index;
		int n_attempts = 0;
		bool failure = false;

		// Ensure (within reason) that boundary sites aren't included
		do{	if(n_attempts > 0){
				failure = true;
				break;
			}
			// Randomly pick an unbound list entry, then get the corresponding site coordinate and MT index
			unbound_index = gsl_rng_uniform_int(rng, unbound_list.size());
			site_coord = unbound_list[unbound_index]->site_coord;
			mt_index = unbound_list[unbound_index]->mt_index;
			n_attempts++;
		}while(site_coord == 0 || site_coord == (length_of_microtubule -1));

		// Verify that an eligible site index was found
		if(failure != true){
			// Verify that site is actually unoccupied 
			if(mt_array[mt_index].lattice[site_coord].occupant != NULL){
				printf("Error in motor binding code at site %i on microtubule %i.\n", site_coord, mt_index);
				exit(1); 
			}
			// Find a motor to bind
			int motor_index = 0;
			while(motor_list[motor_index].bound == true){
				motor_index++;
			}
			// Update motor details
			motor_list[motor_index].bound = true;
			motor_list[motor_index].mt_index = mt_index;
			motor_list[motor_index].site_coord = site_coord;
			motor_list[motor_index].motor_index = motor_index;
			// Update microtubule details
			mt_array[mt_index].lattice[site_coord].occupant = &motor_list[motor_index];
			mt_array[mt_index].n_bound++;
			// Remove pointer to this site from unbound_list
			unbound_list.erase(unbound_list.begin() + unbound_index);
			// Get pointer to this motor, then add it to bound_list
			motor* motor_address = &motor_list[motor_index];
			bound_list.push_back(motor_address);
		}
		// Announce if an eligible site index was NOT found
		else{
			printf("gnarly bro; failed to bind @ %i_%i!\n", mt_index, site_coord);
		}
	}
}

void motors_unbind(system_parameters *parameters, microtubule *mt_array, std::vector<motor> &motor_list, std::vector<motor*> &bound_list, std::vector<tubulin*> &unbound_list, gsl_rng *rng){

	int length_of_microtubule = parameters->length_of_microtubule;
	int n_microtubules = parameters->n_microtubules;

	// Check to make sure at least one bound motor exists
	if(bound_list.empty() != true){
	
		int bound_index, site_coord, mt_index;
		int n_attempts = 0;
		bool failure = false;

		// Ensure (within reason) that boundary sites aren't included
		do{	if(n_attempts > bound_list.size()){
				failure = true;
				break;
			}
			// Randomly pick a bound list entry, then get the corresponding site coordinate and MT index
			bound_index = gsl_rng_uniform_int(rng, bound_list.size());
			site_coord = bound_list[bound_index]->site_coord;
			mt_index = bound_list[bound_index]->mt_index;
			n_attempts++;

		}while(site_coord == 0 || site_coord == (length_of_microtubule -1));

		// Verify that an eligible bound motor was found
		if(failure != true){
			// Verify that this site is actually occupied 
			if(mt_array[mt_index].lattice[site_coord].occupant == NULL){
				printf("Error in motor unbinding code at site %i on microtubule %i.\n", site_coord, mt_index);
				exit(1); 
			}
 
			// Get the index in motor_list that corresponds to this motor
			int motor_index = bound_list[bound_index]->motor_index;			
			// Update motor details
			motor_list[motor_index].bound = false;
			// Update microtubule details
			mt_array[mt_index].lattice[site_coord].occupant = NULL;
			mt_array[mt_index].n_bound--;
			// Remove pointer to this motor from bound_list
			bound_list.erase(bound_list.begin() + bound_index);
			// Get pointer to this site, then add it to unbound_list
			tubulin	*site_address = &mt_array[mt_index].lattice[site_coord];
			unbound_list.push_back(site_address);
		}
		// Announce if an eligible site index was NOT found
		else{
			printf("gnarly bro; failed to unbind @ %i_%i!\n", mt_index, site_coord);
		}
	}
}
/*
void motors_switch(system_parameters *parameters, microtubule *mt_array, std::vector<motor> &motor_list, std::vector<motor*> &bound_list, std::vector<tubulin*> &unbound_list, gsl_rng *rng){
	
	int n_microtubules = parameters->n_microtubules;
	int length_of_microtubule = parameters->length_of_microtubule;

	if(bound_list.empty() != true){

		// Randomly pick a list entry and get the corresponding bound motor coordinates
		int bound_index, site_coord, mt_index, mt_index_adj;

		int n_attempts = 0;
		bool failure = false;
		// Ensures (within reason) that we obtain the index of a switchable motor that isn't on the boundary
		do{	if(n_attempts > 2*bound_list.size()){
				failure = true;
				break;
			}
			bound_index = gsl_rng_uniform_int(rng, bound_list.size());
			site_coord = bound_list[bound_index]->site_coord;
			mt_index = bound_list[bound_index]->mt_index;
			mt_index_adj = mt_array[mt_index].mt_index_adj;
			n_attempts++;

		}while(mt_array[mt_index_adj].lattice[site_coord] != NULL || site_coord == 0 || (site_coord == length_of_microtubule - 1));
		
		// Only attempt to switch if a suitable motor was found
		if(failure != true){
			// Verifies there is a motor capable of switching
			if(mt_array[mt_index].lattice[site_coord] == NULL){
				printf("Error in motor switching code at site %i on microtubule %i.\n", site_coord, mt_index);
				exit(1);
			}
			// Verifies that neighbor site on adjacent MT is empty
			if(mt_array[mt_index_adj].lattice[site_coord] != NULL){
				printf("Error in motor switching code (type two) at site %i on microtubule %i.\n", site_coord, mt_index);
				exit(1);
			}

			// Find motor list entry that corresponds to this motor
			int motor_index = bound_list[bound_index]->motor_index;
			// Get old global coordinate of motor
			int old_global_coord = motor_list[motor_index].global_coord;
			// Calculate new global coordinate of motor
			int new_global_coord = mt_index_adj*length_of_microtubule + site_coord;
			// Update motor
			motor_list[motor_index].mt_index = mt_index_adj;
			motor_list[motor_index].global_coord = new_global_coord;
			// Update microtubules
			mt_array[mt_index].lattice[site_coord] = NULL;
			mt_array[mt_index].n_bound--;
			mt_array[mt_index_adj].lattice[site_coord] = &motor_list[motor_index];
			mt_array[mt_index_adj].n_bound++;
			// Remove new global coordinate from unbound_list
			auto unbound_entry = std::find(unbound_list.begin(), unbound_list.end(), new_global_coord);
			int unbound_index = std::distance(unbound_list.begin(), unbound_entry);
			unbound_list.erase(unbound_list.begin() + unbound_index);
			// Add old global coordinate to unbound_list
			unbound_list.push_back(old_global_coord);
		}
		// Announce if an eligible site index was NOT found
		else{
			printf("gnarly bro; failed to switch @ %i_%i!\n", mt_index, site_coord);
		}
	}
}
*/ 
void motors_move(system_parameters *parameters, microtubule *mt_array, std::vector<motor> &motor_list, std::vector<motor*> &bound_list, std::vector<tubulin*> &unbound_list, gsl_rng *rng){

	int n_microtubules = parameters->n_microtubules;
	int length_of_microtubule = parameters->length_of_microtubule;

	if(bound_list.empty() != true){

		int bound_index, site_coord, mt_index, plus_end, minus_end, delta_x;
		
		int n_attempts = 0;
		bool failure = false;
		// Ensure (within reason) that a motor capable of stepping is found
		do{ if(n_attempts > 3*bound_list.size()){
				failure = true;
				break;
			}
			bound_index = gsl_rng_uniform_int(rng, bound_list.size());
			site_coord = bound_list[bound_index]->site_coord;
			mt_index = bound_list[bound_index]->mt_index;
			plus_end = mt_array[mt_index].plus_end;
			minus_end = mt_array[mt_index].minus_end;
			delta_x = mt_array[mt_index].delta_x;
			n_attempts++;
			// Ensure (within reason) that the plus_end is not included (i.e. enforce end-pausing)
			while(site_coord == plus_end){
				if(n_attempts > 3*bound_list.size()){
					failure = true;
					break;
				}
				bound_index = gsl_rng_uniform_int(rng, bound_list.size());
				site_coord = bound_list[bound_index]->site_coord;
				mt_index = bound_list[bound_index]->mt_index;
				plus_end = mt_array[mt_index].plus_end;
				minus_end = mt_array[mt_index].minus_end;
				delta_x = mt_array[mt_index].delta_x;
				n_attempts++;
			}
		}while(mt_array[mt_index].lattice[site_coord + delta_x].occupant != NULL);
		
		if(failure != true){
			// Verify that there is a motor capable of stepping 
			if(mt_array[mt_index].lattice[site_coord].occupant == NULL){
				printf("Error in motor stepping code at site %i on microtubule %i.\n", site_coord, mt_index);
				exit(1);
			}
			// Verify that the selected motor is not blocked
			if(mt_array[mt_index].lattice[site_coord+delta_x].occupant != NULL){
				printf("Error in motor stepping code (type two) at site %i on microtubule %i.\n", site_coord, mt_index);
				exit(1);
			}

			// Get the index of motor_list that corresponds to this motor
			int motor_index = mt_array[mt_index].lattice[site_coord].occupant->motor_index;
			// Update motor details
			motor_list[motor_index].site_coord = site_coord + delta_x;
			// Update microtubule details
			mt_array[mt_index].lattice[site_coord].occupant = NULL;
			mt_array[mt_index].lattice[site_coord + delta_x].occupant = &motor_list[motor_index];
			// Check if new site corresponds to the plus end, in which case its pointer is already absent from unbound_list 
			if(site_coord + delta_x != plus_end){
				// Find entry in unbound_list that points to new site, then remove it
				tubulin *new_site_address = &mt_array[mt_index].lattice[site_coord + delta_x];
				auto unbound_entry = std::find(unbound_list.begin(), unbound_list.end(), new_site_address);		// Get iterator pointing to new_site_address
				int unbound_index = std::distance(unbound_list.begin(), unbound_entry);				// Get numerical distance from iterator to vector start
				unbound_list.erase(unbound_list.begin() + unbound_index);							// Erase desired entry
			}
			// Check if old site corresponds to the minus end, in which case its pointer should be suppressed from unbound_list
			if(site_coord != minus_end){
				// Get pointer to old site, then add it to unbound_list
				tubulin *old_site_address = &mt_array[mt_index].lattice[site_coord];
				unbound_list.push_back(old_site_address);
			}
		}
		// Announce if an eligible site index was NOT found
		else if(failure == true){
			printf("gnarly bro; failed to move @ %i_%i\n", mt_index, site_coord);
		}
	}
}

void motors_boundaries(system_parameters *parameters, microtubule *mt_array, std::vector<motor> &motor_list, std::vector<motor*> &bound_list, gsl_rng *rng, int n_events){

	int n_microtubules, length_of_microtubule, plus_end, minus_end, delta_x;
	double alpha, beta, motor_speed, delta_t, p_move, alpha_eff, beta_eff;

	n_microtubules = parameters->n_microtubules;
	length_of_microtubule = parameters->length_of_microtubule;
	alpha = parameters->alpha;
	beta = parameters->beta;
	motor_speed = parameters->motor_speed; 		
	delta_t = parameters->delta_t;

	p_move = motor_speed*delta_t;		
	// For boundary conditions to work, alpha/beta must be scaled down such that motor speed is unity relative to them. Furthermore, 
	// since boundaries are checked after every single kMC event, divide by n_events to keep probability per timestep constant.
	alpha_eff = alpha*p_move/n_events;
	beta_eff = beta*p_move/n_events;

	for(int mt_index = 0; mt_index < n_microtubules; mt_index++){
		// Get appropriate plus_end and minus_end site coordinates		
		plus_end = mt_array[mt_index].plus_end;
		minus_end = mt_array[mt_index].minus_end;
		delta_x = mt_array[mt_index].delta_x;

		// Generate two random numbers in the range [0.0, 1.0)
		double ran1 = gsl_rng_uniform(rng);
		double ran2 = gsl_rng_uniform(rng);		

		// If minus end is unoccupied, insert a motor onto the microtubule lattice with probability alpha_eff
		if(mt_array[mt_index].lattice[minus_end].occupant == NULL && ran1 < alpha_eff){
			// Find motor to bind
			int motor_index = 0;
			while(motor_list[motor_index].bound == true){
				motor_index++;
			}
			// Update motor details
			motor_list[motor_index].bound = true;
			motor_list[motor_index].mt_index = mt_index;
			motor_list[motor_index].site_coord = minus_end;
			motor_list[motor_index].motor_index = motor_index;
			// Update microtubule
			mt_array[mt_index].lattice[minus_end].occupant = &motor_list[motor_index];
			mt_array[mt_index].n_bound++;
			// Get memory location of this motor and add it to bound_list
			motor* motor_address = &motor_list[motor_index];
			bound_list.push_back(motor_address);
		}
		// If a motor is at the plus end, remove it from the microtubule lattice with probability beta_eff 
		if(mt_array[mt_index].lattice[plus_end].occupant != NULL && ran2 < beta_eff){
			// Find index of motor_list that corresponds to this motor
			int motor_index = mt_array[mt_index].lattice[plus_end].occupant->motor_index;
			// Update motor
			motor_list[motor_index].bound = false; 
			// Update microtubule
			mt_array[mt_index].lattice[plus_end].occupant = NULL;	
			mt_array[mt_index].n_bound--;
			// Find bound list entry that points to the memory location of this motor, then remove it
			motor* motor_address = &motor_list[motor_index];
			auto bound_entry = std::find(bound_list.begin(), bound_list.end(), motor_address);		// Get iterator pointing to motor_address
			int bound_index = std::distance(bound_list.begin(), bound_entry);				// Get numerical distance from iterator to vector start
			bound_list.erase(bound_list.begin() + bound_index);								// Erase desired entry 
		}
	}
}


