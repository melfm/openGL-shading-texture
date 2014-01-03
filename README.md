OpenGL-Shading-Texture
======================

Sample example of advanced OpenGL calls for shading and lighting and texture.

Supported Functionality
======================
1. 3D scene built of 3D objects
2. 3D objects coloured, shaded, texture mapped and lit 3. Multiple viewpoints of the scene
4. Animated light source moving around the model
5. Complex model rendering
6. Pick objects in the scene and change texture


Further Improvements
======================
The current implementation uses display lists to store arrays of vertices. This certainly improves the performance, however using display lists generally takes more memory than immediate mode. This is because for every “glVertex3f()” call, the display list must provide storage for 3 values (these are usually 32-bit float values).
In a different scenario with a more complicated scene, the weakness with this approach is that when building complex models, we make many function calls to “glvertex3fv” to draw each model. We even add more calculations when adding texture and lighting.
This application has been tested on a high performance machine. Another approach could make use of vertex arrays facility, provided by OpenGL. This allows storing array data in the implementation, hence gives us a much faster processing. An experiment could be carried out to find out whether putting ver- tex arrays inside display list will improve the speed, perhaps on a less powerful machine to compare performance.

Resources
======================
Model Data All the data has been generated through the software “Blender”. Complex model data obtained from: http://www.3dmodelfree.com/3dmodel/list499-1.htm
Texture SOIL library used to read “JPG” data for textue mapping - http: //www.lonesock.net/soil.html
Textures obtained from the website http://www.flickr.com/search/?q= texture&l=cc&ss=0&ct=0&mt=all&w=all&adv=1

Running the program
======================
The program has been tested on XCode 4.6.3. The program can also be run from the command line, you need to build it and go to the build directory and run : ./ShadingTexture Further instructions on how to use the program will appear on the console
when the application is running.

Demo Video
======================
http://www.youtube.com/watch?v=bhCx7zJ-zuU