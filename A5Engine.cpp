#include "A5Engine.hpp"

#include <CSCI441/objects.hpp>
#include <iostream>
#include <iomanip>

//*************************************************************************************
//
// Public Interface

A5Engine::A5Engine(int OPENGL_MAJOR_VERSION, int OPENGL_MINOR_VERSION,
                         int WINDOW_WIDTH, int WINDOW_HEIGHT, const char* WINDOW_TITLE)
         : CSCI441::OpenGLEngine(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) {

    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    modelMatrix = glm::mat4(1.0);
}

A5Engine::~A5Engine() {
    delete _arcballCam;
}

/// ****************************************************************************************************************
/// General Setup
/// ****************************************************************************************************************

void A5Engine::_setupGLFW() {
    CSCI441::OpenGLEngine::_setupGLFW();

    // set our callbacks
    glfwSetKeyCallback(_window, a3_keyboard_callback);
    glfwSetMouseButtonCallback(_window, a3_mouse_button_callback);
    glfwSetCursorPosCallback(_window, a3_cursor_callback);
}

void A5Engine::_setupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                    // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	            // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// clear the frame buffer to black
}

void A5Engine::_setupShaders() {

    //******************************************************************************************************************
    // Flat Shader Creation
    //******************************************************************************************************************

    // creating shader program
    _flatShaderProgram = new CSCI441::ShaderProgram( "shaders/flatShader.v.glsl", "shaders/flatShader.f.glsl" );

    // getting uniform locations
    _flatShaderProgramUniformLocations.modelMtx = _flatShaderProgram->getUniformLocation("modelMtx");
    _flatShaderProgramUniformLocations.viewMtx = _flatShaderProgram->getUniformLocation("viewMtx");
    _flatShaderProgramUniformLocations.projMtx = _flatShaderProgram->getUniformLocation("projMtx");
    _flatShaderProgramUniformLocations.matColor = _flatShaderProgram->getUniformLocation("matColor");

    // getting attrib locations
    _flatShaderProgramAttributeLocations.vPos = _flatShaderProgram->getAttributeLocation("vPosition");

    // attaching to shader
    _flatShaderProgram->useProgram();

    // setting up the model mtx for flat shader -- used for movement with hero
    glm::mat4 identity(1.0);
    glUniformMatrix4fv(_flatShaderProgramUniformLocations.modelMtx, 1, GL_FALSE, &identity[0][0]);

    // adding identity to transformation stack so the default will always be the identity
    transformationStack.emplace_back(identity);

    // needed to connect our 3D Object Library to our shader
    CSCI441::setVertexAttributeLocations(_flatShaderProgramAttributeLocations.vPos);

    //******************************************************************************************************************
    // Texture Shader Creation
    //******************************************************************************************************************
    _textureShaderProgram = new CSCI441::ShaderProgram("shaders/texShader.v.glsl", "shaders/texShader.f.glsl");

    // getting uniform locations
    _textureShaderUniformLocations.mvpMatrix = _textureShaderProgram->getUniformLocation("mvpMatrix");
    _textureShaderUniformLocations.textureMap = _textureShaderProgram->getUniformLocation("textureMap");
    _textureShaderUniformLocations.colorTint = _textureShaderProgram->getUniformLocation("colorTint");

    // getting attribute locations
    _textureShaderAttributeLocations.vPos = _textureShaderProgram->getAttributeLocation("vPos");
    _textureShaderAttributeLocations.vTexCoords = _textureShaderProgram->getAttributeLocation("vTexCoord");

    _textureShaderProgram->useProgram();

    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.textureMap, 0);

    CSCI441::setVertexAttributeLocations(_textureShaderAttributeLocations.vPos, -1, _textureShaderAttributeLocations.vTexCoords);
}

