#include "WPILib.h"
#include "Vision/RGBImage.h"
#include "Vision/BinaryImage.h"
#include "Math.h"
#include "Turret.cpp"
#include "Loader.cpp"
#include "CameraCode.cpp"
#include "Robot_2013.h"
//#include "DashBoardControl.cpp"

// Note: Currently the code regarding to dashboard is commented out line #s are: 56 154

/**
 * Sample program to use NIVision to find rectangles in the scene that are illuminated
 * by a LED ring light (similar to the model from FIRSTChoice). The camera sensitivity
 * is set very low so as to only show light sources and remove any distracting parts
 * of the image.
 * 
 * The CriteriaCollection is the set of criteria that is used to filter the set of
 * rectangles that are detected. In this example we're looking for rectangles with
 * a minimum width of 30 pixels and maximum of 400 pixels.
 * 
 * The algorithm first does a color threshold operation that only takes objects in the
 * scene that have a bright green color component. Then a convex hull operation fills 
 * all the rectangle outlines (even the partially occluded ones). Then a small object filter
 * removes small particles that might be caused by green reflection scattered from other 
 * parts of the scene. Finally all particles are scored on rectangularity, aspect ratio,
 * and hollowness to determine if they match the target.
 *
 * Look in the VisionImages directory inside the project that is created for the sample
 * images as well as the NI Vision Assistant file that contains the vision command
 * chain (open it with the Vision Assistant)
 */



class Robot_2013 : public SimpleRobot
{
	//Structure to represent the scores for the various tests used for target identification
/*	struct Scores {
		double rectangularity;
		double aspectRatioInner;
		double aspectRatioOuter;
		double xEdge;
		double yEdge;

	};*/
	
	RobotDrive myRobot; // robot drive system
	Joystick leftStick; // left joystick
	Joystick rightStick; // right joystick
	Joystick codriverStick; // stick for the co-driver
	Scores *scores;
	Lifter *spikeLifter;
	Shooter *frisbeeShooter;
	Limiter *Loader;
	cameraCode *cameraFunctions;
//	dashboardControl *DBControl;

public:
	Robot_2013(void):
		myRobot(rightDrivePort, leftDrivePort),	// these must be initialized in the same order
		leftStick(leftStickPort),		// as they are declared above.
		rightStick(rightStickPort),
		codriverStick(codriverStickPort)
	{
		spikeLifter = new Lifter;
		frisbeeShooter = new Shooter;
		Loader = new Limiter;
		cameraFunctions = new cameraCode;
//		DBControl = new dashboardControl;
		myRobot.SetExpiration(0.1);
		myRobot.SetSafetyEnabled(false);
	}

