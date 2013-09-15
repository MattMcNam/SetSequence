SetSequence
===========

About
-
SetSequence is a plugin for Portal which allows you to override Chell's animation sequences. This can be used to fix the T-pose sequence which will sometimes occur when leaving a portal.  
It is used in an upcoming Portal video to fix the above issue, and will be linked to here upon release.

Usage
-
`tpose_hook_set_sequence`  
This command is required in order to use any of the other plugin commands. Make sure you run it while in a map or viewing a demo. It only needs to be run once until you close Portal.  
  
`tpose_use_sequence [0-18]`  
The sequence to use in place of sequence 0 (T-pose). A list will be of IDs will be made available shortly, until then use the logging command below.  
  
`tpose_enable_console_logging [0-1]`  
This will cause the plugin to print a console message when Chell's sequence is changed. Useful with `developer 1` to see the messages in the upper-left of your screen.  
  
`tpose_force_sequence [0-1]`  
This will cause the plugin to override *all* sequences, not just the T-pose.  

Building
-
Requires hl2sdk-ob-valve.  
`cmake -DHL2SDK=<path-to-hl2sdk...> -G "Visual Studio 10" ..`  
Then open in Visual Studio/C++ Express and build.

License
-
SetSequence is provided under the BSD 2-Clause License, which is available in full in LICENSE.md