void A5Engine::_setupTextures(){
    _texHandles[TEXTURE_ID::TOP] = _loadAndRegisterTexture("textures/top.png");
    _texHandles[TEXTURE_ID::BOTTOM] = _loadAndRegisterTexture("textures/bottom.png");
    _texHandles[TEXTURE_ID::RIGHT] = _loadAndRegisterTexture("textures/right.png");
    _texHandles[TEXTURE_ID::LEFT] = _loadAndRegisterTexture("textures/left.png");
    _texHandles[TEXTURE_ID::FRONT] = _loadAndRegisterTexture("textures/front.png");
    _texHandles[TEXTURE_ID::BACK] = _loadAndRegisterTexture("textures/back.png");
    _texHandles[TEXTURE_ID::ENEMY] = _loadAndRegisterTexture("textures/enemy.png");
}

/// ********************************************************************************************************************
/// Buffer Setup
/// ********************************************************************************************************************

void A5Engine::_setupBuffers() {
    // ------------------------------------------------------------------------------------------------------
    // generate all of our VAO/VBO/IBO descriptors
    glGenVertexArrays( NUM_VAOS, _vaos );
    glGenBuffers( NUM_VAOS, _vbos );
    glGenBuffers( NUM_VAOS, _ibos );

    // create vao and vbo for grid
    _hero = new Hero(_flatShaderProgram,
                     _flatShaderProgramUniformLocations.modelMtx,
                     _flatShaderProgramUniformLocations.viewMtx,
                     _flatShaderProgramUniformLocations.projMtx,
                     _flatShaderProgramUniformLocations.matColor,
                     glm::vec3(0, 5, 0));

    _setupPlatform();

    _setupSkybox(_vaos[VAO_ID::SKYBOX], _vbos[VAO_ID::SKYBOX], _ibos[VAO_ID::SKYBOX], _numVAOPoints[VAO_ID::SKYBOX]);

    _createEnemies();
}


//**********************************************************************************************************************
// Skybox creation
//**********************************************************************************************************************

void A5Engine::_setupSkybox(GLuint vao, GLuint vbo, GLuint ibo, GLsizei &numVAOPoints) {
    VertexNormalTextured platformVertices[4] = {
            {-250.0f, 0.0f, -250.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f}, // 0 - BL
            {250.0f, 0.0f, -250.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f}, // 1 - BR
            {-250.0f, 0.0f, 250.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f}, // 2 - TL
            {250.0f, 0.0f, 250.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f}  // 3 - TR
    };

    GLushort platformIndices[4] = {0, 1, 2, 3};

    numVAOPoints = 4;

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData( GL_ARRAY_BUFFER, sizeof( platformVertices ), platformVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( _textureShaderAttributeLocations.vPos );
    glVertexAttribPointer( _textureShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) 0 );

    glEnableVertexAttribArray(_textureShaderAttributeLocations.vTexCoords);
    glVertexAttribPointer(_textureShaderAttributeLocations.vTexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) (sizeof(GLfloat) * 6));

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( platformIndices ), platformIndices, GL_STATIC_DRAW );
}


//**********************************************************************************************************************
// Platform creation and scene setup
//**********************************************************************************************************************

