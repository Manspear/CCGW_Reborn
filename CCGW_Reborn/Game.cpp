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
	pActionState = nullptr;
	mDelayCleared = 2.0f;
	mCounter = 0;
	tactical = false;
	createScreenQuad();
	data.pGame = this;
	data.mBabyCount = 25;
	data.pAssets = new Assets();
	data.pCamera = new Camera( 45.0f, (float)gWidth/gHeight, 0.5f, 150.0f );
	data.pDeferredProgram = new DeferredProgram("deferred.vertex", "deferred.pixel", "deferred.geometry");
	data.pForwardProgram = new ForwardProgram("forward.vertex", "forward.pixel", " ");
	data.pBillboardProgram = new BillboardProgram("billboard.vertex", "billboard.pixel", "billboard.geometry");
	data.pEmission = new Emission(&data, 1100);

	Texture* particleTexture = data.pAssets->load<Texture>( "Models/pns.png" );

	Emitter playerEmitter;
	data.pEmission->allocEmitter(&playerEmitter, 100);
	playerEmitter.load( particleTexture );

	data.pPlayer = new Player(&data, &playerEmitter);
	data.boxScale = 2;
	data.pScore = 0;
	data.pGold = 5;

	Model* playerModel = data.pAssets->load<Model>( "Models/klara.mole" );
	Model* boxModel = data.pAssets->load<Model>("Models/box.mole");
	Model* enemyModel = data.pAssets->load<Model>("Models/molerat.mole");
	Model* molebatModel = data.pAssets->load<Model>("Models/molebat.mole");
	Model* terrainModel = data.pAssets->load<Model>("Models/terrain.mole");
	Model* markerModel = data.pAssets->load<Model>("Models/marker.mole");
	Model* boundingBoxModel = data.pAssets->load<Model>( "Models/box.mole" );

	Enemy::pBoundingBoxModel = boundingBoxModel;

	data.pGrid = new Grid(16, 50);
	for( int i=0; i<16; i++ )
		data.pGrid->setTile( i, 0, TILE_BLOCKED );

	data.mTowers = 16*50;
	data.pTowers = new Tower[data.mTowers];

	Emitter towerEmitter;
	data.pEmission->allocEmitter( &towerEmitter, 1000 );
	towerEmitter.load( particleTexture );

	for( int i=0; i<data.mTowers; i++ )
	{
		int x = ( i % data.pGrid->getWidth() ) * data.boxScale;
		int y = ( i / data.pGrid->getWidth() ) * data.boxScale;
		data.pTowers[i].load( &data, glm::vec3( x, 1, y ), boxModel, enemyModel, &towerEmitter );
		data.pTowers[i].setAlive( false );
	}

	data.pPlayer->load( playerModel );
	data.pPlayer->setPosition( glm::vec3( 14.0f, 0.0f, 14.0f ) );
	mGround.load(terrainModel);
	mActionMarker.load(markerModel);
	mTacticalMarker.load(boxModel);
	mTacticalMarker.setScale( data.boxScale );
	mMoleratman.load( enemyModel );
	mMoleratman.setPosition( glm::vec3( 14.0f, 0.0f, 14.0f ) );

	mMolebat.load( molebatModel );
	mMolebat.setPosition( glm::vec3( 16.0f, 1.0f, 16.0f ) );
	mMolebat.setGameData( &data );

	data.mMolebats = 15;
	data.pMolebats = new Molebat[data.mMolebats];
	for( int i=0; i < data.mMolebats; i++ )
	{
		data.pMolebats[i].load(molebatModel);
		data.pMolebats[i].setGameData( &data );
	}

	data.mMoleratmen = 50;
	data.pMoleratmen = new Moleratman[data.mMoleratmen];
	for (int i = 0; i < data.mMoleratmen; i++) {
		data.pMoleratmen[i].load(enemyModel);
		data.pMoleratmen[i].pGameData = &data;
	}

	pWaveSpawner = new WaveSpawner( &data );
	pWaveSpawner->setPosition({ 14,0,-10 });
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
	delete pActionState;
	delete[] data.pTowers;
	delete pWaveSpawner;
	delete[] data.pMoleratmen;
	delete[] data.pMolebats;

	data.pAssets->unload();
	delete data.pAssets;
}

