# 3501 Leagues Under the Sea
a.k.a Bjag

Authors: Joey Villeneuve; Ben Seguin-Frank; Laura Jin; George Power

__Compilation Instructions__

Other than the standard libraries provided, this project also depends on ``ImGui`` and ``<INSERT AUDIO LIBRARY HERE>``

- Install ``ImGui``
	- Source files for ImGui are found in the ``./imgui`` directory

- Configure ``CMakeLists.txt``
	- Set the library path, ``set(LIBRARY_PATH C:\Software\Libraries)``, to your local library directory

- Configure ``path_config.h``
	- The material directory is to be the same directory as the source files 
	- (e.g. ``#define MATERIAL_DIRECTORY  "C:\\Users\\nino\\Documents\\GitHub\\bjag"``)