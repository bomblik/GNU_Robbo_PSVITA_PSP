+-------+
| About |
+-------+

GNU Robbo is a free open source reimplementation of Janusz Pelc's
Robbo for the Atari XE/XL which was distributed by LK Avalon in 1989.

+---------+
| History |
+---------+

The first version of Robbo was released in 1989 by LK Avalon, and in a
short time became very popular in "little" Atari's world. Their main
developer Janusz Pelc did a very good job. There are rumours that he
wrote the game just three months before his graduation exam ;) In 1990
Janusz Pelc and LK Avalon released their next product Robbo Konstruktor
which enabled people to create their own levels.

In 1991, Maciej Miasik (http://www.linkedin.com/in/tosiabunio)
developed The Adventures of Robbo for XLanD Games -- an updated
PC-DOS port of the Atari version -- and published by Epic Games. In
2006 Maciej Miasik declared that the idea and code of the PC-DOS
version to be licenced under the Creative Commons Attribution-Share
Alike 2.5 Generic licence
(http://www.miasik.net/archive/2006/06/moje-gry-dla-kazdego/).

I (Alus) just ported it to Linux trying to set Robbo's behaviour as 
similar as it is possible. You can still buy Windows version of Robbo
in "LK Avalon", with better graphics and new objects and levels.
Thunor joined the project in 2007 and completely rebuilt first version.

+------------------------------------------+
| A More Complicated History, Allegedly :p |
+------------------------------------------+

Janusz Pelc's company XLanD Games created the Atari Robbo game and
it was distributed by LK Avalon, a promoter of independent developers.
In 1991 whilst working for XLanD Games, Maciej Miasik created the PC
version. In 1994 XLanD Games agreed to sell the rights to the PC
version to LK Avalon and then XLanD Games went out of business. Later
in 1996, Maciej Miasik joined LK Avalon and worked on several projects
until 2001. In 2006 with the permission of past and present LK Avalon
employees he released the PC version under a Creative Commons licence.

As far as licencing goes, any agreements between Janusz Pelc/XLanD
and LK Avalon regarding the Atari version disappeared with the demise
of XLanD Games, but the GNU Robbo team have been told that the Atari
Robbo game was the property of Janusz Pelc/XLanD Games and Janusz
Pelc has no problems with the existence of the GNU Robbo project.

+--------------+
| Requirements |
+--------------+

libSDL
libSDL_mixer
libSDL_image
and accompanying devel packages.

Optional: libSDL_ttf and accompanying devel package.

Most Linux distributions come pre-installed with these libraries but
you will likely have to install the development versions yourself.

For more information about the SDL libraries, please visit
http://www.libsdl.org/
http://www.libsdl.org/projects/SDL_mixer/
http://www.libsdl.org/projects/SDL_image/
http://www.libsdl.org/projects/SDL_ttf/

+--------------+
| Installation |
+--------------+

See COMPILING.

+------+
| Keys |
+------+

Default PC controls :-

    Enter               Confirm
    Cursors             Move
    Cursors + RShift    Shoot
    R                   Restart
    Escape              Exit

All keys are reconfigurable and are listed in Options. Digital and
analogue joysticks are also supported and it's possible to mix input
devices.

+-----------------+
| Pointer Support |
+-----------------+

The game can be controlled entirely with a pointing device (mouse or
stylus). By default the menu items throughout the game can be clicked,
and to activate the in-game control pad and toolbar you should simply
click anywhere on the screen. To shoot use either of these methods :-

* Drag from Robbo to a direction to shoot once and remain in walk mode.
* Click Robbo to toggle shoot mode on, press a direction to shoot one
  or more bullets and then click Robbo again to toggle shoot mode off.

+---------------+
| Music Support |
+---------------+

Now GNU Robbo supports playing music.

The way it does it, it plays ogg files during the game, if you go back
to the main screen, music stops, and it is resumed when you get back.
If music is on, volume keys control music volume, if it is disabled
they control sound fx volume.

If you want to have music, there are some things you should do:

First of all, music support must have been compiled in (the default is
to enable music support - see COMPILING), then find some music in ogg
format, you want to be played in the game. Place the ogg files in GNU
Robbo's data directory within the music folder and you should have
music playing.

+----------------+
| Level Designer |
+----------------+

Now GNU Robbo includes a level designer.

The level designer can be activated in-game via the Toggle Designer
control (PC default F9). Primarily it is mouse driven and there are a
set of icons top-right that control map scrolling, but you can also
scroll using the up, down, left and right controls. Additionally the
previous level, next level, previous pack and next pack controls
function as expected.

The most effective way to use the designer is to use a high resolution
video mode with a maximised viewport, or by forcing the smaller 16x16
tile set to utilise lower resolutions. Try any of the following:

    gnurobbo -800x600 -vpx -ts 16
    gnurobbo -1152x864 -vpx
    gnurobbo -1280x1024 -vpx

It has pretty basic functionality:
    * User can edit existing levels or add new levels to existing
      level sets
    * User can create/edit levels in two sizes - 16x31 and 32x31
    * User can add GNU Robbo specific objects such as rotating laser /
      blaster guns, shooting vertical birds, moving and rotating guns
    * User can add shots that will be shot at level start

It has some limitations:
    * User cannot create new level sets
    * User cannot edit the level author information
    * User cannot edit the level colors
    * User cannot edit the level set name

Currently the above limitations can be overcome by manually creating
empty level files outside of the game, and by editing them with your
favourite text editor.

Whether or not the user has permission to write to the packaged level
files depends on the target platform and the installation location,
but the level file "MyLevels" is always maintained to be writable.

Designer Usage Example
----------------------
Whilst in-game, press the Toggle Designer control (PC default F9).
Now the user can edit the existing level or add a new one at the end
of the level set.

If the user wants to add a new level, they should double-click on the
"clean page" icon.

If the user wants to clear the level, they should double-click on the
"eraser" icon.

If the user clicks certain icons once (such as clear/insert) then a
hint is displayed that explains that the user should click again to
trigger the action (nothing will be committed until selecting "save").

To test the level, the user should save it first and then exit the
designer by using either the keyboard (PC default ESC) or by clicking
the "X" in the top-right upper corner (black cross on red background).

The icons on purple backgrounds are control icons, and they are:
    * Gun type (laser/blaster/regular)
    * Guns movable, rotable or movable and rotable
    * Direction - this controls direction of bears, birds, shots, and
      gun movement
    * Eraser (double-click) - clears current level
    * New level (double-click) - clears level data and triggers that
      the level will be appended to the level set
    * Save level (double-click)
    * Reload level
    * Level size 16x31 or 32x31

+----------------------+
| Command Line Options |
+----------------------+

GNU Robbo version 0.xx
Usage: gnurobbo [options]

Options:
    -XRESxYRES    e.g. -800x480
    -ts n         Tile size 16 or 32
    -vpx          Maximise the viewport
    -f            Run the game fullscreen

The tile size of 16 is used in video modes less than 480x480.
On suitably large video modes, maximising the viewport will enable you
to see more of the level but this is considered cheating and is only
meant for level design and testing. If you are interested in
designing levels then please see the Level Designer section above.


Homepage of this game is available at: http://gnurobbo.sourceforge.net

-- The GNU Robbo Team

