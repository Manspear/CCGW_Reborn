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

Game::Game() 
{
	mDelayCleared = 2.0f;
	mCounter = 0;
	tactical = false;
	createScreenQuad();
	data.pGame = this;
	data.mBabyCount = 25;
	data.pAssets = new Assets();
	data.pCamera = new Camera( 45.0f, (float)gWidth/gHeight, 0.5f, 150.0f );
	data.pDeferredProgram = new DeferredProgram("skinned.vertex", "skinned.pixel", "skinned.geometry");
	data.pDeferredProgramNonAni = new DeferredProgram("deferred.vertex", "deferred.pixel", "deferred.geometry");
	data.pForwardProgram = new ForwardProgram("forward.vertex", "forward.pixel", " ");
	data.pBillboardProgram = new BillboardProgram("billboard.vertex", "billboard.pixel", "billboard.geometry");
	data.pShadowProgram = new Shadow("shadow.vertex", "shadow.pixel");
	data.pEmission = new Emission(&data, 10000);

	Texture* particleTexture = data.pAssets->load<Texture>( "Models/pns.png" );
	Texture* bloodTexture = data.pAssets->load<Texture>("Models/blood.png");

	Emitter playerEmitter;
	data.pEmission->allocEmitter(&playerEmitter, 100);
	playerEmitter.load( particleTexture );

	Emitter enemyEmitter;
	data.pEmission->allocEmitter(&enemyEmitter, 600);
	enemyEmitter.load(bloodTexture);

	data.pPlayer = new Player(&data, &playerEmitter, &enemyEmitter);
	data.boxScale = 2;
	gStartGold = 66;
	data.pGold = gStartGold;//gold all waves.  600 gold by "end game"

	Model* playerModel = data.pAssets->load<Model>("Models/klara_lordag.mole");
	Model* boxModel = data.pAssets->load<Model>("Models/wallbox.mole");
	Model* moleratModel = data.pAssets->load<Model>("Models/molerat_lordag5.mole");
	Model* molebatModel = data.pAssets->load<Model>("Models/molebat_lordag2.mole");
#if RENDER_TERRAIN
	Model* terrainModel = data.pAssets->load<Model>("Models/terrain.mole");
#endif
	Model* boundingBoxModel = data.pAssets->load<Model>("Models/rotationCube3.mole");
	Model* babyModel = data.pAssets->load<Model>("Models/baby.mole");

	Model* towerModels[TOWER_MODELS] = {
		data.pAssets->load<Model>("Models/ballista_box.mole"),
		data.pAssets->load<Model>( "Models/ballista_lid.mole" ),
		data.pAssets->load<Model>("Models/ballista_crossbow.mole"),
		data.pAssets->load<Model>( "Models/ballista_small_cylinder.mole" ),
		data.pAssets->load<Model>( "Models/ballista_mid_cylinder.mole" ),
		data.pAssets->load<Model>( "Models/ballista_low_wheel.mole" ),
		data.pAssets->load<Model>( "Models/ballista_high_wheel.mole" )
	};

	data.mBabies = 25;
	data.pBabies = new GameObject[data.mBabies];

	float p = glm::pi<float>()*0.5f;
	glm::mat4 babyMat(cosf(p)*0.2f, 0, -sinf(p)*0.2f, 0,
		0, 0.2f, 0, 0,
		sinf(p)*0.2f, 0, cosf(p)*0.2f, 0,
		0, 0, 0, 1
	);

	Sound* babysound = data.pAssets->load<Sound>("Sounds/dead_baby.wav");

	for (int i = 0; i < data.mBabies; i++)
	{
		data.pBabies[i].loadSound(babysound);
		data.pBabies[i].load(babyModel);
		babyMat[3][0] = 6.5f*(i / 5);
		babyMat[3][1] = 0.3f;
		babyMat[3][2] = 100 + (i % 5);
		data.pBabies[i].setWorld(babyMat);
	}

	data.pGrid = new Grid(16, 48);
	for( int i=0; i<16; i++ )
		data.pGrid->setTile( i, 0, TILE_BLOCKED );

	data.mTowers = 16*48;
	data.pTowers = new Tower[data.mTowers];

	mpVisibleTowers = new Tower*[data.mTowers];
	mVisibleTowers = 0;

	Emitter towerEmitter;
	data.pEmission->allocEmitter( &towerEmitter, 5000 );
	towerEmitter.load( particleTexture );

	for( int i=0; i<data.mTowers; i++ )
	{
		int x = ( i % data.pGrid->getWidth() ) * data.boxScale;
		int y = ( i / data.pGrid->getWidth() ) * data.boxScale;
		data.pTowers[i].load( &data, glm::vec3( x, 1, y ), boxModel, towerModels, &towerEmitter );
		data.pTowers[i].setAlive( false );
	}


	data.pPlayer->load( playerModel );
	data.pPlayer->setPosition( glm::vec3( 14.0f, 0.0f, 14.0f ) );
	data.pPlayer->playAnimation(1, true, 1.0f);
#if RENDER_TERRAIN
	mGround.load(terrainModel);
#endif
	mTacticalMarker.load(boxModel);
	mTacticalMarker.setScale( data.boxScale );

	data.mMolebats = 500;
	Sound* sound = data.pAssets->load<Sound>("Sounds/monstersound.wav");
	data.pMolebats = new Molebat[data.mMolebats];
	for( int i=0; i < data.mMolebats; i++ )
	{
		data.pMolebats[i].load(molebatModel, &enemyEmitter);
		data.pMolebats[i].setGameData( &data );
		data.pMolebats[i].loadSound(sound);
		data.pMolebats[i].playAnimation(1, true, 2.0f );
	}

	data.mMoleratmen = 1000;
	data.pMoleratmen = new Moleratman[data.mMoleratmen];
	for (int i = 0; i < data.mMoleratmen; i++) {
		data.pMoleratmen[i].load(moleratModel, &enemyEmitter);
		data.pMoleratmen[i].pGameData = &data;
		data.pMoleratmen[i].loadSound(sound);
		data.pMoleratmen[i].playAnimation( 1, true, 2.0f );
		data.pMoleratmen[i].setScale(0.1f);
	}

	sound = data.pAssets->load<Sound>("Sounds/arrowfired.wav");
	data.pPlayer->mWeapon.loadSound(sound);

	data.pWavespawner = new WaveSpawner( &data );
	data.pWavespawner->setPosition({ 14,0,-10 });	

	string path;
	for (int i = 1; i < 26; i++) {
		path = "Sounds/wave_" + std::to_string(i);
		path.append(".wav");
		sound = data.pAssets->load<Sound>(path);
		data.pWavespawner->loadSound(sound, i - 1);
	}
	sound = data.pAssets->load<Sound>("Sounds/wave_completed.wav");
	data.pWavespawner->loadSound(sound, 25);
	sound = data.pAssets->load<Sound>("Sounds/not_enough_gold.wav");
	sound->setVolume(20);
	data.pWavespawner->loadSound(sound, 26);
	mDeltaTimer = 0.0f;
}

