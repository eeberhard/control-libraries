#pragma once

#include <list>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <random>
#include "state_representation/geometry/Shape.hpp"
#include "state_representation/space/cartesian/CartesianPose.hpp"

namespace state_representation {
/**
 * @class Ellipsoid
 */
class Ellipsoid : public Shape {
private:
  std::vector<double> axis_lengths_; ///< axis lengths in x,y directions
  double rotation_angle_; ///< angle of rotation around z axis of the reference frame

public:
  /**
   * @brief Empty constructor
   */
  Ellipsoid();

  /**
   * @brief Constructor with name and reference frame
   * @param name Name of the Ellipsoid and its state
   * @param reference_frame The reference frame in which the state is expressed (default is "world")
   */
  explicit Ellipsoid(const std::string& name, const std::string& reference_frame = "world");

  /**
   * @brief Copy constructor from another Ellipsoid
   * @param shape The Ellipsoid to copy from
   */
  Ellipsoid(const Ellipsoid& ellipsoid);

  /**
   * @brief Constructor for a Ellipsoid with identity state, unit axis lengths and zero rotation angle
   * @param name Name of the Ellipsoid and its state
   * @param reference_frame The reference frame in which the state is expressed (default is "world")
   */
  static Ellipsoid Unit(const std::string& name, const std::string& reference_frame = "world");

  /**
   * @brief Swap the values of the two Ellipsoids
   * @param state1 Ellipsoid to be swapped with 2
   * @param state2 Ellipsoid to be swapped with 1
   */
  friend void swap(Ellipsoid& state1, Ellipsoid& state2);

  /**
   * @brief Copy assignment operator that has to be defined to the custom assignment operator
   * @param state The Ellipsoid with value to assign
   * @return Reference to the current Ellipsoid with new values
   */
  Ellipsoid& operator=(const Ellipsoid& state);

  /**
   * @brief Getter of the axis lengths
   * @return The axis lengths
   */
  const std::vector<double>& get_axis_lengths() const;

  /**
   * @brief Getter of the axis length in one direction
   * @param index The index of the length (0 for x, 1 for y and 2 for z)
   * @return The length in the desired direction
   */
  double get_axis_length(unsigned int index) const;

  /**
   * @brief Setter of the axis lengths
   * @param axis_lengths The new values
   */
  void set_axis_lengths(const std::vector<double>& axis_lengths);

  /**
   * @brief Setter of the axis length at given index
   * @param index The desired index
   * @param axis_length The new length
   */
  void set_axis_lengths(unsigned int index, double axis_length);

  /**
   * @brief Getter of the rotation angle
   * @return The rotation angle
   */
  double get_rotation_angle() const;

  /**
   * @brief Setter of the rotation angle
   * @param rotation_angle The rotation angle
   */
  void set_rotation_angle(double rotation_angle);

  /**
   * @brief Getter of the rotation as a pose
   */
  const CartesianPose get_rotation() const;

  /**
   * @brief Function to sample an obstacle from its parameterization
   * @param nb_samples The number of sample points to generate
   * @return The list of sample points
   */
  const std::list<CartesianPose> sample_from_parameterization(unsigned int nb_samples) const;

  /**
   * @brief Compute an Ellipsoid from its algebraic equation ax2 + bxy + cy2 + cx + ey + f
   * @return The Ellipsoid in its geometric representation
   */
  static const Ellipsoid from_algebraic_equation(
      const std::string& name, const std::vector<double>& coefficients, const std::string& reference_frame = "world"
  );

  /**
   * @brief Fit an Ellipsoid on a set of points
   * This method uses direct least square fitting from
   * Fitzgibbon, A., et al. (1999). "Direct least square fitting of ellipses."
    * IEEE Transactions on pattern analysis and machine intelligence 21(5)
   */
  static const Ellipsoid fit(
      const std::string& name, const std::list<CartesianPose>& points, const std::string& reference_frame = "world",
      double noise_level = 0.01
  );

  /**
   * @brief Convert the Ellipsoid to an std vector representation of its parameter
   * @return An std vector with [center_position, rotation_angle, axis_lengths]
   */
  const std::vector<double> to_std_vector() const;

  /**
   * @brief Set the ellipsoid data from an Eigen vector
   * @param The data vector with [center_position, rotation_angle, axis_lengths]
   */
  void set_data(const Eigen::VectorXd& data) override;

  /**
   * @brief Set the ellipsoid data from a std vector
   * @param The data vector with [center_position, rotation_angle, axis_lengths]
   */
  void set_data(const std::vector<double>& data) override;

  /**
    * @brief Overload the ostream operator for printing
    * @param os The ostream to append the string representing the state
    * @param state The state to print
    * @return The appended ostream
     */
  friend std::ostream& operator<<(std::ostream& os, const Ellipsoid& ellipsoid);
};

inline void swap(Ellipsoid& state1, Ellipsoid& state2) {
  swap(static_cast<Shape&>(state1), static_cast<Shape&>(state2));
  std::swap(state1.axis_lengths_, state2.axis_lengths_);
  std::swap(state1.rotation_angle_, state2.rotation_angle_);
}

inline Ellipsoid& Ellipsoid::operator=(const Ellipsoid& state) {
  Ellipsoid tmp(state);
  swap(*this, tmp);
  return *this;
}

inline const std::vector<double>& Ellipsoid::get_axis_lengths() const {
  return this->axis_lengths_;
}

inline double Ellipsoid::get_axis_length(unsigned int index) const {
  return this->axis_lengths_[index];
}

inline void Ellipsoid::set_axis_lengths(const std::vector<double>& axis_lengths) {
  this->axis_lengths_ = axis_lengths;
  this->set_empty(false);
}

inline void Ellipsoid::set_axis_lengths(unsigned int index, double axis_length) {
  this->axis_lengths_[index] = axis_length;
  this->set_empty(false);
}

inline double Ellipsoid::get_rotation_angle() const {
  return this->rotation_angle_;
}

inline void Ellipsoid::set_rotation_angle(double rotation_angle) {
  this->rotation_angle_ = rotation_angle;
  this->set_empty(false);
}

inline const std::vector<double> Ellipsoid::to_std_vector() const {
  if (this->is_empty()) {
    throw exceptions::EmptyStateException(this->get_name() + " state is empty");
  }
  std::vector<double> representation(6);
  // position
  representation[0] = this->get_center_position()(0);
  representation[1] = this->get_center_position()(1);
  representation[2] = this->get_center_position()(2);
  // rotation angle
  representation[3] = this->get_rotation_angle();
  // axis lengths
  representation[4] = this->get_axis_length(0);
  representation[5] = this->get_axis_length(1);
  return representation;
}

inline const CartesianPose Ellipsoid::get_rotation() const {
  if (this->is_empty()) {
    throw exceptions::EmptyStateException(this->get_name() + " state is empty");
  }
  Eigen::Quaterniond rotation(Eigen::AngleAxisd(this->rotation_angle_, Eigen::Vector3d::UnitZ()));
  return CartesianPose(
      this->get_center_pose().get_name() + "_rotated", Eigen::Vector3d::Zero(), rotation,
      this->get_center_pose().get_name());
}
}