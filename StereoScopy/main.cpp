//#include "Ogre\Ogre.h"
#include <OgreCompositorManager.h>
#include "MyFramelistener.hpp"

int main(void)
{
	MyApplication* app = new MyApplication();
	app->startup();

	MyFrameListener* listener = new MyFrameListener( app->mWindow, app->mLeftCamera, app->mRightCamera, app->mLeftViewport, app->mRightViewport, app->_SinbadNode, app->_SinbadEnt );
	listener->setParent( app );
	app->_root->addFrameListener( listener );

	while( app->keepRunning() )
	{
		app->renderOneFrame();
	}

	delete app;
	delete listener;

	return 0;
}