//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MeltPoolMomentumTractionForm.h"
#include "Function.h"

registerMooseObject("MalamuteApp", MeltPoolMomentumTractionForm);

InputParameters
MeltPoolMomentumTractionForm::validParams()
{
  InputParameters params = INSBase::validParams();
  params.addClassDescription("This class computes momentum equation residual and Jacobian viscous "
                             "contributions for the 'Laplacian' form of the governing equations.");
  params.addRequiredCoupledVar("level_set", "Level set variable.");
  params.addRequiredCoupledVar("temperature", "Temperature variable.");
  params.addRequiredParam<unsigned>("component", "The velocity component that this is applied to.");
  params.addParam<bool>(
      "integrate_p_by_parts", true, "Whether to integrate the pressure term by parts.");
  params.addParam<bool>(
      "supg", false, "Whether to perform SUPG stabilization of the momentum residuals");
  params.addParam<FunctionName>("forcing_func", 0, "The mms forcing function.");

  params.addRequiredCoupledVar("level_set", "Level set variable");
  params.addRequiredCoupledVar("curvature", "Regularized curvature variable");
  params.addRequiredParam<Real>("surface_tension", "Surface tension coefficient.");
  params.addRequiredParam<Real>("thermal_capillary", "Thermalcapillary coefficient.");
  params.addRequiredParam<Real>("rho_l", "Liquid density.");
  params.addRequiredParam<Real>("rho_g", "Gas density.");

  return params;
}

MeltPoolMomentumTractionForm::MeltPoolMomentumTractionForm(const InputParameters & parameters)
  : INSBase(parameters),
    _component(getParam<unsigned>("component")),
    _integrate_p_by_parts(getParam<bool>("integrate_p_by_parts")),
    _supg(getParam<bool>("supg")),
    _ffn(getFunction("forcing_func")),
    _mu_dc(getMaterialProperty<Real>("mu_dc")),
    _mu_dT(getMaterialProperty<Real>("mu_dT")),
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
    _rho(getMaterialProperty<Real>("rho")),
    _rho_dc(getMaterialProperty<Real>("rho_dc")),
    _rho_dT(getMaterialProperty<Real>("rho_dT")),
    _rho_l(getParam<Real>("rho_l")),
    _rho_g(getParam<Real>("rho_g")),
    _melt_pool_mass_rate(getMaterialProperty<Real>("melt_pool_mass_rate")),
    _melt_pool_mass_rate_dT(getMaterialProperty<Real>("melt_pool_mass_rate_dT")),
    _saturated_vapor_pressure(getMaterialProperty<Real>("saturated_vapor_pressure")),
    _saturated_vapor_pressure_dT(getMaterialProperty<Real>("saturated_vapor_pressure_dT")),
    _f_l(getMaterialProperty<Real>("liquid_mass_fraction"))
{
  if (_supg && !_convective_term)
    mooseError("It doesn't make sense to conduct SUPG stabilization without a convective term.");
}

Real
MeltPoolMomentumTractionForm::computeQpResidual()
{
  Real r = 0;

  // viscous term
  r += computeQpResidualViscousPart();

  r += computeQpResidualMeltPoolPart();

  // pressure term
  if (_integrate_p_by_parts)
    r += _grad_test[_i][_qp](_component) * weakPressureTerm();
  else
    r += _test[_i][_qp] * strongPressureTerm()(_component);

  // body force term
  r += _test[_i][_qp] * (gravityTerm()(_component) - _ffn.value(_t, _q_point[_qp]));

  // convective term
  if (_convective_term)
    r += _test[_i][_qp] * convectiveTerm()(_component);

  if (_supg)
    r += computeQpPGResidual();

  return r;
}

Real
MeltPoolMomentumTractionForm::computeQpPGResidual()
{
  const auto U = relativeVelocity();

  const auto convective_term = _convective_term ? convectiveTerm() : RealVectorValue(0, 0, 0);
  const auto viscous_term = _laplace ? strongViscousTermLaplace() : strongViscousTermTraction();
  const auto transient_term = _transient_term ? timeDerivativeTerm() : RealVectorValue(0, 0, 0);

  return tau() * U * _grad_test[_i][_qp] *
         ((convective_term + viscous_term + transient_term + strongPressureTerm() +
           strongMeltPoolTerm() + gravityTerm())(_component)-_ffn.value(_t, _q_point[_qp]));

  // For GLS as opposed to SUPG stabilization, one would need to modify the test function functional
  // space to include second derivatives of the Galerkin test functions corresponding to the viscous
  // term. This would look like:
  // Real lap_test =
  //     _second_test[_i][_qp](0, 0) + _second_test[_i][_qp](1, 1) + _second_test[_i][_qp](2, 2);

  // Real pg_viscous_r = -_mu[_qp] * lap_test * tau() *
  //                     (convective_term + viscous_term + strongPressureTerm()(_component) +
  //                      gravityTerm())(_component);
}

