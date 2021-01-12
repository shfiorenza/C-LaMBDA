#include "protein.hpp"
#include "binding_head.hpp"
#include "protofilament.hpp"
#include "system_namespace.hpp"

void Protein::InitializeNeighborList() {}

bool Protein::HasSatellite() {}

void Protein::UntetherSatellite() {}

bool Protein::UpdateExtension() {
  if (n_heads_active_ != 2) {
    return true;
  }
  // Update head positions
  for (int i_dim{0}; i_dim < _n_dims_max; i_dim++) {
    head_one_.pos_[i_dim] = head_one_.site_->pos_[i_dim];
    head_two_.pos_[i_dim] = head_two_.site_->pos_[i_dim];
  }
  // printf("r1 = (%g, %g)\n", head_one_.pos_[0], head_one_.pos_[1]);
  // printf("r2 = (%g, %g)\n", head_two_.pos_[0], head_two_.pos_[1]);
  // Update spring position
  bool spring_attached{spring_.UpdatePosition()};
  if (!spring_attached) {
    return false;
  }
  // bool head_one_attached{pivot_one_.UpdatePosition()};
  // if (!head_one_attached) {
  //   return false;
  // }
  // bool head_two_attached{pivot_two_.UpdatePosition()};
  // if (!head_two_attached) {
  //   return false;
  // }
  // If spring is still attached after updates, apply forces & torques
  spring_.ApplyForces();
  // pivot_one_.ApplyForces();
  // pivot_two_.ApplyForces();
  // printf("**\n\n");
  return true;
}

int Protein::GetDirectionTowardRest(BindingHead *head) {
  if (n_heads_active_ == 1) {
    return 1;
  } else if (n_heads_active_ == 2) {
    // printf("%g > %g?\n", head->site_->pos_[0], GetAnchorCoordinate(0));
    if (head->site_->pos_[0] > GetAnchorCoordinate(0)) {
      return -1;
    } else if (head->site_->pos_[0] < GetAnchorCoordinate(0)) {
      return 1;
    } else {
      return 0;
    }
  } else {
    Sys::ErrorExit("Protein::GetDirToRest()\n");
  }
  return 0;
}

double Protein::GetAnchorCoordinate(int i_dim) {
  if (n_heads_active_ != 2) {
    Sys::ErrorExit("Protein::GetAnchorCoord()");
  }
  return (head_one_.site_->pos_[i_dim] + head_two_.site_->pos_[i_dim]) / 2;
}

void Protein::UpdateNeighbors_Bind_II() {

  n_neighbors_bind_ii_ = 0;
  BindingSite *site{GetActiveHead()->site_};
  Protofilament *neighb_fil{site->filament_->neighbor_};
  double r_y{site->filament_->pos_[1] - neighb_fil->pos_[1]};
  double r_x_max{sqrt(Square(spring_.r_max_) - Square(r_y))};
  int delta_max{(int)std::ceil(r_x_max / Params::Filaments::site_size)};
  for (int delta{-delta_max}; delta <= delta_max; delta++) {
    BindingSite *neighb{neighb_fil->GetNeighb(site, delta)};
    if (neighb == nullptr) {
      continue;
    }
    if (neighb->occupant_ == nullptr) {
      neighbors_bind_ii_[n_neighbors_bind_ii_++] = neighb;
    }
  }
}

double Protein::GetSoloWeight_Bind_II(BindingSite *neighb) {

  BindingSite *site{GetActiveHead()->site_};
  double r_x{neighb->pos_[0] - site->pos_[0]};
  double r_y{neighb->pos_[1] - site->pos_[1]};
  double r{sqrt(Square(r_x) + Square(r_y))};
  if (r < spring_.r_min_ or r > spring_.r_max_) {
    return 0.0;
  }
  double weight_spring{spring_.GetWeight_Bind(r)};
  double weight_site{neighb->GetWeight_Bind()};
  return weight_spring * weight_site;
}

