#ifndef _ASSOCIATED_PROTEIN_MANAGEMENT_H
#define _ASSOCIATED_PROTEIN_MANAGEMENT_H
#include "associated_protein.h"
#include <string>
#include <functional>

struct system_parameters;
struct system_properties;

//XXX	to-do:
//XXX		-see kinesin_mgmt header

class AssociatedProteinManagement{
	private:
		// Structure that holds all pertinent info for a given MC event:
		struct event{
			event(int i, int code, std::string l, std::string tar, 
					std::function<int(double, int)> p_dist, 
					int *pop, double p): index_(i), kmc_code_(code), 
					label_(l), target_pop_(tar), prob_dist_(p_dist), 
					pop_ptr_(pop), p_occur_(p) {}
			void SampleStatistics(){
				if(*pop_ptr_ > 0) 
					n_expected_ = prob_dist_(p_occur_, *pop_ptr_);
				else n_expected_ = 0;
			}
		private:
		public:
			std::function<int(double, int)> prob_dist_;
			int index_ = -1; 			// Serialized unique index of event
			int kmc_code_ = -1; 
			std::string label_ = "BRUH";
			std::string target_pop_ = "not me"; 
			double p_occur_ = 0;
			int *pop_ptr_ = nullptr; 	// Pointer to population size variable
			int n_expected_ = 0;
		};
		system_parameters *parameters_ = nullptr;
		system_properties *properties_ = nullptr;

	public:
		int n_xlinks_ = 0;
		// xlinks actively bound to some MT or motor; dynamically updated
		int n_active_ = 0;
		int n_free_tethered_ = 0;
		std::vector<int> n_bound_i_;
		int n_bound_i_tethered_tot_ = 0;		// needed?
		int n_bound_untethered_ = 0;
		std::vector<int> n_bound_i_bindable_;
		std::vector<int> n_bound_ii_;
		std::vector<int> n_bound_i_tethered_;
		std::vector< std::vector<int> > n_bound_i_tethered_bindable_;
		std::vector< std::vector<int> > n_bound_ii_tethered_;
		
		// Only one population for singly-bound untethered xlink heads
        int n_sites_i_ = 0;
		// Population for each xlink extension
		std::vector<int> n_sites_ii_;
		// Population for each tether extension
		std::vector<int> n_sites_i_tethered_;
		// Population for each teth AND each xlink extension
		std::vector< std::vector<int> > n_sites_ii_tethered_same_; 
		std::vector< std::vector<int> > n_sites_ii_tethered_oppo_; 

		int teth_cutoff_; 		// see kinesin header (is dist_cutoff_ there)
		int dist_cutoff_;		// see assoc. protein header
		int rest_dist_;			// see assoc. protein header

		// characteristic time to diffuse one site (~8nm) for bound_i/ii
		double tau_i_;	
		double tau_ii_;		

		// Stand-alone xlink diffusion probabilities
		double p_diffuse_i_fwd_;
		double p_diffuse_i_bck_;
		std::vector<double> p_diffuse_ii_to_rest_;
		std::vector<double> p_diffuse_ii_from_rest_;
		// Tether-dependent xlink diffusion probabilities
		std::vector<double> p_diffuse_i_to_teth_rest_;
		std::vector<double> p_diffuse_i_from_teth_rest_;
		std::vector< std::vector<double> > p_diffuse_ii_to_both_rest_;
		std::vector< std::vector<double> > p_diffuse_ii_from_both_rest_;
		std::vector< std::vector<double> > p_diffuse_ii_to_self_from_teth_;
		std::vector< std::vector<double> > p_diffuse_ii_from_self_to_teth_;

		// Kinematics probabilities
		std::vector<double> p_bind_i_;			
		double p_bind_i_tethered_;
		double p_bind_ii_;
		std::vector<double> p_unbind_i_;		
		double p_tether_free_; 
		double p_untether_free_;
		std::vector<double> p_unbind_i_tethered_;	
		std::vector<double> p_unbind_ii_;
		std::vector< std::vector<double> > p_unbind_ii_to_teth_;
		std::vector< std::vector<double> > p_unbind_ii_from_teth_;

