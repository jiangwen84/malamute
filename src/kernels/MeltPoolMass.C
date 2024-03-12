//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MeltPoolMass.h"
#include "Function.h"

registerMooseObject("MalamuteApp", MeltPoolMass);

InputParameters
MeltPoolMass::validParams()
{
  InputParameters params = INSBase::validParams();

  params.addClassDescription("This class computes the mass equation residual and Jacobian "
                             "contributions for the incompressible Navier-Stokes momentum "
                             "equation.");
  params.addParam<bool>(
      "pspg", false, "Whether to perform PSPG stabilization of the mass equation");
  params.addParam<FunctionName>("x_vel_forcing_func", 0, "The x-velocity mms forcing function.");
  params.addParam<FunctionName>("y_vel_forcing_func", 0, "The y-velocity mms forcing function.");
  params.addParam<FunctionName>("z_vel_forcing_func", 0, "The z-velocity mms forcing function.");
  params.addRequiredCoupledVar("level_set", "Level set variable");
  params.addRequiredCoupledVar("curvature", "Regularized curvature variable");
  params.addRequiredParam<Real>("surface_tension", "Surface tension coefficient.");
  params.addRequiredParam<Real>("thermal_capillary", "Thermalcapillary coefficient.");
  params.addRequiredCoupledVar("level_set", "Level set variable.");
  params.addRequiredCoupledVar("temperature", "Temperature variable.");
  return params;
}

MeltPoolMass::MeltPoolMass(const InputParameters & parameters)
  : INSBase(parameters),
    _pspg(getParam<bool>("pspg")),
    _x_ffn(getFunction("x_vel_forcing_func")),
    _y_ffn(getFunction("y_vel_forcing_func")),
    _z_ffn(getFunction("z_vel_forcing_func")),
    _c_id(coupled("level_set")),
    _temp_id(coupled("temperature")),
    _curvature_id(coupled("curvature")),

    _grad_c(coupledGradient("level_set")),
    _temp(coupledValue("temperature")),
    _grad_temp(coupledGradient("temperature")),
    _curvature(coupledValue("curvature")),
    _permeability(getMaterialProperty<Real>("permeability")),
    _permeability_dT(getMaterialProperty<Real>("permeability_dT")),
    _sigma(getParam<Real>("surface_tension")),
    _sigmaT(getParam<Real>("thermal_capillary")),
    _delta_function(getMaterialProperty<Real>("delta_function")),
    _delta_function_dc(getMaterialProperty<RealVectorValue>("delta_function_dc")),
    _heaviside_function(getMaterialProperty<Real>("heaviside_function")),
    _heaviside_function_dc(getMaterialProperty<Real>("heaviside_function_dc")),
    _saturated_vapor_pressure(getMaterialProperty<Real>("saturated_vapor_pressure")),
    _saturated_vapor_pressure_dT(getMaterialProperty<Real>("saturated_vapor_pressure_dT")),
    _f_l(getMaterialProperty<Real>("liquid_mass_fraction"))

{
}

Real
MeltPoolMass::computeQpResidual()
{
  // (div u) * q
  // Note: we (arbitrarily) multiply this term by -1 so that it matches the -p(div v)
  // term in the momentum equation.  Not sure if that is really important?
  Real r = -(_grad_u_vel[_qp](0) + _grad_v_vel[_qp](1) + _grad_w_vel[_qp](2)) * _test[_i][_qp];

  if (_pspg)
    r += computeQpPGResidual();

  return r;
}

