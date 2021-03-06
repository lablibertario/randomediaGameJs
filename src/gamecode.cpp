/*

Copyright (c) 2003 - 2014 Horacio Hernan Moraldo

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held liable
for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "generalmaths.h"// for mathematical functions
#include "gamecode.h"// for accessing to the code related only with the game
#include "gamedata.h"
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "players.h"// for managing the players
#include "genericunits.h"// for managing the generic units...
#include "enemyai.h"// for managing the enemy AI...
#include "bullets.h"// for managing the bullets
#include "collisions.h"// for functions related to the collisions of units
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
#include "diwrap.h"// for understanding the input
#include "wavegens.h"// for managing the wave generators
#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
#include "staging.h"// for the management of stages, levels, and the switching between these
#include <string.h>
#include <stdlib.h>

// this integer tells what's the current frame number, only counting
// those frames in which we aren't in pause mode.
int nonPausedFrameNumber;

// moveCamera ************
// Move the camera
bool moveCamera()
{
	// move the camera itself
	gameWorld.xCamera+=gameWorld.xSpeedCamera;
	gameWorld.yCamera+=gameWorld.ySpeedCamera;

	// move the background alone
	gameWorld.bgOffX+=gameWorld.bgSpeedX;
	gameWorld.bgOffY+=gameWorld.bgSpeedY;

	return true;// all ok
}// moveCamera

// drawBackgroundComplements ***************
// This function draws all the useful information and stuff that goes
// just over the background.
bool drawBackgroundComplements() {
	int x, y;
	char tempText[200];
	vectorListNodeStruct<playerUnitStruct>* nodeP;


	// draw the scores in the screen, even of those players who aren't
	// active right now (.nowActive=false)
	x=44; y=89;
	for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {

		sprintf(tempText,
			LAN_SEL("PUNTAJE: %i\nENERGIA: %i",
				"SCORE : %i\nENERGY: %i"),
			int(nodeP->data.score),
			int(nodeP->data.energyLeft));
		fntEdiaFont.drawMultilineText(DD, tempText, strlen(tempText),
			x, y, 83, 19, true,
			FILE_DDWRAP_ALIGN_LEFT, FILE_DDWRAP_VALIGN_CENTER);

		x+=300;
	}

	// draw the level number
/*	sprintf(tempText, "%i", gameWorld.currentLevel);
	fntDahFont.drawMultilineText(DD, tempText, strlen(tempText),
		24, 87, 20, 23, true,
		FILE_DDWRAP_ALIGN_CENTER, FILE_DDWRAP_VALIGN_CENTER);
*/
	// all ok
	return true;
}// drawBackgroundComplements

