/****************************************************************************
 * THE GRAND METAMORPHOSIS
 * CS148 Assignment #4 - Fall 2010, Stanford University
 ****************************************************************************/


#include "parseConfig.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// --------------------------------------------------------------------------
// Structure to contain an image feature for a morph. A feature is a directed
// line segment from P to Q, with coordinates in pixel units relative to the
// lower-left corner of the image.
// --------------------------------------------------------------------------

struct Feature
{
    STPoint2 P, Q;
    Feature(const STPoint2 &p, const STPoint2 &q) : P(p), Q(q) { }
};

// --------------------------------------------------------------------------
// Constants, a few global variables, and function prototypes
// --------------------------------------------------------------------------

const int kWindowWidth  = 512;
const int kWindowHeight = 512;
const int kFrames       = 30;   // number of frames to generate

STImage *gDisplayedImage = 0;   // an image to display (for testing/debugging)

std::vector<Feature> gSourceFeatures;   // feature set on source image
std::vector<Feature> gTargetFeatures;   // corresponding features on target

// Copies an image into the global image for display
void DisplayImage(STImage *image);

// --------------------------------------------------------------------------
// CS148 TODO: Implement the functions below to compute the morph
// --------------------------------------------------------------------------

/**
 * Compute a linear blend of the pixel colors in two provided images according
 * to a parameter t.
 */
STImage *BlendImages(STImage *image1, STImage *image2, float t)
{
    STImage *result = 0;
    
    return result;
}

/**
 * Compute a field morph on an image using two sets of corresponding features
 * according to a parameter t.  Arguments a, b, and p are weighting parameters
 * for the field morph, as described in Beier & Nelly 1992, section 3.
 */
STImage *FieldMorph(STImage *image,
                    const std::vector<Feature> &sourceFeatures,
                    const std::vector<Feature> &targetFeatures,
                    float t, float a, float b, float p)
{
    STImage *result = 0;

    return result;
}

/**
 * Compute a morph between two images by first distorting each toward the
 * other, then combining the results with a blend operation.
 */
STImage *MorphImages(STImage *sourceImage, const std::vector<Feature> &sourceFeatures,
                     STImage *targetImage, const std::vector<Feature> &targetFeatures,
                     float t, float a, float b, float p)
{
    STImage *result = 0;

    return result;
}

/**
 * Compute a morph through time by generating appropriate values of t and
 * repeatedly calling MorphImages(). Saves the image sequence to disk.
 */
void GenerateMorphFrames(STImage *sourceImage, const std::vector<Feature> &sourceFeatures,
                         STImage *targetImage, const std::vector<Feature> &targetFeatures,
                         float a, float b, float p)
{
    // iterate and generate each required frame
    for (int i = 0; i < kFrames; ++i)
    {
        std::cout << "Metamorphosizing frame #" << i << "...";
        
        
        // **********
        // CS148 TODO: Compute a t value for the current frame and generate
        //             the morphed image here.
        // **********
        STImage *result = 0;
        
        
        // generate a file name to save
        std::ostringstream oss;
        oss << "frame" << std::setw(3) << std::setfill('0') << i << ".png";

        // write and deallocate the morphed image
        if (result) {
            result->Save(oss.str());
            delete result;
        }

        std::cout << " done." << std::endl;
    }
}

// --------------------------------------------------------------------------
// Utility and support code below that you do not need to modify
// --------------------------------------------------------------------------

/**
 * Copies an image into the global image for display
 */
void DisplayImage(STImage *image)
{
    // clean up the previous image
    if (gDisplayedImage) {
        delete gDisplayedImage;
        gDisplayedImage = 0;
    }

    // allocate a new image and copy it over
    if (image) {
        gDisplayedImage = new STImage(image->GetWidth(), image->GetHeight());
        size_t bytes = image->GetWidth() * image->GetHeight() * sizeof(STImage::Pixel);
        memcpy(gDisplayedImage->GetPixels(), image->GetPixels(), bytes);
    }
}

/**
 * Display callback function draws a single image to help debug
 */
void DisplayCallback()
{
    glClearColor(.2f, 2.f, 2.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gDisplayedImage)
        gDisplayedImage->Draw();

    glutSwapBuffers();
}

/**
 * Window resize callback function
 */
void ReshapeCallback(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}

/**
 * Keyboard callback function
 */
void KeyboardCallback(unsigned char key, int x, int y)
{
    switch (key)
    {
        // exit program on escape press
        case 27:
            exit(0);
            break;
        // save the currently displayed image if S is pressed
        case 's':
        case 'S':
            if (gDisplayedImage)
                gDisplayedImage->Save("screenshot.png");
            break;
        default:
            break;
    }
}

/**
 * This function is called by the parsing functions to populate the feature sets
 */
void AddFeatureCallback(STPoint2 p, STPoint2 q, ImageChoice image)
{
    if (image == IMAGE_1 || image == BOTH_IMAGES)
        gSourceFeatures.push_back(Feature(p, q));
    if (image == IMAGE_2 || image == BOTH_IMAGES)
        gTargetFeatures.push_back(Feature(p, q));
}

/**
 * Program entry point
 */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("Metamorphosis: CS148 Assignment 4");

    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardCallback);

    //
    // load the configuration from config.txt, or other file as specified
    //
    std::string configFile = "config.txt";
    if (argc > 1) configFile = argv[1];

    char sourceName[64], targetName[64];
    char saveName[64], loadName[64];
    STImage *sourceImage, *targetImage;
    parseConfigFile(configFile.c_str(),
                    sourceName, targetName,
                    saveName, loadName,
                    &sourceImage, &targetImage);
    delete sourceImage;
    delete targetImage;

    //
    // load the features from the saved features file
    //
    loadLineEditorFile(loadName, AddFeatureCallback,
                       sourceName, targetName,
                       &sourceImage, &targetImage);

    //
    // run the full morphing algorithm before going into the main loop to
    // display an image
    //

    // these weighting parameters (Beier & Nelly 1992) can be changed if desired
    const float a = 0.5f, b = 1.0f, p = 0.2f;

    GenerateMorphFrames(sourceImage, gSourceFeatures,
                        targetImage, gTargetFeatures,
                        a, b, p);


    //
    // display a test or debug image here if desired
    // (note: comment this out if you call DisplayImage from elsewhere)
    //
    STImage *result = sourceImage;

    // use this to test your image blending
    /*
    STImage *result = BlendImages(sourceImage, targetImage, 0.5f);
    */

    // use this to test your field morph
    /*
    STImage *result = FieldMorph(sourceImage, gSourceFeatures, gTargetFeatures,
                                 0.5f, a, b, p);
    */

    // use this to test your image morphing
    /*
    STImage *result = MorphImages(sourceImage, gSourceFeatures,
                                  targetImage, gTargetFeatures,
                                  0.5f, a, b, p);
    */
    
    DisplayImage(result);

    // enter the GLUT main loop
    glutMainLoop();

    return 0;
}

// --------------------------------------------------------------------------
