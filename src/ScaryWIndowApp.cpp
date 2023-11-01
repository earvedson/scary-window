#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTimeGl.h"
#include "cinder/Log.h"
#include <string>


using namespace ci;
using namespace ci::app;
using namespace std;

#define BUFSIZE 80
#define READ_INTERVAL 0.2

#define IDLE 0
#define SCARY 1
#define CALM_DOWN 2
#define TERRIFY 3


class ScaryWIndowApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    void keyDown( KeyEvent event ) override;
    
    string removeInitialAsterisks(const string& input);
    
    void setMode(const int nextMode);
    void translateSerialToMode(const int serial);
    
    // movie stuff
    qtime::MovieGlRef loadMovieFile( const fs::path &path);
    gl::TextureRef            mFrameTexture;
    qtime::MovieGlRef        mMovie;
    
    string scaryMoviesNames[3] = {"Zombie_wallpaper_live.mp4", "Zombie_wallpaper_live.mp4", "Zombie_wallpaper_live.mp4"};
    string movieNames[3] =  {"WarningSign.mp4", "Zombie_wallpaper_live.mp4", "Zombie_wallpaper_live.mp4"};
    
    qtime::MovieGlRef movies[3];
    
    int currentMode = 0;
    
    // serial stuff
    void setupSerial();
    bool        mSendSerialMessage;
    SerialRef    mSerial;
    string        mLastString;
//    gl::TextureRef    mTexture;
    double mLastRead, mLastUpdate;
    bool serialEstablished;
    
    // detector stuff
    int currentDistance;
};

void ScaryWIndowApp::setup()
{
    int index = 0;
    for (const string& movieName : movieNames) {
        fs::path moviePath = getAssetPath(movieName);
        if( ! moviePath.empty() ) {
            movies[index] = loadMovieFile( moviePath);
        }
        index++;
    }
    
    currentMode = IDLE;
    setMode(IDLE);
    
   // currentMode = IDLE;
    // movies[IDLE]->seekToStart();
    // movies[IDLE]->play(true);
    
    
   // fs::path moviePath = getAssetPath("Zombie_wallpaper_live.mp4");
    //if( ! moviePath.empty() ) {
    //    mMovie = loadMovieFile( moviePath);
    //}
    
     setupSerial();
}

void ScaryWIndowApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'f' ) {
        // Toggle full screen when the user presses the 'f' key.
        setFullScreen( ! isFullScreen() );
    }
    else if( event.getChar() == '1' ) {
        console() << "Key 1 pressed \n" << endl;
        translateSerialToMode(IDLE);
        //setMode(IDLE);
        /*
        mMovie->setActiveSegment(1, 5);
        mMovie->setLoop(true, true);
        mMovie->play(true);
        */
    }
    else if( event.getChar() == '2' ) {
        console() << "Key 2 pressed \n" << endl;
        translateSerialToMode(SCARY);
    }
    else if( event.getChar() == '3' ) {
        console() << "Key 3 pressed \n" << endl;
        setMode(CALM_DOWN);
    }
    else if( event.getCode() == KeyEvent::KEY_SPACE ) {
        //mMovie->play();
    }
    else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
        // Exit full screen, or quit the application, when the user presses the ESC key.
        if( isFullScreen() )
            setFullScreen( false );
        else
            quit();
    }
}

void ScaryWIndowApp::translateSerialToMode(const int serialMode) {
    if (currentMode != CALM_DOWN) {
        if (serialMode == IDLE) {
            if (currentMode == SCARY) {
                setMode(CALM_DOWN);
            }
        } else if (serialMode == SCARY) {
            if (currentMode == IDLE) {
                setMode(SCARY);
            }
        }
    }
}

void ScaryWIndowApp::setMode(const int nextMode) {
    int oldMode = currentMode;
    currentMode = nextMode;
    if (nextMode == IDLE) {
        movies[nextMode]->seekToStart();
        movies[nextMode]->setLoop(true);
        movies[nextMode]->play(true);
     
        // going with blank idle mode for now
        // movies[nextMode]->seekToStart();
        // movies[nextMode]->stop();
    }
    else if (nextMode == SCARY) {
        movies[nextMode]->seekToStart();
        movies[nextMode]->play(true);
    }
    else if (nextMode == CALM_DOWN) {
        movies[nextMode]->seekToTime(32);
        movies[nextMode]->play();
    }
    if (nextMode != oldMode) {
        movies[oldMode]->stop();
        movies[oldMode]->seekToStart();
    }
}