// drawForegroundComplements ***************
// This function draws all the useful information and stuff that goes
// just over the foreground (that is, over everything else).
bool drawForegroundComplements()
{
	int x, y;
	static char tempText[200];

	if (drawFallingLimits) {
		// Also, draw the falling limits
		// for sheeps and heads
		// draw min for W
		if (!gameWorld.srfGameScreen->drawLine(
			gameWorld.minXFallenWSheeps,// x
			getGroundWall(gameWorld.maxXFallenWSheeps),// y
			gameWorld.minXFallenWSheeps,// x2
			gameWorld.hCamera-1,// y2
			255, 0, 0)) {// color

				// error when drawing!
				logger.logLine(
					"Failed drawing the minXFallenWSheeps line");
				return false;
		}
		// draw max for W
		if (!gameWorld.srfGameScreen->drawLine(
			gameWorld.maxXFallenWSheeps,// x
			getGroundWall(gameWorld.maxXFallenWSheeps),// y
			gameWorld.maxXFallenWSheeps,// x2
			gameWorld.hCamera-1,// y2
			255, 0, 0)) {// color

				// error when drawing!
				logger.logLine(
					"Failed drawing the maxXFallenWSheeps line");
				return false;
		}
		// draw min for B
		if (!gameWorld.srfGameScreen->drawLine(
			gameWorld.minXFallenBSheeps,// x
			getGroundWall(gameWorld.minXFallenBSheeps),// y
			gameWorld.minXFallenBSheeps,// x2
			gameWorld.hCamera-1,// y2
			0, 255, 0)) {// color

				// error when drawing!
				logger.logLine(
					"Failed drawing the minBFallenWSheeps line");
				return false;
		}
		// draw max for B
		if (!gameWorld.srfGameScreen->drawLine(
			gameWorld.maxXFallenBSheeps,// x
			getGroundWall(gameWorld.maxXFallenBSheeps),// y
			gameWorld.maxXFallenBSheeps,// x2
			gameWorld.hCamera-1,// y2
			0, 255, 0)) {// color

				// error when drawing!
				logger.logLine(
					"Failed drawing the maxXFallenBSheeps line");
				return false;
		}
		// draw min for heads
		if (!gameWorld.srfGameScreen->drawLine(
			gameWorld.minXFallenHeads,// x
			getGroundWall(gameWorld.minXFallenHeads),// y
			gameWorld.minXFallenHeads,// x2
			gameWorld.hCamera-1,// y2
			0, 0, 255)) {// color

				// error when drawing!
				logger.logLine(
					"Failed drawing the minBFallenHeads line");
				return false;
		}
		// draw max for heads
		if (!gameWorld.srfGameScreen->drawLine(
			gameWorld.maxXFallenHeads,// x
			getGroundWall(gameWorld.maxXFallenHeads),// y
			gameWorld.maxXFallenHeads,// x2
			gameWorld.hCamera-1,// y2
			0, 0, 255)) {// color

				// error when drawing!
				logger.logLine(
					"Failed drawing the maxXFallenHeads line");
				return false;
		}
	}

	// Now, draw the virtual walls limits
	// But only draw them if required
	if (drawVirtualWalls) {
		for (y=0; y<int(gameWorld.hCamera); y++) {
			// draw left wall
			x=getLeftWall(y);
			x=(x<0?0:x);// not less than zero
			x=(x>int(gameWorld.wCamera)-1?// not more than the screen width
				int(gameWorld.wCamera)-1:x);
			if (!gameWorld.srfGameScreen->putPixel(
				x+int(gameWorld.screenX-(gameWorld.xCamera)),// x
				y+int(gameWorld.screenY-(gameWorld.yCamera)),// y
				255, 255, 255)) {

				// error when drawing!
				logger.logLine(
					"Failed drawing the left wall at (%i, %i)",
					true, true, true, x, y);
				return false;
			}

			// draw rightwall
			x=getRightWall(y);
			x=(x<0?0:x);// not less than zero
			x=(x>int(gameWorld.wCamera)-1?// not more than the screen width
				int(gameWorld.wCamera)-1:x);
			if (!gameWorld.srfGameScreen->putPixel(
				x+int(gameWorld.screenX-(gameWorld.xCamera)),// x
				y+int(gameWorld.screenY-(gameWorld.yCamera)),// y
				255, 255, 255)) {

				// error when drawing!
				logger.logLine(
					"Failed drawing the right wall at (%i, %i)",
					true, true, true, x, y);
				return false;
			}
		}

		for (x=0; x<int(gameWorld.wCamera); x++) {
			// draw ground wall
			y=getGroundWall(x);
			y=(y<0?0:y);// not less than zero
			y=(y>int(gameWorld.hCamera)-1?// not more than the screen width
				int(gameWorld.hCamera)-1:y);
			if (!gameWorld.srfGameScreen->putPixel(
				x+int(gameWorld.screenX-(gameWorld.xCamera)),// x
				y+int(gameWorld.screenY-(gameWorld.yCamera)),// y
				255, 255, 255)) {

				// error when drawing!
				logger.logLine(
					"Failed drawing the ground wall at (%i, %i)",
					true, true, true, x, y);
				return false;
			}
		}
	}


	// if it's time to show the game over message, draw it
	char* gameOverText=LAN_SEL("-- JUEGO TERMINADO --","-- GAME OVER --");
	if (gameWorld.showGameOverMessage) {
		fntEdiaBigFont.drawMultilineText(
			DD,// where to draw the text to
			gameOverText,// text to draw
			strlen(gameOverText),// text size
			0, 0,// position
			DD.getWidth(), DD.getHeight(),// text size
			true,// transparency
			FILE_DDWRAP_ALIGN_CENTER,// alignment
			FILE_DDWRAP_VALIGN_CENTER);// vertical alignment
	}

	// if it's paused, draw a text telling about it
	char* pausedText=LAN_SEL("-- PAUSA --", "-- PAUSED --");
	if (gameWorld.isGamePaused) {
		fntEdiaBigFont.drawMultilineText(
			DD,// where to draw the text to
			pausedText,// text to draw
			strlen(pausedText),// text size
			0, 0,// position
			DD.getWidth(), DD.getHeight(),// text size
			true,// transparency
			FILE_DDWRAP_ALIGN_CENTER,// alignment
			FILE_DDWRAP_VALIGN_CENTER);// vertical alignment
	}

	// all ok
	return true;
}// drawForegroundComplements

