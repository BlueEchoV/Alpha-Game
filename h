[33mcommit a036c2f84a4c32aa799576d62f440b317e8a9a23[m[33m ([m[1;36mHEAD[m[33m -> [m[1;32mmaster[m[33m)[m
Author: Michael <michaelepettric@gmail.com>
Date:   Mon Jan 13 15:55:03 2025 -0800

    Finished mp_render_copy_ex function and tested it throughly. I added the debug information to the draw debug info function for visualizations.

[33mcommit aecb4eaa29229715b5c8f1f3d1239fb1725bfe42[m[33m ([m[1;31morigin/master[m[33m)[m
Author: Michael <michaelepettric@gmail.com>
Date:   Mon Jan 13 15:24:56 2025 -0800

    Implemented a swap function and implemented the flip parameter for sdl_render_copy_ex with debug code to test it.

[33mcommit 1eed035a4c582ef6982cc838159545631202e578[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sun Jan 12 18:06:24 2025 -0800

    Implemented and tested operator overload for subtracting two vectors together.

[33mcommit 549e2accdeeb3bd1d8733a08f282164e76f79593[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sun Jan 12 17:53:31 2025 -0800

    Improved the way in which fonts are initialized and selected from the unordered map.

[33mcommit a65419424d9c85510a50813eaaf0a0a79c79989c[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sun Jan 12 17:44:10 2025 -0800

    Implemented a way to speed up the character using the shift key.

[33mcommit 7aa787f21f653f7bc7d8da7840a967f8402bb600[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sat Jan 11 14:40:10 2025 -0800

    Fixed some minor issues with the 2d rotation visualization. It is now complete.

[33mcommit 70e5b80d19542334215406d2db2c09355ea079e2[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sat Jan 11 14:32:37 2025 -0800

    Refactored 2d rotation matrix visualization code and fixed some bugs with that code, and removed some areas where window width and height was being initialized and causing issues.

[33mcommit 87382b87837db9c167b082a9948711a7aea18b86[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sat Jan 11 13:24:02 2025 -0800

    Fixed bugs regarding the rectangle visualization code. It's no longer stuttering.

[33mcommit 77dc17a1ac24ad157defdaca952b2a632543359a[m
Author: Michael <michaelepettric@gmail.com>
Date:   Fri Jan 10 00:17:37 2025 -0800

    Created and added an image for an arrow for the upcoming weapon system.

[33mcommit 9e7b7c2c275a5af1033f0ae6882fc4afdff4fb97[m
Author: Michael <michaelepettric@gmail.com>
Date:   Fri Jan 10 00:09:04 2025 -0800

    Added a image of a top down view of the player for testing.

[33mcommit 251207c91985fd3c84d94a9a6729a7663c0b9b60[m
Author: Michael <michaelepettric@gmail.com>
Date:   Fri Jan 10 00:02:39 2025 -0800

    Added a rectangle to visualization the 2D matrix transformations that is going to be done by mp_render_copy_ex.

[33mcommit 6e1bf7e674271563778c85d046b50243bf990080[m
Author: Michael <michaelepettric@gmail.com>
Date:   Thu Jan 9 15:34:57 2025 -0800

    Got perlin noise working properly.

[33mcommit 520c98610a0811c881256def12cba36168be2957[m
Author: Michael <michaelepettric@gmail.com>
Date:   Thu Jan 9 00:14:23 2025 -0800

    Added Sean Barret's perlin noise algorithm.

[33mcommit 95da056708d7b5efc4a104fbca566628bd3e4348[m
Author: Michael <michaelepettric@gmail.com>
Date:   Thu Jan 9 00:06:11 2025 -0800

    Fixed a bug with the water tile.

[33mcommit eab1403057e58509dc32b4f1beee3ad1b030f1e3[m
Author: Michael <michaelepettric@gmail.com>
Date:   Wed Jan 8 01:36:27 2025 -0800

    Changed to using namespaces for globals. Enums are being used in the images unordered maps as the identifier. Got texture tiles drawing and tiles being drawn based off the player. I created a very simple hash for generating tiles in a predetermined way just so I could walk around in the world a little. Also, I fixed some bugs associated with that and refactored some other bits of the code.

[33mcommit 5d349cff3bf24a47de7cc75e3fb09f2e4888fb0f[m
Author: Michael <michaelepettric@gmail.com>
Date:   Wed Jan 8 00:52:32 2025 -0800

    Got the baseline implementation of the tilemap functioning. I'm now drawing tiles around the player.

[33mcommit 1bd70aac9410a514c3994badc9f9b81eaa092775[m
Author: Michael <michaelepettric@gmail.com>
Date:   Wed Jan 8 00:40:45 2025 -0800

    Adding cleaning up resources logic. Added namespaces for the globals. Began work on the tiles.

[33mcommit a064b4f181a7e3b2c0c89418139b8e1c42d96888[m
Author: Michael <michaelepettric@gmail.com>
Date:   Tue Jan 7 15:40:13 2025 -0800

    Improved render function. Added a way to load images and fonts without crashing the program if the fonts don't load properly. Did some misc refactoring.

[33mcommit ff71ff1fde851744f854dfadef346b729623d757[m
Author: Michael <michaelepettric@gmail.com>
Date:   Mon Jan 6 15:22:26 2025 -0800

    Lots of changes. Sorted out camera space, world space, and I am now drawing everything around the player.

[33mcommit 51ac66485391bc4ba5c5c3a62e0ee5f9db6a830f[m
Author: Michael <michaelepettric@gmail.com>
Date:   Tue Dec 3 16:34:12 2024 -0800

    More work on the renderer and improving the debugging images.

[33mcommit a0505c4b6d01563ac81b589d67bf7b790a665f28[m
Author: Michael <michaelepettric@gmail.com>
Date:   Mon Nov 11 15:51:19 2024 -0800

    Tremendous work on the debugging images, got a string function working and debugged it entirely, and refactored.

[33mcommit 9d9af5b2d9fd7fc17a90ab596646139aa2e8745d[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sat Nov 9 20:19:46 2024 -0800

    More work on the renderer and debug images.

[33mcommit a96bbf5aea44cb70df0f125641f3258d6eeea6ea[m
Author: Michael <michaelepettric@gmail.com>
Date:   Wed Oct 30 15:43:26 2024 -0700

    More work on the library and I got debug images displaying properly with the ability to toggle them.

[33mcommit 7a39d85b82dc532b2e90e205a7f978422ae6ee01[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sun Oct 27 18:52:27 2024 -0700

    Got the texture loading properly. Confirmed the texture functions all worked well. And, I got input working and a sun is being displayed and moved around the screen.

[33mcommit e9b2c511deb2f234f9f8c74f209e9d53a3aec6ea[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sat Oct 26 17:37:01 2024 -0700

    Got a texture loading and displaying on the screen.

[33mcommit 4ad9a9100d5f7686a781a1f4367fe686c73d027c[m
Author: Michael <michaelepettric@gmail.com>
Date:   Tue Oct 22 17:48:36 2024 -0700

    More work on the renderer.

[33mcommit e93a0b351c4e01a2620b08c505f6d3cf176fa3f9[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sat Oct 19 22:55:38 2024 -0700

    More work on implementing the renderer.

[33mcommit 15169151b871cbbf8bb8bb690bb214dd728920d7[m
Author: Michael <michaelepettric@gmail.com>
Date:   Tue Oct 15 15:27:41 2024 -0700

    Working more on my variation of the SDL library. Primarily focused on textures and getting textures working properly.

[33mcommit e9c9af8d88b06c3bd33795994468b401cf662f25[m
Author: Michael <michaelepettric@gmail.com>
Date:   Mon Oct 14 21:35:34 2024 -0700

    Work on replicating the sdl api. Texture loading, rendercopy, renderpresent, and more.

[33mcommit b211f2d7a81c4db26f346b486982c21b85f275ab[m
Author: Michael <michaelepettric@gmail.com>
Date:   Wed Oct 9 15:25:38 2024 -0700

    More work on openGL and getting a texture on screen

[33mcommit be1f3f4c5255a16328e5ca42ab4f6c28ef35e35c[m
Author: Michael <michaelepettric@gmail.com>
Date:   Fri Sep 27 17:03:56 2024 -0700

    Refactored a lot. Added new files to improve clarity. Began implementing openGL and I got the functions loaded.

[33mcommit 8119e7eb3d9421e8a6b31ed026adaeb15224a505[m
Author: Michael <michaelepettric@gmail.com>
Date:   Sun Sep 22 22:40:06 2024 -0700

    Improved input and got the pfd filled out and working.

[33mcommit 2b6076c64a219150545df0f8e87f5803f18c848a[m
Author: Michael <michaelepettric@gmail.com>
Date:   Fri Sep 20 15:25:56 2024 -0700

    Removed Debug folder from repository and updated .gitignore

[33mcommit 97e9fdc40082c47f7627260d4bf2c188364dd62d[m
Author: Michael <michaelepettric@gmail.com>
Date:   Fri Sep 20 15:18:16 2024 -0700

    Initial commit