RealVectorValue
MeltPoolMass::strongMeltPoolTerm()
{
  const auto U = relativeVelocity();

  RealVectorValue darcy_term = -_permeability[_qp] * (1 - _heaviside_function[_qp]) * U;

  RealVectorValue surface_tension_term = RealVectorValue(0.0);
  RealVectorValue thermalcapillary_term = RealVectorValue(0.0);
  RealVectorValue recoil_pressure = RealVectorValue(0.0);
  RankTwoTensor iden(RankTwoTensor::initIdentity);
  RankTwoTensor proj;
  RealVectorValue normal = RealVectorValue(0.0);

  normal = _grad_c[_qp] /
           (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();

  proj.vectorOuterProduct(normal, normal);
  proj = iden - proj;
  surface_tension_term = _sigma * _curvature[_qp] * _grad_c[_qp];

  thermalcapillary_term = proj * _grad_temp[_qp] * _sigmaT * _delta_function[_qp];

  // Recoil Pressure
  recoil_pressure = -0.55 * _saturated_vapor_pressure[_qp] * _grad_c[_qp];
  // }

  return -(darcy_term + surface_tension_term + recoil_pressure + thermalcapillary_term);
}

Real
MeltPoolMass::dstrongMeltPoolTerm(unsigned)
{
  return _permeability[_qp] * (1 - _heaviside_function[_qp]);
}

Real
MeltPoolMass::computeQpPGResidual()
{
  RealVectorValue viscous_term =
      _laplace ? strongViscousTermLaplace() : strongViscousTermTraction();
  RealVectorValue transient_term =
      _transient_term ? timeDerivativeTerm() : RealVectorValue(0, 0, 0);
  RealVectorValue convective_term = _convective_term ? convectiveTerm() : RealVectorValue(0, 0, 0);
  Real r = -1. / _rho[_qp] * tau() * _grad_test[_i][_qp] *
           (strongPressureTerm() + gravityTerm() + viscous_term + strongMeltPoolTerm() +
            convective_term + transient_term -
            RealVectorValue(_x_ffn.value(_t, _q_point[_qp]),
                            _y_ffn.value(_t, _q_point[_qp]),
                            _z_ffn.value(_t, _q_point[_qp])));

  return r;
}

Real
MeltPoolMass::computeQpJacobian()
{
  // Derivative wrt to p is zero
  Real r = 0;

  // Unless we are doing PSPG stabilization
  if (_pspg)
    r += computeQpPGJacobian();

  return r;
}

Real
MeltPoolMass::computeQpPGJacobian()
{
  return -1. / _rho[_qp] * tau() * _grad_test[_i][_qp] * dStrongPressureDPressure();
}

Real
MeltPoolMass::computeQpOffDiagJacobian(const unsigned int jvar)
{
  if (jvar == _u_vel_var_number)
  {
    Real jac = -_grad_phi[_j][_qp](0) * _test[_i][_qp];
    if (_pspg)
      jac += computeQpPGOffDiagJacobian(0);
    return jac;
  }

  else if (jvar == _v_vel_var_number)
  {
    Real jac = -_grad_phi[_j][_qp](1) * _test[_i][_qp];
    if (_pspg)
      jac += computeQpPGOffDiagJacobian(1);
    return jac;
  }

  else if (jvar == _w_vel_var_number)
  {
    Real jac = -_grad_phi[_j][_qp](2) * _test[_i][_qp];
    if (_pspg)
      jac += computeQpPGOffDiagJacobian(2);
    return jac;
  }

  else
    return 0.0;
}

Real
MeltPoolMass::computeQpPGOffDiagJacobian(const unsigned int comp)
{
  RealVectorValue convective_term = _convective_term ? convectiveTerm() : RealVectorValue(0, 0, 0);
  RealVectorValue d_convective_term_d_u_comp =
      _convective_term ? dConvecDUComp(comp) : RealVectorValue(0, 0, 0);
  RealVectorValue viscous_term =
      _laplace ? strongViscousTermLaplace() : strongViscousTermTraction();
  RealVectorValue d_viscous_term_d_u_comp =
      _laplace ? dStrongViscDUCompLaplace(comp) : dStrongViscDUCompTraction(comp);
  RealVectorValue transient_term =
      _transient_term ? timeDerivativeTerm() : RealVectorValue(0, 0, 0);
  RealVectorValue d_transient_term_d_u_comp =
      _transient_term ? dTimeDerivativeDUComp(comp) : RealVectorValue(0, 0, 0);

  return -1. / _rho[_qp] * tau() * _grad_test[_i][_qp] *
             (d_convective_term_d_u_comp + d_viscous_term_d_u_comp + d_transient_term_d_u_comp) -
         1. / _rho[_qp] * dTauDUComp(comp) * _grad_test[_i][_qp] *
             (convective_term + viscous_term + transient_term + strongPressureTerm() +
              gravityTerm() -
              RealVectorValue(_x_ffn.value(_t, _q_point[_qp]),
                              _y_ffn.value(_t, _q_point[_qp]),
                              _z_ffn.value(_t, _q_point[_qp])));
}
