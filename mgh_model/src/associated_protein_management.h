#pragma once 
#include <string>
#include <functional>
#include "event.h"
#include "associated_protein.h"
struct system_parameters;
struct system_properties;
class Curator; 

class AssociatedProteinManagement{
	private:
		// Aliases to make our work a little more neat
		template<class T>
		using vec = std::vector<T>;
		using monomer = AssociatedProtein::Monomer*;
		using event = Event<monomer, AssociatedProteinManagement*>;
		// All possible KMC event objects; arbitrary sequential order
		vec<event> events_;
		// IDs of events; segregated by target population
		vec<vec<int>> IDs_by_pop_;
		// List of event IDs to execute any given timestep; dynamic
		vec<int> IDs_to_exe_;
		// Scratch jawn
		vec<vec<monomer>> scratch_;
		vec<int> n_scratch_;
		// Pointers to global system params & props; same for all classes
		system_parameters *parameters_ = nullptr;
		system_properties *properties_ = nullptr;
		// WALLACE, MISTA
		Curator* wally = nullptr;

	public:
		int dist_cutoff_;		// see assoc. protein header
		int rest_dist_;			// see assoc. protein header
		int teth_cutoff_; 		// see kinesin header (dist_cutoff_ there)
		int comp_cutoff_;		// see kinesin header (comp_cutoff_ there)
		// Neighbor coop stuff; still kinda preliminary
		int max_neighbs_;
		double interaction_energy_;		// in kBT

		/* Population size trackers */ 
		int n_xlinks_ = 0; 		  // Total number of xlink objects created
		int n_active_ = 0; 		  // No. actively bound; dynamically updated
		int n_free_teth_ = 0;
		int n_bound_unteth_ = 0;
		int n_heads_i_teth_tot_ = 0;
		// First index is number of PRC1 neighbors: [0], [1], or [2]
		// The last entry, [3], includes ALL regardless of n_neighbs
		vec<int> n_heads_i_;
		// Second index is [x] or [x_dub] for base or teth pops.
		vec<vec<int>> n_heads_i_teth_;
		vec<vec<int>> n_heads_ii_;
		// Second index is [x_dub], third & final index is [x]
		vec<vec<vec<int>>> n_heads_ii_teth_same_; 
		vec<vec<vec<int>>> n_heads_ii_teth_oppo_; 

		/* Probabilities of possible KMC events */
		double p_bind_i_teth_base_;
		double p_bind_ii_base_;
		double p_tether_free_; 
		double p_untether_free_;
		// First index is number of PRC1 neighbors: [0], [1], or [2]
		vec<double> p_bind_i_;			
		vec<double> p_unbind_i_;		
		vec<double> p_diffuse_i_fwd_;
		vec<double> p_diffuse_i_bck_;
		// Second index is [x] or [x_dub] for base or teth pops.
		vec<vec<double>> p_unbind_i_teth_;	
		vec<vec<double>> p_unbind_ii_;
		vec<vec<double>> p_diffuse_i_to_teth_rest_;
		vec<vec<double>> p_diffuse_i_fr_teth_rest_;
		vec<vec<double>> p_diffuse_ii_to_rest_;
		vec<vec<double>> p_diffuse_ii_fr_rest_;
		// Second index is [x_dub]; third & final index is [x]
		vec<vec<vec<double>>> p_unbind_ii_to_teth_;
		vec<vec<vec<double>>> p_unbind_ii_fr_teth_;
		vec<vec<vec<double>>> p_diffuse_ii_to_both_;
		vec<vec<vec<double>>> p_diffuse_ii_fr_both_;
		vec<vec<vec<double>>> p_diffuse_ii_to_self_fr_teth_;
		vec<vec<vec<double>>> p_diffuse_ii_fr_self_to_teth_;

		/* Lists that track different population types */
		vec<AssociatedProtein> xlinks_;				// Actual xlink objects
		vec<AssociatedProtein*> active_;
		vec<AssociatedProtein*> free_teth_;
		vec<AssociatedProtein*> bound_unteth_;
		// First index is number of PRC1 neighbors: [0], [1], or [2]
		// The last entry, [3], includes ALL regardless of n_neighbs
		// Second index is actual xlink entry
		vec<vec<AssociatedProtein::Monomer*>> heads_i_;
		// Second index is [x] or [x_dub]; third index is xlink entry
		vec<vec<vec<AssociatedProtein::Monomer*>>> heads_ii_;
		vec<vec<vec<AssociatedProtein::Monomer*>>> heads_i_teth_;
		// Second index is [x_dub]; third index is [x]; fourth is entry
		// e.g., [0][16][2][1] -> 2nd xlink w/ x_dub=16, x=2, & 0 neighbs
		vec<vec<vec<vec<AssociatedProtein::Monomer*>>>> heads_ii_teth_oppo_;
		vec<vec<vec<vec<AssociatedProtein::Monomer*>>>> heads_ii_teth_same_;

	private:
		void GenerateXLinks();
		void SetParameters();
		void InitializeLists();
		void InitializeEvents();

	public:
		AssociatedProteinManagement();
		void Initialize(system_parameters *parameters, 
						system_properties *properties);

		AssociatedProtein* GetFreeXlink();
		AssociatedProtein* GetBoundUntetheredXlink();
		double GetWeight_Bind_II(); 
		double GetWeight_Bind_I_Teth();
		double GetWeight_Bind_II_Teth();

		void Update_All();
		void Update_Free_Teth();
		void Update_Bound_Unteth(); 
		void Update_Heads_I();
		void Update_Heads_I_Teth();
		void Update_Heads_II();
		void Update_Heads_II_Teth();

		void Run_KMC();
		void Generate_KMC_List();
		void KMC_Relay(monomer target, int code); 
		void CheckScratchFor(std::string state, int n_neighbs);
		// Diffusion events
		void Diffuse_I_Fwd(int n_neighbs);
		void Diffuse_I_Bck(int n_neighbs);
		void Diffuse_II_To_Rest(int n_neighbs, int x);
		void Diffuse_II_Fr_Rest(int n_neighbs, int x);
		void Diffuse_I_To_Teth(int n_neighbs, int x_dub);
		void Diffuse_I_Fr_Teth(int n_neighbs, int x_dub);
		void Diffuse_II_To_Both(int n_neighbs, int x_dub, int x);
		void Diffuse_II_Fr_Both(int n_neighbs, int x_dub, int x);
		void Diffuse_II_To_Self_Fr_Teth(int n_neighbs, int x_dub, int x);
		void Diffuse_II_Fr_Self_To_Teth(int n_neighbs, int x_dub, int x);
		// Kinematic events
		void Bind_I(int n_neighbs);
		void Bind_II();		//XXX add neighb coop in weights
		void Unbind_I(int n_neighbs);
		void Unbind_II(int n_neighbs, int x);
		void Bind_I_Teth(); //XXX add neighb coop in weights
		void Bind_II_Teth(); //XXX add neighb coop in weights
		void Unbind_I_Teth(int n_neighbs, int x_dub); 
		void Unbind_II_To_Teth(int n_neighbs, int x_dub, int x);
		void Unbind_II_Fr_Teth(int n_neighbs, int x_dub, int x);
		void Tether_Free(); 
		void Untether_Free();
};