GameData * Game::getGameData()
{
	return &data;
}

void Game::restartGame()
{
	mCounter = 0;
	data.pScore = 0;
	data.pGold = 5;
	data.pPlayer->setAlive(true);
	for (int i = 0; i<16; i++)
		data.pGrid->setTile(i, 0, TILE_BLOCKED);
	for (int i = 0; i<data.mTowers; i++)
		data.pTowers[i].setAlive(false);
	delete data.pGrid;
	data.pGrid = new Grid(16, 50);
	
	for (int i = 0; i<16; i++)
		data.pGrid->setTile(i, 0, TILE_BLOCKED);
	
	for (int i = 0; i < data.mMolebats; i++)
		data.pMolebats[i].setAlive(false);
	
	for (int i = 0; i < data.mMoleratmen; i++) 
		data.pMoleratmen[i].setAlive(false);
	
	data.pPlayer->setPosition(glm::vec3(14.0f, 0.0f, 14.0f));
	pWaveSpawner->restart();
}

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
			 data.pCamera->tacticalMovement(data.pPlayer->tacticalUpdate(inputs, dt, data), 30);
			 mTacticalMarker.update( inputs, data );
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

	mMoleratman.render( data.pDeferredProgram->getProgramID());
	mMolebat.render( data.pDeferredProgram->getProgramID() );

	for( int i=0; i<data.mTowers; i++ )
		if( data.pTowers[i].getAlive() )
			data.pTowers[i].render( data.pDeferredProgram->getProgramID() );
	if(data.pCamera->getPosition().y < 15)
		mActionMarker.render(data.pDeferredProgram->getProgramID());
	else
	{
		mTacticalMarker.render(data.pDeferredProgram->getProgramID());
	}

	data.pBillboardProgram->use();
	data.pBillboardProgram->begin( data.pCamera );

	data.pEmission->draw();
	data.pBillboardProgram->end();
	data.pBillboardProgram->unUse();
	data.pDeferredProgram->unUse();

	drawOnScreenQuad();	
}

void Game::update(Input* inputs, float dt) 
{
	data.pPlayer->update(inputs, dt);
	data.pEmission->update(dt);
	data.pCamera->follow(data.pPlayer->getPosition(), data.pPlayer->getLookAt(), 5, {0,1,0});

	mMoleratman.update( 0.0f , &data);
	mMolebat.update( 0.0f );
	
	bool waveDone = true;
	for (int i = 0; i < data.mMoleratmen; i++)
	{
		if (data.pMoleratmen[i].getAlive())
		{
			data.pMoleratmen[i].update(dt, &data);
			waveDone = false;
		}
	}
	for (int i = 0; i < data.mMolebats; i++)
	{
		if (data.pMolebats[i].getAlive())
		{
			data.pMolebats[i].update(dt);
			waveDone = false;
		}
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
	else
		mCounter = 0;
	
	for( int i=0; i<data.mTowers; i++ )
		if( data.pTowers[i].getAlive() )
			data.pTowers[i].update( &data, dt );

	mActionMarker.update(data.pPlayer);

	pWaveSpawner->update( dt );

	if( inputs->keyPressed( SDLK_k ) )
		pWaveSpawner->spawn();

	if (!data.pPlayer->isAlive())
		int a = 0;
}

void Game::drawOnScreenQuad()
{
	data.pForwardProgram->use();
	data.pDeferredProgram->enableTextures(data.pForwardProgram->getProgramID());

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
	data.pForwardProgram->unUse();
}