Game::~Game() {
	delete data.pDeferredProgram;
	delete data.pDeferredProgramNonAni;
	delete data.pForwardProgram;
	delete data.pBillboardProgram;
	delete data.pShadowProgram;
	delete data.pPlayer;
	delete data.pCamera;
	delete data.pEmission;
	delete data.pGrid;
	delete[] data.pTowers;
	delete data.pWavespawner;
	delete[] data.pMoleratmen;
	delete[] data.pMolebats;
	delete[] data.pBabies;
	delete[] mpVisibleTowers;

	data.pAssets->unload();
	delete data.pAssets;
}

GameData * Game::getGameData()
{
	return &data;
}

void Game::restartGame()
{
	mTacticalMarker.resetMarkedTiles(&data);
	mCounter = 0;
	data.pGold = gStartGold;
	data.pPlayer->setAlive(true);
	for (int i = 0; i<data.mTowers; i++)
		data.pTowers[i].setAlive(false);
	delete data.pGrid;
	data.pGrid = new Grid(16, 50);
	for (int i = 0; i<16; i++)
		data.pGrid->setTile(i, 0, TILE_BLOCKED);
	for (int i = 16; i < data.mTowers; i++)
		data.pGrid->setTile(i, 0, TILE_EMPTY);
	
	for (int i = 0; i < data.mMolebats; i++)
		data.pMolebats[i].setAlive(false);
	
	for (int i = 0; i < data.mMoleratmen; i++) 
		data.pMoleratmen[i].setAlive(false);
	
	
	data.pPlayer->setPosition(glm::vec3(14.0f, 0.0f, 14.0f));
	data.pWavespawner->restart();
}

