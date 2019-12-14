@author		Hamza Mian, Brandon Becker
@project	UDP Network-based real-time game

Demo: https://youtu.be/pRhGwifDCJE

Build & Run Instructions:
	- Select the proper platform toolset and Windows SDK for your system
	- Please only build in x64 and Release Configuration
	- Please run using visual studio debugger, launch server first and then clients

Notes:
	- The scene is layed out in the file assets/scenes/water.scene.xml
	- The server must run before the client, or else input will not register

	- Optimizations:
		- Server sends player rotation and position as a single vec3 (x, z position and y rotation)
		- Server sends projectile position and velocity as vec2 (y is always assumed to be 50.0)
		- Client sends no more than 10 bytes at a time (4 for request_id, and 6 for input)

Controls:

WS -		Move forward/backwards
AD -		Turn left/right
Space -		Shoot ball
R -		Respawn