void A5Engine::_setupPlatform(){
    //******************************************************************
    // parameters to make up our grid size and spacing
    _groundSize = 100;
    const GLfloat GRID_WIDTH = 200;
    const GLfloat GRID_LENGTH = 200;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    //******************************************************************
    // draws a grid as our ground plane

    std::vector< glm::vec3 > points;
    for(GLfloat i = LEFT_END_POINT; i <= RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        points.emplace_back( glm::vec3(i, 0, BOTTOM_END_POINT) );
        points.emplace_back( glm::vec3(i, 0, TOP_END_POINT) );
    }
    for(GLfloat j = BOTTOM_END_POINT; j <= TOP_END_POINT; j += GRID_SPACING_LENGTH) {
        points.emplace_back( glm::vec3(LEFT_END_POINT, 0, j) );
        points.emplace_back( glm::vec3(RIGHT_END_POINT, 0, j) );
    }
    _gridSize = points.size();

    // generating and binding vao
    glGenVertexArrays(1, &_vaos[VAO_ID::PLATFORM]);
    glBindVertexArray(_vaos[VAO_ID::PLATFORM]);

    // generating vbod and sending data to it
    glGenBuffers(1, &_vbos[VAO_ID::PLATFORM]);
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[VAO_ID::PLATFORM]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_gridSize*3, &points[0], GL_STATIC_DRAW);

    // getting pointer for vPos
    glEnableVertexAttribArray(_flatShaderProgramAttributeLocations.vPos);
    glVertexAttribPointer(_flatShaderProgramAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void A5Engine::_setupScene() {
    // camera set up
    _arcballCam = new CSCI441::ArcballCam();
    _arcballCam->setPosition( glm::vec3( 60, 40, 30 ) ); // give the camera a scenic starting point
    _arcballCam->setTheta( -M_PI / 1.3f );                              // and a nice view
    _arcballCam->setPhi( M_PI / 2.8f );
    _arcballCam->setLookAtPoint(_hero->getLocation());
    _arcballCam->setRadius(-20.0f);
    _arcballCam->recomputeOrientation();
}

void A5Engine::_drawSkybox(){
    _textureShaderProgram->useProgram();

    //******************************************************************************************************************
    // Top and Bottom
    //******************************************************************************************************************

    // Drawing top
    glm::mat4 texModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 250.0f, 0.0f));
    glm::mat4 mvpMtx = projMatrix * viewMatrix * texModel;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.colorTint, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::TOP]);
    glBindVertexArray( _vaos[VAO_ID::SKYBOX] );
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Drawing bottom
    texModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -250.0f, 0.0f));
    texModel = glm::scale(texModel, glm::vec3(-1, 1, 1));

    mvpMtx = projMatrix * viewMatrix * texModel;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);

    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::BOTTOM]);
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    //******************************************************************************************************************
    // Front and Back
    //******************************************************************************************************************

    // Drawing front
    texModel = glm::rotate(glm::mat4(1.0f), float(M_PI), glm::vec3(1.0f, 1.0f, 0.0f));
    texModel = glm::translate(texModel, glm::vec3(0.0f, -250.0f, 0.0f));

    mvpMtx = projMatrix * viewMatrix * texModel;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);

    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::FRONT]);
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Drawing back
    texModel = glm::rotate(glm::mat4(1.0f), float(M_PI), glm::vec3(1.0f, 1.0f, 0.0f));
    texModel = glm::translate(texModel, glm::vec3(0.0f, 250.0f, 0.0f));
    texModel = glm::scale(texModel, glm::vec3(-1, 1, 1));

    mvpMtx = projMatrix * viewMatrix * texModel;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);

    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::BACK]);
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    //******************************************************************************************************************
    // Right and Left
    //******************************************************************************************************************

    // Drawing right
    texModel = glm::rotate(glm::mat4(1.0f), float(M_PI), glm::vec3(0.0f, 1.0f, 1.0f));
    texModel = glm::translate(texModel, glm::vec3(0.0f, -250.0f, 0.0f));

    mvpMtx = projMatrix * viewMatrix * texModel;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);

    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::RIGHT]);
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );

    // Drawing left
    texModel = glm::rotate(glm::mat4(1.0f), float(M_PI), glm::vec3(0.0f, 1.0f, 1.0f));
    texModel = glm::translate(texModel, glm::vec3(0.0f, 250.0f, 0.0f));
    texModel = glm::scale(texModel, glm::vec3(-1, 1, 1));


    mvpMtx = projMatrix * viewMatrix * texModel;
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);

    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::LEFT]);
    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::SKYBOX], GL_UNSIGNED_SHORT, (void*)0 );
}

/// ****************************************************************************************************************
/// Rendering/Drawing Functions
/// ****************************************************************************************************************