BindingSite *Protein::GetNeighbor_Bind_II() {

  double weight_tot{GetWeight_Bind_II()};
  double ran{SysRNG::GetRanProb()};
  double p_cum{0.0};
  Sys::Log(2, "%i NEIGHBS\n", n_neighbors_bind_ii_);
  Sys::Log(2, "ran = %g\n", ran);
  for (int i_neighb{0}; i_neighb < n_neighbors_bind_ii_; i_neighb++) {
    BindingSite *neighb{neighbors_bind_ii_[i_neighb]};
    p_cum += GetSoloWeight_Bind_II(neighb) / weight_tot;
    Sys::Log(2, "p_cum = %g\n", p_cum);
    if (ran < p_cum) {
      Sys::Log(2, "*** chose neighb %i ***\n\n", neighb->index_);
      return neighb;
    }
  }
  return nullptr;
}

double Protein::GetWeight_Diffuse(BindingHead *head, int dir) {

  if (n_heads_active_ != 2) {
    Sys::ErrorExit("Protein::GetWeight_diffuse");
  }
  int dx{dir * head->GetDirectionTowardRest()};
  // For xlinks exactly at rest,
  if (dx == 0) {
    // Diffuse from rest in a random direction
    if (dir == -1) {
      if (SysRNG::GetRanProb() < 0.5) {
        dx = 1;
      } else {
        dx = -1;
      }
      // Impossible to diffuse toward rest
    } else {
      return 0.0;
    }
  }
  BindingSite *new_loc{head->site_->GetNeighbor(dx)};
  if (new_loc == nullptr) {
    return 0.0;
  }
  if (new_loc->occupant_ != nullptr) {
    return 0.0;
  }
  BindingSite *old_loc{head->site_};
  if (old_loc->GetNumNeighborsOccupied() == _n_neighbs_max) {
    return 0.0;
  }
  BindingSite *static_loc{head->GetOtherHead()->site_};
  if (old_loc->filament_ == static_loc->filament_) {
    Sys::ErrorExit("Protein::GetWeight_diffuse [2]");
  }
  double weight_spring{spring_.GetWeight_Shift(static_loc, old_loc, new_loc)};
  double weight_neighb{head->site_->GetWeight_Unbind()};
  return weight_spring * weight_neighb;
}

double Protein::GetWeight_Bind_II() {

  double tot_weight{0.0};
  UpdateNeighbors_Bind_II();
  for (int i_neighb{0}; i_neighb < n_neighbors_bind_ii_; i_neighb++) {
    tot_weight += GetSoloWeight_Bind_II(neighbors_bind_ii_[i_neighb]);
  }
  return tot_weight;
}

double Protein::GetWeight_Unbind_II(BindingHead *head) {

  if (n_heads_active_ != 2) {
    Sys::ErrorExit("Protein::GetWeight_Unbind_II()\n");
  }
  double weight_spring{spring_.GetWeight_Unbind()};
  double weight_site{head->site_->GetWeight_Unbind()};
  return weight_spring * weight_site;
}

bool Protein::Diffuse(BindingHead *head, int dir) {

  int dx{dir * head->GetDirectionTowardRest()};
  // FIXME ran num MUST be synchronized with one in GetWeight() above
  // For xlinks exactly at rest,
  if (dx == 0) {
    // Diffuse from rest in a random direction
    if (dir == -1) {
      if (SysRNG::GetRanProb() < 0.5) {
        dx = 1;
      } else {
        dx = -1;
      }
      // Impossible to diffuse toward rest
    } else {
      return false;
    }
  }
  // printf("dx: %i\n", dx);
  BindingSite *old_site = head->site_;
  // printf("no\n");
  int i_new{old_site->index_ + dx};
  // printf("i_old: %i | i_new: %i\n", old_site->index_, i_new);
  if (i_new < 0 or i_new > old_site->filament_->sites_.size() - 1) {
    return false;
  }
  // printf("chaching\n");
  BindingSite *new_site{&old_site->filament_->sites_[i_new]};
  if (new_site->occupant_ != nullptr) {
    return false;
  }
  old_site->occupant_ = nullptr;
  new_site->occupant_ = head;
  head->site_ = new_site;
  // printf("frfr\n\n");
  return true;
}

bool Protein::Bind(BindingSite *site, BindingHead *head) {

  if (site->occupant_ != nullptr) {
    return false;
  }
  site->occupant_ = head;
  head->site_ = site;
  n_heads_active_++;
  return true;
}

bool Protein::Unbind(BindingHead *head) {

  BindingSite *site{head->site_};
  site->occupant_ = nullptr;
  head->site_ = nullptr;
  n_heads_active_--;
  return true;
}

bool Protein::Tether() {}

bool Protein::Untether() {}