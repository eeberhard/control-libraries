#include "state_representation_bindings.h"

#include <state_representation/parameters/Parameter.hpp>
#include <state_representation/parameters/ParameterMap.hpp>

#include "parameter_container.h"
#include "py_parameter_map.h"

using namespace py_parameter;

void parameter_interface(py::module_& m) {
  py::class_<ParameterInterface, std::shared_ptr<ParameterInterface>, State> c(m, "ParameterInterface");

  c.def(py::init<const StateType&, const std::string&>(), "Constructor with parameter name and type of the parameter", "type"_a, "name"_a);
  c.def(py::init<const ParameterInterface&>(), "Copy constructor from another ParameterInterface", "parameter"_a);
}

void parameter(py::module_& m) {
  py::class_<ParameterContainer, std::shared_ptr<ParameterContainer>, ParameterInterface> c(m, "Parameter");

  c.def(py::init<const std::string&, const StateType&>(), "Constructor of a parameter with name and type", "name"_a, "type"_a);
  c.def(py::init<const std::string&, const py::object&, const StateType&>(), "Constructor of a parameter with name, value and type", "name"_a, "value"_a, "type"_a);
  c.def(py::init<const ParameterContainer&>(), "Copy constructor from another Parameter", "parameter"_a);
  c.def(py::init([](const std::shared_ptr<ParameterInterface>& parameter) { return interface_ptr_to_container(parameter); }), "Constructor from a parameter interface pointer", "parameter"_a);

  c.def("get_value", &ParameterContainer::get_value, "Getter of the value attribute.");
  c.def("set_value", &ParameterContainer::set_value, "Setter of the value attribute.", py::arg("value"));

  c.def("__copy__", [](const ParameterContainer& parameter) {
    return ParameterContainer(parameter);
  });
  c.def("__deepcopy__", [](const ParameterContainer& parameter, py::dict) {
    return ParameterContainer(parameter);
  }, "memo"_a);
  c.def("__repr__", [](const ParameterContainer& parameter) {
    std::stringstream buffer;
    switch (parameter.get_type()) {
      case StateType::PARAMETER_INT: {
        Parameter<int> param(parameter.get_name(), parameter.values.int_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_INT_ARRAY: {
        Parameter<std::vector<int>> param(parameter.get_name(), parameter.values.int_array_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_DOUBLE: {
        Parameter<double> param(parameter.get_name(), parameter.values.double_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_DOUBLE_ARRAY: {
        Parameter<std::vector<double>> param(parameter.get_name(), parameter.values.double_array_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_BOOL: {
        Parameter<bool> param(parameter.get_name(), parameter.values.bool_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_BOOL_ARRAY: {
        Parameter<std::vector<bool>> param(parameter.get_name(), parameter.values.bool_array_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_STRING: {
        Parameter<std::string> param(parameter.get_name(), parameter.values.string_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_STRING_ARRAY: {
        Parameter<std::vector<std::string>> param(parameter.get_name(), parameter.values.string_array_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_CARTESIANSTATE: {
        Parameter<CartesianState> param(parameter.get_name(), parameter.values.cartesian_state);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_CARTESIANPOSE: {
        Parameter<CartesianPose> param(parameter.get_name(), parameter.values.cartesian_pose);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_JOINTSTATE: {
        Parameter<JointState> param(parameter.get_name(), parameter.values.joint_state);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_JOINTPOSITIONS: {
        Parameter<JointPositions> param(parameter.get_name(), parameter.values.joint_positions);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_MATRIX: {
        Parameter<Eigen::MatrixXd> param(parameter.get_name(), parameter.values.matrix_value);
        buffer << param;
        break;
      }
      case StateType::PARAMETER_VECTOR: {
        Parameter<Eigen::VectorXd> param(parameter.get_name(), parameter.values.vector_value);
        buffer << param;
        break;
      }
      default:
        break;
    }
    return buffer.str();
  });
}

void parameter_map(py::module_& m) {
  py::class_<ParameterMap, std::shared_ptr<ParameterMap>, PyParameterMap> c(m, "ParameterMap");

  c.def(py::init(), "Empty constructor");
  c.def(
      py::init([](const std::map<std::string, ParameterContainer>& parameters) {
        auto parameter_map = container_to_interface_ptr_map(parameters);
        return ParameterMap(parameter_map);
      }), "Construct the parameter map with an initial list of parameters", "parameters"_a
  );
  c.def(
      py::init([](const std::list<ParameterContainer>& parameters) {
        auto parameter_list = container_to_interface_ptr_list(parameters);
        return ParameterMap(parameter_list);
      }), "Construct the parameter map with an initial map of parameters", "parameters"_a);

  c.def(
      "get_parameter", [](ParameterMap& self, const std::string& name) -> ParameterContainer {
        return interface_ptr_to_container(self.get_parameter(name));
       } , "Get a parameter by its name", "name"_a
  );
  c.def(
      "get_parameters", [](ParameterMap& self) {
        return interface_ptr_to_container_map(self.get_parameters());
      } , "Get a map of all the <name, parameter> pairs"
  );
  c.def(
      "get_parameter_value", [](ParameterMap& self, const std::string& name) -> py::object {
        return interface_ptr_to_container(self.get_parameter(name)).get_value();
      }, "Get a parameter value by its name", "name"_a
  );
  c.def(
      "get_parameter_list", [](ParameterMap& self) {
        return interface_ptr_to_container_list(self.get_parameter_list());
      } , "Get a list of all the parameters"
  );

  c.def("set_parameter", [](ParameterMap& self, const ParameterContainer& parameter) {
    self.set_parameter(container_to_interface_ptr(parameter));
  }, "Set a parameter", "parameter"_a);
  c.def("set_parameters", [](ParameterMap& self, const std::list<ParameterContainer>& parameters) {
    self.set_parameters(container_to_interface_ptr_list(parameters));
  }, "Set parameters from a list of parameters", "parameters"_a);
  c.def("set_parameters", [](ParameterMap& self, const std::map<std::string, ParameterContainer>& parameters) {
    self.set_parameters(container_to_interface_ptr_map(parameters));
  }, "Set parameters from a map with <name, parameter> pairs", "parameters"_a);
  c.def(
      "set_parameter_value", [](ParameterMap& self, const std::string& name, const py::object& value, const StateType& type) -> void {
        auto param = ParameterContainer(name, value, type);
        self.set_parameter(container_to_interface_ptr(param));
      }, "Set a parameter value by its name", "name"_a, "value"_a, "type"_a
  );
}

void bind_parameters(py::module_& m) {
  parameter_interface(m);
  parameter(m);
  parameter_map(m);
}