void A5Engine::_renderScene() {
    // attaching program handle
    _flatShaderProgram->useProgram();

    // updating viewMtx and projMtx
    glUniformMatrix4fv(_flatShaderProgramUniformLocations.viewMtx, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(_flatShaderProgramUniformLocations.projMtx, 1, GL_FALSE, &projMatrix[0][0]);

    // Drawing grid
    glBindVertexArray(_vaos[VAO_ID::PLATFORM]);
    glm::vec3 color(13.0/255.0, 77.0/255.0, 12.0/255.0);
    glUniform3fv(_flatShaderProgramUniformLocations.matColor, 1, &color[0]);

    if(isGrid){
        glDrawArrays(GL_LINES, 0, _gridSize);
    } else {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _gridSize);
    }

    // Function to draw hero
    if(isHeroAlive){
        _hero->draw(modelMatrix, viewMatrix, projMatrix, gunAngle);
    }

    // function to draw skybox
    _drawSkybox();

    //******************************************************************************************************************
    // drawing enemies
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::ENEMY]);
    for(int i = 0; i < _enemies.size(); i++){
        _enemies[i]->draw(_textureShaderProgram, _textureShaderUniformLocations.mvpMatrix, _textureShaderUniformLocations.colorTint, glm::mat4(1.0f), projMatrix * viewMatrix);
    }

    //******************************************************************************************************************
    // drawing bullet
    _flatShaderProgram->setProgramUniform(_flatShaderProgramUniformLocations.modelMtx, modelMatrix);
    for(int i = 0; i < _bullets.size(); i++){
        _bullets[i]->draw(_flatShaderProgram,
                          _flatShaderProgramUniformLocations.modelMtx,
                          _flatShaderProgramUniformLocations.viewMtx,
                          _flatShaderProgramUniformLocations.projMtx,
                          _flatShaderProgramUniformLocations.matColor,
                          modelMatrix, viewMatrix, projMatrix, heroRotAngle);
    }
}

void A5Engine::_updateScene() {
    // enemy update functions
    _moveEnemies();
    _inBoundsCheck();
    _collisionWithOtherEnemies();
    _enemyHeroCollision();
    _isEnemyInWorld();
    _recomputeEnemyDirection();

    // bullets update functions
    _moveBullets();
    _bulletHitsEnemy();
    _checkBulletBound();

    // hero update functions
    if(isHeroAlive){
        _checkHeroBounds();
        _isHeroInWorld();
    }


    gunAngle += M_PI/100.0f;
    if(gunAngle >= 2*M_PI){
        gunAngle = 0;
    }

    if(isHeroAlive){
        // turn right
        if( _keys[GLFW_KEY_D] ) {
            _hero->updateAngle(false);
        }
        // turn left
        if( _keys[GLFW_KEY_A] ) {
            _hero->updateAngle(true);
        }
        // move forward
        if( _keys[GLFW_KEY_W] ) {
            _hero->moveForward();
            _arcballCam->setLookAtPoint(_hero->getLocation());
            _arcballCam->recomputeOrientation();
        }
        // move backwards
        if( _keys[GLFW_KEY_S] ) {
            _hero->moveBackwards();
            _arcballCam->setLookAtPoint(_hero->getLocation());
            _arcballCam->recomputeOrientation();
        }
    }

    if(_keys[GLFW_KEY_Q] || _keys[GLFW_KEY_ESCAPE]){
        setWindowShouldClose();
    }
}

void A5Engine::run() {
    std::cout << "W - Move Hero Forward" << endl;
    std::cout << "S - Move Hero Backwards" << endl;
    std::cout << "A - Move Hero Left" << endl;
    std::cout << "D - Move Hero Right" << endl;
    std::cout << "C - Fire a Bullet" << endl;
    std::cout << "R - Respawn Hero" << endl;
    std::cout << "T - Increase Speed of Enemies" << endl;
    std::cout << "F - Spawn in N More Enemies" << endl;

    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(_window) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 100].
        projMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 700.0f );

        // set up our look at matrix to position our camera
        viewMatrix = _arcballCam->getViewMatrix();

        // draw everything to the window
        _renderScene();

        _updateScene();

        glfwSwapBuffers(_window);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

