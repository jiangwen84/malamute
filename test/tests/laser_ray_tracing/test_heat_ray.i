[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 80
    ny = 80
    nz = 80
    xmax = 40e-4
    ymax = 40e-4
    zmax = 40e-4
    zmin = 0
  []
  [new_block]
    type = SubdomainBoundingBoxGenerator
    input = gen
    block_id = 1
    bottom_left = '0 0 0'
    top_right = '40e-4 40e-4 20e-4'
  []
[]

[Variables]
  [temp]
    initial_condition = 300
  []
[]

[Kernels]
  [heat_cond]
    type = ADHeatConduction
    thermal_conductivity = thermal_conductivity
    variable = temp
    block = 1
  []
  [heat_time]
    type = ADHeatConductionTimeDerivative
    specific_heat = specific_heat
    density_name = rho
    variable = temp
  []
  [heat_source]
    type = MeltPoolHeatSource
    variable = temp
    laser_power = 100
    effective_beam_radius = 5e-4
    absorption_coefficient = 1
    heat_transfer_coefficient = 100
    StefanBoltzmann_constant = 5.67e-8
    material_emissivity = 0.59
    ambient_temperature = 300
    laser_location_x = '20e-4'
    laser_location_y = '20e-4'
    laser_location_z = '20e-4'
    rho_l = 8000
    rho_g = 1.184
    vaporization_latent_heat = 6.1e6
    laser_deposition = deposition
    laser_deposition_number = deposition_number
    block = 1
  []
[]

[AuxVariables]
  [refractive_index]
    [InitialCondition]
      type = FunctionIC
      function = 0
    []
  []
  [deposition]
    order = CONSTANT
    family = MONOMIAL
  []
  [deposition_number]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[RayBCs]
  [kill]
    type = KillRayBC
    boundary = 'top right bottom left front back'
  []
[]

[RayKernels]
  [refraction]
    type = LaserReflectionRayKernel
    phase = ls
    refractive_index = refractive_index
  []
  [deposition]
    type = LaserDepositionRayKernel
    variable = deposition
    depends_on = refraction
    phase = ls
  []
  [deposition_number]
    type = LaserDepositionNumberRayKernel
    variable = deposition_number
    depends_on = refraction
    phase = ls
  []
[]

[UserObjects/study]
  type = LaserRayStudy
  execute_on = TIMESTEP_BEGIN

  vertex_to_vertex = false
  centroid_to_vertex = false
  centroid_to_centroid = false

  always_cache_traces = true
  data_on_cache_traces = true
[]

[AuxVariables]
  [ls]
    order = FIRST
    family = LAGRANGE
  []
[]

[AuxKernels]
  [ls]
    type = FunctionAux
    variable = ls
    function = ls_func
    execute_on = initial
  []
[]

[Functions]
  [ls_func]
    type = LevelSetOlssonPlane
    epsilon = 4e-4
    point = '0 0 20e-4'
    normal = '0 0 -1'
  []
[]

[Materials]
  [rho]
    type = ADGenericConstantMaterial
    prop_names = 'rho specific_heat thermal_conductivity melt_pool_mass_rate'
    prop_values = '7000 500 40 1'
  []
  [delta]
    type = LevelSetDeltaFunction
    #level_set_gradient = grad_ls
    level_set = ls
    outputs = all
  []
[]

[Postprocessors]
  [total_temp]
    type = ElementAverageValue
    variable = temp
    block = 1
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  dt = 0.01
  nl_abs_tol = 1e-6
  num_steps = 2
  nl_max_its = 12
  l_max_its = 100

  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -ksp_gmres_restart -sub_ksp_type'
  petsc_options_value = ' asm      lu           2               100                 preonly'

[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  []
[]

[Outputs]
  exodus = true
  csv = true
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = TIMESTEP_END
  []
[]
