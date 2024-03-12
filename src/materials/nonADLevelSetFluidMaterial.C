/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "nonADLevelSetFluidMaterial.h"

registerADMooseObject("MalamuteApp", nonADLevelSetFluidMaterial);

InputParameters
nonADLevelSetFluidMaterial::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Computes fluid properties in melt pool heat equations.");
  params.addRequiredParam<Real>("rho_g", "Gas density.");
  params.addRequiredParam<Real>("rho_s", "Solid density.");
  params.addRequiredParam<Real>("rho_l", "Liquid density.");
  params.addRequiredParam<Real>("mu_g", "Gas viscosity.");
  params.addRequiredParam<Real>("mu_l", "Liquid viscosity.");
  params.addRequiredParam<Real>("permeability_constant", "Permeability constant");
  return params;
}

nonADLevelSetFluidMaterial::nonADLevelSetFluidMaterial(const InputParameters & parameters)
  : Material(parameters),
    _heaviside_function(getMaterialProperty<Real>("heaviside_function")),
    _heaviside_function_dc(getMaterialProperty<Real>("heaviside_function_dc")),
    _rho(declareProperty<Real>("rho")),
    _rho_dc(declareProperty<Real>("rho_dc")),
    _rho_dT(declareProperty<Real>("rho_dT")),
    _mu(declareProperty<Real>("mu")),
    _mu_dc(declareProperty<Real>("mu_dc")),
    _mu_dT(declareProperty<Real>("mu_dT")),
    _rho_g(getParam<Real>("rho_g")),
    _rho_l(getParam<Real>("rho_l")),
    _rho_s(getParam<Real>("rho_s")),
    _mu_g(getParam<Real>("mu_g")),
    _mu_l(getParam<Real>("mu_l")),
    _f_l(getMaterialProperty<Real>("liquid_mass_fraction")),
    _f_l_dT(getMaterialProperty<Real>("liquid_mass_fraction_dT")),
    _f_s(getMaterialProperty<Real>("solid_mass_fraction")),
    _f_s_dT(getMaterialProperty<Real>("solid_mass_fraction_dT")),
    _g_l(getMaterialProperty<Real>("liquid_volume_fraction")),
    _g_l_dT(getMaterialProperty<Real>("liquid_volume_fraction_dT")),
    _g_s(getMaterialProperty<Real>("solid_volume_fraction")),
    _g_s_dT(getMaterialProperty<Real>("solid_volume_fraction_dT")),
    _permeability(declareProperty<Real>("permeability")),
    _permeability_dT(declareProperty<Real>("permeability_dT")),
    _K0(getParam<Real>("permeability_constant"))
{
}

void
nonADLevelSetFluidMaterial::computeQpProperties()
{
  Real rho_m = _g_s[_qp] * _rho_s + _g_l[_qp] * _rho_l;
  Real rho_m_dT = _g_s_dT[_qp] * _rho_s + _g_l_dT[_qp] * _rho_l;

  _rho[_qp] = (1 - _heaviside_function[_qp]) * rho_m + _heaviside_function[_qp] * _rho_g;
  _rho_dc[_qp] = -_heaviside_function_dc[_qp] * rho_m + _heaviside_function_dc[_qp] * _rho_g;
  _rho_dT[_qp] = (1 - _heaviside_function[_qp]) * rho_m_dT;

  Real mu_m = _mu_l * rho_m / _rho_l;
  Real mu_m_dT = _mu_l * rho_m_dT / _rho_l;
  _mu[_qp] = (1 - _heaviside_function[_qp]) * mu_m + _heaviside_function[_qp] * _mu_g;
  _mu_dc[_qp] = -_heaviside_function_dc[_qp] * mu_m + _heaviside_function_dc[_qp] * _mu_g;
  _mu_dT[_qp] = (1 - _heaviside_function[_qp]) * mu_m_dT;

  Real f_l = _f_l[_qp] * (1 - _heaviside_function[_qp]);
  // Real f_l_dc = -_f_l[_qp] * _heaviside_function_dc[_qp];
  Real f_l_dT = _f_l_dT[_qp] * (1 - _heaviside_function[_qp]);

  _permeability[_qp] = mu_m / _K0 * Utility::pow<2>(1 - f_l) / (Utility::pow<3>(f_l) + 1.0e-3);
  _permeability_dT[_qp] =
      mu_m_dT / _K0 * Utility::pow<2>(1 - f_l) / (Utility::pow<3>(f_l) + 1.0e-3) +
      mu_m / _K0 *
          (Utility::pow<2>(1 - f_l) * 3.0 * f_l * f_l +
           2.0 * (1 - f_l) * (Utility::pow<3>(f_l) + 1.0e-3)) /
          Utility::pow<2>((Utility::pow<3>(f_l) + 1.0e-3)) * f_l_dT;
}
