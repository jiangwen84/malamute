/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2024, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "SolidificationRate.h"

registerMooseObject("MalamuteApp", SolidificationRate);

InputParameters
SolidificationRate::validParams()
{
  InputParameters params = Material::validParams();
  params.addRequiredCoupledVar("temperature", "Temperature Variable");
  params.addRequiredParam<Real>("solidus_temperature", "Solidus temperature.");
  params.addRequiredParam<Real>("liquidus_temperature", "Liquidus temperature.");
  return params;
}

SolidificationRate::SolidificationRate(const InputParameters & parameters)
  : Material(parameters),
    _solidus_temperature(getParam<Real>("solidus_temperature")),
    _liquidus_temperature(getParam<Real>("liquidus_temperature")),
    _thermal_conductivity(getADMaterialProperty<Real>("thermal_conductivity")),
    _temp_grad(adCoupledGradient("temperature")),
    _temp(adCoupledValue("temperature")),
    _temp_old(coupledValueOld("temperature")),
    _temp_gradient(declareProperty<Real>("temperature_gradient")),
    _cooling_rate(declareProperty<Real>("cooling_rate")),
    _solidification_rate(declareProperty<Real>("solidification_rate")),
    _liquidus_time(declareProperty<Real>("liquidus_time")),
    _liquidus_time_old(getMaterialPropertyOld<Real>("liquidus_time")),
    _solidus_time(declareProperty<Real>("solidus_time")),
    _solidus_time_old(getMaterialPropertyOld<Real>("solidus_time"))
{
}

void
SolidificationRate::initQpStatefulProperties()
{
  _solidus_time[_qp] = 0.0;
  _liquidus_time[_qp] = 0.0;
}

void
SolidificationRate::computeQpProperties()
{
  if (MetaPhysicL::raw_value(_temp[_qp]) > _liquidus_temperature &&
      _liquidus_time_old[_qp] < 1.0e-8)
    _liquidus_time[_qp] = _t;
  else
    _liquidus_time[_qp] = _liquidus_time_old[_qp];

  if ((_liquidus_time_old[_qp] > 0.0 &&
       MetaPhysicL::raw_value(_temp[_qp]) < _solidus_temperature) &&
      _solidus_time_old[_qp] < 1.0e-8)
    _solidus_time[_qp] = _t;
  else
    _solidus_time[_qp] = _solidus_time_old[_qp];

  if (_liquidus_time[_qp] > 1.0e-8 && _solidus_time[_qp] > 1.0e-8)
    _cooling_rate[_qp] = std::abs((_liquidus_temperature - _solidus_temperature) /
                                  (_liquidus_time[_qp] - _solidus_time[_qp]));
  else
    _cooling_rate[_qp] = 0.0;

  _temp_gradient[_qp] = MetaPhysicL::raw_value(_temp_grad[_qp].norm() / _thermal_conductivity[_qp]);

  _solidification_rate[_qp] = 1.0 / _temp_gradient[_qp] * _cooling_rate[_qp];
}
