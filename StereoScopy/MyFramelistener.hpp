#include "MyApplication.hpp"
#include "OIS\OIS.h"

class MyFrameListener : public Ogre::FrameListener
{
public:

	OIS::InputManager* _InputManager;
	OIS::Keyboard* _Keyboard;
	OIS::Mouse* _Mouse;
	Ogre::Camera* mLeftCam;
	Ogre::Camera* mRightCam;
	float _movementspeed;
	Ogre::SceneNode* _node;
	Ogre::AnimationState* _aniState;
	Ogre::AnimationState* _aniStateTop;
	Ogre::AnimationState* mDanceState;
	MyApplication* mParent;

	MyFrameListener( Ogre::RenderWindow* win, Ogre::Camera* leftCam, Ogre::Camera* rightCam, Ogre::Viewport* leftViewport, Ogre::Viewport* rightViewport, Ogre::SceneNode* node, Ogre::Entity* ent )
	{
		_aniState = ent->getAnimationState( "IdleBase");
		_aniStateTop = ent->getAnimationState( "IdleTop");
 
		_aniState->setLoop(true);
		_aniStateTop->setLoop(true);
		_aniState->setEnabled(true);
		_aniStateTop->setEnabled(true);

		mLeftCam = leftCam;
		mRightCam = rightCam;
		_movementspeed = 25.0f;

		OIS::ParamList parameters;
		unsigned int windowHandle = 0;
		std::ostringstream windowHandleString;

		win->getCustomAttribute( "WINDOW", &windowHandle );
		windowHandleString << windowHandle;

		parameters.insert( std::make_pair( "WINDOW", windowHandleString.str() ));

		_InputManager = OIS::InputManager::createInputSystem( parameters );

		_Keyboard = static_cast< OIS::Keyboard* >( _InputManager->createInputObject( OIS::OISKeyboard, false ));
		_Mouse = static_cast< OIS::Mouse* >( _InputManager->createInputObject( OIS::OISMouse, false ));
	}

	~MyFrameListener()
	{
		_InputManager->destroyInputObject( _Keyboard );
		_InputManager->destroyInputObject( _Mouse );
		OIS::InputManager::destroyInputSystem( _InputManager );
	}

	void setParent( MyApplication* parent ){ mParent = parent; }

	bool frameStarted( const Ogre::FrameEvent& evt )
	{
		_Keyboard->capture();
		if( _Keyboard->isKeyDown( OIS::KC_ESCAPE ) )
		{
			return false;
		}

		Ogre::Vector3 translate( 0, 0, 0 );
		if( _Keyboard->isKeyDown( OIS::KC_W ))
		{
			translate += Ogre::Vector3( 0, 0, -1 );			
		}
		if( _Keyboard->isKeyDown( OIS::KC_S ))
		{
			translate += Ogre::Vector3( 0, 0, 1 );
		}
		if( _Keyboard->isKeyDown( OIS::KC_A ))
		{
			translate += Ogre::Vector3( -1, 0, 0 );
		}
		if( _Keyboard->isKeyDown( OIS::KC_D ))
		{
			translate += Ogre::Vector3( 1, 0, 0 );
		}

		if( _Keyboard->isKeyDown( OIS::KC_1 ))
		{
			mParent->changeIpd( 0.0001f );
		}
		if( _Keyboard->isKeyDown( OIS::KC_2 ))
		{
			mParent->changeIpd( -0.0001f );
		}

		mLeftCam->setCustomViewMatrix( false );
		mRightCam->setCustomViewMatrix( false );

		mLeftCam->moveRelative( translate * evt.timeSinceLastFrame * _movementspeed );
		mRightCam->moveRelative( translate * evt.timeSinceLastFrame * _movementspeed );
		
		_Mouse->capture();
		float rotX = _Mouse->getMouseState().X.rel * evt.timeSinceLastFrame * -1;
		float rotY = _Mouse->getMouseState().Y.rel * evt.timeSinceLastFrame * -1;
		mLeftCam->yaw( Ogre::Radian( rotX ));
		mRightCam->yaw( Ogre::Radian( rotX ));
		mLeftCam->pitch( Ogre::Radian( rotY ));
		mRightCam->pitch( Ogre::Radian( rotY ));

		mParent->centerView = mLeftCam->getViewMatrix();
		mParent->calcViews();

		_aniState->addTime( evt.timeSinceLastFrame );
		_aniStateTop->addTime( evt.timeSinceLastFrame );

		return true;
	}
};
