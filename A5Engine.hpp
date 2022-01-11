#ifndef LAB04_LAB04_ENGINE_HPP
#define LAB04_LAB04_ENGINE_HPP

#include <CSCI441/FreeCam.hpp>
#include <CSCI441/ModelLoader.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include <CSCI441/materials.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <CSCI441/ArcballCam.hpp>
#include "Enemy.h"
#include "Bullet.h"
#include "Hero.h"

class A5Engine : public CSCI441::OpenGLEngine {
public:
    A5Engine(int OPENGL_MAJOR_VERSION, int OPENGL_MINOR_VERSION,
                int WINDOW_WIDTH, int WINDOW_HEIGHT,
                const char* WINDOW_TITLE);
    ~A5Engine();

    void run() final;

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

private:
    /// ****************************************************************************************************************
    /// General Setup
    /// ****************************************************************************************************************

    void _setupGLFW() final;
    void _setupOpenGL() final;
    void _setupShaders() final;
    void _setupBuffers() final;
    void _setupTextures() final;
    void _setupScene() final;

    void _cleanupBuffers() final;
    void _cleanupShaders() final;
    void _cleanupTextures() final;

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene();
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();

    /// \desc tracks the number of different keys that can be present as determined by GLFW
    static const GLuint NUM_KEYS = GLFW_KEY_LAST;
    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with
    GLboolean _keys[NUM_KEYS];

    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;

    /// \desc camera
    CSCI441::ArcballCam *_arcballCam;

    /// \desc bool to switch between flat plane and grid
    bool isGrid = false;

    /// ****************************************************************************************************************
    /// Shader Information
    /// ****************************************************************************************************************

    /// \desc creating a shader program handle data member
    CSCI441::ShaderProgram* _flatShaderProgram = nullptr;

    /// \desc stores locations of our shader program uniforms
    struct FlatShaderProgramUniformLocations {
        /// \desc location to send the precomputed MVP matrix to and location of materialColor
        GLint modelMtx;
        GLint viewMtx;
        GLint projMtx;
        GLint matColor;
    } _flatShaderProgramUniformLocations;

