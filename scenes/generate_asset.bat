:: Meshes
::"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-meshes.py -- cheese.blend:Collection ../dist/cheese.pnct
::"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-meshes.py -- lower_bun.blend:Collection ../dist/lower_bun.pnct
::"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-meshes.py -- patty.blend:Collection ../dist/patty.pnct
::"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-meshes.py -- upper_bun.blend:Collection ../dist/upper_bun.pnct
::"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-meshes.py -- veggies.blend:Collection ../dist/veggies.pnct
::"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-meshes.py -- walkmesh.blend:Collection ../dist/walkmesh.pnct


::Walkmesh
::"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-walkmeshes.py -- walkmesh.blend:Collection ../dist/walkmesh.w


"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-meshes.py -- ../final_assets/house.blend ../dist/house.pnct
"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-walkmeshes.py -- ../final_assets/house.blend:walkmeshes ../dist/house.w
"E:\SteamLibrary\steamapps\common\Blender\blender.exe" --background --python export-scene.py -- ../final_assets/house.blend ../dist/house.scene