State Game::run(Input* inputs, const float &dt, bool menuActive)
{
	if(menuActive)
		update(inputs, dt);
	State result = GAME_PLAYING;
	if (!data.pPlayer->isAlive())
		result = GAME_LOST;
	if (data.pWavespawner->hasWon())
		result = GAME_WON;
	render();
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

		 data.pCamera->updateFrustum();
		 data.pGrid->cull( data.pCamera->getFrustum(), data.pTowers, mpVisibleTowers, &mVisibleTowers );

		 for( int i=0; i<mVisibleTowers; i++ )
		 {
			 if( mpVisibleTowers[i]->getAlive() && mpVisibleTowers[i]->getHasBallista() )
				 mpVisibleTowers[i]->update( &data, dt );
		 }
	 }
	 if (inputs->keyPressed(SDLK_t))
	 {
		 inputs->setMouseLock(true);
		 mTacticalMarker.resetMarkedTiles(&data);
		 tactical = false;
		 data.pWavespawner->spawn();
	 }
	render();
 }

void Game::render()
{
	data.pShadowProgram->use();
	data.pShadowProgram->updateUniforms();

	GLuint worldLocation = data.pShadowProgram->getWorldLocation();
	GLuint animationLocation = data.pShadowProgram->getAnimationLocation();

	data.pPlayer->render(worldLocation, animationLocation);

	for (int i = 0; i < data.mMoleratmen; i++)
	{
		if (data.pMoleratmen[i].getAlive())
			data.pMoleratmen[i].renderAni(worldLocation, animationLocation);
	}
	for (int i = 0; i < data.mMolebats; i++)
	{
		if (data.pMolebats[i].getAlive())
 			data.pMolebats[i].renderAni(worldLocation, animationLocation);
	}

	data.pShadowProgram->unUse();
	data.pDeferredProgramNonAni->use();
	data.pCamera->updateUniforms( data.pDeferredProgramNonAni->getViewPerspectiveLocation(), data.pDeferredProgramNonAni->getCameraPositionLocation() );

	worldLocation = data.pDeferredProgramNonAni->getWorldLocation();
	GLuint tintLocation = data.pDeferredProgramNonAni->getTintLocation();

	// set default tint to white
	glUniform3f( tintLocation, 1.0f, 1.0f, 1.0f );

#if RENDER_TERRAIN
	mGround.renderNonAni( worldLocation, tintLocation );
#endif

	data.pPlayer->renderArrows(worldLocation, tintLocation);
	for (int i = 0; i < data.mTowers; i++)
		data.pTowers->renderArrows(worldLocation, tintLocation);
	for (int i = 0; i < mVisibleTowers; i++)
	{
		if (mpVisibleTowers[i]->getAlive())
		{
			mpVisibleTowers[i]->renderNonAni(worldLocation, tintLocation);
		}
	}

	for (int i = 0; i < data.mBabyCount; i++)
		data.pBabies[i].renderNonAni(worldLocation, tintLocation);

	if (tactical)
		mTacticalMarker.render( worldLocation, tintLocation );

	data.pDeferredProgramNonAni->unUse();

	worldLocation = data.pDeferredProgram->getWorldLocation();
	animationLocation = data.pDeferredProgram->getAnimationLocation();
	
	data.pDeferredProgram->use(data.pDeferredProgramNonAni->getFrameBuffer());
	data.pCamera->updateUniforms(data.pDeferredProgram->getViewPerspectiveLocation(), data.pDeferredProgram->getCameraPositionLocation());
	data.pPlayer->render( worldLocation, animationLocation);

	for (int i = 0; i < mVisibleTowers; i++)
	{
		if (mpVisibleTowers[i]->getAlive() && mpVisibleTowers[i]->getHasBallista())
		{
			mpVisibleTowers[i]->renderAni(worldLocation,animationLocation);
		}
	}

	for( int i=0; i<data.mMoleratmen; i++ )
	{
		if( data.pMoleratmen[i].getAlive() )
			data.pMoleratmen[i].renderAni(worldLocation, animationLocation);
	}
	for (int i = 0; i < data.mMolebats; i++)
	{
		if (data.pMolebats[i].getAlive())
			data.pMolebats[i].renderAni(worldLocation, animationLocation);
	}
	data.pBillboardProgram->use();
	data.pBillboardProgram->begin( data.pCamera );

	data.pEmission->draw();
	data.pBillboardProgram->end();
	data.pBillboardProgram->unUse();
	data.pDeferredProgram->unUse();

	drawOnScreenQuad();	
}

