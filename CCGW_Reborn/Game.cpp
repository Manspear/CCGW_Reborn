 #include "Game.h"
#include "BinaryReader.h"
#include "Mole\MoleReader.h"

struct ScreenVertex 
{
	float x, y, s, t;
};

void Game::createScreenQuad() 
{
	glGenBuffers(1, &testScreen);
	ScreenVertex vertexData[6];
	vertexData[0].x = -1.0; vertexData[0].y = -1.0; vertexData[0].s = 0.0; vertexData[0].t = 0.0;
	vertexData[1].x = -1.0; vertexData[1].y = 1.0; vertexData[1].s = 0.0; vertexData[1].t = 1.0;
	vertexData[2].x = 1.0; vertexData[2].y = -1.0; vertexData[2].s = 1.0; vertexData[2].t = 0.0;
	vertexData[3].x = 1.0; vertexData[3].y = -1.0; vertexData[3].s = 1.0; vertexData[3].t = 0.0;
	vertexData[4].x = 1.0; vertexData[4].y = 1.0; vertexData[4].s = 1.0; vertexData[4].t = 1.0;
	vertexData[5].x = -1.0; vertexData[5].y = 1.0; vertexData[5].s = 0.0; vertexData[5].t = 1.0;
	glBindBuffer(GL_ARRAY_BUFFER, testScreen);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Game::Game() /*mCamera(45.0f, (float)gWidth/gHeight, 0.5, 50), mPlayer(&mAssets)*/
{
	mBabyCount = 25;
	pActionState = nullptr;
	mDelayCleared = 2.0f;
	mCounter = 0;
	tactical = false;
	
	createScreenQuad();


	data.pAssets = new Assets();
	data.pCamera = new Camera( 45.0f, (float)gWidth/gHeight, 0.5f, 50.0f );
	data.pDeferredProgram = new DeferredProgram("deferred.vertex", "deferred.pixel", "deferred.geometry");
	data.pForwardProgram = new ForwardProgram("forward.vertex", "forward.pixel", " ");
	data.pBillboardProgram = new BillboardProgram("billboard.vertex", "billboard.pixel", "billboard.geometry");
	data.pShader2 = new ForwardProgram("molevertices.vertex", "molevertices.pixel", "molevertices.geometry");
	data.pEmission = new Emission(&data, 10000);
	data.pPlayer = new Player(&data);
	data.boxScale = 2;
	data.pScore = 0;
	data.pGold = 5;

	//tempMesh* playerModel = data.pAssets->load<tempMesh>( "Models/highreztear.obj" );
	Mesh* playerModel2 = data.pAssets->load<Mesh>("Models/testBox2.mole");
	tempMesh* playerModel = data.pAssets->load<tempMesh>("Models/box2.obj");	  
	Texture* groundTexture = data.pAssets->load<Texture>( "Models/ground.png" );
	Texture* playerTexture = data.pAssets->load<Texture>( "Models/cube.png" );
	Texture* specMap = data.pAssets->load<Texture>("Models/specMap.png");
	Texture* normalMap = data.pAssets->load<Texture>("Models/tegelNormal.png");

	data.pGrid = new Grid(36, 100, playerModel);

	sNode start = { 0, 0 };
	sNode end = { 2, 2 };
	mpPath = new sNode[18*100];
	mTargets = 0;

	data.pGrid->findPath( start, end, mpPath, &mTargets );

	data.pPlayer->load( playerModel2, playerTexture, specMap, normalMap);
	mGround.load(data.pAssets->load<tempMesh>("Models/plane.obj"), groundTexture, specMap, nullptr);
	//mGround.load(data.pAssets->load<tempMesh>("Models/groundPlane.mole"), groundTexture, specMap, nullptr);
	mActionMarker.load(data.pAssets->load<tempMesh>("Models/marker.obj"), data.pAssets->load<Texture>("Models/pns.png"), specMap, nullptr);
	//mActionMarker.load(data.pAssets->load<tempMesh>("Models/Marker.mole"), data.pAssets->load<Texture>("Models/pns.png"), specMap, nullptr);
	mTacticalMarker.load(playerModel, groundTexture, specMap, nullptr);
	mTacticalMarker.setScale(data.boxScale);
	mTowerModel.load(playerModel, groundTexture, nullptr, nullptr);
	mTowerModel.setScale(data.boxScale);

	data.mMolebats = 15;
	data.pMolebats = new Molebat[data.mMolebats];
	for( int i=0; i<data.mMolebats; i++ )
	{
		data.pMolebats[i].load( playerModel, playerTexture, specMap, normalMap );
		data.pMolebats[i].setGameData( &data );
	}

	data.mMoleratmen = 50;
	data.pMoleratmen = new Moleratman[data.mMoleratmen];
	for (int i = 0; i < data.mMoleratmen; i++) {
		data.pMoleratmen[i].load(playerModel, playerTexture, specMap, normalMap);
		data.pMoleratmen[i].pGameData = &data;
	}

	pWaveSpawner = new WaveSpawner( &data );

	Sound* sound = data.pAssets->load<Sound>( "Sounds/chant.wav" );
	if( !sound )
	{
		const char* error = Mix_GetError();
		std::cout << "Mixer error: " << error << std::endl;
	}
	else
 		sound->play();
}

Game::~Game() {
	delete data.pDeferredProgram;
	delete data.pForwardProgram;
	delete data.pBillboardProgram;
	delete data.pPlayer;
	delete data.pCamera;
	delete data.pEmission;
	delete data.pGrid;
	//delete data.pMenuProgram;
	delete pActionState;
	for (int i = 0; i < data.mpTowers.size(); i++) {
		delete data.mpTowers[i];
	}
	delete[] mpPath;
	delete pWaveSpawner;
	delete[] data.pMoleratmen;
	delete[] data.pMolebats;

	data.pAssets->unload();
	delete data.pAssets;
}
int a = 0;
State Game::run(Input* inputs, const float &dt, bool menuActive) 
{
	if(menuActive)
		update(inputs, dt);
	render();
	State result = GAME_PLAYING;
	if (!data.pPlayer->isAlive())
		result = GAME_LOST;
	if (pWaveSpawner->hasWon())
		result = GAME_WON;
	return result;
}

 void Game::tacticalRun(Input* inputs, const float &dt, bool menuActive) 
 {
	 if (menuActive) {
		 if (data.pCamera->getPosition().y < 18) {
			 glm::vec3 dPosition = (glm::vec3(0, 20, 0) - (data.pCamera->getPosition() - glm::vec3(0, -1, 0))) * (5 * dt);
			 this->data.pCamera->follow(data.pCamera->getPosition() + dPosition - glm::vec3(0, 1, 0), { 0,-1,0 }, 1, { 0,0,-1 });
		 }
		 else {
			 data.pCamera->tacticalMovement(data.pPlayer->tacticalUpdate(inputs, dt, data), 20);
			 if (mTacticalMarker.update(inputs, data))
				 buildTowers();
		 }
	 }
	 if (inputs->keyPressed(SDLK_t))
	 {
		 inputs->setMouseLock(true);
		 tactical = false;
		 pWaveSpawner->spawn();
	 }
	render();
 }

void Game::render()
{
	data.pDeferredProgram->use();
	data.pCamera->updateUniforms( data.pDeferredProgram->getViewPerspectiveLocation(), data.pDeferredProgram->getCameraPositionLocation() );
	data.pPlayer->render(data.pDeferredProgram->getProgramID(), data.pCamera->getView());
	mGround.render( data.pDeferredProgram->getProgramID() );
	for( int i=0; i<data.mMoleratmen; i++ )
		if( data.pMoleratmen[i].getAlive() )
			data.pMoleratmen[i].render( data.pDeferredProgram->getProgramID() );
	
	for( int i=0; i<data.mMolebats; i++ )
		if( data.pMolebats[i].getAlive() )
			data.pMolebats[i].render( data.pDeferredProgram->getProgramID() );

	for (int i = 0; i < data.mpTowers.size(); i++) {
		data.mpTowers[i]->render(data.pDeferredProgram->getProgramID());
	}
	if(data.pCamera->getPosition().y < 15)
		mActionMarker.render(data.pDeferredProgram->getProgramID());
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mTacticalMarker.render(data.pDeferredProgram->getProgramID());
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	data.pBillboardProgram->use();
	data.pBillboardProgram->begin( data.pCamera );

	data.pEmission->draw();
	data.pBillboardProgram->end();
	data.pBillboardProgram->unUse();
	data.pDeferredProgram->unUse();

	/*data.pShader2->use();
	data.pPlayer->render2(data.pShader2->getProgramID());
	data.pShader2->unUse();*/

	drawOnScreenQuad();	
}

void Game::update(Input* inputs, float dt) 
{
	data.pPlayer->update(inputs, dt);
	data.pEmission->update(dt);
	data.pCamera->follow(data.pPlayer->getPosition(), data.pPlayer->getLookAt(), 5, {0,1,0});
	
	bool waveDone = true;
	for( int i=0; i<data.mMoleratmen; i++ )
		if (data.pMoleratmen[i].getAlive())
		{
			data.pMoleratmen[i].update(dt);
			waveDone = false;
		}
	for (int i = 0; i<data.mMolebats; i++)
		if (data.pMolebats[i].getAlive())
		{
			data.pMolebats[i].update(dt);
			waveDone = false;
		}
	if (waveDone)
	{
		mCounter += dt;
		if (mCounter > mDelayCleared) 
		{
			inputs->setMouseLock(false);
			tactical = true;
			mCounter = 0;
		}
	}
	
	for (int i = 0; i < data.mpTowers.size(); i++)
	{
		data.mpTowers[i]->update(&data, dt);
	}

	mActionMarker.update(data.pPlayer);
	
	// NOTE: Debug
	float x = (float)( rand() % 100 - 50 );
	float z = (float)( rand() % 100 - 50 );
	glm::vec3 v = glm::normalize( glm::vec3( x, 50.0f, z ) ) * 0.25f;

	pWaveSpawner->update( dt );

	if( inputs->keyPressed( SDLK_k ) )
		pWaveSpawner->spawn();

	if (!data.pPlayer->isAlive())
		int a = 0;
}


void Game::buildTowers() {
	std::vector<glm::vec2> tempVec = mTacticalMarker.getMarkedTiles();
	for (int i = 0; i < tempVec.size(); i++) {
		data.mpTowers.push_back(new Tower(&data, glm::vec3(tempVec[i].x, 1, tempVec[i].y), mTowerModel, data.boxScale));
		glm::vec3 ppos = data.pPlayer->getPosition();
		if (!data.pPlayer->checkMove(ppos))
		{
			data.pPlayer->setPosition({ ppos.x, 5, ppos.z });
		}
	}
	mTacticalMarker.resetMarkedTiles();
}

void Game::drawOnScreenQuad()
{
	data.pForwardProgram->use();
	data.pDeferredProgram->enableTextures(data.pForwardProgram->getProgramID());
	//data.pShader2->enableTextures(data.pForwardProgram->getProgramID());

	GLuint cameraPos = glGetUniformLocation(data.pForwardProgram->getProgramID(), "cameraPos");
	glUniform3fv(cameraPos, 1, &data.pCamera->getPosition()[0]);
	GLuint inverseViewPerspective = glGetUniformLocation(data.pForwardProgram->getProgramID(), "invViewPerspective");
	glUniformMatrix4fv(inverseViewPerspective, 1, GL_FALSE, &glm::inverse(data.pCamera->getViewPerspective())[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, testScreen);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*)offsetof(ScreenVertex, x));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*)offsetof(ScreenVertex, s));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	data.pDeferredProgram->disableTextures();
	//data.pShader2->disableTextures();
	data.pForwardProgram->unUse();
}