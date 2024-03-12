[GlobalParams]
  gravity = '0 0 0'
  supg = true
  pspg = true
  convective_term = true
  integrate_p_by_parts = false
  transient_term = true
  u = velocity_x
  v = velocity_y
  pressure = p
  alpha = 0.01
  order = FIRST
  family = LAGRANGE
[]

[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.01
    ymin = 0
    ymax = 0.01
    nx = 100
    ny = 100
    elem_type = QUAD4
  []
  [corner_node]
    type = ExtraNodesetGenerator
    new_boundary = 'pinned_node'
    coord = '0.0 0.01'
    input = gen
  []
[]

[ICs]
  [ls_ic]
    type = FunctionIC
    function = ls_exact
    variable = ls
  []
  [velocity_x]
    type = ConstantIC
    value = 1e-10
    variable = velocity_x
  []
  [velocity_y]
    type = ConstantIC
    value = 1e-10
    variable = velocity_y
  []
[]

[Variables]
  # [temp]
  #   initial_condition = 300
  # []
  [velocity_x]
    # order = SECOND
    family = LAGRANGE
  []
  [velocity_y]
    # order = SECOND
    family = LAGRANGE
  []
  [p]
  []
  # [curvature]
  # []
[]

[AuxKernels]
  [temp]
    type = FunctionAux
    variable = temp
    function = 'if(sqrt((x-0.005)*(x-0.005)+(y-0.005)*(y-0.005))<1e-3,300+100000*t,300)'
  []
[]

[AuxVariables]
  [ls]
  []
  [curvature]
    initial_condition = 0
  []
  [temp]
    initial_condition = 300
  []
[]

[Functions]
  [ls_exact]
    type = LevelSetOlssonPlane
    epsilon = 0.0002
    point = '0.005 0.005 0'
    normal = '0 1 0'
  []
[]

[BCs]
  [no_slip_x]
    type = FunctionDirichletBC
    variable = velocity_x
    boundary = 'bottom left right'
    function = 0
  []
  [no_slip_y]
    type = FunctionDirichletBC
    variable = velocity_y
    boundary = 'bottom left right'
    function = 0
  []
  [no_bc_x]
    type = INSMomentumNoBCBCTractionForm
    boundary = top
    variable = velocity_x
    u = velocity_x
    v = velocity_y
    pressure = p
    component = 0
  []
  [no_bc_y]
    type = INSMomentumNoBCBCTractionForm
    boundary = top
    variable = velocity_y
    u = velocity_x
    v = velocity_y
    pressure = p
    component = 1
  []
  [pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  []
[]

[Kernels]
  # [curvature]
  #   type = nonADLevelSetCurvatureRegularization
  #   level_set = ls
  #   variable = curvature
  #   varepsilon = 2e-3
  # []

  # [heat_time]
  #   type = INSTemperatureTimeDerivative
  #   variable = temp
  # []

  # [heat]
  #   type = MeltPoolTemperature
  #   variable = temp
  #   u = velocity_x
  #   v = velocity_y
  #   temperature = temp
  #   level_set = ls
  # []

  # [heat_source]
  #   type = nonADMeltPoolHeatSource
  #   variable = temp
  #   laser_power = 1000
  #   effective_beam_radius = 1e-3
  #   absorption_coefficient = 0.27
  #   heat_transfer_coefficient = 100
  #   StefanBoltzmann_constant = 5.67e-8
  #   material_emissivity = 0.59
  #   ambient_temperature = 300
  #   laser_location_x = '0.005 + 6e-3*t'
  #   laser_location_y = '0.005'
  #   rho_l = 8000
  #   rho_g = 1.184
  #   vaporization_latent_heat = 6.1e6
  #   level_set = ls
  # []

  [mass]
    type = MeltPoolMass
    variable = p
    surface_tension = 1.169
    thermal_capillary = -4.3e-4
    temperature = temp
    curvature = curvature
    level_set = ls
  []

  [x_time]
    type = INSMomentumTimeDerivative
    variable = velocity_x
  []
  [y_time]
    type = INSMomentumTimeDerivative
    variable = velocity_y
  []

  [x_momentum_space]
    type = MeltPoolMomentumTractionForm
    variable = velocity_x
    component = 0
    surface_tension = 1.169
    thermal_capillary = -4.3e-4
    rho_l = 8000
    rho_g = 1.184
    temperature = temp
    curvature = curvature
    level_set = ls
  []

  # y-momentum, space
  [y_momentum_space]
    type = MeltPoolMomentumTractionForm
    variable = velocity_y
    component = 1
    surface_tension = 1.169
    thermal_capillary = 4.3e-4
    rho_l = 8000
    rho_g = 1.184
    temperature = temp
    curvature = curvature
    level_set = ls
  []
[]

[Materials]
  [thermal]
    type = nonADLevelSetThermalMaterial
    temperature = temp
    c_g = 300
    c_s = 500
    c_l = 500
    k_g = 0.017
    k_s = 31.8724
    k_l = 209.3
    solidus_temperature = 1648
    latent_heat = 2.5e5
  []
  [mushy]
    type = nonADMushyZoneMaterial
    temperature = temp
    liquidus_temperature = 1673
    solidus_temperature = 1648
    rho_s = 8000
    rho_l = 8000
  []
  [delta]
    type = nonADLevelSetDeltaFunction
    level_set = ls
    outputs = all
  []
  [heaviside]
    type = nonADLevelSetHeavisideFunction
    level_set = ls
  []
  [mass_transfer]
    type = nonADMeltPoolMassTransferMaterial
    temperature = temp
    Boltzmann_constant = 1.38064852e-23
    vaporization_latent_heat = 6.1e6
    atomic_weight = 97.43e-27
    vaporization_temperature = 3134
    reference_pressure = 1.01e5
    outputs = all
  []
  [fluid]
    type = nonADLevelSetFluidMaterial
    rho_g = 1.184
    rho_s = 8000
    rho_l = 8000
    mu_g = 1.81e-5
    mu_l = 0.1
    permeability_constant = 1e-8
    outputs = all
  []
[]

# [Preconditioning]
#   [SMP]
#     type = SMP
#     full = true
#   []
# []

[Preconditioning]
  # [FSP]
  #   type = FSP
  #   # It is the starting point of splitting

  #   topsplit = 'by_var'
  #   [by_var]
  #     splitting = 'u p temp'
  #     splitting_type = ADDITIVE
  #     petsc_options_iname = '-ksp_type'
  #     petsc_options_value = 'fgmres'
  #   []

  #   [up]
  #     splitting = 'u p' # 'u' and 'p' are the names of subsolvers
  #     splitting_type = schur
  #     petsc_options_iname = '-pc_fieldsplit_schur_fact_type  -pc_fieldsplit_schur_precondition -ksp_gmres_restart -ksp_rtol -ksp_type'
  #     petsc_options_value = 'full                            selfp                             300                1e-4      fgmres'
  #   []
  #   [u]
  #     vars = 'velocity_x velocity_y'
  #     # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
  #     # petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
  #     petsc_options_iname = '-pc_type -pc_hypre_typ'
  #     petsc_options_value = '     hypre boomeramg'
  #   []
  #   [p]
  #     vars = 'p'
  #     # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
  #     # petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
  #     petsc_options_iname = '-pc_type -pc_hypre_typ'
  #     petsc_options_value = '     hypre boomeramg'
  #   []

  #   [temp]
  #     vars = 'temp'
  #     # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -pc_hypre_type  -ksp_pc_side'
  #     # petsc_options_value = 'gmres    300                5e-2      hypre  boomeramg  right'
  #     petsc_options_iname = '-pc_type -pc_hypre_typ'
  #     petsc_options_value = '     hypre boomeramg'
  #   []
  # []

  # [FSP]
  #   type = FSP
  #   # It is the starting point of splitting
  #   topsplit = 'up' # 'up' should match the following block name
  #   [up]
  #     splitting = 'u p' # 'u' and 'p' are the names of subsolvers
  #     splitting_type = schur
  #     # Splitting type is set as schur, because the pressure part of Stokes-like systems
  #     # is not diagonally dominant. CAN NOT use additive, multiplicative and etc.
  #     #
  #     # Original system:
  #     #
  #     # | Auu Aup | | u | = | f_u |
  #     # | Apu 0   | | p |   | f_p |
  #     #
  #     # is factorized into
  #     #
  #     # |I             0 | | Auu  0|  | I  Auu^{-1}*Aup | | u | = | f_u |
  #     # |Apu*Auu^{-1}  I | | 0   -S|  | 0  I            | | p |   | f_p |
  #     #
  #     # where
  #     #
  #     # S = Apu*Auu^{-1}*Aup
  #     #
  #     # The preconditioning is accomplished via the following steps
  #     #
  #     # (1) p* = f_p - Apu*Auu^{-1}f_u,
  #     # (2) p = (-S)^{-1} p*
  #     # (3) u = Auu^{-1}(f_u-Aup*p)

  #     petsc_options_iname = '-pc_fieldsplit_schur_fact_type  -pc_fieldsplit_schur_precondition -ksp_gmres_restart -ksp_rtol -ksp_type'
  #     petsc_options_value = 'full                            selfp                             100                1e-4      fgmres'
  #   []
  #   [u]
  #     vars = 'velocity_x velocity_y'
  #     petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
  #     petsc_options_value = 'hypre    boomeramg      gmres    5e-1      100                 right'
  #     # petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -ksp_pc_side'
  #     # petsc_options_value = 'gmres    300                5e-1      jacobi    right'
  #   []
  #   [p]
  #     vars = 'p'
  #     petsc_options_iname = '-ksp_type -ksp_gmres_restart -ksp_rtol -pc_type -ksp_pc_side'
  #     petsc_options_value = 'gmres    300                5e-1      jacobi    right'
  #     # petsc_options_iname = '-pc_type -pc_hypre_type -ksp_type -ksp_rtol -ksp_gmres_restart -ksp_pc_side'
  #     # petsc_options_value = 'hypre    boomeramg      gmres    5e-1      100                 right'
  #   []
  # []

  [SMP]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  dt = 1e-3
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-8
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -pc_factor_shift_type'
  petsc_options_value = ' asm      lu           2               31 NONZERO'
  # petsc_options_iname = '-pc_type -pc_factor_shift_type'
  # petsc_options_value = 'lu       NONZERO'
  l_max_its = 30
  l_tol = 1e-4
  num_steps = 100
  line_search = 'none'
  nl_div_tol = 1e20
  automatic_scaling = true
  off_diagonals_in_auto_scaling = true
[]

[Outputs]
  exodus = true
[]
