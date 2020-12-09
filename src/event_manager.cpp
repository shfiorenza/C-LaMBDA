#include "event_manager.hpp"
#include "curator.hpp"
#include "system_namespace.hpp"
#include "system_rng.hpp"

EventManager::EventManager() {}

void EventManager::Initialize(Curator *wally) {

  wally_ = wally;
  // gsl_ = w
}

void EventManager::SampleEventStatistics() {

  n_events_to_exe_ = 0;
  for (auto &&event : events_) {
    n_events_to_exe_ += event.SampleStatistics();
  }
  if (n_events_to_exe_ <= 1) {
    return;
  }
  // Put all events with >1 target into an active_ array
  Pair<Event *, Object *> active_events[n_events_to_exe_];
  int i_active{0};
  for (auto &&event : events_) {
    // Add a ptr to the event for each target it has
    for (int i_tar{0}; i_tar < event.n_expected_; i_tar++) {
      active_events[i_active++] = std::make_pair(&event, event.targets_[i_tar]);
    }
  }
  // Scan through all active events to ensure that no 2 target the same
  for (int i_entry{0}; i_entry < n_events_to_exe_; i_entry++) {
    Event *event_i{active_events[i_entry].first};
    Object *tar_i{active_events[i_entry].second};
    for (int j_entry{i_entry + 1}; j_entry < n_events_to_exe_; j_entry++) {
      Event *event_j{active_events[j_entry].first};
      // wally_->Log(2, "   event_j = %s\n", event_j->name_.c_str());
      Object *tar_j{active_events[j_entry].second};
      // If event_i and event_j target the same motor or motor head, remove one
      if (tar_i == tar_j) {
        double p_one{event_i->p_occur_};
        double p_two{event_j->p_occur_};
        double ran{gsl_->GetRanProb()};
        if (ran < p_one / (p_one + p_two)) {
          event_i->RemoveTarget(active_events[i_entry].second);
          active_events[i_entry] = active_events[n_events_to_exe_ - 1];
          i_entry--;
          n_events_to_exe_--;
          break;
        } else {
          event_j->RemoveTarget(active_events[j_entry].second);
          active_events[j_entry] = active_events[n_events_to_exe_ - 1];
          j_entry--;
          n_events_to_exe_--;
        }
      }
    }
  }
}

void EventManager::GenerateExecutionSequence() {

  if (n_events_to_exe_ == 0) {
    return;
  }
  int i_array{0};
  Event *pre_array[n_events_to_exe_];
  for (auto &&event : events_) {
    for (int i_entry{0}; i_entry < event.n_expected_; i_entry++) {
      pre_array[i_array++] = &event;
    }
  }
  if (i_array != n_events_to_exe_) {
    Sys::ErrorExit("K_MGMT::GenerateExecutionSequence()");
  }
  if (n_events_to_exe_ > 1) {
    gsl_->Shuffle(pre_array, n_events_to_exe_, sizeof(Event *));
  }
  if (n_events_to_exe_ > events_to_exe_.size()) {
    events_to_exe_.resize(n_events_to_exe_);
  }
  for (int i_event{0}; i_event < n_events_to_exe_; i_event++) {
    events_to_exe_[i_event] = pre_array[i_event];
  }
}

void EventManager::ExecuteEvents() {

  SampleEventStatistics();
  GenerateExecutionSequence();
  for (int i_event{0}; i_event < n_events_to_exe_; i_event++) {
    events_to_exe_[i_event]->Execute();
  }
}