Real
MeltPoolMomentumTractionForm::computeQpJacobian()
{
  Real jac = 0;

  // viscous term
  jac += computeQpJacobianViscousPart();

  jac += computeQpJacobianMeltPoolPart();

  // convective term
  if (_convective_term)
    jac += _test[_i][_qp] * dConvecDUComp(_component)(_component);

  if (_supg)
    jac += computeQpPGJacobian(_component);

  return jac;
}

Real
MeltPoolMomentumTractionForm::computeQpPGJacobian(unsigned comp)
{
  const auto U = relativeVelocity();
  RealVectorValue d_U_d_U_comp(0, 0, 0);
  d_U_d_U_comp(comp) = _phi[_j][_qp];

  const Real convective_term = _convective_term ? convectiveTerm()(_component) : 0;
  const Real d_convective_term_d_u_comp = _convective_term ? dConvecDUComp(comp)(_component) : 0;
  const Real viscous_term =
      _laplace ? strongViscousTermLaplace()(_component) : strongViscousTermTraction()(_component);
  const Real d_viscous_term_d_u_comp = _laplace ? dStrongViscDUCompLaplace(comp)(_component)
                                                : dStrongViscDUCompTraction(comp)(_component);
  const Real transient_term = _transient_term ? timeDerivativeTerm()(_component) : 0;
  const Real d_transient_term_d_u_comp =
      _transient_term ? dTimeDerivativeDUComp(comp)(_component) : 0;
  const Real d_melt_pool_term_d_u_comp = dstrongMeltPoolTerm(comp);

  return dTauDUComp(comp) * U * _grad_test[_i][_qp] *
             (convective_term + viscous_term + strongPressureTerm()(_component) +
              gravityTerm()(_component) + transient_term - _ffn.value(_t, _q_point[_qp])) +
         tau() * d_U_d_U_comp * _grad_test[_i][_qp] *
             (convective_term + viscous_term + strongPressureTerm()(_component) +
              gravityTerm()(_component) + transient_term - _ffn.value(_t, _q_point[_qp])) +
         tau() * U * _grad_test[_i][_qp] *
             (d_convective_term_d_u_comp + d_viscous_term_d_u_comp + d_transient_term_d_u_comp +
              d_melt_pool_term_d_u_comp);
}

Real
MeltPoolMomentumTractionForm::computeQpOffDiagJacobian(unsigned jvar)
{
  Real jac = 0;
  if (jvar == _u_vel_var_number)
  {
    Real convective_term = _convective_term ? _test[_i][_qp] * dConvecDUComp(0)(_component) : 0.;
    Real viscous_term = computeQpOffDiagJacobianViscousPart(jvar);

    jac += convective_term + viscous_term;

    if (_supg)
      jac += computeQpPGJacobian(0);

    return jac;
  }
  else if (jvar == _v_vel_var_number)
  {
    Real convective_term = _convective_term ? _test[_i][_qp] * dConvecDUComp(1)(_component) : 0.;
    Real viscous_term = computeQpOffDiagJacobianViscousPart(jvar);

    jac += convective_term + viscous_term;

    if (_supg)
      jac += computeQpPGJacobian(1);

    return jac;
  }
  else if (jvar == _w_vel_var_number)
  {
    Real convective_term = _convective_term ? _test[_i][_qp] * dConvecDUComp(2)(_component) : 0.;
    Real viscous_term = computeQpOffDiagJacobianViscousPart(jvar);

    jac += convective_term + viscous_term;

    if (_supg)
      jac += computeQpPGJacobian(2);

    return jac;
  }

  else if (jvar == _p_var_number)
  {
    if (_integrate_p_by_parts)
      jac += _grad_test[_i][_qp](_component) * dWeakPressureDPressure();
    else
      jac += _test[_i][_qp] * dStrongPressureDPressure()(_component);

    if (_supg)
    {
      const auto U = relativeVelocity();
      jac += tau() * U * _grad_test[_i][_qp] * dStrongPressureDPressure()(_component);
    }

    return jac;
  }

  else
    return 0;
}

