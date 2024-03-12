/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "nonADLevelSetThermalMaterial.h"

registerMooseObject("MalamuteApp", nonADLevelSetThermalMaterial);

InputParameters
nonADLevelSetThermalMaterial::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Computes thermal properties in melt pool heat equations");
  params.addRequiredCoupledVar("temperature", "Temperature vaπriable");
  params.addRequiredParam<Real>("c_g", "Gas specific heat.");
  params.addRequiredParam<Real>("c_s", "Solid specific heat.");
  params.addRequiredParam<Real>("c_l", "Liquid specific heat.");
  params.addRequiredParam<Real>("k_g", "Gas heat conductivity.");
  params.addRequiredParam<Real>("k_s", "Solid conductivity.");
  params.addRequiredParam<Real>("k_l", "Liquid conductivity.");
  params.addRequiredParam<Real>("solidus_temperature", "Solidus temperature.");
  params.addRequiredParam<Real>("latent_heat", "Latent heat.");
  return params;
}

nonADLevelSetThermalMaterial::nonADLevelSetThermalMaterial(const InputParameters & parameters)
  : Material(parameters),
    _temp(coupledValue("temperature")),
    _heaviside_function(getMaterialProperty<Real>("heaviside_function")),
    _heaviside_function_dc(getMaterialProperty<Real>("heaviside_function_dc")),
    _h(declareProperty<Real>("enthalpy")),
    _h_dc(declareProperty<Real>("enthalpy_dc")),
    _h_dT(declareProperty<Real>("enthalpy_dT")),
    _k(declareProperty<Real>("k")),
    _k_dc(declareProperty<Real>("k_dc")),
    _k_dT(declareProperty<Real>("k_dT")),
    _cp(declareProperty<Real>("cp")),
    _cp_dc(declareProperty<Real>("cp_dc")),
    _cp_dT(declareProperty<Real>("cp_dT")),
    _c_g(getParam<Real>("c_g")),
    _c_s(getParam<Real>("c_s")),
    _c_l(getParam<Real>("c_l")),
    _k_g(getParam<Real>("k_g")),
    _k_s(getParam<Real>("k_s")),
    _k_l(getParam<Real>("k_l")),
    _latent_heat(getParam<Real>("latent_heat")),
    _solidus_temperature(getParam<Real>("solidus_temperature")),
    _f_l(getMaterialProperty<Real>("liquid_mass_fraction")),
    _f_l_dT(getMaterialProperty<Real>("liquid_mass_fraction_dT")),
    _f_s(getMaterialProperty<Real>("solid_mass_fraction")),
    _f_s_dT(getMaterialProperty<Real>("solid_mass_fraction_dT")),
    _g_l(getMaterialProperty<Real>("liquid_volume_fraction")),
    _g_l_dT(getMaterialProperty<Real>("liquid_volume_fraction_dT")),
    _g_s(getMaterialProperty<Real>("solid_volume_fraction")),
    _g_s_dT(getMaterialProperty<Real>("solid_volume_fraction_dT"))
{
}

void
nonADLevelSetThermalMaterial::computeQpProperties()
{
  Real delta_l = (_c_s - _c_l) * _solidus_temperature + _latent_heat;

  Real f_l = _f_l[_qp] * (1 - _heaviside_function[_qp]);
  Real f_l_dc = -_f_l[_qp] * _heaviside_function_dc[_qp];
  Real f_l_dT = _f_l_dT[_qp] * (1 - _heaviside_function[_qp]);

  Real f_s = _f_s[_qp] * (1 - _heaviside_function[_qp]);
  Real f_s_dc = -_f_s[_qp] * _heaviside_function_dc[_qp];
  Real f_s_dT = _f_s_dT[_qp] * (1 - _heaviside_function[_qp]);

  Real c_m = (f_s * _c_s + f_l * _c_l) * (1 - _heaviside_function[_qp]);
  Real c_m_dc = -(f_s * _c_s + f_l * _c_l) * _heaviside_function_dc[_qp] +
                (f_s_dc * _c_s + f_l_dc * _c_l) * (1 - _heaviside_function[_qp]);
  Real c_m_dT = (f_s_dT * _c_s + f_l_dT * _c_l) * (1 - _heaviside_function[_qp]);

  Real k_m = 1.0 / (_g_s[_qp] / _k_s + _g_l[_qp] / _k_l);
  Real k_m_dT = -1.0 / Utility::pow<2>(_g_s[_qp] / _k_s + _g_l[_qp] / _k_l) *
                (_g_s_dT[_qp] / _k_s + _g_l_dT[_qp] / _k_l);

  Real h_m = c_m * _temp[_qp] + f_l * (1 - _heaviside_function[_qp]) * delta_l;
  Real h_m_dc = c_m_dc * _temp[_qp] + f_l_dc * (1 - _heaviside_function[_qp]) * delta_l -
                f_l * _heaviside_function_dc[_qp] * delta_l;
  Real h_m_dT = c_m_dT * _temp[_qp] + c_m + f_l_dT * (1 - _heaviside_function[_qp]) * delta_l;

  Real h_g = _c_g * _temp[_qp];
  Real h_g_dT = _c_g;

  _h[_qp] = (1 - _heaviside_function[_qp]) * h_m + _heaviside_function[_qp] * h_g;
  _h_dc[_qp] = -_heaviside_function_dc[_qp] * h_m + (1 - _heaviside_function[_qp]) * h_m_dc;
  _h_dT[_qp] = (1 - _heaviside_function[_qp]) * h_m_dT + _heaviside_function[_qp] * h_g_dT;

  _k[_qp] = (1 - _heaviside_function[_qp]) * k_m + _heaviside_function[_qp] * _k_g;
  _k_dc[_qp] = -_heaviside_function_dc[_qp] * k_m + _heaviside_function_dc[_qp] * _k_g;
  _k_dT[_qp] = (1 - _heaviside_function[_qp]) * k_m_dT;

  _cp[_qp] = (1 - _heaviside_function[_qp]) * c_m + _heaviside_function[_qp] * _c_g;
  _cp_dc[_qp] = _heaviside_function_dc[_qp] * c_m + (1 - _heaviside_function[_qp]) * c_m_dc +
                _heaviside_function_dc[_qp] * _c_g;
  _cp_dT[_qp] = (1 - _heaviside_function[_qp]) * c_m_dT;
}
