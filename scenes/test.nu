camera.type = pinhole
camera.fov = 60
camera.asp = 1
camera.transform.position.x = 0
camera.transform.position.y = 100
camera.transform.position.z = 100
camera.transform.dirtype = gaze_up
camera.transform.gaze.x = 0
camera.transform.gaze.y = 0
camera.transform.gaze.z = 0
camera.transform.up.x = 0
camera.transform.up.y = 0
camera.transform.up.z = 1

film.type = ldr
film.width = 128
film.height = 128

integrator.type = path
integrator.spp = 32

materials += matlight1
materials += matdiffuse1
matlight1.type = material_light_diffuse
matlight1.radiance.r = 100
matlight1.radiance.g = 100
matlight1.radiance.b = 100
matdiffuse1.type = material_lambert1
matdiffuse1.f.r = 0.8
matdiffuse1.f.g = 0.8
matdiffuse1.f.b = 0.8

objects += objlight
objects += objfloor
objlight.type = from_obj_file
objlight.filename = mitsuba/mitsuba-back.obj
objlight.material = matlight1
objfloor.type = from_obj_file
objfloor.filename = mitsuba/mitsuba-ball.obj
objfloor.material = matdiffuse1
