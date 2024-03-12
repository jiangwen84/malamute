/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "nonADMeltPoolHeatSource.h"
#include "Function.h"

registerMooseObject("MalamuteApp", nonADMeltPoolHeatSource);

InputParameters
nonADMeltPoolHeatSource::validParams()
{
  InputParameters params = KernelValue::validParams();
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
  params.addRequiredParam<Real>("rho_l", "Liquid density.");
  params.addRequiredParam<Real>("rho_g", "Gas density.");
  params.addRequiredCoupledVar("level_set", "Level set variable.");
  return params;
}

nonADMeltPoolHeatSource::nonADMeltPoolHeatSource(const InputParameters & parameters)
  : KernelValue(parameters),
    _delta_function(getMaterialProperty<Real>("delta_function")),
    _delta_function_dc(getMaterialProperty<RealVectorValue>("delta_function_dc")),
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
    _rho(getMaterialProperty<Real>("rho")),
    _rho_dc(getMaterialProperty<Real>("rho_dc")),
    _rho_dT(getMaterialProperty<Real>("rho_dT")),
    _melt_pool_mass_rate(getMaterialProperty<Real>("melt_pool_mass_rate")),
    _melt_pool_mass_rate_dT(getMaterialProperty<Real>("melt_pool_mass_rate_dT")),
    _cp(getMaterialProperty<Real>("cp")),
    _cp_dc(getMaterialProperty<Real>("cp_dc")),
    _cp_dT(getMaterialProperty<Real>("cp_dT")),
    _Lv(getParam<Real>("vaporization_latent_heat")),
    _rho_l(getParam<Real>("rho_l")),
    _rho_g(getParam<Real>("rho_g")),
    _c_id(coupled("level_set"))
{
}

Real
nonADMeltPoolHeatSource::precomputeQpResidual()
{
  Point p(0, 0, 0);
  RealVectorValue laser_location(_laser_location_x.value(_t, p),
                                 _laser_location_y.value(_t, p),
                                 _laser_location_z.value(_t, p));

  Real r = (_q_point[_qp] - laser_location).norm();

  Real laser_source = 2 * _power.value(_t, p) * _alpha / (libMesh::pi * Utility::pow<2>(_Rb)) *
                      std::exp(-2.0 * Utility::pow<2>(r / _Rb));

  Real convection = -_Ah * (_u[_qp] - _T0);
  Real radiation =
      -_stefan_boltzmann * _varepsilon * (Utility::pow<4>(_u[_qp]) - Utility::pow<4>(_T0));

  Real heat_source = (convection + radiation + laser_source) * _delta_function[_qp];

  // Phase change
  heat_source += _melt_pool_mass_rate[_qp] * _delta_function[_qp] * _rho[_qp] *
                     (1.0 / _rho_g - 1.0 / _rho_l) * _cp[_qp] * _u[_qp] -
                 _Lv * _melt_pool_mass_rate[_qp] * _delta_function[_qp];

  return -heat_source;
}

Real
nonADMeltPoolHeatSource::precomputeQpJacobian()
{
  Real convection_dT = -_Ah * (_phi[_j][_qp]);
  Real radiation_dT =
      -_stefan_boltzmann * _varepsilon * (4.0 * Utility::pow<3>(_u[_qp]) * _phi[_j][_qp]);

  Real heat_source_dT = (convection_dT + radiation_dT) * _delta_function[_qp];

  // Phase change
  heat_source_dT += _melt_pool_mass_rate_dT[_qp] * _delta_function[_qp] * _rho[_qp] *
                        (1.0 / _rho_g - 1.0 / _rho_l) * _cp[_qp] * _u[_qp] * _phi[_j][_qp] +
                    _melt_pool_mass_rate[_qp] * _delta_function[_qp] * _rho_dT[_qp] *
                        (1.0 / _rho_g - 1.0 / _rho_l) * _cp[_qp] * _u[_qp] * _phi[_j][_qp] +
                    _melt_pool_mass_rate[_qp] * _delta_function[_qp] * _rho_dT[_qp] *
                        (1.0 / _rho_g - 1.0 / _rho_l) * _cp_dT[_qp] * _u[_qp] * _phi[_j][_qp] +
                    _melt_pool_mass_rate[_qp] * _delta_function[_qp] * _rho_dT[_qp] *
                        (1.0 / _rho_g - 1.0 / _rho_l) * _cp[_qp] * _phi[_j][_qp] -
                    _Lv * _melt_pool_mass_rate_dT[_qp] * _delta_function[_qp];

  return -heat_source_dT;
}

Real
nonADMeltPoolHeatSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _c_id)
  {
    Point p(0, 0, 0);
    RealVectorValue laser_location(_laser_location_x.value(_t, p),
                                   _laser_location_y.value(_t, p),
                                   _laser_location_z.value(_t, p));

    Real r = (_q_point[_qp] - laser_location).norm();

    Real laser_source = 2 * _power.value(_t, p) * _alpha / (libMesh::pi * Utility::pow<2>(_Rb)) *
                        std::exp(-2.0 * Utility::pow<2>(r / _Rb));

    Real convection = -_Ah * (_u[_qp] - _T0);
    Real radiation =
        -_stefan_boltzmann * _varepsilon * (Utility::pow<4>(_u[_qp]) - Utility::pow<4>(_T0));

    Real heat_source_dc =
        (convection + radiation + laser_source) * _delta_function_dc[_qp] * _grad_phi[_j][_qp];

    // Phase change
    heat_source_dc +=
        _melt_pool_mass_rate[_qp] * _delta_function_dc[_qp] * _rho[_qp] *
            (1.0 / _rho_g - 1.0 / _rho_l) * _cp[_qp] * _u[_qp] * _grad_phi[_j][_qp] +
        _melt_pool_mass_rate[_qp] * _delta_function[_qp] * _rho_dc[_qp] *
            (1.0 / _rho_g - 1.0 / _rho_l) * _cp_dc[_qp] * _u[_qp] * _phi[_j][_qp] +
        _melt_pool_mass_rate[_qp] * _delta_function[_qp] * _rho[_qp] *
            (1.0 / _rho_g - 1.0 / _rho_l) * _cp[_qp] * _u[_qp] * _phi[_j][_qp] -
        _Lv * _melt_pool_mass_rate[_qp] * _delta_function_dc[_qp] * _grad_phi[_j][_qp];

    return -heat_source_dc;
  }

  return 0.0;
}
