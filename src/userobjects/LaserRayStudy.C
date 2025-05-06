//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LaserRayStudy.h"

// Local includes
#include "RayTracingAngularQuadrature.h"

// libMesh includes
#include "libmesh/parallel_algebra.h"

registerMooseObject("MalamuteApp", LaserRayStudy);

InputParameters
LaserRayStudy::validParams()
{
  auto params = RepeatableRayStudyBase::validParams();
  params.addParam<Real>("laser_power", 0.0, "Laser power.");
  params.addParam<Real>("beam_radius", 1e-3, "Laser beam radius.");
  params.addParam<Real>("cutoff_radius", 1e-3, "Cut off radius.");
  params.addParam<unsigned int>("nx", 1, "Number of rays in the X direction");
  params.addParam<unsigned int>("ny", 0, "Number of rays in the Y direction");
  params.addParam<Real>("xmin", 0.0, "Lower X Coordinate of the generated rays");
  params.addParam<Real>("ymin", 0.0, "Lower Y Coordinate of the generated rays");
  params.addParam<Real>("xmax", 1.0, "Upper X Coordinate of the generated rays");
  params.addParam<Real>("ymax", 1.0, "Upper Y Coordinate of the generated rays");
  params.addParam<Real>("starting_height", 1.0, "Starting Y(Z) coord of rays.");
  params.addParam<Real>("max_distance", 0.002, "Max distacne for ray.");
  return params;
}

LaserRayStudy::LaserRayStudy(const InputParameters & parameters)
  : RepeatableRayStudyBase(parameters),
    _energy_density_index(registerRayData("energy_density")),
    _num_reflection_index(registerRayData("num_reflection")),
    _num_deposition_index(registerRayData("num_deposition")),
    _power(getParam<Real>("laser_power")),
    _Rb(getParam<Real>("beam_radius")),
    _cut_off_r(getParam<Real>("cutoff_radius")),
    _nx(getParam<unsigned int>("nx")),
    _ny(getParam<unsigned int>("ny")),
    _xmin(getParam<Real>("xmin")),
    _xmax(getParam<Real>("xmax")),
    _ymin(getParam<Real>("ymin")),
    _ymax(getParam<Real>("ymax")),
    _starting_height(getParam<Real>("starting_height")),
    _max_distance(getParam<Real>("max_distance"))
{
}

void
LaserRayStudy::defineRays()
{

  if (_ny < 1)
  {
    // unsigned int nx = 300;

    // Real r0 = 0.15e-3;
    // Real xmin = 0.0004;
    // Real xmax = 0.0006;

    // Real Q = 100;
    Real size = (_xmax - _xmin) / _nx;
    unsigned int num_rays = 0;

    Real xc = (_xmax + _xmin) / 2.0;

    std::vector<std::string> names;
    std::vector<Real> x_coords;
    std::vector<Real> energy_rays;
    std::vector<Point> directions;
    std::vector<Point> start_points;

    Real Q_bar = _power * size;

    num_rays = 0;

    for (unsigned int i = 0; i < _nx; ++i)
    {
      Real xcell = (i + 0.5) * size + _xmin;
      if (((xcell - xc) * (xcell - xc)) < _Rb * _Rb)
      {
        num_rays++;
        names.push_back("ray_" + std::to_string(num_rays));
        x_coords.push_back(xcell);
        start_points.push_back(Point(xcell, _starting_height, 0));
        directions.push_back(Point(0, -1, 0));
        Real energy = 2.0 * Q_bar / (_Rb * _Rb * libMesh::pi) *
                      std::exp(-2.0 / _Rb / _Rb * ((xcell - xc) * (xcell - xc)));
        energy_rays.push_back(energy);
      }
    }
    
    std::cout << "number of rays = " << num_rays << std::endl;

    for (std::size_t i = 0; i < names.size(); ++i)
    {
      std::shared_ptr<Ray> ray = acquireRegisteredRay(names[i]);

      // Point shift_x((_t - 0.02) * 0.005, 0, 0);

      // if (_t < 0.02)
      //   shift_x(0) = 0.0;

      // Point shift_x(_t * 0.01, 0, 0);

      ray->setStart(start_points[i]);

      ray->setStartingDirection((directions)[i]);

      ray->data(_energy_density_index) = energy_rays[i];

      ray->setStartingMaxDistance(_max_distance);

      _rays.emplace_back(std::move(ray));
    }
  }

  else
  {

    // unsigned int nx = 100;
    // unsigned int ny = 100;
    // Real r0 = 0.140e-3;
    // Real cut_off_r = 0.140e-3;
    // Real xmin = 0.0007;
    // Real ymin = 0.0007;
    // Real xmax = 0.0007;
    // Real ymax = 0.0008;

    // Real r0 = 0.25e-3;
    // Real cut_off_r = 0.1e-3;
    // Real xmin = -0.00015;
    // Real ymin = -0.00015;
    // Real xmax = 0.00015;
    // Real ymax = 0.00015;
    // Real Q = 200;
    //_starting_height = 0.002

    Real size = (_xmax - _xmin) / _nx;
    unsigned int num_rays = 0;

    Real xc = (_xmax + _xmin) / 2.0;
    Real yc = (_ymax + _ymin) / 2.0;

    std::vector<std::string> names;
    std::vector<Real> x_coords;
    std::vector<Real> y_coords;
    std::vector<Real> energy_rays;
    std::vector<Point> directions;
    std::vector<Point> start_points;

    Real Q_bar = _power * size * size;

    num_rays = 0;

    for (unsigned int i = 0; i < _nx; ++i)
    {
      for (unsigned int j = 0; j < _ny; ++j)
      {
        Real xcell = (i + 0.5) * size + _xmin;
        Real ycell = (j + 0.5) * size + _ymin;
        if (((xcell - xc) * (xcell - xc) + (ycell - yc) * (ycell - yc)) < _cut_off_r * _cut_off_r)
        {
          num_rays++;
          names.push_back("ray_" + std::to_string(num_rays));
          x_coords.push_back(xcell);
          y_coords.push_back(ycell);
          start_points.push_back(Point(xcell, ycell, _starting_height));
          directions.push_back(Point(0, 0, -1));
          Real energy = 2.0 * Q_bar / (_Rb * _Rb * libMesh::pi) *
                        std::exp(-2.0 / _Rb / _Rb *
                                 ((xcell - xc) * (xcell - xc) + (ycell - yc) * (ycell - yc)));
          energy_rays.push_back(energy);
        }
      }
    }

    std::cout << "Total Number of Rays = " << num_rays << std::endl;

    for (std::size_t i = 0; i < names.size(); ++i)
    {
      std::shared_ptr<Ray> ray = acquireRegisteredRay(names[i]);

      // Point shift_x((_t - 0.28) * 0.009, 0, 0);

      // if (_t < 0.28)
      //   shift_x(0) = 0.0;

      // Point shift_x(-_t * 300e-4, 0, 0);

      // ray->setStart(start_points[i] + shift_x);
      ray->setStart(start_points[i]);

      ray->setStartingDirection((directions)[i]);

      ray->data(_energy_density_index) = energy_rays[i];

      ray->setStartingMaxDistance(_max_distance);

      _rays.emplace_back(std::move(ray));
    }
  }
}
