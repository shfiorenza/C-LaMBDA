#ifndef _KINESIN_H
#define _KINESIN_H
#include <vector>
class Microtubule;
class Tubulin;
class AssociatedProtein;
struct system_properties;
struct system_parameters;

class Kinesin{
	private:

	public:
		int ID_;				// Unique ID of this kinesin in resevoir
		int speciesID_ = 2;		// Unique ID of this species (kinesin)
		int heads_active_ = 0;
		int n_neighbor_sites_ = 0;
		int n_neighbor_xlinks_ = 0;

		int x_dist_doubled_ = 0;	// in no. of sites 
		int dist_cutoff_ = 18;	// max value x_dist_ can reach
		double rest_dist_ = 15;	// spring extension is ~0 for this

		double kbT_ = 4.114; 			// in pN * nm
		double site_size_ = 8;			// tubulin dimer size in nm
		double r_0_ = 120;				// in nm
		double k_spring_ = 0.3;			// in pN / nm
		double k_eff_slack_ = 0.02;		// for when tether is 'compressed'
		double stall_force_ = 5;		// in pN
		double extension_ = 0;			// in nm
	
		bool tethered_ = false;

		Microtubule *mt_ = nullptr; 		
		Tubulin *front_site_ = nullptr;
		Tubulin *rear_site_ = nullptr;
		AssociatedProtein *xlink_ = nullptr; 

		// Neighbor xlinks are for when the motor is bound but untethered
		std::vector<AssociatedProtein*> neighbor_xlinks_;
		// Neighbor sites are for when the motor is tethered but unbound
		std::vector<Tubulin*> neighbor_sites_;
		// Indices for lookup tables correspond to distance
		// in (no. of sites)/2, NOT extension of tether 
		// e.g. ...lookup_[1] is an x-dist of 1/2 of a site
		std::vector<double> tethering_weight_lookup_;
		std::vector<double> binding_weight_lookup_;
		std::vector<double> spring_force_lookup_;

		system_parameters *parameters_ = nullptr;
		system_properties *properties_ = nullptr;
	private:

	public:
		Kinesin();
		void Initialize(system_parameters *parameters, 
			system_properties *properties, int ID);
		void InitiateNeighborLists();
		void PopulateTetheringLookupTable();
		void PopulateBindingLookupTable();

		void UpdateNeighborXlinks();
		bool NeighborXlinkExists(int x_dist_doubled);
		void UpdateNeighborSites();
		bool NeighborSiteExists(int x_dist_doubled); 

		void UpdateExtension();
		void ForceUntether(int x_dub_pre);

		int SampleTailExtensionDoubled();
		int GetDirectionTowardXlink();
		double GetStalkCoordinate(); // tail originates from stalk
		double GetTetheringWeight(AssociatedProtein *xlink);
		double GetBindingWeight(Tubulin *site);
		Tubulin* GetActiveHeadSite();
		Tubulin* GetSiteCloserToXlink();
		Tubulin* GetSiteFartherFromXlink();
		Tubulin* GetNeighborSite(int x_dist_doubled);
		AssociatedProtein* GetNeighborXlink(int x_dist_doubled);
};
#endif