    struct FlatShaderProgramAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
    } _flatShaderProgramAttributeLocations;

    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram* _textureShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct TextureShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc texture map location
        GLint textureMap;
        /// \desc color tint of object
        GLint colorTint;
    } _textureShaderUniformLocations;

    /// \desc stores the locations of all of our shader attributes
    struct TextureShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        /// \desc location of texture coordinates
        GLint vTexCoords;

    } _textureShaderAttributeLocations;


    /// ****************************************************************************************************************
    /// VAO Information
    /// ****************************************************************************************************************

    /// \desc total number of VAOs in our scene
    static constexpr GLuint NUM_VAOS = 4;
    /// \desc used to index through our VAO/VBO/IBO array to give named access
    enum VAO_ID {
        /// \desc the platform that represents our ground for everything to appear on
        PLATFORM = 0,
        /// \desc skybox quad
        SKYBOX = 3
    };

    /// \desc VAO for our objects
    GLuint _vaos[NUM_VAOS];
    /// \desc VBO for our objects
    GLuint _vbos[NUM_VAOS];
    /// \desc IBO for our objects
    GLuint _ibos[NUM_VAOS];
    /// \desc the number of points that make up our VAO
    GLsizei _numVAOPoints[NUM_VAOS];

    /// ****************************************************************************************************************
    /// functions for drawings
    /// ****************************************************************************************************************

    /// \desc setup the environment buffers
    void _setupPlatform();


    /// ****************************************************************************************************************
    /// functions for skybox creation
    /// ****************************************************************************************************************

    /// \desc struct to store information
    struct VertexNormalTextured {
        float x, y, z;
        float nx, ny, nz;
        float s, t;
    };

    /// \desc setup vao/vbo/ibo for specific quad
    void _setupSkybox(GLuint vao, GLuint vbo, GLuint ibo, GLsizei &numVAOPoints);

    /// \desc draw skybox
    void _drawSkybox();

    /// ****************************************************************************************************************
    /// variables for objects
    /// ****************************************************************************************************************

    /// \desc variables for grid
    GLuint _gridSize;

    /// \desc transformation stack
    vector<glm::mat4> transformationStack;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;

    /// ****************************************************************************************************************
    /// texture information
    /// ****************************************************************************************************************

    /// \desc total number of textures in our scene
    static constexpr GLuint NUM_TEXTURES = 7;
    /// \desc used to index through our texture array to give named access
    enum TEXTURE_ID {
        /// \desc top texture
        TOP = 0,
        /// \desc bottom texture
        BOTTOM = 1,
        /// \desc right texture
        RIGHT = 2,
        /// \desc left texture
        LEFT = 3,
        /// \desc font texture
        FRONT = 4,
        /// \desc back texture
        BACK = 5,
        /// \desc enemy texture
        ENEMY = 6
    };
    /// \desc texture handles for our textures
    GLuint _texHandles[NUM_TEXTURES];

    /// ****************************************************************************************************************
    /// functions for hero
    /// ****************************************************************************************************************

    /// \desc setting up the hero
    Hero *_hero;

    /// \desc variables for hero
    GLfloat heroRotAngle = 0;
    GLfloat gunAngle = 0;
    bool isHeroAlive = true;

    /// \desc variables to store death information of hero - if the user wants to play again
    glm::vec3 heroesDeathPos;
    glm::vec3 heroesDeathDir;

    /// \desc respawn hero
    void _respawnHero();

    /// \desc check if hero is in bounds of the world. If not, then have the hero fall down
    void _checkHeroBounds();

    /// \desc checks if hero is still in the world
    void _isHeroInWorld();

    /// ****************************************************************************************************************
    /// functions for enemies
    /// ****************************************************************************************************************

    /// \desc setting up the enemies
    void _createEnemies();

    /// \desc moving the enemies
    void _moveEnemies();

    /// \desc collision with other enemies
    void _collisionWithOtherEnemies();

    /// \desc bounds check for the enemies
    void _inBoundsCheck();

    /// \desc vector to store the particle system of enemies
    /// \note using this as a vector so more enemies can spawn as time goes
    std::vector<Enemy*> _enemies;

    /// \desc destroy an enemy and respawn a new enemy
    void _enemyDeletion(int index);

    /// \desc checks if an enemy has fallen through to the bottom of the skybox. if so, then delete it and respawn a new enemy
    void _isEnemyInWorld();

    /// \desc check if enemy hits the hero
    void _enemyHeroCollision();

    /// \desc increases speed of enemies
    void _increaseEnemySpeed();

    /// \desc spawns in N enemies
    void _spawnMoreEnemies();

    /// \desc change the direction of enemy to points toward hero
    void _recomputeEnemyDirection();


    /// \desc variable to store the number of enemies created
    GLint _numEnemies;

    /// \desc variable to store how much to move hero by
    const GLfloat ANGLE_DIV = 3.0f;

    /// \desc size of ground plant
    GLfloat _groundSize;

    /// \desc variables for enemy creation
    GLfloat RANGE_X;
    GLfloat RANGE_Z;

    /// ****************************************************************************************************************
    /// information for bullets
    /// ****************************************************************************************************************

    /// \desc create a new bullet
    void _createBullet();

    /// \desc move bullets
    void _moveBullets();

    /// \desc vector to store the bullets
    std::vector<Bullet*> _bullets;

    /// \desc see if bullet hits an enemy
    void _bulletHitsEnemy();

    /// \desc see if bullet leaves the skybox
    void _checkBulletBound();

    /// ****************************************************************************************************************
    /// helper functions
    /// ****************************************************************************************************************
    static GLuint _loadAndRegisterTexture(const char* FILENAME);
};

void a3_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void a3_cursor_callback(GLFWwindow *window, double x, double y);
void a3_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

#endif // LAB04_LAB04_ENGINE_HPP