// drawBackground ********
// Draw the background on the game screen.
// Only return false for non-recover errors (not for lost surfaces,
// for example).
bool drawBackground()
{
	int bgX, bgY;
	int bgW, bgH;

	// Fill all the screen using our background image (tiled)...
	// The position of the background is calculated considering
	// both the xCamera and yCamera values (which tell the current
	// camera position), and the bgOffX and bgOffY (which tell the
	// current background offset)

	// Calculate the background size
	bgW=gameWorld.srfBg->getWidth();
	bgH=gameWorld.srfBg->getHeight();

	// go check what's the initial Y background offset
	bgY=-((int)(gameWorld.yCamera+gameWorld.bgOffY)%bgH);
	if (bgY>0) bgY=bgY-bgH;

	// Loop for all the rows of tiles in the background.
	for (; bgY<(int)(gameWorld.hCamera); bgY+=bgH) {

		// go check what's the initial Y background offset
		bgX=-((int)(gameWorld.xCamera+gameWorld.bgOffX)%bgW);
		if (bgX>0) bgX=bgX-bgW;

		// Loop for all the tiles in this row...
		for (; bgX<(int)(gameWorld.wCamera); bgX+=bgW) {

			// Blt it.
			gameWorld.srfGameScreen->bltFrom(
				*(gameWorld.srfBg),
				gameWorld.screenX+bgX,// dest coordinates
				gameWorld.screenY+bgY,
				0, 0, bgW, bgH,// source coordinates
				gameWorld.screenX, gameWorld.screenY,// clipping coordinates
				gameWorld.wCamera, gameWorld.hCamera,
				true, false);
		}// for bgX
	}// for bgY

	// draw all what goes just over the background
	if (!drawBackgroundComplements()) return false;

	return true;// all ok
}// drawBackground

