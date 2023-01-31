#include "state_representation/space/cartesian/CartesianPose.hpp"
#include "state_representation/exceptions/EmptyStateException.hpp"
#include "state_representation/exceptions/IncompatibleSizeException.hpp"

using namespace state_representation::exceptions;

namespace state_representation {

CartesianPose::CartesianPose() {
  this->set_type(StateType::CARTESIAN_POSE);
}

CartesianPose::CartesianPose(const std::string& name, const std::string& reference) : CartesianState(name, reference) {
  this->set_type(StateType::CARTESIAN_POSE);
}

CartesianPose::CartesianPose(const std::string& name, const Eigen::Vector3d& position, const std::string& reference) :
    CartesianState(name, reference) {
  this->set_type(StateType::CARTESIAN_POSE);
  this->set_position(position);
}

CartesianPose::CartesianPose(
    const std::string& name, double x, double y, double z, const std::string& reference
) : CartesianState(name, reference) {
  this->set_type(StateType::CARTESIAN_POSE);
  this->set_position(x, y, z);
}

CartesianPose::CartesianPose(
    const std::string& name, const Eigen::Quaterniond& orientation, const std::string& reference
) : CartesianState(name, reference) {
  this->set_type(StateType::CARTESIAN_POSE);
  this->set_orientation(orientation);
}

CartesianPose::CartesianPose(
    const std::string& name, const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation,
    const std::string& reference
) : CartesianState(name, reference) {
  this->set_type(StateType::CARTESIAN_POSE);
  this->set_position(position);
  this->set_orientation(orientation);
}

CartesianPose::CartesianPose(const CartesianState& state) : CartesianState(state) {
  // set all the state variables to 0 except position and orientation
  this->set_type(StateType::CARTESIAN_POSE);
  this->set_zero();
  this->set_pose(state.get_pose());
  this->set_empty(state.is_empty());
}

CartesianPose::CartesianPose(const CartesianPose& pose) : CartesianPose(static_cast<const CartesianState&>(pose)) {}

CartesianPose::CartesianPose(const CartesianTwist& twist) : CartesianPose(std::chrono::seconds(1) * twist) {}

CartesianPose CartesianPose::Identity(const std::string& name, const std::string& reference) {
  return CartesianState::Identity(name, reference);
}

CartesianPose CartesianPose::Random(const std::string& name, const std::string& reference) {
  return CartesianPose(name, Eigen::Vector3d::Random(), Eigen::Quaterniond::UnitRandom(), reference);
}

Eigen::VectorXd CartesianPose::data() const {
  return this->get_pose();
}

void CartesianPose::set_data(const Eigen::VectorXd& data) {
  if (data.size() != 7) {
    throw IncompatibleSizeException(
        "Input is of incorrect size: expected 7, given " + std::to_string(data.size()));
  }
  this->set_pose(data);
}

void CartesianPose::set_data(const std::vector<double>& data) {
  this->set_data(Eigen::VectorXd::Map(data.data(), data.size()));
}

CartesianPose CartesianPose::copy() const {
  CartesianPose result(*this);
  return result;
}

CartesianPose CartesianPose::inverse() const {
  return this->CartesianState::inverse();
}

CartesianPose CartesianPose::normalized(const CartesianStateVariable& state_variable_type) const {
  return CartesianState::normalized(state_variable_type);
}

std::vector<double> CartesianPose::norms(const CartesianStateVariable& state_variable_type) const {
  return CartesianState::norms(state_variable_type);
}

CartesianPose& CartesianPose::operator*=(const CartesianPose& pose) {
  this->CartesianState::operator*=(pose);
  return (*this);
}

CartesianPose CartesianPose::operator*(const CartesianPose& pose) const {
  return this->CartesianState::operator*(pose);
}

CartesianState CartesianPose::operator*(const CartesianState& state) const {
  return this->CartesianState::operator*(state);
}

CartesianTwist CartesianPose::operator*(const CartesianTwist& twist) const {
  return this->CartesianState::operator*(twist);
}

CartesianAcceleration CartesianPose::operator*(const CartesianAcceleration& acceleration) const {
  return this->CartesianState::operator*(acceleration);
}

CartesianWrench CartesianPose::operator*(const CartesianWrench& wrench) const {
  return this->CartesianState::operator*(wrench);
}

CartesianPose& CartesianPose::operator*=(double lambda) {
  this->CartesianState::operator*=(lambda);
  return (*this);
}

CartesianPose CartesianPose::operator*(double lambda) const {
  return this->CartesianState::operator*(lambda);
}

Eigen::Vector3d CartesianPose::operator*(const Eigen::Vector3d& vector) const {
  return this->get_orientation() * vector + this->get_position();
}

CartesianPose operator*(const CartesianState& state, const CartesianPose& pose) {
  return state.operator*(pose);
}

CartesianPose operator*(double lambda, const CartesianPose& pose) {
  return pose * lambda;
}

CartesianPose& CartesianPose::operator/=(double lambda) {
  this->CartesianState::operator/=(lambda);
  return (*this);
}

CartesianPose CartesianPose::operator/(double lambda) const {
  return this->CartesianState::operator/(lambda);
}

CartesianTwist CartesianPose::operator/(const std::chrono::nanoseconds& dt) const {
  // sanity check
  if (this->is_empty()) {
    throw EmptyStateException(this->get_name() + " state is empty");
  }
  // operations
  CartesianTwist twist(this->get_name(), this->get_reference_frame());
  // convert the period to a double with the second as reference
  double period = dt.count();
  period /= 1e9;
  // set linear velocity
  twist.set_linear_velocity(this->get_position() / period);
  // set angular velocity from the log of the quaternion error
  Eigen::Quaterniond log_q = math_tools::log(this->get_orientation());
  if (this->get_orientation().dot(log_q) < 0) {
    log_q = Eigen::Quaterniond(-log_q.coeffs());
  }
  twist.set_angular_velocity(2 * log_q.vec() / period);
  return twist;
}

CartesianPose& CartesianPose::operator+=(const CartesianPose& pose) {
  this->CartesianState::operator+=(pose);
  return (*this);
}

CartesianPose CartesianPose::operator+(const CartesianPose& pose) const {
  return this->CartesianState::operator+(pose);
}

CartesianPose& CartesianPose::operator-=(const CartesianPose& pose) {
  this->CartesianState::operator-=(pose);
  return (*this);
}

CartesianPose CartesianPose::operator-(const CartesianPose& pose) const {
  return this->CartesianState::operator-(pose);
}

std::ostream& operator<<(std::ostream& os, const CartesianPose& pose) {
  os << pose.to_string();
  return os;
}

}// namespace state_representation