		// 1-D vectors, index is simply xlink entry
		std::vector<AssociatedProtein> xlinks_;
		std::vector<AssociatedProtein*> active_;
		std::vector<AssociatedProtein*> free_tethered_;
		std::vector<std::vector<AssociatedProtein*>> bound_i_;
		std::vector<AssociatedProtein*> bound_i_bindable_;
		std::vector<AssociatedProtein*> bound_untethered_;
		// 2-D vectors, 1st index is x_dist, 2nd is xlink entry
		std::vector< std::vector<AssociatedProtein*> > bound_ii_;
//		std::vector< std::vector<AssociatedProtein*> > bound_i_bindable_;
		// 1st index is x_dist_dub, 2nd is xlink entry
		std::vector< std::vector<AssociatedProtein*> > bound_i_tethered_;
		// 3-D vectors, 1st index is x_dist_dub, 2nd is x_dist, 3rd is entry
		// e.g. vec_[16][2][0] is the 1st xlink w/ x_dist_dub=16, x_dist=2
		std::vector< std::vector< std::vector<AssociatedProtein*> > >
			bound_ii_tethered_;
		std::vector< std::vector< std::vector<AssociatedProtein*> > > 
			bound_i_tethered_bindable_;

		// Following vectors refer to sites that xlinks are bound
		// to, but the same indexing convention as above applies
		std::vector<Tubulin*> sites_i_untethered_; 
		std::vector< std::vector<Tubulin*> > sites_ii_untethered_;
		std::vector< std::vector<Tubulin*> > sites_i_tethered_;
		// 'oppo' refers to tether rest and xlink rest being 
		// on opposite sides of the site; likewise for 'same' 
		std::vector< std::vector< std::vector<Tubulin*> > >
			sites_ii_tethered_oppo_;
		std::vector< std::vector< std::vector<Tubulin*> > >
			sites_ii_tethered_same_;	

		// Holds diffusion and KMC events encoded in integer values
		std::vector<int> dif_list_;
		std::vector<int> kmc_list_;
		// Serialized (in regards to self/teth extension) vectors that store
		// number of entries, the population/event label, x, and x_dub
		// (facilitates the parallelization of statistical sampling) 
		std::vector<event> diffu_events_;
		std::vector<event> kmc_events_;

	private:
		void GenerateXLinks();
		void SetParameters();
		void InitializeLists();
		void InitializeDiffusionEvents();
		void InitializeKMCEvents();

	public:
		AssociatedProteinManagement();
		void Initialize(system_parameters *parameters, 
						system_properties *properties);

		void UpdateAllLists();				// FIXME
		void UpdateSingleBoundList();
		void UpdateBoundITethered();
		void UpdateDoubleBoundList();
		void UpdateBoundIITethered();
		void UpdateFreeTetheredList();
		void UpdateBoundUntethered();
		
		void UpdateAllSiteLists();
		void UpdateSingleUntetheredSites();
		void UpdateDoubleUntetheredSites();
		void UpdateSingleTetheredSites();
		void UpdateDoubleTetheredSites();

		AssociatedProtein* GetFreeXlink();
		AssociatedProtein* GetBoundUntetheredXlink();

		void GenerateDiffusionList();		// XXX

		void RunDiffusion();
		void RunDiffusionI_Forward();
		void RunDiffusionI_Backward();
		void RunDiffusionII_ToRest(int x_dist);
		void RunDiffusionII_FromRest(int x_dist);
		void RunDiffusionI_ToTethRest(int x_dist_dub);
		void RunDiffusionI_FromTethRest(int x_dist_dub);
		void RunDiffusionII_ToBothRest(int x_dist_dub, int x_dist);
		void RunDiffusionII_FromBothRest(int x_dist_dub, int x_dist);
		void RunDiffusionII_ToSelf_FromTeth(int x_dist_dub, int x_dist);
		void RunDiffusionII_FromSelf_ToTeth(int x_dist_dub, int x_dist);

		void GenerateKMCList();				// XXX
		double GetWeightBindII(); 
		double GetWeightBindITethered();
		double GetWeightBindIITethered();

		void RunKMC();
		void RunKMC_Bind_I(int n_neighbs);
		void RunKMC_Bind_II();
		void RunKMC_Unbind_I(int n_neighbs);
		void RunKMC_Unbind_II(int x_dist);
		void RunKMC_Bind_I_Tethered();
		void RunKMC_Bind_II_Tethered();
		void RunKMC_Unbind_I_Tethered(int x_dist_dub); 
		void RunKMC_Unbind_II_To_Teth(int x_dist_dub, int x_dist);
		void RunKMC_Unbind_II_From_Teth(int x_dist_dub, int x_dist);
		void RunKMC_Tether_Free(); // FIXME
		void RunKMC_Untether_Free();
};
#endif
