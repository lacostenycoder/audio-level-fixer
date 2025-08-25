#include <JuceHeader.h>
#include "MainComponent.h"
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
// Window configuration constants
namespace WindowConfig
{
    static constexpr int DEFAULT_WIDTH = 1000;
    static constexpr int DEFAULT_HEIGHT = 700;
    static constexpr int MIN_WIDTH = 900;
    static constexpr int MIN_HEIGHT = 650;
    static constexpr int MAX_WIDTH = 2000;
    static constexpr int MAX_HEIGHT = 1500;
}

//==============================================================================
class AudioProcessorApplication : public juce::JUCEApplication
{
public:
    //==============================================================================
    AudioProcessorApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return false; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // Setup file logging
        auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                    .getChildFile("AudioProcessor").getChildFile("logs");
        dir.createDirectory();
        fileLogger.reset(new juce::FileLogger(dir.getChildFile("latest.txt"), "App start", 0));
        juce::Logger::setCurrentLogger(fileLogger.get());

        // This method is where you should put your application's initialisation code..
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        // Cleanup logging
        juce::Logger::setCurrentLogger(nullptr);
        fileLogger.reset();

        // Add your application's shutdown code here..
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            setupWindowBounds();
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        void setupWindowBounds()
        {
            auto displays = juce::Desktop::getInstance().getDisplays();
            auto mainDisplay = displays.getMainDisplay();
            auto workArea = mainDisplay.userArea;
            
            // Calculate dynamic limits based on screen size
            int maxWidth = juce::jmin(WindowConfig::MAX_WIDTH, workArea.getWidth() - 100);
            int maxHeight = juce::jmin(WindowConfig::MAX_HEIGHT, workArea.getHeight() - 100);
            
            // Ensure minimum size fits on screen
            int minWidth = juce::jmin(WindowConfig::MIN_WIDTH, workArea.getWidth() - 200);
            int minHeight = juce::jmin(WindowConfig::MIN_HEIGHT, workArea.getHeight() - 200);
            
            // Ensure minimum size is not too small for usability
            minWidth = juce::jmax(minWidth, 600);
            minHeight = juce::jmax(minHeight, 400);
            
            // Set the resize limits with screen-aware bounds
            setResizeLimits(minWidth, minHeight, maxWidth, maxHeight);
            
            // Use default size if it fits, otherwise scale down proportionally
            int defaultWidth = WindowConfig::DEFAULT_WIDTH;
            int defaultHeight = WindowConfig::DEFAULT_HEIGHT;
            
            if (defaultWidth > maxWidth || defaultHeight > maxHeight)
            {
                float scale = juce::jmin(float(maxWidth) / defaultWidth, float(maxHeight) / defaultHeight);
                defaultWidth = int(defaultWidth * scale);
                defaultHeight = int(defaultHeight * scale);
            }
            
            centreWithSize(defaultWidth, defaultHeight);
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<juce::FileLogger> fileLogger;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (AudioProcessorApplication)