	/**
	 * Image processing code to identify 2013 Vision targets
	 */
	void Autonomous(void)
	{		
		Threshold threshold(60, 100, 90, 255, 20, 255);	//HSV threshold criteria, ranges are in that order ie. Hue is 60-100
		ParticleFilterCriteria2 criteria[] = {
				{IMAQ_MT_AREA, AREA_MINIMUM, 65535, false, false}
		};												//Particle filter criteria, used to filter out small particles
//		 AxisCamera &camera = AxisCamera::GetInstance();	//To use the Axis camera uncomment this line
		printf("we are in autonomous\n");
	
		// Reset the actuator for the lifter and do a shot
		spikeLifter->lower();
		Wait(3);
		spikeLifter->off();
		frisbeeShooter->fire();

		while (IsAutonomous() && IsEnabled()) {
            /**
             * Do the image capture with the camera and apply the algorithm described above. This
             * sample will either get images from the camera or from an image file stored in the top
             * level directory in the flash memory on the cRIO. The file name in this case is "testImage.jpg"
             */
			ColorImage *image;
		
			image = new RGBImage("/testImage.jpg");		// get the sample image from the cRIO flash

//			camera.GetImage(image);				//To get the images from the camera comment the line above and uncomment this one
			BinaryImage *thresholdImage = image->ThresholdHSV(threshold);	// get just the green target pixels
			//thresholdImage->Write("/threshold.bmp");
			BinaryImage *convexHullImage = thresholdImage->ConvexHull(false);  // fill in partial and full rectangles
			//convexHullImage->Write("/ConvexHull.bmp");
			BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 1);	//Remove small particles
			//filteredImage->Write("Filtered.bmp");

			vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();  //get a particle analysis report for each particle
			scores = new Scores[reports->size()];
			
			//Iterate through each particle, scoring it and determining whether it is a target or not
			for (unsigned i = 0; i < reports->size(); i++) {
				ParticleAnalysisReport *report = &(reports->at(i));
				
				scores[i].rectangularity = scoreRectangularity(report);
				scores[i].aspectRatioOuter = cameraFunctions->scoreAspectRatio(filteredImage, report, true);
				scores[i].aspectRatioInner = cameraFunctions->scoreAspectRatio(filteredImage, report, false);			
				scores[i].xEdge = scoreXEdge(thresholdImage, report);
				scores[i].yEdge = scoreYEdge(thresholdImage, report);
				
				if(scoreCompare(scores[i], false))
				{
					printf("particle: %d  is a High Goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
					printf("Distance: %f \n", cameraFunctions->computeDistance(thresholdImage, report, false));
				} else if (scoreCompare(scores[i], true)) {
					printf("particle: %d  is a Middle Goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
					printf("Distance: %f \n", cameraFunctions->computeDistance(thresholdImage, report, true));
				} else {
					printf("particle: %d  is not a goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
				}
				printf("rect: %f  ARinner: %f \n", scores[i].rectangularity, scores[i].aspectRatioInner);
				printf("ARouter: %f  xEdge: %f  yEdge: %f  \n", scores[i].aspectRatioOuter, scores[i].xEdge, scores[i].yEdge);	
			}
			printf("\n");
			
			// be sure to delete images after using them
			delete filteredImage;
			delete convexHullImage;
			delete thresholdImage;
			delete image;
			
			//delete allocated reports and Scores objects also
			delete scores;
			delete reports;
		}
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
//		DBControl->dashboardOut(1);
		bool spikeOn = false;
		int debugOutput = 0;
		printf("starting Teleop\n");
//		spikeLifter->cycle_linear_actuator(true);
		myRobot.SetSafetyEnabled(false);
		printf("we are in teleop, accepting joystick input now\n");
		
		while (IsOperatorControl())
		{
			if (Loader->checkLimit()) {
				myRobot.StopMotor();
			}
			else {
				myRobot.TankDrive(leftStick, rightStick, true); // Third argument squares the inputs, which is better for percise control
			}
			if (rightStick.GetRawButton(11) == true){
				spikeLifter->raise();
				if (debugOutput != 1){
					printf("extending the relay\n");
					debugOutput = 1;
				}
				spikeOn = true;
			}
			else if(rightStick.GetRawButton(10) == true){
				spikeLifter->lower();
				if (debugOutput != 2){
					printf("retracting the relay\n");
					debugOutput = 2;
				}
				spikeOn = true;
			}
			else {
				if (spikeOn){
					spikeLifter->off();
					if (debugOutput != 3){
						printf("stopping the spike\n");
						debugOutput = 3;
					}
					spikeOn = false;
				}
			}
			Wait(0.005);				// wait for a motor update time
		}
	}

	void Disabled(void)
	{
		printf("I\'m Disabled!!\n"); // This code runs whenever the robot is disabled, even if the printf buffer sometimes forgets to flush
	}
	/**
	 * Compares scores to defined limits and returns true if the particle appears to be a target
	 * 
	 * @param scores The structure containing the scores to compare
	 * @param outer True if the particle should be treated as an outer target, false to treat it as a center target
	 * 
	 * @return True if the particle meets all limits, false otherwise
	 */
	bool scoreCompare(Scores scores, bool outer){
		bool isTarget = true;
		
		isTarget &= scores.rectangularity > RECTANGULARITY_LIMIT;
		if(outer){
			isTarget &= scores.aspectRatioOuter > ASPECT_RATIO_LIMIT;
		} else {
			isTarget &= scores.aspectRatioInner > ASPECT_RATIO_LIMIT;
		}
		isTarget &= scores.xEdge > X_EDGE_LIMIT;
		isTarget &= scores.yEdge > Y_EDGE_LIMIT;

		return isTarget;
	}
	
	/**
	 * Computes a score (0-100) estimating how rectangular the particle is by comparing the area of the particle
	 * to the area of the bounding box surrounding it. A perfect rectangle would cover the entire bounding box.
	 * 
	 * @param report The Particle Analysis Report for the particle to score
	 * @return The rectangularity score (0-100)
	 */
	double scoreRectangularity(ParticleAnalysisReport *report){
		if(report->boundingRect.width*report->boundingRect.height !=0){
			return 100*report->particleArea/(report->boundingRect.width*report->boundingRect.height);
		} else {
			return 0;
		}	
	}
	
	/**
	 * Computes a score based on the match between a template profile and the particle profile in the X direction. This method uses the
	 * the column averages and the profile defined at the top of the sample to look for the solid vertical edges with
	 * a hollow center.
	 * 
	 * @param image The image to use, should be the image before the convex hull is performed
	 * @param report The Particle Analysis Report for the particle
	 * 
	 * @return The X Edge Score (0-100)
	 */
	double scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report){
		double total = 0;
		LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_COLUMN_AVERAGES, report->boundingRect);
		for(int i=0; i < (averages->columnCount); i++){
			if(xMin[i*(XMINSIZE-1)/averages->columnCount] < averages->columnAverages[i] 
			   && averages->columnAverages[i] < xMax[i*(XMAXSIZE-1)/averages->columnCount]){
				total++;
			}
		}
		total = 100*total/(averages->columnCount);		//convert to score 0-100
		imaqDispose(averages);							//let IMAQ dispose of the averages struct
		return total;
	}
	
	/**
	 * Computes a score based on the match between a template profile and the particle profile in the Y direction. This method uses the
	 * the row averages and the profile defined at the top of the sample to look for the solid horizontal edges with
	 * a hollow center
	 * 
	 * @param image The image to use, should be the image before the convex hull is performed
	 * @param report The Particle Analysis Report for the particle
	 * 
	 * @return The Y Edge score (0-100)
	 */
	double scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report){
		double total = 0;
		LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_ROW_AVERAGES, report->boundingRect);
		for(int i=0; i < (averages->rowCount); i++){
			if(yMin[i*(YMINSIZE-1)/averages->rowCount] < averages->rowAverages[i] 
			   && averages->rowAverages[i] < yMax[i*(YMAXSIZE-1)/averages->rowCount]){
				total++;
			}
		}
		total = 100*total/(averages->rowCount);		//convert to score 0-100
		imaqDispose(averages);						//let IMAQ dispose of the averages struct
		return total;
	}		
};

START_ROBOT_CLASS(Robot_2013);

