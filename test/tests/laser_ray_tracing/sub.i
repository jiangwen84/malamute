[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 100
    ny = 100
    xmax = 5
    ymax = 5
  []
[]

[Variables]
  [phi]
  []
[]

[AuxVariables]
  [phi_0]
  []
  [marker]
  []
[]

[Kernels]
  [time]
    type = TimeDerivative
    variable = phi
  []
  [reinit]
    type = LevelSetOlssonReinitialization
    variable = phi
    phi_0 = phi_0
    epsilon = 0.05
  []
[]

[Problem]
  type = LevelSetReinitializationProblem
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  num_steps = 10
  nl_abs_tol = 1e-14
  scheme = crank-nicolson
  line_search = none
  petsc_options_iname = '-pc_type -pc_sub_type'
  petsc_options_value = 'asm      ilu'
  dt = 0.003
[]

[Outputs]
[]
