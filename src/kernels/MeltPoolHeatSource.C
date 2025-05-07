/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "MeltPoolHeatSource.h"
#include "Function.h"

registerMooseObject("MalamuteApp", MeltPoolHeatSource);

InputParameters
MeltPoolHeatSource::validParams()
{
  InputParameters params = ADKernelValue::validParams();
  params.addClassDescription(
      "Computes the laser heat source and heat loss in the melt pool heat equation");
  params.addParam<FunctionName>(
      "laser_location_x", 0, "The laser center function of x coordinate.");
  params.addParam<FunctionName>(
      "laser_location_y", 0, "The laser center function of y coordinate.");
  params.addParam<FunctionName>(
      "laser_location_z", 0, "The laser center function of z coordinate.");
  params.addRequiredParam<FunctionName>("laser_power", "Laser power.");
  params.addRequiredParam<Real>("effective_beam_radius", "Effective beam radius.");
  params.addRequiredParam<Real>("absorption_coefficient", "Absorption coefficient.");
  params.addRequiredParam<Real>("heat_transfer_coefficient", "Heat transfer coefficient.");
  params.addRequiredParam<Real>("StefanBoltzmann_constant", "Stefan Boltzmann constant.");
  params.addRequiredParam<Real>("material_emissivity", "Material emissivity.");
  params.addRequiredParam<Real>("ambient_temperature", "Ambient temperature.");
  params.addRequiredParam<Real>("vaporization_latent_heat", "Latent heat of vaporization.");
  params.addCoupledVar("laser_deposition", "Laser Deposition Aux Variable");
  return params;
}

MeltPoolHeatSource::MeltPoolHeatSource(const InputParameters & parameters)
  : ADKernelValue(parameters),
    _delta_function(getADMaterialProperty<Real>("delta_function")),
    _power(getFunction("laser_power")),
    _alpha(getParam<Real>("absorption_coefficient")),
    _Rb(getParam<Real>("effective_beam_radius")),
    _Ah(getParam<Real>("heat_transfer_coefficient")),
    _stefan_boltzmann(getParam<Real>("StefanBoltzmann_constant")),
    _varepsilon(getParam<Real>("material_emissivity")),
    _T0(getParam<Real>("ambient_temperature")),
    _laser_location_x(getFunction("laser_location_x")),
    _laser_location_y(getFunction("laser_location_y")),
    _laser_location_z(getFunction("laser_location_z")),
    _melt_pool_mass_rate(getADMaterialProperty<Real>("melt_pool_mass_rate")),
    _Lv(getParam<Real>("vaporization_latent_heat")),
    _use_ray_laser(isCoupled("laser_deposition")),
    _laser_deposition(_use_ray_laser ? coupledValue("laser_deposition") : _zero),
    _vpp_name(getParam<VectorPostprocessorName>("deposition_coord")),
    _x(getVectorPostprocessorValue("deposition_coord", "x")),
    _y(getVectorPostprocessorValue("deposition_coord", "y")),
    _z(getVectorPostprocessorValue("deposition_coord", "z")),
    _energy(getVectorPostprocessorValue("deposition_coord", "energy"))
{
}

ADReal
MeltPoolHeatSource::precomputeQpResidual()
{
  Point p(0, 0, 0);
  RealVectorValue laser_location(
      _laser_location_x.value(_t, p), _laser_location_y.value(_t, p), _q_point[_qp](2));

  ADReal r = (_ad_q_point[_qp] - laser_location).norm();

  ADReal laser_source = 0.0;

  if (_use_ray_laser)
    laser_source = _laser_deposition[_qp] / _current_elem->volume();
  else
    for (int i = 0; i < _x.size(); i++){
      RealVectorValue deposition_coord(_x[i], _y[i], _q_point[_qp](2));
      Real r = (deposition_coord - _q_point[_qp]).norm();
      Real Q = _energy[i];
      auto Pij = 2 * _power.value(_t, p) * _alpha / (libMesh::pi * Utility::pow<2>(_Rb)) *
                std::exp(-2.0 * Utility::pow<2>(r / _Rb));
      laser_source += Q * Pij;
    }
    /**
     * laser_source = 2 * _power.value(_t, p) * _alpha / (libMesh::pi * Utility::pow<2>(_Rb)) *
                   std::exp(-2.0 * Utility::pow<2>(r / _Rb));
     */
  ADReal convection = _Ah * (_u[_qp] - _T0);
  ADReal radiation =
      -_stefan_boltzmann * _varepsilon * (Utility::pow<4>(_u[_qp]) - Utility::pow<4>(_T0));

  ADReal evap = -_Lv * _melt_pool_mass_rate[_qp];

  ADReal heat_source = (radiation + evap) * _delta_function[_qp];

  heat_source += laser_source;

  // Evaporation
  heat_source += -_Lv * _melt_pool_mass_rate[_qp] * _delta_function[_qp];

  return -heat_source;
}
