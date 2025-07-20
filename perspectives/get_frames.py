import bpy
import math
import mathutils
import os

num_frames = 46
angle_step_deg = 2
radius = 8
output_path = "/Users/tomraynes/Desktop/MSc CS/Programming in C/Projects/Orbit/frames/saturn_ring"

os.makedirs(bpy.path.abspath(output_path), exist_ok=True)

earth = bpy.data.objects['Sphere']
camera = bpy.data.objects['Camera']

scene = bpy.context.scene
scene.camera = camera
scene.render.engine = 'CYCLES'
scene.render.resolution_x = 650
scene.render.resolution_y = 650
scene.render.resolution_percentage = 100
scene.render.film_transparent = True
scene.render.image_settings.file_format = 'PNG'
scene.render.image_settings.color_mode = 'RGBA'

def set_camera_position(i):
    theta_deg = i * angle_step_deg
    theta_rad = math.radians(theta_deg)

    # Use fixed axes ignoring object's rotation
    local_x = mathutils.Vector((1, 0, 0))  # world X axis
    local_z = mathutils.Vector((0, 0, 1))  # world Z axis

    # Rotate local Z around local X by theta
    rotation = mathutils.Quaternion(local_x, theta_rad)
    cam_dir = rotation @ local_z

    # Position the camera along this direction
    cam_location = earth.location + cam_dir * radius
    camera.location = cam_location

    # Make camera look at Earth
    direction = earth.location - cam_location
    rot_quat = direction.to_track_quat('-Z', 'X')  # -Z forward, X left
    camera.rotation_euler = rot_quat.to_euler()

for i in range(num_frames):
    print(f"Rendering frame {i+1}/{num_frames}")
    set_camera_position(i)
    scene.render.filepath = os.path.join(output_path, f"{i+1:02d}.png")
    bpy.ops.render.render(write_still=True)

print("Rendering Complete")

