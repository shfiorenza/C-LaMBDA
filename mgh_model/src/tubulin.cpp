#include "tubulin.h"
#include "master_header.h"

Tubulin::Tubulin() {}

void Tubulin::Initialize(system_parameters *parameters,
                         system_properties *properties, Microtubule *mt,
                         int i_site) {

  parameters_ = parameters;
  properties_ = properties;
  index_ = i_site;
  coord_ = i_site;
  mt_ = mt;
}

bool Tubulin::EquilibriumInSameDirection() {

  if (occupied_ == true && xlink_head_ != nullptr) {
    if (xlink_head_->xlink_->heads_active_ == 2 &&
        xlink_head_->xlink_->tethered_ == true) {
      Kinesin *motor = xlink_head_->xlink_->motor_;
      double site_coord = index_ + mt_->coord_;
      double xlink_rest = xlink_head_->xlink_->GetAnchorCoordinate();
      double motor_rest = motor->GetRestLengthCoordinate();
      // When tether is extended past rest length, ...
      if (motor->x_dist_doubled_ > 2 * motor->rest_dist_) {
        // ... equils are in same dir. if coords are on same side
        if ((xlink_rest > site_coord && motor_rest > site_coord) ||
            (xlink_rest < site_coord && motor_rest < site_coord)) {
          return true;
        }
        // ... equils are in oppo dir. if coords are on oppo side
        else if ((xlink_rest > site_coord && motor_rest < site_coord) ||
                 (xlink_rest < site_coord && motor_rest > site_coord)) {
          return false;
        } else {
          printf("error in Tubulin::SpringEquilOnSameSide\n");
          exit(1);
        }
      }
      // When tether at or compressed below rest length, ...
      else {
        // ... equils are in same dir. if coords are on oppo side
        if ((xlink_rest > site_coord && motor_rest < site_coord) ||
            (xlink_rest < site_coord && motor_rest > site_coord)) {
          return true;
        }
        // ... equils are in oppo dir. if coords are on same side
        else if ((xlink_rest > site_coord && motor_rest > site_coord) ||
                 (xlink_rest < site_coord && motor_rest < site_coord)) {
          return false;
        } else {
          printf("error in Tubulin::SpringEquilOnSameSide TWO\n");
          exit(1);
        }
      }
    } else {
      printf("error NO. 2 in tubulin: spring equil on same side ??\n");
      exit(1);
    }
  } else {
    printf("error in tubulin:spring equil on same side\n");
    exit(1);
  }
}

int Tubulin::GetPRC1NeighborCount() {

  int n_neighbs{0};
  int mt_end{mt_->n_sites_ - 1};
  for (int delta{-1}; delta <= 1; delta += 2) {
    int i_scan = index_ + delta;
    if (i_scan < 0 or i_scan > mt_end) {
      continue;
    } else if (mt_->lattice_[i_scan].xlink_head_ != nullptr) {
      n_neighbs++;
    }
  }
  return n_neighbs;
}
