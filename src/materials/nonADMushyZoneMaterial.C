/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "nonADMushyZoneMaterial.h"

registerMooseObject("MalamuteApp", nonADMushyZoneMaterial);

InputParameters
nonADMushyZoneMaterial::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Computes material properties in the mushy zone.");
  params.addRequiredCoupledVar("temperature", "Temperature variable");
  params.addRequiredParam<Real>("rho_s", "Solid density.");
  params.addRequiredParam<Real>("rho_l", "Liquid density.");
  params.addRequiredParam<Real>("solidus_temperature", "Solidus temperature.");
  params.addRequiredParam<Real>("liquidus_temperature", "Liquidus temperature.");
  return params;
}

nonADMushyZoneMaterial::nonADMushyZoneMaterial(const InputParameters & parameters)
  : Material(parameters),
    _temp(coupledValue("temperature")),
    _solidus_temperature(getParam<Real>("solidus_temperature")),
    _liquidus_temperature(getParam<Real>("liquidus_temperature")),
    _f_l(declareProperty<Real>("liquid_mass_fraction")),
    _f_l_dT(declareProperty<Real>("liquid_mass_fraction_dT")),
    _f_s(declareProperty<Real>("solid_mass_fraction")),
    _f_s_dT(declareProperty<Real>("solid_mass_fraction_dT")),
    _g_l(declareProperty<Real>("liquid_volume_fraction")),
    _g_l_dT(declareProperty<Real>("liquid_volume_fraction_dT")),
    _g_s(declareProperty<Real>("solid_volume_fraction")),
    _g_s_dT(declareProperty<Real>("solid_volume_fraction_dT")),
    _rho_s(getParam<Real>("rho_s")),
    _rho_l(getParam<Real>("rho_l"))
{
}

void
nonADMushyZoneMaterial::computeQpProperties()
{
  _f_l[_qp] = 1;
  _f_l_dT[_qp] = 0;

  if (_temp[_qp] < _solidus_temperature)
  {
    _f_l[_qp] = 0;
  }
  else if (_temp[_qp] >= _solidus_temperature && _temp[_qp] <= _liquidus_temperature)
  {
    _f_l[_qp] =
        (_temp[_qp] - _solidus_temperature) / (_liquidus_temperature - _solidus_temperature);
    _f_l_dT[_qp] = 1.0 / (_liquidus_temperature - _solidus_temperature);
  }

  _f_s[_qp] = 1.0 - _f_l[_qp];
  _f_s_dT[_qp] = -_f_l_dT[_qp];

  _g_l[_qp] = _f_l[_qp] / _rho_l / ((1 - _f_l[_qp]) / _rho_s + _f_l[_qp] / _rho_l);
  _g_l_dT[_qp] = _f_l_dT[_qp] / _rho_l / ((1 - _f_l[_qp]) / _rho_s + _f_l[_qp] / _rho_l) -
                 _f_l[_qp] / _rho_l /
                     Utility::pow<2>((1 - _f_l[_qp]) / _rho_s + _f_l[_qp] / _rho_l) *
                     (-_f_l_dT[_qp] / _rho_s + _f_l_dT[_qp] / _rho_l);

  _g_s[_qp] = 1.0 - _g_l[_qp];
  _g_s_dT[_qp] = -_g_l_dT[_qp];
}
