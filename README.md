# 3501 Leagues Under the Sea

Authors: Joey Villeneuve; Ben Seguin-Frank; Laura Jin; George Power

__Compilation Instructions__

Other than the standard libraries provided, this project also depends on ``ImGui`` and ``irrKlang`` (for audio).

Source files for ImGui are found in the ``./imgui`` directory. No further set-up should be required for it to work.

- Install ``irrKlang``
	- Copy and paste the ``/irrKlang/`` folder in your ``Libraries/include/`` folder. Your directory should be structured as: ``Libraries/include/irrKlang/``
	- Copy and paste ``irrKlang.dll`` & ``irrKlang.lib`` into your ``Libraries/lib/`` folder
	- While they should already be included, ensure that ``ikpFlac.dll``, ``ikpMP3.dll``, & ``irrKlang.dll``

- Configure ``CMakeLists.txt``
	- Set the library path, ``set(LIBRARY_PATH C:\Software\Libraries)``, to your local library directory

- Configure ``path_config.h``
	- The material directory is to be the same directory as the source files 
	- (e.g. ``#define MATERIAL_DIRECTORY  "C:\\Users\\nino\\Documents\\GitHub\\bjag"``)