Real
MeltPoolMomentumTractionForm::computeQpResidualViscousPart()
{
  // The component'th row (or col, it's symmetric) of the viscous stress tensor
  RealVectorValue tau_row;

  switch (_component)
  {
    case 0:
      tau_row(0) = 2. * _grad_u_vel[_qp](0);                  // 2*du/dx1
      tau_row(1) = _grad_u_vel[_qp](1) + _grad_v_vel[_qp](0); // du/dx2 + dv/dx1
      tau_row(2) = _grad_u_vel[_qp](2) + _grad_w_vel[_qp](0); // du/dx3 + dw/dx1
      break;

    case 1:
      tau_row(0) = _grad_v_vel[_qp](0) + _grad_u_vel[_qp](1); // dv/dx1 + du/dx2
      tau_row(1) = 2. * _grad_v_vel[_qp](1);                  // 2*dv/dx2
      tau_row(2) = _grad_v_vel[_qp](2) + _grad_w_vel[_qp](1); // dv/dx3 + dw/dx2
      break;

    case 2:
      tau_row(0) = _grad_w_vel[_qp](0) + _grad_u_vel[_qp](2); // dw/dx1 + du/dx3
      tau_row(1) = _grad_w_vel[_qp](1) + _grad_v_vel[_qp](2); // dw/dx2 + dv/dx3
      tau_row(2) = 2. * _grad_w_vel[_qp](2);                  // 2*dw/dx3
      break;

    default:
      mooseError("Unrecognized _component requested.");
  }

  // The viscous part, _mu[_qp] * tau : grad(v)
  return _mu[_qp] * (tau_row * _grad_test[_i][_qp]);
}

Real
MeltPoolMomentumTractionForm::computeQpJacobianViscousPart()
{
  // Viscous part, full stress tensor.  The extra contribution comes from the "2"
  // on the diagonal of the viscous stress tensor.
  return _mu[_qp] * (_grad_phi[_j][_qp] * _grad_test[_i][_qp] +
                     _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](_component));
}

Real
MeltPoolMomentumTractionForm::computeQpOffDiagJacobianViscousPart(unsigned int jvar)
{
  // In Stokes/Laplacian version, off-diag Jacobian entries wrt u,v,w are zero
  if (jvar == _u_vel_var_number)
    return _mu[_qp] * _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](0);

  else if (jvar == _v_vel_var_number)
    return _mu[_qp] * _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](1);

  else if (jvar == _w_vel_var_number)
    return _mu[_qp] * _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](2);

  else if (jvar == _c_id)
  { // The component'th row (or col, it's symmetric) of the viscous stress tensor
    RealVectorValue tau_row;

    switch (_component)
    {
      case 0:
        tau_row(0) = 2. * _grad_u_vel[_qp](0);                  // 2*du/dx1
        tau_row(1) = _grad_u_vel[_qp](1) + _grad_v_vel[_qp](0); // du/dx2 + dv/dx1
        tau_row(2) = _grad_u_vel[_qp](2) + _grad_w_vel[_qp](0); // du/dx3 + dw/dx1
        break;

      case 1:
        tau_row(0) = _grad_v_vel[_qp](0) + _grad_u_vel[_qp](1); // dv/dx1 + du/dx2
        tau_row(1) = 2. * _grad_v_vel[_qp](1);                  // 2*dv/dx2
        tau_row(2) = _grad_v_vel[_qp](2) + _grad_w_vel[_qp](1); // dv/dx3 + dw/dx2
        break;

      case 2:
        tau_row(0) = _grad_w_vel[_qp](0) + _grad_u_vel[_qp](2); // dw/dx1 + du/dx3
        tau_row(1) = _grad_w_vel[_qp](1) + _grad_v_vel[_qp](2); // dw/dx2 + dv/dx3
        tau_row(2) = 2. * _grad_w_vel[_qp](2);                  // 2*dw/dx3
        break;

      default:
        mooseError("Unrecognized _component requested.");
    }

    // The viscous part, _mu[_qp] * tau : grad(v)
    return _mu_dc[_qp] * _phi[_j][_qp] * (tau_row * _grad_test[_i][_qp]);
  }
  else if (jvar == _temp_id)
  {
    RealVectorValue tau_row;

    switch (_component)
    {
      case 0:
        tau_row(0) = 2. * _grad_u_vel[_qp](0);                  // 2*du/dx1
        tau_row(1) = _grad_u_vel[_qp](1) + _grad_v_vel[_qp](0); // du/dx2 + dv/dx1
        tau_row(2) = _grad_u_vel[_qp](2) + _grad_w_vel[_qp](0); // du/dx3 + dw/dx1
        break;

      case 1:
        tau_row(0) = _grad_v_vel[_qp](0) + _grad_u_vel[_qp](1); // dv/dx1 + du/dx2
        tau_row(1) = 2. * _grad_v_vel[_qp](1);                  // 2*dv/dx2
        tau_row(2) = _grad_v_vel[_qp](2) + _grad_w_vel[_qp](1); // dv/dx3 + dw/dx2
        break;

      case 2:
        tau_row(0) = _grad_w_vel[_qp](0) + _grad_u_vel[_qp](2); // dw/dx1 + du/dx3
        tau_row(1) = _grad_w_vel[_qp](1) + _grad_v_vel[_qp](2); // dw/dx2 + dv/dx3
        tau_row(2) = 2. * _grad_w_vel[_qp](2);                  // 2*dw/dx3
        break;

      default:
        mooseError("Unrecognized _component requested.");
    }

    // The viscous part, _mu[_qp] * tau : grad(v)
    return _mu_dT[_qp] * _phi[_j][_qp] * (tau_row * _grad_test[_i][_qp]);
  }

  return 0.;
}

