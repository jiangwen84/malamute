//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "LevelSetOlssonSpheres.h"
#include <limits>

registerMooseObject("LevelSetApp", LevelSetOlssonSpheres);

InputParameters
LevelSetOlssonSpheres::validParams()
{
  InputParameters params = Function::validParams();
  params.addClassDescription("Implementation of 'bubble' ranging from 0 to 1.");
  params.addParam<std::vector<Real>>("x_coords", "X coordinates");
  params.addParam<std::vector<Real>>("y_coords", "Y coordinates");
  params.addParam<std::vector<Real>>("z_coords", "Z coordinates");
  params.addParam<std::vector<Real>>("radius", "Radius");
  params.addParam<RealVectorValue>("point", RealVectorValue(0, 0, 0), "A point on the plane.");
  params.addParam<RealVectorValue>(
      "normal", RealVectorValue(0, 1, 0), "The normal vector to the plane.");
  params.addParam<Real>("epsilon", 0.01, "The interface thickness.");
  return params;
}

LevelSetOlssonSpheres::LevelSetOlssonSpheres(const InputParameters & parameters)
  : Function(parameters),
    _x(getParam<std::vector<Real>>("x_coords")),
    _y(getParam<std::vector<Real>>("y_coords")),
    _z(getParam<std::vector<Real>>("z_coords")),
    _r(getParam<std::vector<Real>>("radius")),
    _point(getParam<RealVectorValue>("point")),
    _normal(getParam<RealVectorValue>("normal")),
    _epsilon(getParam<Real>("epsilon"))
{
}

Real
LevelSetOlssonSpheres::value(Real /*t*/, const Point & p) const
{
  Real dist = std::numeric_limits<Real>::max();
  for (unsigned int i = 0; i < _x.size(); ++i)
  {
    Point center(_x[i], _y[i], _z[i]);
    Real d = (p - center).norm() - _r[i];
    if (d < dist)
      dist = d;
  }

  const RealVectorValue unit_normal = _normal / _normal.norm();
  const Real distance_from_orgin = -unit_normal * _point;
  Real d = -(unit_normal * p + distance_from_orgin);

  if (d < dist)
    dist = d;

  const auto m = dist / _epsilon;
  return 1.0 / (1 + std::exp(m));
}

ADReal
LevelSetOlssonSpheres::value(const ADReal & /*t*/, const ADPoint & p) const
{
  ADReal dist = std::numeric_limits<Real>::max();
  for (unsigned int i = 0; i < _x.size(); ++i)
  {
    ADPoint center(_x[i], _y[i], _z[i]);
    ADReal d = (p - center).norm() - _r[i];
    if (d < dist)
      dist = d;
  }

  const RealVectorValue unit_normal = _normal / _normal.norm();
  const Real distance_from_orgin = -unit_normal * _point;
  ADReal d = -(unit_normal * p + distance_from_orgin);

  if (d < dist)
    dist = d;

  const auto m = dist / _epsilon;
  return 1.0 / (1 + std::exp(m));
}

RealGradient
LevelSetOlssonSpheres::gradient(Real /*t*/, const Point & p) const
{
  std::cout << "NOT IMPLEMENTED YET" << std::endl;
  RealGradient output(1.0, 0, 0);
  return output;
}