/// ****************************************************************************************************************
/// enemy creation functions
/// ****************************************************************************************************************

void A5Engine::_respawnHero(){
    isHeroAlive = true;
    _hero = new Hero(_flatShaderProgram,
                     _flatShaderProgramUniformLocations.modelMtx,
                     _flatShaderProgramUniformLocations.viewMtx,
                     _flatShaderProgramUniformLocations.projMtx,
                     _flatShaderProgramUniformLocations.matColor,
                     heroesDeathPos);
    _hero->setDirection(heroesDeathDir);
    _arcballCam->setLookAtPoint(_hero->getLocation());
    _arcballCam->recomputeOrientation();
}

void A5Engine::_checkHeroBounds(){
    if(_hero->getLocation().x > _groundSize + 2.0f * _hero->getRadius()){
        _hero->moveDown();
        _arcballCam->setLookAtPoint(_hero->getLocation());
        _arcballCam->recomputeOrientation();
    }
    if(_hero->getLocation().z > _groundSize + 2.0f*_hero->getRadius()){
        _hero->moveDown();
        _arcballCam->setLookAtPoint(_hero->getLocation());
        _arcballCam->recomputeOrientation();
    }
    if(_hero->getLocation().x < -_groundSize - 2.0f*_hero->getRadius()){
        _hero->moveDown();
        _arcballCam->setLookAtPoint(_hero->getLocation());
        _arcballCam->recomputeOrientation();
    }
    if(_hero->getLocation().z < -_groundSize - 2.0f*_hero->getRadius()){
        _hero->moveDown();
        _arcballCam->setLookAtPoint(_hero->getLocation());
        _arcballCam->recomputeOrientation();
    }

    if(_hero->getLocation().y <= -5){
        _hero->moveDown();
        _arcballCam->setLookAtPoint(_hero->getLocation());
        _arcballCam->recomputeOrientation();
    }
}

void A5Engine::_isHeroInWorld() {
    if(_hero->getLocation().y <= -250.0f){
        isHeroAlive = false;
        heroesDeathPos = glm::vec3(0.0f, _hero->getRadius() + 2, 0.0f);
        heroesDeathDir = glm::vec3(0.0f, 0.0f, 0.0f);

        delete _hero;
    }
}

/// ****************************************************************************************************************
/// enemy creation functions
/// ****************************************************************************************************************

void A5Engine::_createEnemies(){
    srand(time(0));
    fprintf(stdout, "How many enemies do you want to start off with? ");
    fflush(stdout);
    fscanf(stdin, "%d", &_numEnemies);

    // setting up the initial vector full of enemies
    RANGE_X = _groundSize * 2.0f;
    RANGE_Z = _groundSize * 2.0f;
    for(int i = 0; i < _numEnemies; i++){
        glm::vec3 randomPosition(rand()/(GLfloat)RAND_MAX * RANGE_X - RANGE_X/2.0f,
                                 0.0f,
                                 (i/(GLfloat)_numEnemies) * RANGE_Z - RANGE_Z/2.0f);
        glm::vec3 randomDirection(_hero->getLocation().x - randomPosition.x,
                                  0.0,
                                  _hero->getLocation().z - randomPosition.z);
        randomDirection = glm::normalize(randomDirection);
        GLfloat radius = 5;
        _enemies.push_back(new Enemy(randomPosition, randomDirection, radius));
    }
}

void A5Engine::_moveEnemies(){
    for(int i = 0; i < _enemies.size(); i++){
        _enemies[i]->moveForward();
    }
}

void A5Engine::_spawnMoreEnemies() {
    for(int i = 0; i < _numEnemies; i++){
        glm::vec3 randomPosition(rand()/(GLfloat)RAND_MAX * RANGE_X - RANGE_X/2.0f,
                                 0.0f,
                                 (i/(GLfloat)_numEnemies) * RANGE_Z - RANGE_Z/2.0f);
        glm::vec3 randomDirection(_hero->getLocation().x - randomPosition.x,
                                  0.0,
                                  _hero->getLocation().z - randomPosition.z);
        randomDirection = glm::normalize(randomDirection);
        GLfloat radius = 5;
        _enemies.push_back(new Enemy(randomPosition, randomDirection, radius));
    }
}

