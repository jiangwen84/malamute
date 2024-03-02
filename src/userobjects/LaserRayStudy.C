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

  params.addParam<bool>(
      "vertex_to_vertex",
      true,
      "Enable generation of rays from vertices on boundary sides in the direction of "
      "the vertices on the other side of the same elem");
  params.addParam<bool>("centroid_to_vertex",
                        true,
                        "Enable generation of rays from centroids on boundary sides in the "
                        "direction of all other nodes in the same elem");
  params.addParam<bool>("centroid_to_centroid",
                        true,
                        "Enable generation of rays from centroids on boundary sides to centroids "
                        "of all boundary elements");
  params.addParam<bool>("edge_to_edge",
                        false,
                        "Enable generation of rays from centroids of boundary edges in the "
                        "direction of all other edge centroids in the same elem");
  params.addParam<bool>("side_aq",
                        false,
                        "Enable generation of rays from boundary side centroids in the direction "
                        "of angular quadrature");
  params.addParam<bool>("centroid_aq",
                        false,
                        "Enable generation of rays from boundary element centroids in the "
                        "direction of angular quadrature.");

  params.addRangeCheckedParam<unsigned int>(
      "polar_quad_order",
      5,
      "polar_quad_order % 2",
      "Order of the polar quadrature angular quadrature generation. Polar angle is between ray and "
      "the normal direction. Must be odd.");
  params.addRangeCheckedParam<unsigned int>(
      "azimuthal_quad_order",
      2,
      "azimuthal_quad_order > 0",
      "Order of the azimuthal quadrature per quadrant for angular quadrature generation. The "
      "azimuthal angle is measured in a plane perpendicular to the normal. Not needed in 2D.");

  params.addParam<bool>(
      "compute_expected_distance",
      false,
      "Whether or not to compute the expected distance for all of the Rays generated");

  params.addParam<bool>(
      "use_unsized_rays", false, "Whether or not to size Ray data on defineRays()");

  params.addParam<bool>("set_incoming_side", true, "Whether or not to set the incoming side");

  // When we define rays, they are already localized on the processors that start them
  // and the starting elem/incoming side is set
  params.set<bool>("_claim_after_define_rays") = false;
  // Can't have replicated Rays if the Rays are on their starting processors only
  params.set<bool>("_define_rays_replicated") = false;
  // Don't need to use registration here. For the purposes of testing and this study,
  // we'll really only ever access the Rays by ID (if at all)
  params.set<bool>("_use_ray_registration") = false;

  return params;
}

LaserRayStudy::LaserRayStudy(const InputParameters & parameters)
  : RepeatableRayStudyBase(parameters),
    _vertex_to_vertex(getParam<bool>("vertex_to_vertex")),
    _centroid_to_vertex(getParam<bool>("centroid_to_vertex")),
    _centroid_to_centroid(getParam<bool>("centroid_to_centroid")),
    _edge_to_edge(getParam<bool>("edge_to_edge")),
    _side_aq(getParam<bool>("side_aq")),
    _centroid_aq(getParam<bool>("centroid_aq")),
    _compute_expected_distance(getParam<bool>("compute_expected_distance")),
    _polar_quad_order(getParam<unsigned int>("polar_quad_order")),
    _azimuthal_quad_order(getParam<unsigned int>("azimuthal_quad_order")),
    _use_unsized_rays(getParam<bool>("use_unsized_rays")),
    _set_incoming_side(getParam<bool>("set_incoming_side")),
    _energy_density_index(registerRayData("energy_density")),
    _num_reflection_index(registerRayData("num_reflection")),
    _num_deposition_index(registerRayData("num_deposition")),
    _expected_distance(declareRestartableData<Real>("expected_distance"))

