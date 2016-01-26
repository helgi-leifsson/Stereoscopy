#include "Ogre\Ogre.h"
#include <cmath>

class MyApplication
{
//private:
public:
	Ogre::SceneManager* _sceneManager;
	Ogre::Root* _root;
	bool _keepRunning;
	Ogre::SceneNode* _SinbadNode;
	Ogre::SceneNode* mSibenikNode;
	Ogre::Entity* _SinbadEnt;
	Ogre::Entity* mSibenikEnt;
	Ogre::Viewport* mLeftViewport;
	Ogre::Viewport* mRightViewport;
	Ogre::Camera* mLeftCamera, *mRightCamera;
	float IPD;
	float halfIPD;
	float nearClip;
	Ogre::Matrix4 centerView;
	Ogre::Matrix4 leftView;
	Ogre::Matrix4 rightView;
	Ogre::RenderWindow* mWindow;

//public:
	void loadResources()
	{
		Ogre::ConfigFile cf;
		cf.load( "resources_d.cfg" );
		Ogre::ConfigFile::SectionIterator sectionIter = cf.getSectionIterator();
		Ogre::String sectionName, typeName, dataname;
		while( sectionIter.hasMoreElements() )
		{
			sectionName = sectionIter.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap* settings = sectionIter.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			for( i = settings->begin(); i != settings->end(); ++i )
			{
				typeName = i->first;
				dataname = i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation( dataname, typeName, sectionName );
			}
		}

		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	// Moves the eyes away from each other
	void calcViews()
	{
		leftView = Ogre::Matrix4::Matrix4( 1.0f, 0.0f, 0.0f, halfIPD,
											0.0f, 1.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f ) * centerView;

		rightView = Ogre::Matrix4::Matrix4( 1.0f, 0.0f, 0.0f, -1 * halfIPD,
											0.0f, 1.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f ) * centerView;

		mLeftCamera->setCustomViewMatrix( true, leftView );
		mRightCamera->setCustomViewMatrix( true, rightView );
	}

	void changeIpd( float increase )
	{
		IPD += increase;
		halfIPD = IPD * 0.5f;
		float HResolution = 700.0f;
		float VResolution = 350.0f;
		Ogre::Real aspectRatio = Ogre::Real( HResolution / (2.0f * VResolution ));
		mLeftCamera->setAspectRatio( aspectRatio );
		mRightCamera->setAspectRatio( aspectRatio );

		//float lensSeparationDistance = 0.08f;
		//float distortionK;
		float HScreenSize = 0.176f;
		float VScreenSize = 0.089f;
		float viewCenter = HScreenSize * 0.25f;
		float eyeProjectionShift = viewCenter - halfIPD;
		float projectionCenterOffset = (4.0f * eyeProjectionShift) / HScreenSize;

		float eyeToScreenDistance = 0.09f; // 90mm
		float FOV = 2.0f * atan( VScreenSize / ( 2.0f * eyeToScreenDistance ));
		float farClip = mLeftCamera->getFarClipDistance();
		
		centerView = mLeftCamera->getViewMatrix();

		// Projection calculations courtesy of the Oculus Rift SDK documentation.
		Ogre::Matrix4 centerProjection = Ogre::Matrix4::Matrix4( 1.0f / ( aspectRatio * tan( FOV / 2.0f )), 0.0f, 0.0f, 0.0f,
																0.0f, 1.0f / ( tan( FOV / 2.0f )), 0.0f, 0.0f,
																0.0f, 0.0f, farClip / ( nearClip - farClip ), ( farClip * nearClip ) / (nearClip - farClip ),
																0.0f, 0.0f, -1.0f, 0.0f );

		Ogre::Matrix4 leftProjection = Ogre::Matrix4::Matrix4( 1.0f, 0.0f, 0.0f, projectionCenterOffset,
																0.0f, 1.0f, 0.0f, 0.0f,
																0.0f, 0.0f, 1.0f, 0.0f,
																0.0f, 0.0f, 0.0f, 1.0f ) * centerProjection;

		Ogre::Matrix4 rightProjection = Ogre::Matrix4::Matrix4( 1.0f, 0.0f, 0.0f, -projectionCenterOffset,
																0.0f, 1.0f, 0.0f, 0.0f,
																0.0f, 0.0f, 1.0f, 0.0f,
																0.0f, 0.0f, 0.0f, 1.0f ) * centerProjection;
		

		mLeftCamera->setCustomProjectionMatrix( true, leftProjection );
		mRightCamera->setCustomProjectionMatrix( true, rightProjection );

		calcViews();
	}

	int startup()
	{
		IPD = 0.06f;
		halfIPD = IPD * 0.5f;
		nearClip = 0.001f;

		_root = new Ogre::Root( "plugins_d.cfg" );
		if( !_root->showConfigDialog() )
			return -1;

		mWindow = _root->initialise( true, "Ogre3D Stereoscopy" );
		_sceneManager = _root->createSceneManager( Ogre::ST_GENERIC );

		mLeftCamera = _sceneManager->createCamera( "leftCamera" );
		mLeftCamera->setPosition( Ogre::Vector3( 0, 6, 0 ));
		mLeftCamera->setNearClipDistance( nearClip );

		mRightCamera = _sceneManager->createCamera( "rightCamera" );
		mRightCamera->setPosition( Ogre::Vector3( 0, 6, 0 ));
		mRightCamera->setNearClipDistance( nearClip );

		mLeftViewport = mWindow->addViewport( mLeftCamera, 0, 0, 0, 0.5, 1 );
		mLeftViewport->setBackgroundColour( Ogre::ColourValue( 0.0, 0.0, 0.0 ));

		mRightViewport = mWindow->addViewport( mRightCamera, 1, 0.5, 0, 0.5, 1 );
		mRightViewport->setBackgroundColour( Ogre::ColourValue( 0.0, 0.0, 0.0 ));

		changeIpd( 0.0f );

		loadResources();
		createScene();

		return 0;
	}

	void renderOneFrame()
	{
		Ogre::WindowEventUtilities::messagePump();
		_keepRunning = _root->renderOneFrame();
	}

	bool keepRunning()
	{
		return _keepRunning;
	}

	void createScene()
	{		
		Ogre::Plane plane( Ogre::Vector3::UNIT_Y, -5 );
		Ogre::MeshManager::getSingleton().createPlane( "plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
			1500, 1500, 200, 200, true, 1, 5, 5, Ogre::Vector3::UNIT_Z );

		Ogre::Entity* ground = _sceneManager->createEntity( "LightPlaneEntity", "plane" );
		_sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject( ground );
		ground->setMaterialName( "Examples/BeachStones" );

		Ogre::Light* light = _sceneManager->createLight( "Light1" );
		light->setType( Ogre::Light::LT_DIRECTIONAL );
		light->setDirection( Ogre::Vector3( 1, -1, 0 ));

		_sceneManager->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_ADDITIVE );//Ogre::SHADOWTYPE_STENCIL_ADDITIVE );

		_SinbadEnt = _sceneManager->createEntity( "Sinbad.mesh" );
		_SinbadNode = _sceneManager->getRootSceneNode()->createChildSceneNode( "sinbadNode" );
		_SinbadNode->attachObject( _SinbadEnt );
		_SinbadNode->translate( Ogre::Vector3( 0, 5, -20 ));

		mSibenikEnt = _sceneManager->createEntity( "sibenik", "sibenik.mesh" );
		mSibenikNode = _sceneManager->getRootSceneNode()->createChildSceneNode( "sibenikNode" );
		mSibenikNode->attachObject( mSibenikEnt );
		mSibenikNode->translate( Ogre::Vector3( 0, 0, -50 ));
		mSibenikNode->scale( 3, 3, 3 );
		mSibenikNode->yaw( Ogre::Radian( -1.57079633 ) );

		// For testing only
		/*Ogre::CompositorManager::getSingleton().addCompositor( mLeftViewport, "Oculus" );
		Ogre::CompositorManager::getSingleton().addCompositor( mRightViewport, "Oculus" );
		Ogre::CompositorManager::getSingleton().setCompositorEnabled( mLeftViewport, "Oculus", true );
		Ogre::CompositorManager::getSingleton().setCompositorEnabled( mRightViewport, "Oculus", true );*/

		_sceneManager->setSkyBox(true, "Examples/SpaceSkyBox");
	}

	MyApplication()
	{
		_sceneManager = NULL;
		_root = NULL;
		//_listener = NULL;
	}

	~MyApplication()
	{
		delete _root;
		//delete _listener;
		//delete this->mLeftCamera;
		//delete this->mRightCamera;
		//delete this->mLeftViewport;
		//delete this->mRightViewport;
	}
};
