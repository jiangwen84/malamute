[Mesh]
  [./gmg]
    type = ConcentricCircleMeshGenerator
    num_sectors = 8
    radii = '0.0015'
    rings = '4'
    has_outer_square = no
    pitch = 1.42063
    #portion = left_half
    preserve_volumes = off
  []

  [./extrude]
    type = MeshExtruderGenerator
    input = gmg
    num_layers = 25
    extrusion_vector = '0 0 0.006'
    bottom_sideset = 'new_front'
    top_sideset = 'new_back'
  []
[]

[Adaptivity]
  marker = marker
  max_h_level = 3
  #cycles_per_step = 2
  #initial_steps = 1
[]

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
[]

[Kernels]
  [grad_ls]
    type = VariableGradientRegularization
    regularized_var = ls_0
    variable = grad_ls
  []
  [time]
    type = TimeDerivative
    variable = ls
  []
  [reinit]
    type = LevelSetGradientRegularizationReinitialization
    variable = ls
    epsilon = 0.00006
    level_set_gradient = grad_ls
  []
[]

[Problem]
  type = LevelSetReinitializationProblem
[]


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
    petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
   petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-12'
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
  nl_max_its = 10
  nl_forced_its = 5
  line_search = none
  # petsc_options_iname = '-ksp_type -ksp_gmres_restart -pc_type -pc_hypre_type'
  # petsc_options_value = 'gmres    300              hypre  boomeramg'
      #   petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type -sub_pc_factor_mat_solver_type -sub_pc_factor_shift_amount'
      # petsc_options_value = ' asm      lu           2               31 NONZERO superlu_dist 1e-8'
  #       petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package -ksp_type'
  # petsc_options_value = 'lu NONZERO superlu_dist preonly'
  dt = 1e-8
[]

[Outputs]
  exodus = false
  execute_on = 'TIMESTEP_END'
[]
