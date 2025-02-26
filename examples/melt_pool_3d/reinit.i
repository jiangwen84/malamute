[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.0015
    ymin = 0
    ymax = 0.003
    nx = 25
    ny = 50
    elem_type = QUAD4
  []
  uniform_refine = 1
[]

[Adaptivity]
  marker = marker
  max_h_level = 2
  #cycles_per_step = 2
  #initial_steps = 1
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
  [grad_ls]
    family = LAGRANGE_VEC
  []
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
    level_set_gradient = grad_ls
    epsilon = 0.000015
  []
  [grad_ls]
    type = VariableGradientRegularization
    regularized_var = ls_0
    variable = grad_ls
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


[Preconditioning]
  [FSP]
    type = FSP
    topsplit = 'by_var'
    full = true
    [by_var]
      splitting = 'grad_ls ls'
      splitting_type = multiplicative
      petsc_options_iname = '-ksp_type'
      petsc_options_value = 'fgmres'
    []
    [grad_ls]
      vars = 'grad_ls'
    petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_shift_amount'
   petsc_options_value = ' asm      ilu           2               31 NONZERO  1e-12'
    []
    [ls]
      vars = 'ls'
      petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
      petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
    []
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  num_steps = 10
  nl_abs_tol = 1e-10
  nl_forced_its = 5
  nl_max_its = 10
  line_search = none
  scheme = crank-nicolson
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO superlu_dist preonly'
  # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  # petsc_options_value = ' asm      lu           2               31                 preonly'
  automatic_scaling = true
  off_diagonals_in_auto_scaling = true
  dt = 1e-8
[]
[Outputs]
  exodus = false
  execute_on = 'TIMESTEP_END'
[]