void A5Engine::_enemyDeletion(int index){
    // deleting enemy that fell off world
    GLfloat tempSpeed = _enemies[index]->getSpeed();
    delete _enemies[index];

    // creating a new enemy at a random spot
    glm::vec3 randomPosition(rand()/(GLfloat)RAND_MAX * RANGE_X - RANGE_X/2.0f,
                             0.0f,
                             (rand()/(GLfloat)RAND_MAX * RANGE_Z - RANGE_Z/2.0f));
    glm::vec3 randomDirection(_hero->getLocation().x - randomPosition.x,
                              0.0,
                              _hero->getLocation().z - randomPosition.z);

    randomDirection = glm::normalize(randomDirection);
    GLfloat radius = 5;
    _enemies[index] = new Enemy(randomPosition, randomDirection, radius);
    _enemies[index]->setSpeed(tempSpeed);
}

void A5Engine::_inBoundsCheck(){
    for(int i = 0; i < _enemies.size(); i++){
        if(_enemies[i]->getLocation().x > _groundSize + 2.0f*_enemies[i]->getRadius()){
            glm::vec3 newDir = _enemies[i]->getDirection();
            newDir.y = -9.81f;
            _enemies[i]->setDirection(newDir);
        }
        if(_enemies[i]->getLocation().z > _groundSize + 2.0f*_enemies[i]->getRadius()){
            glm::vec3 newDir = _enemies[i]->getDirection();
            newDir.y = -9.81f;
            _enemies[i]->setDirection(newDir);
        }
        if(_enemies[i]->getLocation().x < -_groundSize - 2.0f*_enemies[i]->getRadius()){
            glm::vec3 newDir = _enemies[i]->getDirection();
            newDir.y = -9.81f;
            _enemies[i]->setDirection(newDir);
        }
        if(_enemies[i]->getLocation().z < -_groundSize - 2.0f*_enemies[i]->getRadius()){
            glm::vec3 newDir = _enemies[i]->getDirection();
            newDir.y = -9.81f;
            _enemies[i]->setDirection(newDir);
        }
        if(_enemies[i]->getLocation().y <= -5){
            glm::vec3 newDir = _enemies[i]->getDirection();
            newDir.y = -9.81f;
            _enemies[i]->setDirection(newDir);
        }
    }
}

void A5Engine::_collisionWithOtherEnemies() {
    for(int i = 0; i < _enemies.size(); i++){
        for(int j = 0; j < _enemies.size(); j++){
            if(j == i) continue;

            GLfloat sumOfRadii = _enemies[i]->getRadius() + _enemies[j]->getRadius();

            GLfloat distBetweenMarbles = glm::distance(_enemies[i]->getLocation(), _enemies[j]->getLocation());

            if(distBetweenMarbles < sumOfRadii){
                _enemies[i]->moveBackwards();
                _enemies[j]->moveBackwards();

                glm::vec3 vectorBetweenMarbles = _enemies[i]->getLocation() - _enemies[j]->getLocation();
                vectorBetweenMarbles = glm::normalize(vectorBetweenMarbles);
                glm::vec3 colPoint = _enemies[i]->getLocation() + _enemies[i]->getRadius() * vectorBetweenMarbles;
                glm::vec3 normalFirstMarble = colPoint - _enemies[i]->getLocation();
                glm::vec3 firstMarbleNewDir = _enemies[i]->getDirection() - 2 * glm::dot(_enemies[i]->getDirection(), normalFirstMarble) * normalFirstMarble;
                _enemies[i]->setDirection(glm::normalize(firstMarbleNewDir));
                _enemies[i]->moveForward();

                vectorBetweenMarbles = -vectorBetweenMarbles;
                colPoint = _enemies[j]->getLocation() * _enemies[j]->getRadius() * vectorBetweenMarbles;
                glm::vec3 normalSecondMarble = colPoint - _enemies[j]->getLocation();
                glm::vec3 secondMarbleNewDir = _enemies[j]->getDirection() - 2 * glm::dot(_enemies[j]->getDirection(), normalSecondMarble) * normalSecondMarble;
                _enemies[j]->setDirection(glm::normalize(secondMarbleNewDir));
                _enemies[j]->moveForward();
            }
        }
    }
}

