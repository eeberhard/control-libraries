#include "state_representation/State.hpp"
#include "state_representation/exceptions/NotImplementedException.hpp"

namespace state_representation {

State::State() : type_(StateType::STATE), empty_(true), timestamp_(std::chrono::steady_clock::now()) {}

State::State(const std::string& name) :
    type_(StateType::STATE), name_(name), empty_(true), timestamp_(std::chrono::steady_clock::now()) {}

State::State(const State& state) :
    std::enable_shared_from_this<State>(state),
    type_(state.type_),
    name_(state.name_),
    empty_(state.empty_),
    // FIXME: do we keep this or copy the timestamp too?
    timestamp_(std::chrono::steady_clock::now()) {}

State& State::operator=(const State& state) {
  State tmp(state);
  swap(*this, tmp);
  return *this;
}

const StateType& State::get_type() const {
  return this->type_;
}

const std::string& State::get_name() const {
  return this->name_;
}

bool State::is_empty() const {
  return this->empty_;
}

const std::chrono::time_point<std::chrono::steady_clock>& State::get_timestamp() const {
  return this->timestamp_;
}

void State::set_type(const StateType& type) {
  this->type_ = type;
}

void State::set_name(const std::string& name) {
  // FIXME: should we prohibit empty names?
  this->name_ = name;
}

void State::set_empty(bool empty) {
  this->empty_ = empty;
}

void State::reset_timestamp() {
  this->timestamp_ = std::chrono::steady_clock::now();
}

void State::set_data(const Eigen::VectorXd&) {
  throw exceptions::NotImplementedException("set_data() is not implemented for the base State class");
}

void State::set_data(const std::vector<double>&) {
  throw exceptions::NotImplementedException("set_data() is not implemented for the base State class");
}

void State::set_data(const Eigen::MatrixXd&) {
  throw exceptions::NotImplementedException("set_data() is not implemented for the base State class");
}

void State::initialize() {
  this->empty_ = true;
}

double State::get_age() const {
  return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now() - this->timestamp_
  ).count()) / 1e9;
}

bool State::is_deprecated(double time_delay) const {
  return this->get_age() >= time_delay;
}

bool State::is_incompatible(const State&) const {
  return false;
}

State::operator bool() const noexcept {
  return !this->empty_;
}

std::ostream& operator<<(std::ostream& os, const State& state) {
  auto prefix = state.is_empty() ? "Empty " : "";
  os << prefix << "State: " << state.get_name();
  return os;
}
}// namespace state_representation
