[Mesh]
    [gmg]
        type = GeneratedMeshGenerator
        dim = 2
        xmin = 0
        xmax = 5
        ymin = 0
        ymax = 5
        nx = 20
        ny = 20
    []
[]

[Functions]
    [phi_exact]
        type = LevelSetOlssonPlane
        epsilon = 0.4
        point = '2.5 2.5 0'
        normal = '1 1 0'
    []
[]

[Variables]
    [phase]
        [InitialCondition]
            type = FunctionIC
            function = phi_exact
        []
    []
[]

[AuxVariables]
    [phi]
        family = LAGRANGE
        order = FIRST
    []
    [deposition]
        order = CONSTANT
        family = MONOMIAL
    []
    [deposition_number]
        order = CONSTANT
        family = MONOMIAL
    []
    [ls]
        [InitialCondition]
            type = FunctionIC
            function = phi_exact
        []
    []
    [refractive_index]
        [InitialCondition]
            type = FunctionIC
            function = '1.0 + (x > 2.5) * 0.3'
        []
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
        epsilon = 0.01
    []
    [deposition_number]
        type = LaserDepositionNumberRayKernel
        variable = deposition_number
        depends_on = refraction
        phase = ls
    []
[]

[RayBCs]
    [kill]
        type = KillRayBC
        boundary = 'top bottom left right'
    []
[]

[UserObjects/study]
    type = LaserRayStudy
    execute_on = TIMESTEP_BEGIN
  
    always_cache_traces = true
    data_on_cache_traces = true
  
    laser_power = 200
    beam_radius = 5
    cutoff_radius = 0.1e-3
    xmin = 2.0
    xmax = 3.0
    nx = 10

    starting_height = 4
    max_distance = 10
[]

[VectorPostprocessors]
    [centroid]
      type = EnergyDepositionPostprocessor
      deposition_number = deposition_number
      deposition = deposition
      execute_on = timestep_end
    []
[]

[Executioner]
    type = Steady
[]

[Problem]
    solve = false
[]

[Outputs]
    exodus = true
    [rays]
        type = RayTracingExodus
        study = study
        execute_on = FINAL
    []
[]