// drawUnit **********
// Draws a given unit on the screen.
// Assumes the unit is drawable and active now (that is, it has to be
// drawn really).
bool drawUnit(genericUnitStruct* unit)
{
	int x, y;// coordinates
	int cx, cy;// coordinates for collision
	genericUnitTypeDataStruct* myUnitType;
	genericUnitTypeDataStruct* typeDataList;

	// frame of this unit
	frameDataStruct* myUnitFrame;

	// we have to get this value from the unit type
	int pixelsToTrembleWhenTrembling;

	// make sure the unit isn't a null pointer
	if (unit==NULL) return false;

	// check if this unit has a slave of his own...
	if (unit->slaveUnit.objectType!=FILE_GAMEDATA_GAMEOBJECT_EMPTY) {
		// it has a slave, draw that one first
		if (!drawUnit(getUnitPointer(unit->slaveUnit)))
			return false;
	}

	// get the data specific to this kind of unit
	typeDataList=getTypeDataStruct(unit->objectType);
	if (typeDataList==NULL) return false;
	myUnitType=&(typeDataList[unit->unitType]);

	// set some data data...
	myUnitFrame=&(myUnitType->frames[unit->currentFrame]);
	pixelsToTrembleWhenTrembling=
		myUnitType->pixelsToTrembleWhenUntouchable;

	// calculate the coordinates...
	// (int)(unit->x)-(myUnitFrame->handlerX): real x
	// +gameWorld.screenX: conversion to screen position
	// -gameWorld.xCamera: conversion to camera
	x=(int)((unit->x)-(myUnitFrame->handlerX)+
		gameWorld.screenX-(gameWorld.xCamera));
	y=(int)((unit->y)-(myUnitFrame->handlerY)+
		gameWorld.screenY-(gameWorld.yCamera));

	// Check if we have to draw the bounding rectangles...
	if (drawBoundingRectangles) {
		// Let us draw the bounding rectangle of this unit
		// Calculate the coordinates...
		cx=x+myUnitFrame->cX;
		cy=y+myUnitFrame->cY;

		// draw the bounding rectangle, only if its size is bigger than
		// zero
		if (myUnitFrame->cW>0 && myUnitFrame->cH>0) {
			gameWorld.srfGameScreen->fillRect(
				0, 0, 0,// color to use
				cx, cy,// dest coordinates
				myUnitFrame->cW,// size
				myUnitFrame->cH);
		}
	}// if drawBoundingRectangles

	// if the surface is in untouchable mode, make it tremble, but
	// only if the game isn't paused and if the user hasn't disabled
	// trembling from the console
	if (unit->trembling && !gameWorld.isGamePaused
		&& pixelsToTrembleWhenTrembling>0
		&& letSpritesTremble) {

		// tremble!
		// (trembling is implemented only in the visualization)
		x=x-(pixelsToTrembleWhenTrembling/2)
			+rand()%(pixelsToTrembleWhenTrembling+1);
		y=y-(pixelsToTrembleWhenTrembling/2)
			+rand()%(pixelsToTrembleWhenTrembling+1);
	}

	// blt it!
	gameWorld.srfGameScreen->bltFrom(
		*(myUnitFrame->surface),
		x,// dest coordinates
		y,
		myUnitFrame->x,// source coordinates
		myUnitFrame->y,
		myUnitFrame->w,
		myUnitFrame->h,
		gameWorld.screenX, gameWorld.screenY,// clipping coordinates
		gameWorld.wCamera, gameWorld.hCamera,
		dbgClipSprites,// only stop clipping it if that debug mode is enabled
		true);

	// draw the 'going to' lines if needed
	if (drawGoingToLines) {
		if (unit->goingToPoint) {
			// some mini local variables
			int x, y;
			int x2, y2;

			// calculate the coordinates
			x=int(unit->x+gameWorld.screenX-(gameWorld.xCamera));
			y=int(unit->y+gameWorld.screenY-(gameWorld.yCamera));
			x2=int(unit->goingToPointX+
				gameWorld.screenX-(gameWorld.xCamera));
			y2=int(unit->goingToPointY+
				gameWorld.screenY-(gameWorld.yCamera));
			if (unit->isPosRelative) {
				x2=int(unit->goingToPointX+gameWorld.screenX);
				y2=int(unit->goingToPointY+gameWorld.screenY);
			}

			// draw it
			gameWorld.srfGameScreen->drawLine(
				x, y,// 'from' coordinates
				x2, y2,// 'to' coordinates
				255,0, 0);// color
		}
	}

	// FOLLOWERS_TEST_BEGIN
	// draw the 'following' lines if needed
	if (drawFollowingLines) {
		if (unit->objectType==FILE_GAMEDATA_GAMEOBJECT_ENEMY) {
			enemyUnitStruct* unit1;
			enemyUnitStruct* unit2;
			int x, y;
			int x2, y2;

			switch (unit->unitType) {
			case FILE_GAMEDATA_ENEMY_SHEEPCARRIER:
			case FILE_GAMEDATA_ENEMY_HEADCARRIER:
				unit1=(enemyUnitStruct*)unit;

				if (unit1->aiSpecific.flockerData.unitToFollow.objectType
					!=FILE_GAMEDATA_GAMEOBJECT_EMPTY) {

					// get the other unit...
					unit2=(enemyUnitStruct*)getUnitPointer(
						unit1->aiSpecific.flockerData.unitToFollow);

					// check for null pointers...
					if (unit2!=NULL) {
						// some mini local variables
						// calculate the coordinates
						x=int(unit1->x+gameWorld.screenX-(gameWorld.xCamera));
						y=int(unit1->y+gameWorld.screenY-(gameWorld.yCamera));
						x2=int(unit2->x+gameWorld.screenX-(gameWorld.xCamera));
						y2=int(unit2->y+gameWorld.screenY-(gameWorld.yCamera));

						// draw it
						gameWorld.srfGameScreen->drawLine(
							x, y,// 'from' coordinates
							x2, y2,// 'to' coordinates
							0,0,255);// color
					}
				}
				break;
			}
		}
	}
	// FOLLOWERS_TEST_END

	// draw the handler points if needed
	if (drawHandlerPoints) {
		// some mini local variables
		int x, y;
		x=int(unit->x+gameWorld.screenX-(gameWorld.xCamera));
		y=int(unit->y+gameWorld.screenY-(gameWorld.yCamera));

		// draw the handler points if needed (as a red box with a
		// white pixel inside, marking the handler)
		gameWorld.srfGameScreen->fillRect(255,0,0,
			x-1, y-1, 3, 3);
		gameWorld.srfGameScreen->fillRect(255,255,255,
			x, y, 1, 1);
	}

	// all ok
	return true;
}// drawUnit