void A5Engine::_enemyHeroCollision(){
    for(int i = 0; i < _enemies.size(); i++){
        if(!isHeroAlive){
            break;
        }

        GLfloat sumOfRadii = _hero->getRadius() + _enemies[i]->getRadius();
        glm::vec3 correctHeroLoc = glm::vec3(_hero->getLocation().x, 0, _hero->getLocation().z);
        GLfloat distance = glm::distance(correctHeroLoc, _enemies[i]->getLocation());

        if(distance < sumOfRadii){
            _hero->decreaseLife();
            if(_hero->getLifesLeft() <= 0) {
                // storing death information for next game
                heroesDeathPos = _hero->getLocation();
                heroesDeathDir = _hero->getDirection();

                delete _hero;
                isHeroAlive = false;
                break;
            } else {
                _hero->setRadius(_hero->getRadius() - 0.5f);
                _enemyDeletion(i);
            }
        }
    }
}

void A5Engine::_isEnemyInWorld(){
    for(int i = 0; i < _enemies.size(); i++){
        if(_enemies[i]->getLocation().y < -250.0f){
            _enemyDeletion(i);
        }
    }
}

void A5Engine::_increaseEnemySpeed(){
    for(int i = 0; i < _enemies.size(); i++){
        _enemies[i]->doubleSpeed();
    }
}

void A5Engine::_recomputeEnemyDirection(){
    for(int i = 0; i < _enemies.size(); i++){
        glm::vec3 correctHeroLoc = glm::vec3(_hero->getLocation().x, 0, _hero->getLocation().z);
        glm::vec3 enemyToHeroVec = correctHeroLoc - _enemies[i]->getLocation();

        std::cout << fixed << setprecision(12);

        GLfloat angleBetweenVecs = acos(glm::clamp(glm::dot(glm::normalize(enemyToHeroVec), glm::normalize(_enemies[i]->getDirection())), -1.0f, 1.0f));

        if(angleBetweenVecs - 0.0f <= 0.001) continue;

        angleBetweenVecs = angleBetweenVecs / ANGLE_DIV;

        glm::vec3 rotAxis = glm::cross(enemyToHeroVec, _enemies[i]->getDirection());

        if(angleBetweenVecs > 0.0f){
            rotAxis = -rotAxis;
        }

        glm::mat4 rotMtx = glm::rotate(glm::mat4(1.0f), angleBetweenVecs, rotAxis);

        glm::vec3 newDir = rotMtx * glm::vec4(_enemies[i]->getDirection(), 0);

        _enemies[i]->setDirection(glm::normalize(newDir));
    }
}

/// ********************************************************************************************************************
/// Bullet Functions
/// ********************************************************************************************************************

void A5Engine::_createBullet(){
    _bullets.push_back(new Bullet(_hero->getLocation(), _hero->getDirection()));
}

void A5Engine::_moveBullets(){
    for(int i = 0; i < _bullets.size(); i++){
        _bullets[i]->moveForward();
    }
}

void A5Engine::_bulletHitsEnemy(){
    for(int i = 0; i < _bullets.size(); i++){
        for(int j = 0; j < _enemies.size(); j++){
            GLfloat sumOfRadii = _bullets[i]->getRadius() + _enemies[j]->getRadius();

            glm::vec3 correctBulletLoc = glm::vec3(_bullets[i]->getLocation().x, 0, _bullets[i]->getLocation().z);
            GLfloat distanceBetween = glm::distance(correctBulletLoc, _enemies[j]->getLocation());

            if(distanceBetween < sumOfRadii){
                _bullets.erase(_bullets.begin() + i);
                _enemies.erase(_enemies.begin() + j);
            }
        }
    }
}

