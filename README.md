# Shell Shape Generator 2

An Unreal Engine 4 application designed to allow control over procedurally generated ammonite shells for the purpose of exporting the shell as a 3D file. 

Created for the Department of Geology and Geophysics at the University of Utah. 

## Obtaining the App

Look in the `Releases` link to the right side of this readme. 

---> 

## How It Works 

This tool works by allowing the edit of two bezier cross sections to create a 3D curve, then instancing the curve around a coiling axis and bridging the instances to create a self-intersecting tube. Ways to affect how the curve is instanced around the coiling axis and thus the proportions of the tube are as follows: 

- Expansion:

    This influence how "tall" the tube gets, presented as a ratio to itself 180° of coil ago (a single whorl).
  
    Note: This expansion starts from the base (dorsal side) of the shell tube, not the center! 

- Inflation:

    This influences how "wide" the tube gets, presented as a ratio to itself one whorl ago. 

- Distance: 

    This determines how much further away from the coiling axis the tube gets as it goes around the whorl, presented as a ratio to itself one whorl ago.

There are up to three "sections" that can have their own independent proportions and cross sections to allow for changes in growth over the length of the shell tube. Sections can also be blended between to allow for natural transitions. By default, only one section is enabled. 

## Visualizers Rundown

The panel to the left of the main screen are the presentation controls. These do not have any effect on the shell, but they affect the scene as well as some visualizations that clarify things about the shell. 

- Visualizers:
  - Show Shell Section Colors:

      This colorizes the shell model in the scene to show on the shell's surface where any given section starts, ends, and blends. Blue corresponds to section 1, green to section 2, and red to section 3. If only one section is enabled (which is default), this option will simply tint the shell blue.

  - Show Shell Final-Stage Ghost:

      This option, when enabled, shows a transparent rendition of the shell as it exists in its fully-grown state. This option only has an effect if the "percent visible" input in the shell controls is anything less than 100%.

  - Show Shell Cross Section Viewer:

      This enables the display that shows a cross section of the entire shell planarly parallel to the aperture opening. This display is in the current version largely a debug tool (hence not really matching the rest of the interface) and can be used to diagnose issues with the measured umbilical exposure.

- Scene Elements:
  - Caustics (%):

      This controls the intensity of the water caustics in the scene. Setting this to 0% turns the caustics off completely. 

  - Caustics Speed:

      This controls how quickly the caustics shift. This is a multiplier, so numbers less than one will reduce the speed and numbers greater than one will increase the speed. 

  - Marine Snow Density:

      This controls the quantity of marine snow put into the scene. Changes to this slider will not have an instantaneous result since the changes have to propagate to all particles. 

- Camera Controls:
  - Angle (°):

      This controls the yaw of the shell as displayed in the scene. Clicking and dragging on the shell horizontally is an alternate way to affect this. "Auto-Rotate Shell" also affects this (see below).

  - Roll (°):

      This controls the roll of the shell (tilt). Clicking and dragging on the shell vertically is an alternate way to affect this.

  - Auto-Rotate Shell:

      This causes the shell to spin around the vertical axis in a turntable fashion.
    
  - Rotate Clockwise:

      This option affects whether—when "Auto-Rotate Shell" is enabled—the shell spins clockwise or counter-clockwise.
    
  - Shell Rotate Speed:

      This affects how quickly the shell rotates when "Auto-Rotate Shell" is enabled. This is a multiplier, so numbers less than one will reduce the speed and numbers greater than one will increase the speed. 

## Shell Controls: The Global Tab 

This is the panel where the controls that affect the entire shell live. 

- Global Properties: 
  - Total Whorls:
  - Percent Visible:
  - Aperture Angle: 
  - Section 2 Controls: 
  - Section 3 Controls:
- Initial Proportions:
  - Expansion, Inflation, and Distance: 
- Resolution:
  - Cross Section Resolution: 
  - Use Fixed Iteration Distance:
  - Iteration Resolution:
  - Outer Slice Count: 
- Performance:
  - Low-Resolution During Editing:
  - Double All Resolutions: 