// ****************************
// MAIN FUNCTIONS
// Addressed as GameLoopCode::*
// ****************************

// drawFrame *************
// Draw the current frame on screen.
// Only return false for non-recover errors (not for lost surfaces,
// for example).
bool GameLoopCode::drawFrame()
{

	vectorListNodeStruct<playerUnitStruct>* nodeP;
	vectorListNodeStruct<enemyUnitStruct>* node;
	vectorListNodeStruct<bulletUnitStruct>* nodeB;
	vectorListNodeStruct<waveGenUnitStruct>* nodeWG;
	int x, y;

	// Draw the background
	if (!drawBackground()) {
		logger.logLine("Failed drawBackground");
		return false;
	}

	// Blt the bullets on screen...
	for (nodeB=bulletList.getFirstNode(); nodeB!=NULL; nodeB=bulletList.getNextNode(nodeB)) {
		// don't draw this unit if it's not active now
		if (!nodeB->data.nowActive) continue;
		// don't draw this utit if it's not visible now
		if (!nodeB->data.nowDrawable) continue;

		// draw this unit...
		drawUnit(&(nodeB->data));
	}

	// Blt the enemies on screen...
	for (node=enemyList.getFirstNode(); node!=NULL; node=enemyList.getNextNode(node)) {
		// don't draw this unit if it's not active now
		if (!node->data.nowActive) continue;
		// don't draw this utit if it's not visible now
		if (!node->data.nowDrawable) continue;

		// draw this unit...
		drawUnit(&(node->data));
	}

	// Blt the players on screen...
	for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {
		// don't draw this unit if it's not active now
		if (!nodeP->data.nowActive) continue;
		// don't draw this utit if it's not visible now
		if (!nodeP->data.nowDrawable) continue;

		// draw this unit...
		drawUnit(&(nodeP->data));
	}

	// Now, over anything else, blt the wave generators on screen...
	// But only draw them if required
	if (drawWaveGenerators) {
		for (nodeWG=waveGenList.getFirstNode(); nodeWG!=NULL; nodeWG=waveGenList.getNextNode(nodeWG)) {
			// don't draw this unit if it's not active now
			if (!nodeWG->data.nowActive) continue;
			// don't draw this utit if it's not visible now
			if (!nodeWG->data.nowDrawable) continue;

			// draw this unit...
			drawUnit(&(nodeWG->data));
		}
	}

	// Now draw the foreground complementary information
	if (!drawForegroundComplements())
		return false;

	// zoom if necessary
	if (gameWorld.zoomed) {
		x=int(double(DD.getWidth())/gameWorld.zoomBy);// zoom width
		y=int(double(DD.getHeight())/gameWorld.zoomBy);// zoom height

		// let's zoom
		DD.stretchBltFrom(DD, 0, 0, DD.getWidth(), DD.getHeight(),
			gameWorld.zoomCX-(x/2), gameWorld.zoomCY-(y/2),
			x, y, false, false, false);
	}

	// If gameWorld.srfGameScreen wasn't DD, here we should blt
	// srfGameScreen to DD...

	return true;// all ok
}// drawFrame