void A5Engine::_checkBulletBound(){
    for(int i = 0; i < _bullets.size(); i++){
        if(_bullets[i]->getLocation().x > 250 || _bullets[i]->getLocation().z > 250){
            _bullets.erase(_bullets.begin() + i);
        }
        if(_bullets[i]->getLocation().x < -250 || _bullets[i]->getLocation().z < -250){
            _bullets.erase(_bullets.begin() + i);
        }
    }
}

/// ********************************************************************************************************************
/// Private Helper Functions
/// ********************************************************************************************************************

GLuint A5Engine::_loadAndRegisterTexture(const char* FILENAME) {
    // our handle to the GPU
    GLuint textureHandle = 0;

    // enable setting to prevent image from being upside down
    stbi_set_flip_vertically_on_load(true);

    // will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;
    // load image from file
    GLubyte* data = stbi_load( FILENAME, &imageWidth, &imageHeight, &imageChannels, 0);

    // if data was read from file
    if( data ) {
        const GLint STORAGE_TYPE = (imageChannels == 4 ? GL_RGBA : GL_RGB);
        glGenTextures(1, &textureHandle);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, STORAGE_TYPE, imageWidth, imageHeight, 0, STORAGE_TYPE, GL_UNSIGNED_BYTE, data);

        fprintf( stdout, "[INFO]: %s texture map read in with handle %d\n", FILENAME, textureHandle);

        // release image memory from CPU - it now lives on the GPU
        stbi_image_free(data);
    } else {
        // load failed
        fprintf( stderr, "[ERROR]: Could not load texture map \"%s\"\n", FILENAME );
    }

    // return generated texture handle
    return textureHandle;
}

/// ****************************************************************************************************************
/// Engine Cleanup
/// ****************************************************************************************************************

void A5Engine::_cleanupShaders() {
    // LOOK HERE #4: we're cleaning up our memory again!
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _flatShaderProgram;
    delete _textureShaderProgram;
}

void A5Engine::_cleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( NUM_VAOS, _vaos );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();
    glDeleteBuffers( NUM_VAOS, _vbos );

    fprintf( stdout, "[INFO]: ...deleting IBOs....\n" );
    glDeleteBuffers( NUM_VAOS, _ibos );
}

void A5Engine::_cleanupTextures() {
    fprintf( stdout, "[INFO]: ...deleting textures\n" );
    glDeleteTextures(NUM_TEXTURES, _texHandles);
}


/// ****************************************************************************************************************
/// Callbacks
/// ****************************************************************************************************************
void a3_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (A5Engine*) glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void a3_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (A5Engine*) glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void a3_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (A5Engine*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}

void A5Engine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN){
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

        if(action == GLFW_RELEASE && key == GLFW_KEY_C){
            if(isHeroAlive){
                _createBullet();
            }
        }
        if(action == GLFW_RELEASE && key == GLFW_KEY_R){
            if(!isHeroAlive){
                _respawnHero();
            }
        }

        if((action == GLFW_RELEASE || action == GLFW_REPEAT) && key == GLFW_KEY_T){
            _increaseEnemySpeed();
        }

        if((action == GLFW_RELEASE || action == GLFW_REPEAT) && key == GLFW_KEY_F){
            _spawnMoreEnemies();
        }
    }
}

void A5Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void A5Engine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        if (_keys[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS) {
            GLfloat diff = (currMousePosition.y - _mousePosition.y);
            if (diff > 0) {
                _arcballCam->moveBackward(1.0);
            } else {
                _arcballCam->moveForward(1.0);
            }
        } else {
            _arcballCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                (_mousePosition.y - currMousePosition.y) * 0.005f );
        }
    }

    // update the mouse position
    _mousePosition = currMousePosition;
}