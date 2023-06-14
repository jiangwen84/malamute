/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "MovingContactLineBC.h"
#include "MooseMesh.h"
#include "INSADObjectTracker.h"
#include "NS.h"

registerMooseObject("MalamuteApp", MovingContactLineBC);

InputParameters
MovingContactLineBC::validParams()
{
  InputParameters params = ADVectorIntegratedBC::validParams();

  params.addClassDescription("This class implements moving contact lines BC.");
  params.addRequiredCoupledVar("level_set_gradient",
                               "Regularized gradient of the level set variable");
  params.addParam<MaterialPropertyName>("mu_name", "mu", "The name of the dynamic viscosity");
  params.addParam<Real>("beta_n", "friction coefficeint between the fluids and the solid wall.");
  params.addParam<Real>("beta_cl", "friction coefficient at the contact line.");
  params.addParam<Real>("penalty", "penalty value.");
  params.addRequiredParam<Real>("surface_tension", "Surface tension coefficient.");
  params.addRequiredParam<Real>("static_contact_angle", "Static contact angle.");
  return params;
}

MovingContactLineBC::MovingContactLineBC(const InputParameters & parameters)
  : ADVectorIntegratedBC(parameters),
    _mu(getADMaterialProperty<Real>("mu_name")),
    _grad_c(adCoupledVectorValue("level_set_gradient")),
    _beta_n(getParam<Real>("beta_n")),
    _beta_cl(getParam<Real>("beta_cl")),
    _penalty(getParam<Real>("penalty")),
    _sigma(getParam<Real>("surface_tension")),
    _theta(getParam<Real>("static_contact_angle"))
{
}

ADReal
MovingContactLineBC::computeQpResidual()
{
  RankTwoTensor iden(RankTwoTensor::initIdentity);
  ADRankTwoTensor proj;
  proj.vectorOuterProduct(_normals[_qp], _normals[_qp]);
  proj = iden - proj;

  ADReal delta = (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();
  ADReal beta =
      _beta_n +
      _beta_cl * delta *
          (proj * _grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm();

  // ADRealVectorValue slip_term =
  //     proj *
  //     (_mu[_qp] * ((_grad_u[_qp] + _grad_u[_qp].transpose()) * _normals[_qp]) +
  //      _sigma * delta *
  //          (std::cos(_theta) -
  //           _grad_c[_qp] /
  //               (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm()
  //               * _normals[_qp]) *
  //          _grad_c[_qp]);

  // ADRealVectorValue slip_term = _sigma * delta * std::cos(_theta) * _grad_c[_qp];

  ADRealVectorValue slip_term =
      _sigma * delta *
      (_grad_c[_qp] /
       (_grad_c[_qp] + RealVectorValue(libMesh::TOLERANCE * libMesh::TOLERANCE)).norm() *
       _normals[_qp]) *
      _grad_c[_qp];

  ADReal residual = _penalty * (beta * _u[_qp] + slip_term) * _test[_i][_qp];

  return residual;
}
