[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.01
    ymin = 0
    ymax = 0.01
    nx = 50
    ny = 50
    elem_type = QUAD4
  []
<<<<<<< HEAD
=======
  uniform_refine = 3
>>>>>>> 1b4375c (improve some models)
[]

# [Adaptivity]
#   steps = 3
#   marker = box
#   max_h_level = 3
#   initial_steps = 3
#   stop_time = 1.0e-10
#   [Markers]
#     [box]
#       bottom_left = '0.000 0.004 0'
#       inside = refine
#       top_right = '0.01 0.006 0'
#       outside = do_nothing
#       type = BoxMarker
#     []
#   []
# []

# [Adaptivity]
#   marker = marker
#   max_h_level = 3
#   # cycles_per_step = 1
#   [Indicators]
#     [error]
#       type = GradientJumpIndicator
#       variable = ls
#     []
#     [error2]
#       type = GradientJumpIndicator
#       variable = temp
#     []
#   []
#   [Markers]
#     [./marker]
#       type = ComboMarker
#       markers = 'marker1 marker2'
#     [../]
#     [marker1]
#       type = ErrorFractionMarker
#       coarsen = 0.01
#       refine = 0.2
#       indicator = error
#     []
#     [./marker2]
#       type = ValueThresholdMarker
#       coarsen = 900
#       variable = temp
#       refine = 1000
#     [../]
#   []
# []

[Variables]
  [ls]
    order = FIRST
  []
  # [grad_ls]
  #   family = LAGRANGE_VEC
  # []
[]
[AuxVariables]
  [ls_0]
    order = FIRST
  []
    [marker]
    family = MONOMIAL
    order = CONSTANT
  []
  # [temp]
  #   order = FIRST
  # []
[]
[Kernels]
  [time]
    type = TimeDerivative
    variable = ls
  []
  [reinit]
    type = LevelSetGradientRegularizationReinitialization
    variable = ls
    level_set = ls_0
    epsilon = 0.00008
  []
[]
[Problem]
  type = LevelSetReinitializationProblem
[]
# [UserObjects]
#   [arnold]
#     type = LevelSetOlssonTerminator
#     tol = 0.5
#     min_steps = 5
#   []
# []
[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  num_steps = 10
  nl_abs_tol = 1e-14
  nl_max_its = 10
  line_search = none
  scheme = crank-nicolson
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO superlu_dist preonly'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  petsc_options_value = ' asm      lu           2               31                 preonly'
  dt = 1e-6
[]
[Outputs]
  exodus = false
  execute_on = 'TIMESTEP_END'
[]