// executeFrame ***********
// Execute a frame, without drawing it on screen. It's what I call
// "virtual frames".
// Only return false for non-recover errors.
bool GameLoopCode::executeFrame(DWORD frameNumber)
{
	int i;// for some small loops
	vectorListNodeStruct<playerUnitStruct> *unit;// for checking how many players are on screen

	// ANY PLAYERS?
	// Count the active players
	for (unit=playerList.getFirstNode(), i=0; unit!=NULL; unit=playerList.getNextNode(unit)) {
		if (!unit->data.nowActive) continue;// don't count inactive units
		i++;
	}
	// If there is no players alive, and the game isn't finishing right
	// now, start finishing it now
	if (i<=0 && gameWorld.gameFinishedTimeLeft==-1) {
		// start finishing the game
		gameWorld.gameFinishedTimeLeft=gameWorld.maxGameFinishedTimeLeft;
	}

/*	// end the game if the escape key was pressed, or if the
	// time for finishing the game is zero (so meaning the game has to
	// end now)
	if (KEYEVENTDOWN(keyboard.keys, keyboard.latestKeys, DIK_ESCAPE)
		|| gameWorld.gameFinishedTimeLeft==0) {
		// that's the end, go to the intro
		if (!switchToProgramZone(pgzon_gameMenues, 0, 0, true))
			return false;
	}

	// check for the pause key...
	if (KEYEVENTDOWN(keyboard.keys, keyboard.latestKeys, DIK_P)) {
		// switch the pause mode, only if console isn't open
		if (!showConsole)
			gameWorld.isGamePaused=!gameWorld.isGamePaused;
	}
*/
	// check for pause mode... if we are in that mode, there is
	// nothing to do here
	if (gameWorld.isGamePaused)
		return true;// all ok

	// increment our internal frame number counter
	nonPausedFrameNumber++;

	// move the camera at first
	if (!moveCamera()) {
		logger.logLine("Failed moveCamera");
		return false;
	}

	// do the management of automatic stage advancement
	if (!manageAutomaticStaging(frameNumber)) {
		logger.logLine("Failed doing the automatic staging");
		return false;
	}

	// Game finish count down (or game over management)
	if (gameWorld.gameFinishedTimeLeft>0)
		gameWorld.gameFinishedTimeLeft--;

	// Move all the bullet units on the game world
	// It _has_ to be done before anything else is moved, so
	// spawned bullets don't move the first time.
	if (!moveBullets(frameNumber)) {
		logger.logLine("Failed moveBullets");
		return false;
	}

	// Now use the keyboard input for managing the player...
	if (!movePlayers(keyboard.keys)) {
		logger.logLine("Failed movePlayers");
		return false;
	}

	// Start the frame managing the enemy waves (that is, the addition
	// of new groups of enemies).
	if (!executeEnemyWaves(frameNumber)) {
		logger.logLine("Failed executeEnemyWaves");
		return false;
	}

	// Execute the enemy AI of all the units on the game world
	if (!executeEnemyAI(frameNumber)) {
		logger.logLine("Failed executeEnemyAI");
		return false;
	}

	// Now go check the collisions between the game objects...
	if (!checkCollisions()) {
		logger.logLine("Failed checkCollisions");
		return false;
	}

	return true;// all ok
}// executeFrame

// beforeLoop **********
// This is the code to be executed every time a program zone linked to
// these functions is switched to.
bool GameLoopCode::beforeLoop()
{
	// set our internal frame counter to zero
	nonPausedFrameNumber=0;

	// initialize all for starting the game...
	if (!gameWorld.initializeGame(0))
		return false;

	return true;// all ok
}// beforeLoop


// afterLoop ***********
// This is the code to be executed every time it's time to switch to
// another program zone, being a program zone currently linked to
// these functions.
bool GameLoopCode::afterLoop()
{
	vectorListNodeStruct<playerUnitStruct>* nodeP;

	// let's log the results...
	logger.logLine("Maximum enemies on screen: %i", true, true, true,
		enemyList.getMaxNodesUntilNow());
	logger.logLine("Maximum players on screen: %i", true, true, true,
		playerList.getMaxNodesUntilNow());
	logger.logLine("Maximum bullets on screen: %i", true, true, true,
		bulletList.getMaxNodesUntilNow());

	// log the statistics
	logger.logLine("Score in hits: %i", true, true, true,
		gameWorld.scoreInHits);
	logger.logLine("Score in free black sheeps: %i", true, true, true,
		gameWorld.scoreInFreeBlackSheeps);
	logger.logLine("Score in free white sheeps: %i", true, true, true,
		gameWorld.scoreInFreeWhiteSheeps);
	logger.logLine("Score in free heads: %i", true, true, true,
		gameWorld.scoreInFreeHeads);

	// Do what's needed with every player
	for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {
		// log the score
		logger.logLine("Player %i- Score : %i Energy: %i",
			true, true, true,
			playerList.getIndex(nodeP)+1,// player number
			int(nodeP->data.score),// score
			int(nodeP->data.energyLeft));// energy
	}

	// close the current zone
	gameWorld.deinitializeGame();

	return true;// all ok
}// afterLoop

