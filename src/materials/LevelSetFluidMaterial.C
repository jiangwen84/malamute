/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "LevelSetFluidMaterial.h"

registerADMooseObject("MalamuteApp", LevelSetFluidMaterial);

InputParameters
LevelSetFluidMaterial::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addClassDescription("Computes fluid properties in melt pool heat equations.");
  params.addRequiredParam<Real>("rho_g", "Gas density.");
  params.addRequiredParam<Real>("rho_s", "Solid density.");
  params.addRequiredParam<Real>("rho_l", "Liquid density.");
  params.addRequiredParam<Real>("mu_g", "Gas viscosity.");
  params.addRequiredParam<Real>("mu_l", "Liquid viscosity.");
  params.addRequiredParam<Real>("mu_s", "solid viscosity.");
  params.addRequiredParam<Real>("permeability_constant", "Permeability constant");
  return params;
}

LevelSetFluidMaterial::LevelSetFluidMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _heaviside_function(getADMaterialProperty<Real>("heaviside_function")),
    _rho(declareADProperty<Real>("rho")),
    _drho_dc(declareADProperty<Real>("drho_dc")),
    _mu(declareADProperty<Real>("mu")),
    _rho_g(getParam<Real>("rho_g")),
    _rho_l(getParam<Real>("rho_l")),
    _rho_s(getParam<Real>("rho_s")),
    _mu_g(getParam<Real>("mu_g")),
    _mu_l(getParam<Real>("mu_l")),
    _mu_s(getParam<Real>("mu_s")),
    _f_l(getADMaterialProperty<Real>("liquid_mass_fraction")),
    _f_s(getADMaterialProperty<Real>("solid_mass_fraction")),
    _g_l(getADMaterialProperty<Real>("liquid_volume_fraction")),
    _g_s(getADMaterialProperty<Real>("solid_volume_fraction")),
    _permeability(declareADProperty<Real>("permeability")),
    _K0(getParam<Real>("permeability_constant"))
{
}

void
LevelSetFluidMaterial::computeQpProperties()
{
  auto heaviside = _heaviside_function[_qp];
  if (heaviside > 1.0)
    heaviside = 1.0;
  else if (heaviside < 0.0)
    heaviside = 0.0;

  ADReal rho_m = _g_s[_qp] * _rho_s + _g_l[_qp] * _rho_l;

  ADReal mu_m = _mu_l * rho_m / _rho_l;
  _permeability[_qp] =
      mu_m / _K0 * Utility::pow<2>(1 - _f_l[_qp]) / (Utility::pow<3>(_f_l[_qp]) + 1.0e-6);

  _rho[_qp] =
      (heaviside) * ((1 - _f_l[_qp]) * _rho_s + _f_l[_qp] * _rho_l) + (1 - heaviside) * _rho_g;

  _mu[_qp] = (heaviside) * ((1 - _f_l[_qp]) * _mu_s + _f_l[_qp] * _mu_l) + (1 - heaviside) * _mu_g;

  _drho_dc[_qp] = (1.0) * ((1 - _f_l[_qp]) * _rho_s + _f_l[_qp] * _rho_l) - 1.0 * _rho_g;
}
