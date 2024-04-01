# Forward-Plus-Renderer

This is an implementation of a forward renderer that leverages clustered light culling. The approach ensures the evaluation of only those lights that have an impact on a specific cluster during the primary rendering phase. Clusters are generated during the initialization process. The determination of intersecting clusters is conducted within a compute shader. Utilizing a texture derived from a depth pre-pass, a compute shader projects the screen space coordinates and depth onto a grid of clusters, thereby identifying which clusters are in an active state.
Subsequently, a compute shader compacts these active clusters to optimize the process. The identification of active clusters is based on their intersection with the bounding spheres of scene point lights. An additional array is constructed to delineate which lights are active in each cluster. During the main rendering pass, the lighting calculation employs the projection of screen space coordinates and depth to cluster grid coordinates. This projection facilitates the retrieval of information pertaining to the active lights within the current cluster, ensuring an efficient and focused lighting evaluation.

##Current Features of the Renderer:

- Depth Pre-Pass: Reduces overdraw by rendering to depth buffer before the main pass, SSAO also uses this buffer
- OBJ Model Loading: Supports loading of OBJ format models
- Multiple dynamic Shadow Maps and Cubemaps: Multiple shadow casters including Point, Spot and Directional Lights are allowed at once
- Blinn-Phong and Physically Based Rendering (PBR) Shading
- Diffuse Irradiance and Specular Image-Based Lighting: Utilizes HDRi images for enhanced lighting
- Normal Mapping
- Multisampling
- Bloom (Bright Pixels are blurred with a Gaussian Blur)
- Screen Space Ambient Occlusion: Normals recreated from depth buffer using GLSL dFdy/dFdx, with some artifacts smoothed by box blurring
- Blending and Post-Processing

##Following features are under consideration right now:

- Screen Space Reflections
- Volumetric Lighting
- More Post-Processing Effects : Like proper HDR (High Dynamic Range) imaging, Tone Mapping, Chromatic Aberration, Vignette, and Depth of Field
- Gizmos for Object Manipulation
- Reflection Probe System: Capturing and interpolating reflection probes for dynamic environments based on thier coverage and size
- Cross-Platform Project Structure Using CMake
- UI Editor Integration

## Dependencies and External Libraries:
* **Glfw:**
	- For creating the window context and handling window and user input events
* **Glad:**
	- To load platform specific OpenGL functions
* **OpenGL:**
	- Graphics API
* **stb_image.h:**
	- Used to decompress and load different image types from disk for use in OpenGL
* **Visual Studio:**
	- Project is set up to be developed with Microsoft Visual Studio.
	
##The following educational resources were drawn upon and I could not have created the renderer without them. Therefore I would like to express my sincere thanks to them:
Ángel Ortiz' Article about Clustered Shading: https://www.aortiz.me/2018/12/21/CG.html
Arijit Nandi's Article about SSAO in a Forward Renderer: https://betterprogramming.pub/depth-only-ssao-for-forward-renderers-1a3dcfa1873a
Joey de Vries' comprehensive OpenGL Tutorial Series: https://learnopengl.com/
The book Real-Time Rendering (Fourth Edition) by Tomas Akenine-Möller, Eric Haines, Naty Hoffman, Angelo Pesce, Michał Iwanicki, and Sébastien Hillaire: https://www.realtimerendering.com/

##Assets used:
Floor model by André Bray:
https://sketchfab.com/3d-models/sci-fi-floor-material-be0d40fbf5f44c388c94879801d3ab7b
License:
CC BY 4.0 DEED
Attribution 4.0 International
Hint:
No changes to the source material were made

Backpack model by Berk Gedik:
https://sketchfab.com/3d-models/survival-guitar-backpack-low-poly-799f8c4511f84fab8c3f12887f7e6b36
License:
CC BY 4.0 DEED
Attribution 4.0 International
Hint:
Source model was converted from FBX to OBJ file format

Sports hall environment map by hdri-haven:
https://hdri-haven.com/hdri/sports-hall
License:
CC0 - “No Rights Reserved”
It is the creator's choice to opt out of copyright and database protection