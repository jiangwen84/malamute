/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "INSMeltPoolMaterial.h"

registerADMooseObject("MalamuteApp", INSMeltPoolMaterial);

InputParameters
INSMeltPoolMaterial::validParams()
{
  InputParameters params = INSADStabilized3Eqn::validParams();
  params.addClassDescription("Computes extra residuals from melt pool for the INS equations.");
  // params.addRequiredCoupledVar("level_set_gradient", "Regularized gradient of Level set
  // variable");
  params.addRequiredCoupledVar("curvature", "Regularized curvature variable");
  params.addRequiredCoupledVar("level_set", "Level set variable");
  params.addRequiredParam<Real>("surface_tension", "Surface tension coefficient.");
  params.addRequiredParam<Real>("thermal_capillary", "Thermalcapillary coefficient.");
  params.addRequiredParam<Real>("rho_l", "Liquid density.");
  params.addRequiredParam<Real>("rho_g", "Gas density.");
  params.addRequiredParam<Real>("fusion_latent_heat", "Latent heat of fusion.");
  params.addCoupledVar("fluid_mass_fraction", "Fluid mass fraction variable");
  return params;
}

INSMeltPoolMaterial::INSMeltPoolMaterial(const InputParameters & parameters)
  : INSADStabilized3Eqn(parameters),
    //_grad_c(adCoupledVectorValue("level_set_gradient")),
    _grad_cv(adCoupledGradient("level_set")),
    _temp(adCoupledValue("temperature")),
    _grad_temp(adCoupledGradient("temperature")),
    _curvature(adCoupledValue("curvature")),
    _permeability(getADMaterialProperty<Real>("permeability")),
    _sigma(getParam<Real>("surface_tension")),
    _sigmaT(getParam<Real>("thermal_capillary")),
    _delta_function(getADMaterialProperty<Real>("delta_function")),
    _heaviside_function(getADMaterialProperty<Real>("heaviside_function")),
    _melt_pool_momentum_source(declareADProperty<RealVectorValue>("melt_pool_momentum_source")),
    _rho(getADMaterialProperty<Real>("rho")),
    _rho_l(getParam<Real>("rho_l")),
    _rho_g(getParam<Real>("rho_g")),
    _melt_pool_mass_rate(getADMaterialProperty<Real>("melt_pool_mass_rate")),
    _saturated_vapor_pressure(getADMaterialProperty<Real>("saturated_vapor_pressure")),
    _f_l(getADMaterialProperty<Real>("liquid_mass_fraction")),
    _drho_dc(getADMaterialProperty<Real>("drho_dc")),
    _dmelt_pool_mass_rate_dT(getADMaterialProperty<Real>("dmelt_pool_mass_rate_dT")),
    _Lm(getParam<Real>("fusion_latent_heat")),
    _grad_fl(coupledGradient("fluid_mass_fraction")),
    _f_l_old(getMaterialPropertyOld<Real>("liquid_mass_fraction")),
    _f_l_rate(declareADProperty<Real>("liquid_mass_fraction_rate"))
{
}

void
INSMeltPoolMaterial::computeQpProperties()
{
  INSADStabilized3Eqn::computeQpProperties();

  _melt_pool_momentum_source[_qp] = 0.0;

  ADRealVectorValue darcy_term = ADRealVectorValue(0.0);
  ADRealVectorValue surface_tension_term = ADRealVectorValue(0.0);
  ADRealVectorValue thermalcapillary_term = ADRealVectorValue(0.0);
  ADRealVectorValue evaporation_term = ADRealVectorValue(0.0);
  RankTwoTensor iden(RankTwoTensor::initIdentity);
  ADRankTwoTensor proj;
  ADRealVectorValue normal = ADRealVectorValue(0.0);

  darcy_term = -_permeability[_qp] * _velocity[_qp];
  //   darcy_term = 0.0;
  evaporation_term =
      1.0 / (_rho[_qp] * _rho[_qp]) *
      (2 * _melt_pool_mass_rate[_qp] * _rho[_qp] * _dmelt_pool_mass_rate_dT[_qp] * _grad_temp[_qp] -
       _drho_dc[_qp] * _grad_cv[_qp] * _melt_pool_mass_rate[_qp] * _melt_pool_mass_rate[_qp]);

  // if (MetaPhysicL::raw_value(_f_l[_qp]) > libMesh::TOLERANCE &&
  //     MetaPhysicL::raw_value(_delta_function[_qp]) > libMesh::TOLERANCE)
  // {
  normal = _grad_cv[_qp] / (_grad_cv[_qp] + RealVectorValue(libMesh::TOLERANCE)).norm();

  proj.vectorOuterProduct(normal, normal);
  proj = iden - proj;
  surface_tension_term = _sigma * _curvature[_qp] *
                         (_grad_cv[_qp] + RealVectorValue(libMesh::TOLERANCE)) *
                         (2.0 * _rho[_qp] / (_rho_l + _rho_g));

  thermalcapillary_term = proj * _grad_temp[_qp] * _sigmaT * _delta_function[_qp] *
                          (2.0 * _rho[_qp] / (_rho_l + _rho_g));
  //   thermalcapillary_term = proj * _grad_temp[_qp] * _sigmaT * _delta_function[_qp];

  _melt_pool_momentum_source[_qp] +=
      thermalcapillary_term + surface_tension_term + evaporation_term + darcy_term;

  // Recoil Pressure
  _melt_pool_momentum_source[_qp] += 0.54 * _saturated_vapor_pressure[_qp] *
                                     (_grad_cv[_qp] + RealVectorValue(libMesh::TOLERANCE)) *
                                     (2.0 * _rho[_qp] / (_rho_l + _rho_g));
  // }

  _momentum_strong_residual[_qp] -= _melt_pool_momentum_source[_qp];

  // CORRECT
  _mass_strong_residual[_qp] += -_melt_pool_mass_rate[_qp] * normal *
                                (-_drho_dc[_qp] * _grad_cv[_qp] / _rho[_qp] / _rho[_qp]);

  //   Real r = (_q_point[_qp] - Point(0.0005, 0.00075, 0)).norm();

  //   Real laser_source = 1e-15 / (libMesh::pi * Utility::pow<2>(0.000025)) *
  //                       std::exp(-2.0 * Utility::pow<2>(r / 0.000025));

  //   _mass_strong_residual[_qp] +=
  //       -laser_source * normal * (-_drho_dc[_qp] * _grad_cv[_qp] / _rho[_qp] / _rho[_qp]);

  //   _mass_strong_residual[_qp] +=
  //       _melt_pool_mass_rate[_qp] * _delta_function[_qp] * (_rho_l - _rho_g) / _rho[_qp] /
  //       _rho[_qp];

  _f_l_rate[_qp] = (_f_l[_qp] - _f_l_old[_qp]) / _dt;

  // _temperature_advective_strong_residual[_qp] +=
  //     _rho[_qp] * _Lm * _velocity[_qp] * _grad_fl[_qp] +
  //     _rho[_qp] * _Lm * ((_f_l[_qp] - _f_l_old[_qp]) / _dt);
}
