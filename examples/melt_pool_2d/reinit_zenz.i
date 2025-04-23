[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = -0.4e-3
    xmax = 0.4e-3
    ymin = -0.6e-3
    ymax = 0.2e-3
    nx = 32
    ny = 32
    elem_type = QUAD4
  []
[]

[Adaptivity]
  marker = marker
  max_h_level = 3
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
    level_set_gradient = grad_ls
    epsilon = 0.00002
  []
 [grad_ls]
    type = LevelSetNormalRegularization
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

[Debug]
  show_var_residual_norms = true
[]

 [Preconditioning]
   [FSP]
     type = FSP
     topsplit = 'by_var'
     [by_var]
       splitting = 'grad_ls ls'
       splitting_type = additive
       petsc_options_iname = '-ksp_type'
       petsc_options_value = 'fgmres'
     []
     [grad_ls]
       vars = 'grad_ls'
      petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_shift_amount'
     petsc_options_value = ' asm      ilu           2               31 NONZERO  1e-12'
   #     petsc_options_iname = '-pc_type -pc_hypre_type'
   # petsc_options_value = 'hypre boomeramg'
#         petsc_options_iname = '-ksp_type -ksp_gmres_restart'
#       petsc_options_value = 'gmres    300'
     []
     [ls]
       vars = 'ls'
       # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
       # petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
   #     petsc_options_iname = '-pc_type -pc_hypre_type'
   # petsc_options_value = 'hypre boomeramg'
         petsc_options_iname = '-ksp_type -ksp_gmres_restart'
       petsc_options_value = 'gmres    300'
   # petsc_options_iname = '-pc_type -pc_sub_type'
   # petsc_options_value = 'asm      ilu'
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
  l_max_its = 50
  line_search = none
  reuse_preconditioner = true
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO superlu_dist preonly'
  # petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  # petsc_options_value = ' asm      lu           2               31                 preonly'

  # petsc_options_iname = '-pc_type -pc_sub_type'
  # petsc_options_value = 'asm      lu'

  #   petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

#            petsc_options_iname = '-ksp_type -ksp_gmres_restart'
#      petsc_options_value = 'gmres    1000'

  automatic_scaling = true
# off_diagonals_in_auto_scaling = true
  dt = 5e-7
[]
[Outputs]
  exodus = false
  execute_on = 'TIMESTEP_END'
[]