RealVectorValue
MeltPoolMomentumTractionForm::strongMeltPoolTerm()
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

  return -(darcy_term + surface_tension_term + recoil_pressure + thermalcapillary_term);
}

Real
MeltPoolMomentumTractionForm::dstrongMeltPoolTerm(unsigned)
{
  return _permeability[_qp] * (1 - _heaviside_function[_qp]);
}

Real
MeltPoolMomentumTractionForm::computeQpResidualMeltPoolPart()
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

  return -(darcy_term + surface_tension_term + recoil_pressure +
           thermalcapillary_term)(_component)*_test[_i][_qp];
}

Real
MeltPoolMomentumTractionForm::computeQpJacobianMeltPoolPart()
{
  return _permeability[_qp] * (1 - _heaviside_function[_qp]) * _phi[_j][_qp] * _test[_i][_qp];
}

Real
MeltPoolMomentumTractionForm::computeQpOffDiagJacobianMeltPoolPart(unsigned int jvar)
{
  if (jvar == _c_id)
  {
    const auto U = relativeVelocity();

    RealVectorValue d_darcy_term_dc =
        -_permeability[_qp] * (-_heaviside_function_dc[_qp] * _phi[_j][_qp]) * U;

    RealVectorValue d_surface_tension_term_dc = RealVectorValue(0.0);
    RealVectorValue d_thermalcapillary_term_dc = RealVectorValue(0.0);
    RealVectorValue d_recoil_pressure_dc = RealVectorValue(0.0);
    RankTwoTensor iden(RankTwoTensor::initIdentity);
    RankTwoTensor proj;
    RealVectorValue normal = RealVectorValue(0.0);

    if (_f_l[_qp] > libMesh::TOLERANCE && _delta_function[_qp] > libMesh::TOLERANCE)
    {
      normal = _grad_c[_qp] /
               (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();

      proj.vectorOuterProduct(normal, normal);
      proj = iden - proj;
      d_surface_tension_term_dc = _sigma * _curvature[_qp] * (_grad_phi[_j][_qp]);

      d_thermalcapillary_term_dc =
          proj * _grad_temp[_qp] * _sigmaT * _delta_function_dc[_qp] * _grad_phi[_j][_qp];

      // Recoil Pressure
      d_recoil_pressure_dc = -0.55 * _saturated_vapor_pressure[_qp] * (_grad_phi[_j][_qp]);
    }

    return (d_darcy_term_dc + d_surface_tension_term_dc + d_thermalcapillary_term_dc +
            d_recoil_pressure_dc)(_component)*_test[_i][_qp];
  }
  else if (jvar == _temp_id)
  {
    RealVectorValue d_thermalcapillary_term_dT = RealVectorValue(0.0);
    RealVectorValue d_recoil_pressure_dT = RealVectorValue(0.0);
    RankTwoTensor iden(RankTwoTensor::initIdentity);
    RankTwoTensor proj;
    RealVectorValue normal = RealVectorValue(0.0);

    if (_f_l[_qp] > libMesh::TOLERANCE && _delta_function[_qp] > libMesh::TOLERANCE)
    {
      normal = _grad_c[_qp] /
               (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();

      proj.vectorOuterProduct(normal, normal);
      proj = iden - proj;

      // TODO
      d_thermalcapillary_term_dT = proj * _grad_phi[_j][_qp] * _sigmaT * _delta_function[_qp];

      // Recoil Pressure
      d_recoil_pressure_dT =
          -0.55 * _saturated_vapor_pressure_dT[_qp] * _phi[_j][_qp] *
          (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE));
    }

    return (d_thermalcapillary_term_dT + d_recoil_pressure_dT)(_component)*_test[_i][_qp];
  }
  else if (jvar == _curvature_id)
  {
    RealVectorValue d_surface_tension_term_dcur = RealVectorValue(0.0);
    if (_f_l[_qp] > libMesh::TOLERANCE && _delta_function[_qp] > libMesh::TOLERANCE)
    {
      d_surface_tension_term_dcur =
          _sigma * _phi[_j][_qp] *
          (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE));
    }
    return d_surface_tension_term_dcur(_component) * _test[_i][_qp];
  }

  return 0.;
}