void Game::updateAnimations()
{
	data.pPlayer->updateAnimation();
	/*for (int i = 0; i < mVisibleTowers; i++)
	{
		if (mpVisibleTowers[i]->getAlive() && mpVisibleTowers[i]->getHasBallista())
		{
			mpVisibleTowers[i]->updateAnimation();
		}
	}*/

	for (int i = 0; i<data.mMoleratmen; i++)
	{
		if (data.pMoleratmen[i].getAlive())
			data.pMoleratmen[i].updateAnimation();
	}
	for (int i = 0; i < data.mMolebats; i++)
	{
		if (data.pMolebats[i].getAlive())
			data.pMolebats[i].updateAnimation();
	}
}

void Game::update(Input* inputs, float dt) 
{
	updateAnimations();
	data.pWavespawner->update(dt);
	data.pPlayer->update(inputs, dt);
	data.pEmission->update(dt);
	data.pCamera->follow(data.pPlayer->getPosition(), data.pPlayer->getLookAt(), 5.0f, {0,1,0});
	//data.pCamera->follow(data.pPlayer->getPosition(), data.pPlayer->getLookAt(), 1.6, { 0,1.3,0 });

	data.pCamera->updateFrustum();
	//data.pGrid->cull( data.pCamera->getFrustumPlanes(), data.pTowers, mVisibleTowers, &mMaxTowers );
	data.pGrid->cull( data.pCamera->getFrustum(), data.pTowers, mpVisibleTowers, &mVisibleTowers );

	//std::cout << "Visible towers: " << mMaxTowers << std::endl;

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
	if (!data.pPlayer->isAlive())
		int a = 0;
}

void Game::drawOnScreenQuad()
{
	data.pForwardProgram->use();
	data.pDeferredProgramNonAni->enableTextures(data.pForwardProgram->getTextureLocations());

	GLuint loc = glGetUniformLocation(data.pForwardProgram->getProgramID(), "shadowT");
	glUniform1i(loc, data.pShadowProgram->getDepthText());

	data.pShadowProgram->enableTexture(data.pForwardProgram->getTextureLocations()[5]);

	loc = glGetUniformLocation(data.pForwardProgram->getProgramID(), "shadowInvViewPers");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &data.pShadowProgram->getMat()[0][0]);

	//GLuint cameraPos = glGetUniformLocation(data.pForwardProgram->getProgramID(), "cameraPos");
	glUniform3fv(data.pForwardProgram->getCameraPositionLocation(), 1, &data.pCamera->getPosition()[0]);
	//GLuint inverseViewPerspective = glGetUniformLocation(data.pForwardProgram->getProgramID(), "invViewPerspective");
	glUniformMatrix4fv(data.pForwardProgram->getInverseViewPerspectiveLocation(), 1, GL_FALSE, &glm::inverse(data.pCamera->getViewPerspective())[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, testScreen);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*)offsetof(ScreenVertex, x));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*)offsetof(ScreenVertex, s));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	data.pDeferredProgramNonAni->disableTextures();
	data.pForwardProgram->unUse();
}