void ScaryWIndowApp::setupSerial() {
    serialEstablished = false;
    mLastRead = 0;
    mLastUpdate = 0;
    mSendSerialMessage = false;

    // print the devices
    for( const auto &dev : Serial::getDevices() ) {
        console() << "Device: " << dev.getName() << endl;
    }
    try {
        Serial::Device dev = Serial::findDeviceByNameContains( "tty.usbmodem" );
        mSerial = Serial::create( dev, 9600 );
    }
    catch( SerialExc &exc ) {
        CI_LOG_EXCEPTION( "coult not initialize the serial device", exc );
        exit( -1 );
    }
    
    // wait for * as a sign for first contact
    char contact = 0;
    console() << "connecting \n";
    while( contact != '*' )    {
        contact = (char)mSerial->readByte();
        console() << "read * \n";
    }
    console() << "connected \n";
    serialEstablished = true;
    
    // request actual data
    mSerial->writeByte( 1 );

    // clear buffer
    mSerial->flush();
    console() << "setup serial done \n";
}

qtime::MovieGlRef ScaryWIndowApp::loadMovieFile( const fs::path &moviePath )
{
    qtime::MovieGlRef movie;
    try {
        // load up the movie
        //mMovie = qtime::MovieGl::create( moviePath );
        movie = qtime::MovieGl::create( moviePath );
        // mMovie = movie;
        //mMovie->setLoop();
        // mMovie->play();
    }
    catch( ci::Exception &exc ) {
        console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
        //mMovie.reset();
        movie.reset();
    }

    mFrameTexture.reset();
    return movie;
}

void ScaryWIndowApp::mouseDown( MouseEvent event )
{
    
}

void ScaryWIndowApp::update()
{
    
    if( movies[currentMode] ) {
        mFrameTexture = movies[currentMode]->getTexture();
    }
    
    if (movies[currentMode]->isDone()) {
        if (currentMode != IDLE) {
            console() << "Done Playing, setting Idle" << std::endl;
            setMode(IDLE);
        }
    }
    
    /*
    static bool sPrintedDone = false;
    if( ! sPrintedDone && mMovie->isDone() ) {
        console() << "Done Playing" << std::endl;
        sPrintedDone = true;
    }
*/
    if (serialEstablished) {
        double now = getElapsedSeconds();
        double deltaTime = now - mLastUpdate;
        mLastUpdate = now;
        mLastRead += deltaTime;
        
        if( mLastRead > READ_INTERVAL )    {
            mSendSerialMessage = true;
            mLastRead = 0.0;
        }
        if( mSendSerialMessage ) {
            // request next chunk
            try {
                mSerial->writeByte( 1 );
            } catch (SerialExcWriteFailure) {
                // avoid crashing here
            }
            
            try{
                // read until newline, to a maximum of BUFSIZE bytes
                mLastString = mSerial->readStringUntil( '\n', BUFSIZE );
            }
            catch( SerialTimeoutExc &exc ) {
                CI_LOG_EXCEPTION( "timeout", exc );
            }
            // clear out * if any
            mLastString = removeInitialAsterisks(mLastString);
            // convert string to int
            try {
                currentDistance = stoi(mLastString);
            } catch (const invalid_argument& e) {
                cerr << "Invalid argument: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range: " << e.what() << std::endl;
            }
            
            translateSerialToMode(currentDistance);
            
            mSendSerialMessage = false;
            mSerial->flush();
        }
    }
}

void ScaryWIndowApp::draw()
{
	 gl::clear( Color( 0, 0, 0 ) );

    if( mFrameTexture ) {
//        Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( getWindowBounds(), true );
        Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFill( getWindowBounds(), true );
        gl::draw( mFrameTexture, centeredRect );
    }
}

string ScaryWIndowApp::removeInitialAsterisks(const string& input) {
    std::size_t firstNonAsterisk = input.find_first_not_of('*');
    if (firstNonAsterisk == string::npos) {
        return "";
    }
    return input.substr(firstNonAsterisk);
}

CINDER_APP( ScaryWIndowApp, RendererGl )