{
  // Computing the expected distance only works with rectanglar domains because we have to compute
  // the intersection with the Ray and the domain bounding box (which only represents the domain if
  // it is rectangular)
  if (_compute_expected_distance && !isRectangularDomain())
    paramError("compute_expected_distance", "Not supported when the domain is non-rectangular.");

  const bool polar_set = parameters.isParamSetByUser("polar_quad_order");
  const bool azimuthal_set = parameters.isParamSetByUser("azimuthal_quad_order");
  if (_mesh.dimension() == 1)
  {
    if (polar_set)
      paramError("polar_quad_order", "Not needed for 1D");
    if (azimuthal_set)
      paramError("azimuthal_quad_order", "Not needed for 1D");
  }
  if (!_side_aq && !_centroid_aq)
  {
    if (polar_set)
      paramError("polar_quad_order", "Not needed without side_aq or centroid_aq enabled");
    if (azimuthal_set)
      paramError("azimuthal_quad_order", "Not needed without side_aq or centroid_aq enabled");
  }
}

void
LaserRayStudy::defineRays()
{

  // std::shared_ptr<Ray> ray = acquireRay();

  // ray->setStart(Point(90, 300, 0));

  // ray->setStartingDirection(Point(0.05, -1, 0));

  // ray->setStartingMaxDistance(1000);

  // _rays.emplace_back(std::move(ray));

  // 2D

  // unsigned int nx = 50;
  // Real r0 = 0.5e-3;
  // Real xmin = 0.0001;
  // Real xmax = 0.0003;
  // Real Q = 350;
  // Real size = (xmax - xmin) / nx;
  // unsigned int num_rays = 0;

  // Real xc = (xmax + xmin) / 2.0;

  // std::vector<std::string> names;
  // std::vector<Real> x_coords;
  // std::vector<Real> energy_rays;
  // std::vector<Point> directions;
  // std::vector<Point> start_points;

  // for (unsigned int i = 0; i < nx; ++i)
  // {

  //   Real xcell = (i + 0.5) * size + xmin;
  //   if (((xcell - xc) * (xcell - xc)) < r0 * r0)
  //   {
  //     num_rays++;
  //   }
  // }

  // Real Q_bar = Q / _mesh.elemPtr(0)->hmin();

  // num_rays = 0;

  // for (unsigned int i = 0; i < nx; ++i)
  // {
  //   Real xcell = (i + 0.5) * size + xmin;
  //   if (((xcell - xc) * (xcell - xc)) < r0 * r0)
  //   {
  //     num_rays++;
  //     names.push_back("ray_" + std::to_string(num_rays));
  //     x_coords.push_back(xcell);
  //     start_points.push_back(Point(xcell, .00125, 0));
  //     directions.push_back(Point(0, -1, 0));
  //     Real energy = 2.0 * Q_bar / (r0 * r0 * libMesh::pi) *
  //                   std::exp(-2.0 / r0 / r0 * ((xcell - xc) * (xcell - xc)));
  //     energy_rays.push_back(energy);
  //   }
  // }

  // std::cout << "number of rays = " << num_rays << std::endl;

  // for (std::size_t i = 0; i < names.size(); ++i)
  // {
  //   std::shared_ptr<Ray> ray = acquireRay();

  //   Point shift_x((_t - 0.02) * 0.005, 0, 0);

  //   if (_t < 0.02)
  //     shift_x(0) = 0.0;

  //   // Point shift_x(_t * 0.01, 0, 0);

  //   ray->setStart(start_points[i] + shift_x);

  //   ray->setStartingDirection((directions)[i]);

  //   ray->data(_energy_density_index) = energy_rays[i];

  //   ray->setStartingMaxDistance(0.003);

  //   _rays.emplace_back(std::move(ray));
  // }

  // return;

  // 3D

  unsigned int nx = 20;
  unsigned int ny = 20;
  Real r0 = 0.25e-3;
  Real cut_off_r = 30e-6;
  Real xmin = 60e-6;
  Real ymin = 60e-6;
  Real xmax = 120e-6;
  Real ymax = 120e-6;
  Real Q = 100;
  Real size = (xmax - xmin) / nx;
  unsigned int num_rays = 0;

  Real xc = (xmax + xmin) / 2.0;
  Real yc = (ymax + ymin) / 2.0;

  std::vector<std::string> names;
  std::vector<Real> x_coords;
  std::vector<Real> y_coords;
  std::vector<Real> energy_rays;
  std::vector<Point> directions;
  std::vector<Point> start_points;

  for (unsigned int i = 0; i < nx; ++i)
  {
    for (unsigned int j = 0; j < ny; ++j)
    {
      Real xcell = (i + 0.5) * size + xmin;
      Real ycell = (j + 0.5) * size + ymin;
      if (((xcell - xc) * (xcell - xc) + (ycell - yc) * (ycell - yc)) < cut_off_r * cut_off_r)
      {
        num_rays++;
      }
    }
  }

  // Real Q_bar = Q * (r0 * r0 * libMesh::pi) / num_rays / _mesh.elemPtr(0)->volume();
  Real Q_bar = Q / _mesh.elemPtr(0)->hmin();

  num_rays = 0;

  for (unsigned int i = 0; i < nx; ++i)
  {
    for (unsigned int j = 0; j < ny; ++j)
    {
      Real xcell = (i + 0.5) * size + xmin;
      Real ycell = (j + 0.5) * size + ymin;
      if (((xcell - xc) * (xcell - xc) + (ycell - yc) * (ycell - yc)) < cut_off_r * cut_off_r)
      {
        num_rays++;
        names.push_back("ray_" + std::to_string(num_rays));
        x_coords.push_back(xcell);
        y_coords.push_back(ycell);
        start_points.push_back(Point(xcell, ycell, 300e-6));
        directions.push_back(Point(0, 0, -1));
        Real energy =
            2.0 * Q_bar / (r0 * r0 * libMesh::pi) *
            std::exp(-2.0 / r0 / r0 * ((xcell - xc) * (xcell - xc) + (ycell - yc) * (ycell - yc)));
        // Real energy = Q_bar;
        energy_rays.push_back(energy);
      }
    }
  }

  std::cout << "number of rays = " << num_rays << std::endl;

  for (std::size_t i = 0; i < names.size(); ++i)
  {
    std::shared_ptr<Ray> ray = acquireRay();

    // Point shift_x((_t - 0.28) * 0.009, 0, 0);

    // if (_t < 0.28)
    //   shift_x(0) = 0.0;

    // Point shift_x(-_t * 300e-4, 0, 0);

    // ray->setStart(start_points[i] + shift_x);
    ray->setStart(start_points[i]);

    ray->setStartingDirection((directions)[i]);

    ray->data(_energy_density_index) = energy_rays[i];

    ray->setStartingMaxDistance(0.003);

    _rays.emplace_back(std::move(ray));
  }

  return;

  if (_compute_expected_distance)
  {
    _bbox_intersection_helper =
        std::make_unique<BoundingBoxIntersectionHelper>(boundingBox(), _mesh.dimension());
    _expected_distance = 0;
  }

  // Gather the boundary elem centroids
  std::vector<Point> boundary_centroids;
  if (_centroid_to_centroid)
  {
    for (const auto & elem : *_mesh.getActiveLocalElementRange())
      if (elem->on_boundary())
        boundary_centroids.push_back(elem->vertex_average());
    _comm.allgather(boundary_centroids);
  }

  const std::unique_ptr<RayTracingAngularQuadrature> half_aq =
      _side_aq ? std::make_unique<RayTracingAngularQuadrature>(
                     _mesh.dimension(), _polar_quad_order, _azimuthal_quad_order, 0, 1)
               : nullptr;

  for (const auto & bnd_elem : *_mesh.getBoundaryElementRange())
  {
    const auto side = bnd_elem->_side;
    const Elem * elem = bnd_elem->_elem;

    // Starting from local elems only
    if (elem->processor_id() != _pid)
      continue;

    // Centroid on this boundary face
    const auto & side_centroid = elemSide(*elem, side).vertex_average();

    // Vertices on this boundary side -> in direction of all other vertices not on said boundary
    // side on elem
    if (_vertex_to_vertex)
      for (const auto n : elem->nodes_on_side(side))
        if (elem->is_vertex(n))
          for (unsigned int v_to = 0; v_to < elem->n_vertices(); ++v_to)
            if (!elem->is_node_on_side(v_to, side))
              defineRay(elem, side, elem->point(n), elem->point(v_to), false);

    // Centroid on this boundary side -> in direction of all other vertices not on said boundary
    // side on elem
    if (_centroid_to_vertex)
      for (unsigned int v_to = 0; v_to < elem->n_vertices(); ++v_to)
        if (!elem->is_node_on_side(v_to, side))
          defineRay(elem, side, side_centroid, elem->point(v_to), false);

    // Centroid on this boundary side -> centroids of all boundary elements
    if (_centroid_to_centroid)
      for (const auto & other_centroid : boundary_centroids)
        defineRay(elem, RayTracingCommon::invalid_side, side_centroid, other_centroid, true);

    // Centroids of edges on this boundary side -> in direction of all other edge centroids
    if (_edge_to_edge)
      for (const auto edge : elem->edge_index_range())
        if (elem->is_edge_on_side(edge, side))
        {
          const Point edge_centroid = elem->build_edge_ptr(edge)->vertex_average();
          for (const auto edge_to : elem->edge_index_range())
            if (edge != edge_to)
              defineRay(elem,
                        side,
                        edge_centroid,
                        elem->build_edge_ptr(edge_to)->vertex_average(),
                        false);
        }

    if (_side_aq)
    {
      const auto inward_normal = -1.0 * getSideNormal(elem, side, /* tid = */ 0);
      half_aq->rotate(inward_normal);

      for (std::size_t l = 0; l < half_aq->numDirections(); ++l)
        defineRay(elem, side, side_centroid, side_centroid + half_aq->getDirection(l), false);
    }
  }

  if (_centroid_aq)
  {
    RayTracingAngularQuadrature full_aq(
        _mesh.dimension(), _polar_quad_order, _azimuthal_quad_order, -1, 1);

    for (const Elem * elem : *_mesh.getActiveLocalElementRange())
      if (elem->processor_id() == _pid && elem->on_boundary())
      {
        const auto centroid = elem->vertex_average();
        for (std::size_t l = 0; l < full_aq.numDirections(); ++l)
          defineRay(elem,
                    RayTracingCommon::invalid_side,
                    centroid,
                    centroid + full_aq.getDirection(l),
                    false);
      }
  }

  if (_compute_expected_distance)
    _communicator.sum(_expected_distance);

  // Insertion point for other derived test objects to modify the Rays
  modifyRays();
}

void
LaserRayStudy::defineRay(const Elem * starting_elem,
                         const unsigned short incoming_side,
                         const Point & p1,
                         const Point & p2,
                         const bool ends_within_mesh)
{
  std::shared_ptr<Ray> ray = _use_unsized_rays ? acquireUnsizedRay() : acquireRay();

  ray->setStart(
      p1, starting_elem, _set_incoming_side ? incoming_side : RayTracingCommon::invalid_side);

  if (ends_within_mesh)
  {
    ray->setStartingEndPoint(p2);

    if (_compute_expected_distance)
      _expected_distance += (p2 - p1).norm();
  }
  else
  {
    ray->setStartingDirection(p2 - p1);

    if (_compute_expected_distance)
    {
      const Point end = _bbox_intersection_helper->intersection(p1, ray->direction());
      if (end == RayTracingCommon::invalid_point)
        mooseError("Expected distance end intersection not found\n\n", ray->getInfo());

      _expected_distance += (end - p1).norm();
    }
  }

  _rays.emplace_back(std::move(ray));
}

void
LaserRayStudy::modifyRays()
{
}
